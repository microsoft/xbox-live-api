//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"

#if XSAPI_SIDECAR

#include "app_service_messages.h"
#include "app_service_client.h"
#include "xsapi/title_callable_ui.h"

// In general we pretend to be building for UWP.  These therefore don't
// get declared when we include windows.h.  We'll use them to try and
// simulate modality for TCUI.
extern "C"
{
	__declspec(dllimport) BOOL WINAPI EnableWindow(
		_In_ HWND hWnd,
		_In_ BOOL bEnable
	);

	__declspec(dllimport) HWND WINAPI GetForegroundWindow(void);

	__declspec(dllimport) BOOL WINAPI SetForegroundWindow(
		_In_ HWND hWnd
	);
}

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

namespace app_service {
	namespace client {
		namespace {

			// Requests that show UI may run very long, beyond the time-out on receiving
			// a direct response.  So the reply must actually be implemented as a request
			// from the service provider.  We use this type to match these up to an 
			// outstanding request.
			class request_context : public std::enable_shared_from_this<request_context>
			{
			public:
				request_context()
				{
					static std::atomic_uint32_t s_requestId = 0;
					m_requestId = ++s_requestId;
					m_responseReady.reset();
				}

				uint32 request_id() const { return m_requestId; }

				void complete_from_response(_In_ Windows::Foundation::Collections::ValueSet^ response)
				{
					m_responseBody = response;
					m_responseReady.set();
				}

				pplx::task<Windows::Foundation::Collections::ValueSet^> wait_for_response()
				{
					return pplx::create_task(
						[sharedThis = shared_from_this()]
					{
						sharedThis->m_responseReady.wait();
						return sharedThis->m_responseBody;
					});
				}

			private:
				uint32 m_requestId;
				pplx::event m_responseReady;
				Windows::Foundation::Collections::ValueSet ^m_responseBody;
			};

			// RAII type to make a TCUI window appear modal through a task chain.
			class modal_window_guard
			{
			public:
				modal_window_guard(_In_ HWND hWnd) : m_hWnd(hWnd)
				{
					if (m_hWnd != NULL)
					{
						EnableWindow(m_hWnd, FALSE);
					}
				}

				modal_window_guard(modal_window_guard &&other)
				{
					m_hWnd = other.m_hWnd;
					other.m_hWnd = NULL;
				}

				// 'Move-on-copy' since ppl copies the continuation provided to .then
				modal_window_guard(_In_ const modal_window_guard &other) : modal_window_guard(std::move(const_cast<modal_window_guard&>(other))) {}

				~modal_window_guard()
				{
					if (m_hWnd != NULL)
					{
						SetForegroundWindow(m_hWnd);
						EnableWindow(m_hWnd, TRUE);
					}
				}

			private:
				HWND m_hWnd;
			};
			
			enum class ui_mode
			{
				allowed,
				not_allowed,
			};

			using namespace Windows::ApplicationModel::AppService;

			AppServiceConnection ^m_connection = nullptr;
			std::vector<std::shared_ptr<request_context>> m_pendingRequests;
			std::mutex m_connectionLock;
			std::mutex m_pendingRequestsLock;
			pplx::event m_connectionReady;
			HWND m_hWnd = NULL;
            Platform::String^ m_pfn = _T("");
            Platform::String^ m_appChannelName = _T("");

			pplx::task<AppServiceConnection^> ensure_connection();
			void on_service_closed(_In_ AppServiceConnection ^sender, _In_ AppServiceClosedEventArgs ^args);
			void on_request_received(_In_ AppServiceConnection ^sender, _In_ AppServiceRequestReceivedEventArgs ^args);


			pplx::task<AppServiceConnection^> ensure_connection()
			{
				std::lock_guard<std::mutex> lock(m_connectionLock);
				if (m_connection != nullptr)
				{
					return pplx::create_task(
						[]()
					{
						m_connectionReady.wait();
						return m_connection;
					});
				}

				m_connectionReady.reset();
				m_connection = ref new AppServiceConnection();
                m_connection->AppServiceName = m_appChannelName;
                m_connection->PackageFamilyName = m_pfn;

				return create_task(m_connection->OpenAsync())
					.then([](pplx::task<AppServiceConnectionStatus> statusTask)
				{
					std::lock_guard<std::mutex> lock(m_connectionLock);
					try
					{
						if (statusTask.get() == AppServiceConnectionStatus::Success)
						{
							m_connection->RequestReceived += ref new Windows::Foundation::TypedEventHandler<AppServiceConnection ^, AppServiceRequestReceivedEventArgs ^>(&on_request_received);
							m_connection->ServiceClosed += ref new Windows::Foundation::TypedEventHandler<AppServiceConnection ^, AppServiceClosedEventArgs ^>(&on_service_closed);
						}
						else
						{
							m_connection = nullptr;
						}
					}
					catch (Platform::Exception^)
					{
						m_connection = nullptr;
					}

					m_connectionReady.set();
					return m_connection;
				});
			}

			void on_request_received(_In_ AppServiceConnection ^sender, _In_ AppServiceRequestReceivedEventArgs ^args)
			{
				if (args->Request->Message->HasKey("request_id"))
				{
					uint32 requestId = safe_cast<uint32>(args->Request->Message->Lookup("request_id"));
					std::lock_guard<std::mutex> lock(m_pendingRequestsLock);
					auto pred = [requestId](const std::shared_ptr<request_context> &p) { return p->request_id() == requestId; };
					auto matchingRequest = std::find_if(m_pendingRequests.begin(), m_pendingRequests.end(), pred);
					if (matchingRequest != m_pendingRequests.end())
					{
						(*matchingRequest)->complete_from_response(args->Request->Message);
						m_pendingRequests.erase(matchingRequest);
					}
				}
			}

