// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "http_call_impl.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "build_version.h"
#include "xsapi/system.h"
#include "xbox_live_app_config_internal.h"
#if TV_API
#include "System/ppltasks_extra.h"
#elif XSAPI_U
#include "request_signer.h"
#endif
#if XSAPI_A
#include "a/user_impl_a.h"
#elif XSAPI_I
#include "user_impl_ios.h"
#endif

using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features

using namespace xbox::services;
using namespace xbox::services::system;

const int MIN_DELAY_FOR_HTTP_INTERNAL_ERROR_IN_SEC = 10;
const double MAX_DELAY_TIME_IN_SEC = 60.0;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<http_call> create_xbox_live_http_call(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const string_t& httpMethod,
    _In_ const string_t& serverName,
    _In_ const web::uri& pathQueryFragment
    )
{  
    return xbox_system_factory::get_factory()->create_http_call(
        xboxLiveContextSettings,
        httpMethod,
        serverName,
        pathQueryFragment,
        xbox_live_api::unspecified
        );
}

http_call_data::http_call_data(
    _In_ const std::shared_ptr<xbox_live_context_settings>& _xboxLiveContextSettings,
    _In_ const xsapi_internal_string& _httpMethod,
    _In_ const xsapi_internal_string& _serverName,
    _In_ const web::uri& _pathQueryFragment,
    _In_ xbox_live_api _xboxLiveApi
) :
    xboxLiveContextSettings(_xboxLiveContextSettings),
    httpMethod(_httpMethod),
    serverName(_serverName),
    pathQueryFragment(_pathQueryFragment),
    xboxLiveApi(_xboxLiveApi),
    hasPerformedRetryOn401(false),
    retryAllowed(true),
    iterationNumber(0),
    httpCallResponseBodyType(http_call_response_body_type::json_body),
    longHttpCall(false),
    httpTimeout(std::chrono::seconds(DEFAULT_HTTP_TIMEOUT_SECONDS)),
    contentTypeHeaderValue("application/json; charset=utf-8"),
    xboxContractVersionHeaderValue("1"),
    addDefaultHeaders(true),
    queue(nullptr),
    callback(nullptr)
{
    HCHttpCallCreate(&callHandle);
    HCHttpCallRequestSetRetryCacheId(callHandle, static_cast<uint32_t>(xboxLiveApi));
    HCHttpCallRequestSetTimeoutWindow(callHandle, static_cast<uint32_t>(xboxLiveContextSettings->http_timeout_window().count()));
    HCHttpCallRequestSetRetryDelay(callHandle, static_cast<uint32_t>(xboxLiveContextSettings->http_retry_delay().count()));

    fullUrl = serverName + utils::internal_string_from_string_t(pathQueryFragment.to_string());
    HCHttpCallRequestSetUrl(callHandle, httpMethod.data(), fullUrl.data());
}

http_call_data::~http_call_data()
{
    HCHttpCallCloseHandle(callHandle);
}

http_call_impl::http_call_impl() :
    m_httpCallData(std::make_shared<http_call_data>(nullptr, xsapi_internal_string(), xsapi_internal_string(), web::uri(), xbox_live_api::unspecified))
{
}

http_call_impl::http_call_impl(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& serverName,
    _In_ const web::uri& pathQueryFragment,
    _In_ xbox_live_api xboxLiveApi
    ) :
    m_httpCallData(xsapi_allocate_shared<http_call_data>(
        xboxLiveContextSettings,
        httpMethod,
        serverName,
        pathQueryFragment,
        xboxLiveApi))
{
}

pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType
    )
{
    pplx::task_completion_event<std::shared_ptr<http_call_response>> tce;
    
    get_response(
        httpCallResponseBodyType,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](std::shared_ptr<http_call_response_internal> response)
        {
            tce.set(std::make_shared<http_call_response>(response));
        });

    return pplx::task<std::shared_ptr<http_call_response>>(tce);
}

