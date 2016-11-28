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

#if UWP_API && !XSAPI_SIDECAR

#include "app_service_messages.h"
#include "app_service_provider.h"
#include "xsapi/title_callable_ui.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

namespace app_service {

	namespace provider
	{
		using namespace Windows::Foundation;
		using namespace Windows::ApplicationModel::AppService;
		
	namespace 
	{
		AppServiceConnection ^m_connection = nullptr;
		std::shared_ptr<xbox_live_user> m_user;

        _Use_decl_annotations_ void send_service_message_with_retry(AppServiceConnection ^connection, Windows::Foundation::Collections::ValueSet ^response);
			
        _Use_decl_annotations_ void wait_and_retry_send(Windows::Foundation::Collections::ValueSet ^response)
		{
			pplx::create_task(
				[]()
			{
				pplx::wait(1000);
			})
				.then([response]()
			{
				send_service_message_with_retry(m_connection, response);
			});
		}

        _Use_decl_annotations_ void send_service_message_with_retry(AppServiceConnection ^connection, Windows::Foundation::Collections::ValueSet ^response)
		{
			if (connection != nullptr)
			{
				pplx::create_task(connection->SendMessageAsync(response))
					.then([response](pplx::task<AppServiceResponse^> t)
				{
					bool delivered = false;
					try
					{
						AppServiceResponse ^deliveryResponse = t.get();
						if (deliveryResponse->Status == AppServiceResponseStatus::Success)
						{
							delivered = true;
						}
					}
					catch (Platform::Exception^)
					{
					}

					if (!delivered)
					{
						m_connection = nullptr;
						wait_and_retry_send(response);
					}
				});
			}
			else
			{
				wait_and_retry_send(response);
			}
		}
			
		template <class T>
        _Use_decl_annotations_ void send_xbl_result(xbox_live_result<T> xblResult)
		{
			Windows::Foundation::Collections::ValueSet ^serviceMessage = nullptr;
			if (xblResult.err())
			{
				serviceMessage = ref new Windows::Foundation::Collections::ValueSet();
				serviceMessage->Insert(L"request_id", xblResult.payload().request_id());
				serviceMessage->Insert(L"err", xblResult.err().value());
				serviceMessage->Insert(L"err_message", ref new Platform::String(utility::conversions::utf8_to_utf16(xblResult.err_message()).c_str()));
			}
			else
			{
				serviceMessage = xblResult.payload().to_service_message();
			}
			send_service_message_with_retry(m_connection, serviceMessage);
		}

		// Catch-all for message types not explicitly handled
		template <class T>
        _Use_decl_annotations_ void handle_app_service_message(const T &message)
		{
			Windows::Foundation::Collections::ValueSet ^serviceMessage = ref new Windows::Foundation::Collections::ValueSet();
			serviceMessage->Insert(L"request_id", message.request_id());
			serviceMessage->Insert(L"err", std::make_error_code(xbox_live_error_code::unsupported).value());
			serviceMessage->Insert(L"err_message", L"Service provider does not support this request type");
			send_service_message_with_retry(m_connection, serviceMessage);
		}

        _Use_decl_annotations_ void handle_app_service_message( const sign_in_request_message &message)
		{
			auto task = message.show_ui() ?
				m_user->signin(Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher) :
				m_user->signin_silently();
			task.then([message](xbox_live_result<sign_in_result> result)
			{
				sign_in_result_message response(
					message.request_id(),
					result.payload().status(),
					m_user->xbox_user_id(),
					m_user->gamertag(),
					m_user->age_group(),
					m_user->privileges(),
					m_user->web_account_id()
				);
				send_xbl_result(xbox_live_result<sign_in_result_message>(response, result.err(), result.err_message()));
			});
		}

        _Use_decl_annotations_ void handle_app_service_message(const token_and_signature_request_message &message)
		{
			auto task = m_user->is_signed_in() ?
				pplx::task_from_result(xbox_live_result<sign_in_result>(sign_in_status::success)) :
				(message.prompt_for_credentials_if_needed() ?
					m_user->signin(Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher) :
					m_user->signin_silently());
			task.then([message](xbox_live_result<sign_in_result> result)
			{
				if (result.err())
				{
					xbox_live_result<token_and_signature_result> tsResult(result.err(), result.err_message());
					return pplx::task_from_result(tsResult);
				}

				if (result.payload().status() != sign_in_status::success || !m_user->is_signed_in())
				{
					xbox_live_result<token_and_signature_result> tsResult(xbox_live_error_code::auth_user_not_signed_in, "User must be signed in to call this API");
					return pplx::task_from_result(tsResult);
				}

				return m_user->get_token_and_signature_array(message.http_method(), message.url(), message.headers(), utility::conversions::from_base64(message.body_base64()));
			})
				.then([message](xbox_live_result<token_and_signature_result> result)
			{
				token_and_signature_result_message response(
					message.request_id(),
					result.payload().token(),
					result.payload().signature(),
					result.payload().xbox_user_id(),
					result.payload().gamertag(),
					result.payload().xbox_user_hash(),
					result.payload().age_group(),
					result.payload().privileges(),
					result.payload().web_account_id(),
					result.payload().reserved());
				send_xbl_result(xbox_live_result<token_and_signature_result_message>(response, result.err(), result.err_message()));
			});
		}

        _Use_decl_annotations_ void handle_app_service_message(const achievements_ui_request_message &message)
		{
			auto uiTask = [=]()
			{
				xbox::services::system::title_callable_ui::show_title_achievements_ui(message.title_id())
					.then([message](xbox_live_result<void> result)
				{
					achievements_ui_result_message response(message.request_id());
					send_xbl_result(xbox_live_result<achievements_ui_result_message>(response, result.err(), result.err_message()));
				});
			};

			auto dispatcherHandler = ref new Windows::UI::Core::DispatchedHandler(uiTask);
			auto asyncOp = Windows::ApplicationModel::Core::CoreApplication::MainView->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, dispatcherHandler);

			pplx::create_task(asyncOp)
				.then([](pplx::task<void> t)
			{
				try
				{
					t.get();
				}
				catch (Platform::Exception ^)
				{
				}
			});
		}

        _Use_decl_annotations_ void on_service_closed(AppServiceConnection ^sender, AppServiceClosedEventArgs ^args)
		{
			m_connection = nullptr;
		}

        _Use_decl_annotations_ void on_request_received(AppServiceConnection ^sender, AppServiceRequestReceivedEventArgs ^args)
		{
#define APP_SERVICE_MESSAGE(type) \
			else if (Platform::String::CompareOrdinal(requestedOperation, L#type) == 0) \
			{ \
				handle_app_service_message(type::from_service_message(args->Request->Message).payload()); \
			} \

			Platform::String ^requestedOperation = safe_cast<Platform::String^>(args->Request->Message->Lookup("message_type"));
			if (false)
			{

			}
			APP_SERVICE_MESSAGE_LIST
#undef APP_SERVICE_MESSAGE
		}

	} // anonymous namespace

    _Use_decl_annotations_ void initialize_from_incoming_connection(AppServiceConnection ^connection)
	{
		if (m_user == nullptr)
		{
			m_user = std::make_shared<xbox_live_user>();
		}

			m_connection = connection;
			m_connection->RequestReceived += ref new TypedEventHandler<AppServiceConnection ^, AppServiceRequestReceivedEventArgs ^>(&on_request_received);
			m_connection->ServiceClosed += ref new TypedEventHandler<AppServiceConnection ^, AppServiceClosedEventArgs ^>(&on_service_closed);
		}
	} // namespace provider
} // namespace app_service

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#endif