			void on_service_closed(_In_ AppServiceConnection ^sender, _In_ AppServiceClosedEventArgs ^args)
			{
				{
					std::lock_guard<std::mutex> lock(m_connectionLock);
					m_connection = nullptr;
				}

				// If we have outstanding requests then we need to keep the connection open
				// so the provider can get back in touch.
				std::lock_guard<std::mutex> lock(m_pendingRequestsLock);
				if (m_pendingRequests.size() > 0)
				{
					pplx::create_task(
						[]()
					{
						// Reopening the connection during the close event fails, so we need a short delay.
						concurrency::wait(1000);
					})
						.then(
							[]()
					{
						ensure_connection();
					});
				}
			}


			template <class REQUEST, class RESPONSE = typename response_type<REQUEST>::value, class... PARAMS>
			pplx::task<xbox_live_result<RESPONSE>> send_app_service_request_and_wait_for_response(_In_ ui_mode uiMode, PARAMS... params)
			{
				modal_window_guard windowGuard = { uiMode == ui_mode::allowed ? m_hWnd : NULL };
				std::shared_ptr<request_context> requestTracker = std::make_shared<request_context>();
				{
					std::lock_guard<std::mutex> lock(m_pendingRequestsLock);
					m_pendingRequests.push_back(requestTracker);
				}

                REQUEST message = { requestTracker->request_id(), params... };

                Platform::String^ protocolUrl = m_appChannelName + "://";
                Windows::Foundation::Uri^ uri = ref new Windows::Foundation::Uri(protocolUrl);

				auto createWindowTask = uiMode == ui_mode::allowed ?
					concurrency::create_task(Windows::System::Launcher::LaunchUriAsync(uri)) :
					task_from_result(true);

				return createWindowTask
					.then([](bool launchSuccessful)
				{
					if (!launchSuccessful)
					{
						throw std::runtime_error("Could not activate app to provide UI");
					}

					return ensure_connection();
				})
					.then([message = std::move(message)](AppServiceConnection ^connection)
				{
					if (!connection)
					{
						throw std::runtime_error("Could not connect to app service");
					}

					return connection->SendMessageAsync(message.to_service_message());
				})
					.then([requestTracker](AppServiceResponse ^response)
				{
					if (response->Status != AppServiceResponseStatus::Success)
					{
						throw std::runtime_error("No response from app service");
					}

					return requestTracker->wait_for_response();
				})
					.then([requestTracker, windowGuard = std::move(windowGuard)](Windows::Foundation::Collections::ValueSet^ responseBody)
				{
					return xbox_live_result<RESPONSE>(RESPONSE::from_service_message(responseBody));
				});
			}
		} // anonymous namespace

        pplx::task<xbox_live_result<sign_in_result_message>> sign_in(
            _In_ bool showUI,
            _In_ bool forceRefresh,
            _In_ std::shared_ptr<local_config> config
		)
        {
            m_pfn = ref new Platform::String(config->sidecar_pfn().c_str());
            m_appChannelName = ref new Platform::String(config->sidecar_appchannel().c_str());

			auto task = send_app_service_request_and_wait_for_response<sign_in_request_message>(
				showUI ? ui_mode::allowed : ui_mode::not_allowed,
				showUI,
				forceRefresh);
			return utils::create_exception_free_task<sign_in_result_message>(task);
		}


		pplx::task<xbox_live_result<token_and_signature_result_message> > get_token_and_signature(
			_In_ const string_t& httpMethod,
			_In_ const string_t& url,
			_In_ const string_t& endpointForNsal,
			_In_ const string_t& headers,
			_In_ const std::vector<unsigned char>& bytes,
			_In_ bool promptForCredentialsIfNeeded,
            _In_ bool forceRefresh,
            _In_ std::shared_ptr<local_config> config
		)
        {
            m_pfn = ref new Platform::String(config->sidecar_pfn().c_str());
            m_appChannelName = ref new Platform::String(config->sidecar_appchannel().c_str());

			auto task = send_app_service_request_and_wait_for_response<token_and_signature_request_message>(
				promptForCredentialsIfNeeded ? ui_mode::allowed : ui_mode::not_allowed,
				httpMethod,
				url,
				endpointForNsal,
				headers,
				utility::conversions::to_base64(bytes),
				promptForCredentialsIfNeeded,
				forceRefresh);

			return utils::create_exception_free_task<token_and_signature_result_message>(task);
		}

		pplx::task<xbox_live_result<achievements_ui_result_message>> show_title_achievements_ui(
            _In_ uint32_t titleId,
            _In_ std::shared_ptr<local_config> config
		)
        {
            m_pfn = ref new Platform::String(config->sidecar_pfn().c_str());
            m_appChannelName = ref new Platform::String(config->sidecar_appchannel().c_str());

			auto task = send_app_service_request_and_wait_for_response<achievements_ui_request_message>(
				ui_mode::allowed,
				titleId);
			return utils::create_exception_free_task(task);
		}

		void set_hwnd(_In_ HWND hWnd)
		{
			m_hWnd = hWnd;
		}
	} // namespace client
} // namespace app_service

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#endif