xbox_live_result<void> 
http_call_impl::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_opt_ async_queue_handle_t queue,
    _In_opt_ http_call_callback callback
    )
{
    m_httpCallData->httpCallResponseBodyType = httpCallResponseBodyType;
    m_httpCallData->queue = queue;
    m_httpCallData->callback = std::move(callback);

    add_default_headers_if_needed(m_httpCallData);

    internal_get_response(m_httpCallData);
    return xbox_live_result<void>();
}

pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ const web::http::http_request& _httpRequest
    )
{
    // make a copy request so we can extract the body
    auto httpRequest = _httpRequest;

    m_httpCallData->httpCallResponseBodyType = httpCallResponseBodyType;

    for (const auto& header : httpRequest.headers())
    {
        add_header(m_httpCallData, utils::internal_string_from_string_t(header.first), utils::internal_string_from_string_t(header.second), true);
    }

    auto body = utils::internal_string_from_string_t(httpRequest.extract_string().get());
    m_httpCallData->requestBody = http_call_request_message_internal(body);
    HCHttpCallRequestSetRequestBodyString(m_httpCallData->callHandle, body.data());

    pplx::task_completion_event<std::shared_ptr<http_call_response>> tce;
    pplx::task<std::shared_ptr<http_call_response>> task(tce);
    m_httpCallData->callback = [tce](std::shared_ptr<http_call_response_internal> response)
    {
        tce.set(std::make_shared<http_call_response>(response));
    };

    internal_get_response(m_httpCallData);
    return task;
}

#if XSAPI_XDK_AUTH // XDK
pplx::task<std::shared_ptr<http_call_response>> http_call_impl::get_response_with_auth(
    _In_ Windows::Xbox::System::User^ user,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
    )
{
    auto userContext = std::make_shared<xbox::services::user_context>(user);
    return get_response_with_auth(userContext, httpCallResponseBodyType, allUsersAuthRequired);
}
#endif 

#if XSAPI_NONXDK_CPP_AUTH
pplx::task<std::shared_ptr<http_call_response>> http_call_impl::get_response_with_auth(
    _In_ std::shared_ptr<system::xbox_live_user> user,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
    )
{
    auto userContext = std::make_shared<xbox::services::user_context>(user);
    return get_response_with_auth(userContext, httpCallResponseBodyType, allUsersAuthRequired);
}
#endif

#if XSAPI_NONXDK_WINRT_AUTH 
pplx::task<std::shared_ptr<http_call_response>> http_call_impl::get_response_with_auth(
    _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
    )
{
    auto userContext = std::make_shared<xbox::services::user_context>(user);
    return get_response_with_auth(userContext, httpCallResponseBodyType, allUsersAuthRequired);
}
#endif


pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::get_response_with_auth(
    _In_ const std::shared_ptr<user_context>& userContext,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
)
{
    pplx::task_completion_event<std::shared_ptr<http_call_response>> tce;
    get_response_with_auth(
        userContext,
        httpCallResponseBodyType,
        allUsersAuthRequired,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](std::shared_ptr<http_call_response_internal> response)
        {
            tce.set(std::make_shared<http_call_response>(response));
        });

    return pplx::task<std::shared_ptr<http_call_response>>(tce);
}

xbox_live_result<void> http_call_impl::get_response_with_auth(
    _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired,
    _In_opt_ async_queue_handle_t queue,
    _In_ http_call_callback callback
    )
{
    m_httpCallData->userContext = userContext;
    m_httpCallData->httpCallResponseBodyType = httpCallResponseBodyType;
    m_httpCallData->queue = queue;
    m_httpCallData->callback = callback;

    add_default_headers_if_needed(m_httpCallData);

#if !TV_API 
#if XSAPI_CPP
    if (!m_httpCallData->userContext->user() || !m_httpCallData->userContext->user()->is_signed_in())
#else
    if (!userContext->user() || !userContext->user()->IsSignedIn)
#endif
    {
        return xbox_live_result<void>(xbox_live_error_code::auth_user_not_signed_in, "User must be signed in to call this API");
    }
#endif

    return internal_get_response_with_auth(allUsersAuthRequired);
}


#if XSAPI_U
pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::get_response_with_auth(
    _In_ http_call_response_body_type httpCallResponseBodyType
    )
{
    auto user = xbox_live_user::get_last_signed_in_user();
    if (user == nullptr)
    {
        return pplx::task_from_result<std::shared_ptr<http_call_response>>(nullptr);
    }
    std::shared_ptr<xbox::services::user_context> userContextPtr = std::make_shared<xbox::services::user_context>(user);
    
    return _Internal_get_response_with_auth(
        userContextPtr,
        httpCallResponseBodyType,
        false
        );
}

#else
pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::get_response_with_auth(
    _In_ http_call_response_body_type httpCallResponseBodyType
    )
{
    assert(false);
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    return pplx::task_from_result<std::shared_ptr<http_call_response>>(nullptr);
}
#endif


#if XSAPI_U
pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::get_response(
    _In_ std::shared_ptr<system::ecdsa> proofKey,
    _In_ const system::signature_policy& signaturePolicy,
    _In_ http_call_response_body_type httpCallResponseBodyType
)
{
    m_httpCallData->request = get_default_request();
    std::string body;

    if (proofKey->pub_key().x.size() != 0 || proofKey->pub_key().y.size() != 0)
    {
        std::vector<unsigned char> bodyData;
        if (m_httpCallData->requestBody.get_http_request_message_type() == http_request_message_type::vector_message)
        {
            bodyData = m_httpCallData->requestBody.request_message_vector();
        }
        else
        {
            body = utility::conversions::to_utf8string(m_httpCallData->requestBody.request_message_string());
            bodyData.assign(body.begin(), body.end());
        }

        string_t signature = xbox::services::system::request_signer::sign_request(
            *proofKey,
            signaturePolicy,
            utility::datetime::utc_now().to_interval(),
            m_httpCallData->httpMethod,
            utils::path_and_query_from_uri(m_httpCallData->request.request_uri()),
            m_httpCallData->request.headers(),
            bodyData
        );

        m_httpCallData->request.headers().add(_T("Signature"), signature);
    }

    return internal_get_response(m_httpCallData);
}
#endif

xbox_live_result<void> http_call_impl::internal_get_response_with_auth(
    _In_ bool allUsersAuthRequired
    )
{
    auto httpCallData = m_httpCallData;

    xbox_live_callback<xbox_live_result<user_context_auth_result>> authCallback =
        [httpCallData](_In_ xbox::services::xbox_live_result<user_context_auth_result> result)
    {
        if (result.err())
        {
            auto httpCallResponse = xsapi_allocate_shared<http_call_response_internal>(httpCallData);
            httpCallResponse->set_error_info(static_cast<xbox_live_error_code>(result.err().value()), result.err_message().data());
            httpCallResponse->route_service_call();
            httpCallData->callback(httpCallResponse);
            return;
        }

        const auto& authResult = result.payload();
        if (!authResult.token().empty())
        {
            add_header(httpCallData, AUTH_HEADER, authResult.token(), false);
        }

        if (!authResult.signature().empty())
        {
            add_header(httpCallData, SIG_HEADER, authResult.signature(), false);
        }

        internal_get_response(httpCallData);
    };

    if (m_httpCallData->requestBody.get_http_request_message_type() == http_request_message_type::vector_message)
    {
        m_httpCallData->userContext->get_auth_result(
            m_httpCallData->httpMethod,
            m_httpCallData->fullUrl,
            utils::headers_to_string(m_httpCallData->requestHeaders),
            m_httpCallData->requestBody.request_message_vector(),
            allUsersAuthRequired,
            m_httpCallData->queue,
            authCallback
            );
    }
    else
    {
        m_httpCallData->userContext->get_auth_result(
            m_httpCallData->httpMethod,
            m_httpCallData->fullUrl,
            utils::headers_to_string(m_httpCallData->requestHeaders),
            m_httpCallData->requestBody.request_message_string(),
            allUsersAuthRequired,
            m_httpCallData->queue,
            authCallback
            );
    }

    return xbox_live_result<void>();
}

void http_call_impl::handle_throttle_error(
    _In_ const std::shared_ptr<http_call_response_internal>& httpCallResponse,
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    auto appConfig = xbox_live_app_config_internal::get_app_config_singleton();
    if (utils::str_icmp(appConfig->sandbox(), "RETAIL") != 0)
    {
        bool disableAsserts = appConfig->is_disable_asserts_for_xbox_live_throttling_in_dev_sandboxes();
        if (!disableAsserts)
        {
            std::stringstream msg;
            LOGS_ERROR << "Xbox Live service call to " << httpCallData->fullUrl << " was throttled";
            LOGS_ERROR << httpCallResponse->err_message();
            LOGS_ERROR << "You can temporarily disable the assert by calling";
            LOGS_ERROR << "xboxLiveContext->settings()->disable_asserts_for_xbox_live_throttling_in_dev_sandboxes()";
            LOGS_ERROR << "Note that this will only disable this assert.  You will still be throttled in all sandboxes.";

#ifndef UNIT_TEST_SERVICES
            XSAPI_ASSERT(false && "Xbox Live service call was throttled.  See Output for more detail");
#endif
        }
    }

}
 
void http_call_impl::handle_unauthorized_error(
    _In_ void* context,
    _In_ const std::shared_ptr<http_call_response_internal>& httpCallResponse,
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    httpCallData->userContext->refresh_token(
        httpCallData->queue, 
        [context, httpCallResponse, httpCallData](xbox_live_result<std::shared_ptr<token_and_signature_result_internal>> result)
        {
            httpCallData->hasPerformedRetryOn401 = true;
            if (!result.err())
            {
                // if got new token, try http call again
                // it won't repeat the token refresh since hasPerformedRetryOn401 is now true
                internal_get_response(httpCallData);
            }
            else
            {
                // if getting a new token failed, then we need to just return the 401 upwards
                utils::get_shared_ptr<http_call_data>(context, true);
                httpCallResponse->route_service_call();
                httpCallData->callback(httpCallResponse);
            }
        });
}

void http_call_impl::internal_get_response(
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    set_http_timeout(httpCallData);
    set_user_agent(httpCallData);

    HCHttpCallRequestSetRetryAllowed(httpCallData->callHandle, httpCallData->retryAllowed);
    HCHttpCallRequestSetTimeout(httpCallData->callHandle, static_cast<uint32_t>(httpCallData->httpTimeout.count()));

    AsyncBlock *asyncBlock = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    asyncBlock->queue = httpCallData->queue;
    asyncBlock->context = utils::store_shared_ptr(httpCallData);
    asyncBlock->callback = [](_Inout_ AsyncBlock* asyncBlock)
    {
        auto httpCallData = utils::get_shared_ptr<http_call_data>(asyncBlock->context, false);
        auto httpCallResponse = xsapi_allocate_shared<http_call_response_internal>(httpCallData);

        void* context = asyncBlock->context;
        xsapi_memory::mem_free(asyncBlock);

        if (httpCallData->retryAllowed && 
            httpCallResponse->http_status() == web::http::status_codes::Unauthorized &&
            httpCallData->userContext != nullptr &&
            !httpCallData->hasPerformedRetryOn401)
        {
            handle_unauthorized_error(context, httpCallResponse, httpCallData);
        }
        else 
        {
            if (httpCallResponse->http_status() == static_cast<int>(xbox_live_error_code::http_status_429_too_many_requests))
            {
                handle_throttle_error(httpCallResponse, httpCallData);
            }

            utils::get_shared_ptr<http_call_data>(context, true);
            httpCallResponse->route_service_call();
            httpCallData->callback(httpCallResponse);
        }
    };

    HCHttpCallPerformAsync(httpCallData->callHandle, asyncBlock);
}

string_t http_call_impl::server_name() const
{
    return utils::string_t_from_internal_string(m_httpCallData->serverName);
}

const web::uri& http_call_impl::path_query_fragment() const
{
    return m_httpCallData->pathQueryFragment;
}

string_t http_call_impl::http_method() const
{
    return utils::string_t_from_internal_string(m_httpCallData->httpMethod);
}

void http_call_impl::set_add_default_headers(_In_ bool value)
{
    m_httpCallData->addDefaultHeaders = value;
}

bool http_call_impl::add_default_headers() const
{
    return m_httpCallData->addDefaultHeaders;
}

void http_call_impl::set_long_http_call(
    _In_ bool value
    )
{
    m_httpCallData->longHttpCall = value;
}

bool http_call_impl::long_http_call() const
{
    return m_httpCallData->longHttpCall;
}

void http_call_impl::set_retry_allowed(
    _In_ bool value
    )
{
    m_httpCallData->retryAllowed = value;
}

bool http_call_impl::retry_allowed() const
{
    return m_httpCallData->retryAllowed;
}

const http_call_request_message_internal& http_call_impl::request_body() const
{
    return m_httpCallData->requestBody;
}

void http_call_impl::set_request_body(
    _In_ const string_t& value
    )
{
    xsapi_internal_string internalValue = utils::internal_string_from_string_t(value);
    m_httpCallData->requestBody = http_call_request_message_internal(internalValue);
    HCHttpCallRequestSetRequestBodyString(m_httpCallData->callHandle, internalValue.data());
}

void http_call_impl::set_request_body(
    _In_ const std::vector<uint8_t>& value
    )
{
    m_httpCallData->requestBody = http_call_request_message_internal(xsapi_internal_vector<uint8_t>(value.begin(), value.end()));
    HCHttpCallRequestSetRequestBodyBytes(m_httpCallData->callHandle, value.data(), static_cast<uint32_t>(value.size()));
}

void http_call_impl::set_request_body(
    _In_ const web::json::value& value
    )
{
    xsapi_internal_string stringValue = utils::internal_string_from_string_t(value.serialize());
    m_httpCallData->requestBody = http_call_request_message_internal(stringValue);
    HCHttpCallRequestSetRequestBodyString(m_httpCallData->callHandle, stringValue.data());
}

void http_call_impl::set_request_body(_In_ const xsapi_internal_string& value)
{
    m_httpCallData->requestBody = http_call_request_message_internal(value);
    HCHttpCallRequestSetRequestBodyString(m_httpCallData->callHandle, value.data());
}

string_t http_call_impl::content_type_header_value() const
{
    return utils::string_t_from_internal_string(m_httpCallData->contentTypeHeaderValue);
}

void http_call_impl::set_content_type_header_value(
    _In_ const string_t& value
    )
{
    m_httpCallData->contentTypeHeaderValue = utils::internal_string_from_string_t(value);
}

string_t http_call_impl::xbox_contract_version_header_value() const
{
    return utils::string_t_from_internal_string(m_httpCallData->xboxContractVersionHeaderValue);
}

void http_call_impl::set_xbox_contract_version_header_value(
    _In_ const string_t& value
    )
{
    m_httpCallData->xboxContractVersionHeaderValue = utils::internal_string_from_string_t(value);
}

void http_call_impl::set_custom_header(
    _In_ const string_t& headerName,
    _In_ const string_t& headerValue
    )
{
    add_header(
        m_httpCallData,
        utils::internal_string_from_string_t(headerName).data(),
        utils::internal_string_from_string_t(headerValue).data(),
        true
        );
}

void http_call_impl::set_custom_header(
    _In_ const xsapi_internal_string& headerName,
    _In_ const xsapi_internal_string& headerValue,
    _In_ bool allowTracing
    )
{
    add_header(
        m_httpCallData,
        headerName,
        headerValue,
        allowTracing
        );
}

void http_call_impl::set_user_agent(
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    if (httpCallData->userContext != nullptr)
    {
        xsapi_internal_string userAgent = DEFAULT_USER_AGENT;
        if (!httpCallData->userContext->caller_context().empty())
        {
            userAgent += " " + httpCallData->userContext->caller_context();
        }

        if (httpCallData->userContext->api_type() != xbox::services::caller_api_type::api_unknown)
        {
            switch (httpCallData->userContext->api_type())
            {
                case xbox::services::caller_api_type::api_c: userAgent += " c"; break;
                case xbox::services::caller_api_type::api_cpp: userAgent += " cpp"; break;
                case xbox::services::caller_api_type::api_winrt: userAgent += " winrt"; break;
            }
        }

        add_header(httpCallData, "User-Agent", userAgent, true);
    }
    else
    {
        xsapi_internal_string userAgent = DEFAULT_USER_AGENT;
        add_header(httpCallData, "User-Agent", userAgent, true);
    }

}

void http_call_impl::set_http_timeout(
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    if (httpCallData->longHttpCall)
    {
        // Long calls such as Title Storage upload/download ignore http_timeout_window so they act as expected with 
        // requiring the game developer to manually adjust http_timeout_window before calling them.
        httpCallData->httpTimeout = httpCallData->xboxLiveContextSettings->long_http_timeout();
    }
    else
    {
        // For all other calls, set the timeout to be how much time left before hitting the http_timeout_window setting with a min of 5 seconds
        std::chrono::milliseconds timeElapsedSinceFirstCall = std::chrono::duration_cast<std::chrono::milliseconds>(httpCallData->requestStartTime - httpCallData->firstCallStartTime);
        std::chrono::seconds remainingTimeBeforeTimeout = std::chrono::duration_cast<std::chrono::seconds>(httpCallData->xboxLiveContextSettings->http_timeout_window() - timeElapsedSinceFirstCall);
        uint64_t secondsLeft = __min(DEFAULT_HTTP_TIMEOUT_SECONDS, remainingTimeBeforeTimeout.count());
        uint64_t secondsLeftCapped = __max(MIN_HTTP_TIMEOUT_SECONDS, secondsLeft);
        httpCallData->httpTimeout = std::chrono::seconds(secondsLeftCapped);
    }
}

void http_call_impl::add_default_headers_if_needed(
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    if (httpCallData->addDefaultHeaders)
    {
        add_header(httpCallData, "x-xbl-contract-version", httpCallData->xboxContractVersionHeaderValue, true);
        add_header(httpCallData, "Content-Type", httpCallData->contentTypeHeaderValue, true);
        add_header(httpCallData, "Accept-Language", utils::get_locales(), true);
    }
}

void http_call_impl::add_header(
    _In_ const std::shared_ptr<http_call_data>& httpCallData,
    _In_ const xsapi_internal_string& headerName,
    _In_ const xsapi_internal_string& headerValue,
    _In_ bool allowTracing
    )
{
    httpCallData->requestHeaders[headerName] = headerValue;
    HCHttpCallRequestSetHeader(httpCallData->callHandle, headerName.data(), headerValue.data(), allowTracing);
}

std::shared_ptr<http_retry_after_manager>
http_retry_after_manager::get_http_retry_after_manager_singleton()
{
    auto xsapiSingleton = xbox::services::get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
    if (xsapiSingleton->m_httpRetryPolicyManagerSingleton == nullptr)
    {
        xsapiSingleton->m_httpRetryPolicyManagerSingleton = std::make_shared<http_retry_after_manager>();
    }

    return xsapiSingleton->m_httpRetryPolicyManagerSingleton;
}

void http_retry_after_manager::set_state(
    _In_ xbox_live_api xboxLiveApi,
    _In_ const http_retry_after_api_state& state
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get()); // STL is not safe for multithreaded writes
    m_apiStateMap[static_cast<uint32_t>(xboxLiveApi)] = state;
}

void http_retry_after_manager::clear_state(
    _In_ xbox_live_api xboxLiveApi
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get()); // STL is not safe for multithreaded writes
    m_apiStateMap.erase(static_cast<uint32_t>(xboxLiveApi));
}

http_retry_after_api_state http_retry_after_manager::get_state(
    _In_ xbox_live_api xboxLiveApi
    )
{
    auto it = m_apiStateMap.find(static_cast<uint32_t>(xboxLiveApi)); // STL is multithread read safe
    if (it != m_apiStateMap.end())
    {
        return it->second; // returning a copy of state struct
    }

    return http_retry_after_api_state();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
