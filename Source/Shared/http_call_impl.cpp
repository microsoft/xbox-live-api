// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "http_call_impl.h"
#include "utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "build_version.h"
#include "xsapi/system.h"
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
    return get_response(httpCallResponseBodyType, get_default_request());
}

pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ const web::http::http_request& httpRequest
    )
{
    m_httpCallData->httpCallResponseBodyType = httpCallResponseBodyType;
    m_httpCallData->request = httpRequest;
    m_httpCallData->taskGroupId = XSAPI_DEFAULT_TASKGROUP;

    pplx::task_completion_event<std::shared_ptr<http_call_response>> tce;
    pplx::task<std::shared_ptr<http_call_response>> task(tce);

    xbox_live_callback<std::shared_ptr<http_call_response>> callback([tce](std::shared_ptr<http_call_response> response)
    {
        tce.set(response);
    });

    m_httpCallData->callback = callback;

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
        XSAPI_DEFAULT_TASKGROUP,
        [tce](std::shared_ptr<http_call_response> response) { tce.set(response); }
        );

    return pplx::task<std::shared_ptr<http_call_response>>(tce);
}

void http_call_impl::get_response_with_auth(
    _In_ const std::shared_ptr<xbox::services::user_context>& userContext,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired,
    _In_ uint64_t taskGroupId,
    _In_ xbox_live_callback<std::shared_ptr<http_call_response>> callback
    )
{
    m_httpCallData->userContext = userContext;
    m_httpCallData->httpCallResponseBodyType = httpCallResponseBodyType;
    m_httpCallData->request = get_default_request();
    m_httpCallData->taskGroupId = taskGroupId;
    m_httpCallData->callback = callback;

#if !TV_API && !XSAPI_SERVER
#if XSAPI_CPP
    if (!m_httpCallData->userContext->user() || !m_httpCallData->userContext->user()->is_signed_in())
#else
    if (!userContext->user() || !userContext->user()->IsSignedIn)
#endif
    {
        auto httpCallResponse = get_http_call_response(m_httpCallData, http_response());
        handle_response_error(httpCallResponse, xbox_live_error_code::auth_user_not_signed_in, "User must be signed in to call this API", http_response());
        callback(httpCallResponse);
        return;
}
#endif

    internal_get_response_with_auth(allUsersAuthRequired);
}

#if XSAPI_U
pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::get_response_with_auth(
    _In_ http_call_response_body_type httpCallResponseBodyType
    )
{
    // TODO: we need to rework this whole flow
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

void http_call_impl::internal_get_response_with_auth(
    _In_ bool allUsersAuthRequired
    )
{
    // TODO here and everywhere replace web::details::uri class
    string_t requestUri = m_httpCallData->request.request_uri().to_string();
    xsapi_internal_string internalRequestUri(requestUri.begin(), requestUri.end());

    xsapi_internal_string fullUrl = m_httpCallData->serverName + internalRequestUri;

    xsapi_internal_vector<unsigned char> requestBodyVector;
    if (m_httpCallData->requestBody.get_http_request_message_type() == http_request_message_type::vector_message)
    {
        requestBodyVector = m_httpCallData->requestBody.m_requestMessageVector;
    }
    else
    {
        requestBodyVector = xsapi_internal_vector<unsigned char>(
            m_httpCallData->requestBody.m_requestMessageString.begin(), 
            m_httpCallData->requestBody.m_requestMessageString.end()
            );
    }

    // Auth stack still using non mem hooked types. Not changing because we will switch to XAL soon anyhow
    string_t tempHttpMethod(m_httpCallData->httpMethod.begin(), m_httpCallData->httpMethod.end());
    string_t tempFullUrl(fullUrl.begin(), fullUrl.end());
    std::vector<unsigned char> tempRequestBodyVector(requestBodyVector.begin(), requestBodyVector.end());

    auto httpCallData = m_httpCallData;

    m_httpCallData->userContext->get_auth_result(
        tempHttpMethod,
        tempFullUrl,
        utils::headers_to_string(m_httpCallData->request.headers()),
        tempRequestBodyVector,
        allUsersAuthRequired,
        m_httpCallData->taskGroupId,
        [httpCallData](_In_ xbox::services::xbox_live_result<user_context_auth_result> result)
        {
            if (result.err())
            {
                auto httpCallResponse = get_http_call_response(httpCallData, http_response());
                handle_response_error(httpCallResponse, static_cast<xbox_live_error_code>(result.err().value()), result.err_message(), http_response());
                httpCallResponse->_Route_service_call();
                httpCallData->callback(httpCallResponse);
                return;
            }

            const auto& authResult = result.payload();
            if (!authResult.token().empty())
            {
                httpCallData->request.headers().add(AUTH_HEADER, authResult.token());
            }

            if (!authResult.signature().empty())
            {
                httpCallData->request.headers().add(SIG_HEADER, authResult.signature());
            }

            internal_get_response(httpCallData);
        });
}

void http_call_impl::internal_get_response(
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    httpCallData->requestStartTime = chrono_clock_t::now();
    http_client_config config = get_config(httpCallData);
    set_user_agent(httpCallData);

    HC_CALL_HANDLE call = nullptr;
    HCHttpCallCreate(&call);

    xsapi_internal_string path = utils::internal_string_from_external_string(httpCallData->pathQueryFragment.to_string());
    xsapi_internal_string uri = httpCallData->serverName + path;
    xsapi_internal_string method = utils::internal_string_from_external_string(httpCallData->request.method());

    HCHttpCallRequestSetUrl(call, method.c_str(), uri.c_str());

    if (httpCallData->requestBody.get_http_request_message_type() != empty_message)
    {
        pplx::task<utility::string_t> taskBody = httpCallData->request.extract_string(); // TODO remove this needless casablanca dependency
        xsapi_internal_string requestBody = utils::internal_string_from_external_string(taskBody.get());
        HCHttpCallRequestSetRequestBodyString(call, requestBody.c_str());
    }
    for (auto& header : httpCallData->request.headers())
    {
        std::string headerName = utility::conversions::to_utf8string(header.first);
        std::string headerValue = utility::conversions::to_utf8string(header.second);
        HCHttpCallRequestSetHeader(call, headerName.c_str(), headerValue.c_str());
    }
    HCHttpCallRequestSetRetryAllowed(call, httpCallData->retryAllowed);
    HCHttpCallRequestSetTimeout(call, static_cast<uint32_t>(httpCallData->httpTimeout.count()));

    auto context = async_helpers::store_shared_ptr(httpCallData);

    HC_TASK_HANDLE taskHandle;
    HCHttpCallPerform(call, &taskHandle, HC_SUBSYSTEM_ID_XSAPI, httpCallData->taskGroupId, context,
        [](_In_ void* context, _In_ HC_CALL_HANDLE call)
    {
        auto httpCallData = async_helpers::remove_shared_ptr<http_call_data>(context);
        auto httpCallResponse = get_http_call_response(httpCallData, http_response());

        HC_RESULT errorCode = HC_OK;
        uint32_t platformErrorCode = 0;
        uint32_t statusCode = 0;
        PCSTR responseBody = nullptr;
        HCHttpCallResponseGetNetworkErrorCode(call, &errorCode, &platformErrorCode);
        HCHttpCallResponseGetStatusCode(call, &statusCode);
        HCHttpCallResponseGetResponseString(call, &responseBody);

        if (errorCode == 0)
        {
#pragma warning(suppress: 4244)
            httpCallResponse->_Set_error_info(std::make_error_code(get_xbox_live_error_code_from_http_status(statusCode)), std::string());
        }
        else
        {
            httpCallResponse->_Set_error_info(std::make_error_code(static_cast<xbox_live_error_code>(errorCode)), std::string());
        }

        switch (httpCallData->httpCallResponseBodyType)
        {
        case http_call_response_body_type::json_body:
            std::error_code errC;
            string_t utf16responseBody = utility::conversions::to_utf16string(responseBody);
            web::json::value responseBodyJson = web::json::value::parse(utf16responseBody, errC);
            if (!errC)
            {
                httpCallResponse->_Set_response_body(responseBodyJson);
            }
            break;
        }
        HCHttpCallCleanup(call);

        chrono_clock_t::time_point responseReceivedTime = chrono_clock_t::now();
        httpCallResponse->_Set_timing(httpCallData->requestStartTime, responseReceivedTime);

        // Call the callback function
        httpCallData->callback(httpCallResponse);
    });

    return;
}

web::http::http_request
http_call_impl::get_default_request()
{
    http_request request(utils::external_string_from_internal_string(m_httpCallData->httpMethod.c_str()));
    request.set_request_uri(m_httpCallData->pathQueryFragment);
    if (add_default_headers())
    {
        request.headers().add(_T("x-xbl-contract-version"), utils::external_string_from_internal_string(m_httpCallData->xboxContractVersionHeaderValue));
        request.headers().add(_T("Content-Type"), utils::external_string_from_internal_string(m_httpCallData->contentTypeHeaderValue));
        request.headers().add(_T("Accept-Language"), utils::get_locales());
    }

    for (auto& customHeader : m_httpCallData->customHeaderMap)
    {
        request.headers()[utils::external_string_from_internal_string(customHeader.first)] = utils::external_string_from_internal_string(customHeader.second);
    }

    switch (m_httpCallData->requestBody.get_http_request_message_type())
    {
        case http_request_message_type::string_message:
            request.set_body(m_httpCallData->requestBody.request_message_string());
            break;

        case http_request_message_type::vector_message:
            request.set_body(m_httpCallData->requestBody.request_message_vector());
            break;
    }

    return request;
}

string_t http_call_impl::server_name() const
{
    return utils::external_string_from_internal_string(m_httpCallData->serverName);
}

const web::uri& http_call_impl::path_query_fragment() const
{
    return m_httpCallData->pathQueryFragment;
}

string_t http_call_impl::http_method() const
{
    return utils::external_string_from_internal_string(m_httpCallData->httpMethod);
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

const http_call_request_message& http_call_impl::request_body() const
{
    return m_httpCallData->requestBody;
}

void http_call_impl::set_request_body(
    _In_ const string_t& value
    )
{
    m_httpCallData->requestBody = http_call_request_message(value);
}

void http_call_impl::set_request_body(
    _In_ const std::vector<uint8_t>& value
    )
{
    m_httpCallData->requestBody = http_call_request_message(value);
}

void http_call_impl::set_request_body(
    _In_ const web::json::value& value
    )
{
    m_httpCallData->requestBody = http_call_request_message(value.serialize());
}

string_t http_call_impl::content_type_header_value() const
{
    return utils::external_string_from_internal_string(m_httpCallData->contentTypeHeaderValue);
}

void http_call_impl::set_content_type_header_value(
    _In_ const string_t& value
    )
{
    m_httpCallData->contentTypeHeaderValue = utils::internal_string_from_external_string(value);
}

string_t http_call_impl::xbox_contract_version_header_value() const
{
    return utils::external_string_from_internal_string(m_httpCallData->xboxContractVersionHeaderValue);
}

void http_call_impl::set_xbox_contract_version_header_value(
    _In_ const string_t& value
    )
{
    m_httpCallData->xboxContractVersionHeaderValue = utils::internal_string_from_external_string(value);
}

void http_call_impl::set_custom_header(
    _In_ const string_t& headerName,
    _In_ const string_t& headerValue
    )
{
    m_httpCallData->customHeaderMap[utils::internal_string_from_external_string(headerName)] = 
        utils::internal_string_from_external_string(headerValue);
}

void
http_call_impl::handle_response_error(
    _In_ const std::shared_ptr<http_call_response>& httpCallResponse,
    _In_ xbox_live_error_code errFromException,
    _In_ const std::string& errMessageFromException,
    _In_ const http_response& response
    )
{
    xbox_live_error_code errFromStatus = get_xbox_live_error_code_from_http_status(response.status_code());
    std::error_code errCode;
    std::string errMessage;
    if (errFromStatus == xbox_live_error_code::no_error)
    {
        errCode = std::make_error_code(errFromException);
        errMessage = errMessageFromException;
    }
    else
    {
        errCode = std::make_error_code(errFromStatus);
        stringstream_t errorMessageHttp;
        errorMessageHttp << _T("http error: ") << errCode.message().c_str();
        errMessage = utility::conversions::to_utf8string(errorMessageHttp.str().c_str());
    }

    // Try to pull out error message from HTTP response
    try
    {
        if (response.body().is_valid())
        {
            string_t debugString = response.extract_string().get();
            if (!debugString.empty())
            {
                std::string debugStringUtf8 = utility::conversions::to_utf8string(debugString);
                errMessage += " HTTP Response Body: ";
                errMessage += debugStringUtf8;
            }
        }
    }
    catch (...)
    {
    }

    httpCallResponse->_Set_error_info(errCode, errMessage);
}

xbox_live_error_code http_call_impl::get_xbox_live_error_code_from_http_status(
    _In_ const web::http::status_code& statusCode
    )
{
    if (statusCode < 300 || statusCode >= 600)
    {
        // Treat as success so 
        //      if (!result.err()) 
        // works properly which requires all non-errors to be 0.
        return xbox_live_error_code::no_error;
    }
    else
    {
        return static_cast<xbox_live_error_code>(statusCode);
    }
}

bool
http_call_impl::should_retry(
    _In_ const std::shared_ptr<http_call_response>& httpCallResponse,
    _In_ const std::shared_ptr<http_call_data>& httpCallData,
    _In_ xbox_live_error_code httpNetworkError
    )
{
    auto httpStatus = httpCallResponse->http_status();

    if (!httpCallData->retryAllowed
        && !(httpStatus == web::http::status_codes::Unauthorized && httpCallData->userContext != nullptr))
    {
        return false;
    }

    if ((httpStatus == web::http::status_codes::Unauthorized && !httpCallData->hasPerformedRetryOn401) ||
        httpStatus == web::http::status_codes::RequestTimeout ||
        httpStatus == static_cast<int>(xbox_live_error_code::http_status_429_too_many_requests) ||
        httpStatus == web::http::status_codes::InternalError ||
        httpStatus == web::http::status_codes::BadGateway ||
        httpStatus == web::http::status_codes::ServiceUnavailable ||
        httpStatus == web::http::status_codes::GatewayTimeout ||
        httpNetworkError != xbox_live_error_code::no_error
        )
    {
        std::chrono::milliseconds retryAfter = httpCallResponse->retry_after();

        // Compute how much time left before hitting the http_timeout_window setting.  
        const chrono_clock_t::time_point& responseReceivedTime = httpCallResponse->_Local_response_time();
        std::chrono::milliseconds timeElapsedSinceFirstCall = std::chrono::duration_cast<std::chrono::milliseconds>(responseReceivedTime - httpCallData->firstCallStartTime);
        std::chrono::milliseconds remainingTimeBeforeTimeout = httpCallData->xboxLiveContextSettings->http_timeout_window() - timeElapsedSinceFirstCall;
        if (remainingTimeBeforeTimeout.count() <= MIN_HTTP_TIMEOUT_MILLISECONDS) // Need at least 5 seconds to bother making a call
        {
            return false;
        }

        // Based on the retry iteration, delay 2,4,8,16,etc seconds by default between retries
        // Jitter the response between the current and next delay based on system clock
        // Max wait time is 1 minute
        double secondsToWaitMin = std::pow(httpCallData->xboxLiveContextSettings->http_retry_delay().count(), httpCallData->iterationNumber);
        double secondsToWaitMax = std::pow(httpCallData->xboxLiveContextSettings->http_retry_delay().count(), httpCallData->iterationNumber+1);
        double secondsToWaitDelta = secondsToWaitMax - secondsToWaitMin;
        const chrono_clock_t::time_point& responseDate = httpCallResponse->_Local_response_time();
        double lerpScaler = (responseDate.time_since_epoch().count() % 10000) / 10000.0; // from 0 to 1 based on clock
#if UNIT_TEST_SERVICES
        lerpScaler = 0; // make unit tests deterministic
#endif
        double secondsToWaitUncapped = secondsToWaitMin + secondsToWaitDelta * lerpScaler; // lerp between min & max wait
        double secondsToWait = __min(secondsToWaitUncapped, MAX_DELAY_TIME_IN_SEC); // cap max wait to 1 min
        std::chrono::milliseconds waitTime = std::chrono::milliseconds(static_cast<int64_t>(secondsToWait * 1000.0));
        if (retryAfter.count() > 0)
        {
            // Use either the waitTime or Retry-After header, whichever is bigger
            httpCallData->delayBeforeRetry = std::chrono::milliseconds(__max(waitTime.count(), retryAfter.count()));
        }
        else
        {
            httpCallData->delayBeforeRetry = waitTime;
        }

        if (remainingTimeBeforeTimeout < httpCallData->delayBeforeRetry + std::chrono::milliseconds(MIN_HTTP_TIMEOUT_MILLISECONDS))
        {
            // Don't bother retrying when out of time
            return false;
        }

        if (!httpCallData->request._reset_body_for_retry())
        {
            // Don't bother if we can't retry the request
            return false;
        }

        if (httpStatus == web::http::status_codes::InternalError)
        {
            // For 500 - Internal Error, wait at least 10 seconds before retrying.
            if (httpCallData->delayBeforeRetry.count() < MIN_DELAY_FOR_HTTP_INTERNAL_ERROR_IN_SEC * 1000)
            {
                httpCallData->delayBeforeRetry = std::chrono::seconds(MIN_DELAY_FOR_HTTP_INTERNAL_ERROR_IN_SEC);
            }
        }
        else if (httpStatus == web::http::status_codes::Unauthorized)
        {
            return handle_unauthorized_error(httpCallData);
        }

        return true;
    }

    return false;
}

std::shared_ptr<http_call_response> 
http_call_impl::get_http_call_response(
    _In_ const std::shared_ptr<http_call_data>& httpCallData,
    _In_ const http_response& response
    )
{
    return std::make_shared<http_call_response>(
        httpCallData->userContext != nullptr ? httpCallData->userContext->xbox_user_id() : string_t(),
        httpCallData->xboxLiveContextSettings,
        utils::external_string_from_internal_string(httpCallData->serverName) + httpCallData->pathQueryFragment.to_string(),
        httpCallData->request,
        httpCallData->requestBody,
        httpCallData->xboxLiveApi,
        response);
}

pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::handle_json_body_response(
    _In_ http_response httpResponse,
    _In_ std::shared_ptr<http_call_response> httpCallResponse
    )
{
    // If the Content-Type header is missing, then assume it is application/json so that extract_json() succeeds
    if (httpResponse.headers().find(_T("Content-Type")) == httpResponse.headers().end())
    {
        httpResponse.headers().add(_T("Content-Type"), _T("application/json"));
    }

    return httpResponse.extract_json()
    .then([httpResponse, httpCallResponse](pplx::task<web::json::value> jsonTask)
    {
        try
        {
            httpCallResponse->_Set_response_body(jsonTask.get());

            if (httpCallResponse->http_status() == static_cast<int>(xbox_live_error_code::http_status_429_too_many_requests))
            {
                std::shared_ptr<xbox_live_app_config> appConfig = xbox::services::xbox_live_app_config::get_app_config_singleton();
                if (utils::str_icmp(appConfig->sandbox(), _T("RETAIL")) != 0)
                {
                    bool disableAsserts = httpCallResponse->_Context_settings()->_Is_disable_asserts_for_xbox_live_throttling_in_dev_sandboxes();
                    if (!disableAsserts)
                    {
#if XSAPI_U
                        string_t utf16Error = httpCallResponse->err_message();
#else
                        string_t utf16Error = utility::conversions::utf8_to_utf16(httpCallResponse->err_message());
#endif
                        std::stringstream msg;
                        LOGS_ERROR << "Xbox Live service call to " << httpCallResponse->_Request().request_uri().to_string() << " was throttled";
                        LOGS_ERROR << utf16Error;
                        LOG_ERROR("You can temporarily disable the assert by calling");
                        LOG_ERROR("xboxLiveContext->settings()->disable_asserts_for_xbox_live_throttling_in_dev_sandboxes()");
                        LOG_ERROR("Note that this will only disable this assert.  You will still be throttled in all sandboxes.");

                        XSAPI_ASSERT(false && "Xbox Live service call was throttled.  See Output for more detail");
                    }
                }
            }
        }
        catch (const std::exception& ex)
        {
            handle_response_error(httpCallResponse, utils::convert_exception_to_xbox_live_error_code(), ex.what(), httpResponse);
        }

        httpCallResponse->_Route_service_call();
        return httpCallResponse;
    });
}

pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::handle_string_body_response(
    _In_ http_response httpResponse,
    _In_ std::shared_ptr<http_call_response> httpCallResponse
    )
{
    return httpResponse.extract_string()
    .then([httpResponse, httpCallResponse](pplx::task<utility::string_t> strTask)
    {
        try
        {
            httpCallResponse->_Set_response_body(strTask.get());
        }
        catch (const std::exception& ex)
        {
            handle_response_error(httpCallResponse, utils::convert_exception_to_xbox_live_error_code(), ex.what(), httpResponse);
        }

        httpCallResponse->_Route_service_call();
        return httpCallResponse;
    });
}

pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::handle_vector_body_response(
    _In_ http_response httpResponse,
    _In_ std::shared_ptr<http_call_response> httpCallResponse
    )
{
    return httpResponse.extract_vector()
    .then([httpResponse, httpCallResponse](pplx::task<std::vector<unsigned char>> vecTask)
    {
        try
        {
            httpCallResponse->_Set_response_body(vecTask.get());
        }
        catch (const std::exception& ex)
        {
            handle_response_error(httpCallResponse, utils::convert_exception_to_xbox_live_error_code(), ex.what(), httpResponse);
        }

        httpCallResponse->_Route_service_call();
        
        return httpCallResponse;
    });
}

http_client_config http_call_impl::get_config(
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    http_client_config config;
    config.set_timeout(httpCallData->httpTimeout);
    auto proxyUri = xbox_live_app_config::get_app_config_singleton()->_Proxy();
    if (!proxyUri.is_empty())
    {
        web::web_proxy proxy(proxyUri);
        config.set_proxy(proxy);
    }

    return config;
}

void http_call_impl::set_user_agent(
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    if (httpCallData->userContext != nullptr)
    {
        string_t userAgent = DEFAULT_USER_AGENT;
        if (!httpCallData->userContext->caller_context().empty())
        {
            userAgent += _T(" ") + httpCallData->userContext->caller_context();
        }
        httpCallData->request.headers().add(_T("User-Agent"), userAgent);
    }
}

bool http_call_impl::handle_unauthorized_error(
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    if (httpCallData->userContext != nullptr)    // if this is null, it does not need a valid token anyways
    {
        auto refreshResult = httpCallData->userContext->refresh_token().get();

        if (!refreshResult.err())
        {
            httpCallData->hasPerformedRetryOn401 = true;
            
        }
        else
        {
            return false;   // if getting a new token failed, then we need to just return the 401 upwards
        }
    }
    else
    {
        httpCallData->hasPerformedRetryOn401 = true;
    }

    return true;
}

bool http_call_impl::should_fast_fail(
    _In_ const http_retry_after_api_state& apiState,
    _In_ const std::shared_ptr<http_call_data>& httpCallData,
    _In_ const chrono_clock_t::time_point& currentTime
    )
{
    if (!apiState.errCode)
    {
        return false;
    }

    std::chrono::milliseconds remainingTimeBeforeRetryAfter = std::chrono::duration_cast<std::chrono::milliseconds>(apiState.retryAfterTime - currentTime);
    if (remainingTimeBeforeRetryAfter.count() <= 0)
    {
        return false;
    }

    chrono_clock_t::time_point timeoutTime = httpCallData->firstCallStartTime + httpCallData->xboxLiveContextSettings->http_timeout_window();

    // If the Retry-After will happen first, just wait till Retry-After is done, and don't fast fail
    if (apiState.retryAfterTime < timeoutTime)
    {
        auto retryAfterCount = static_cast<uint32_t>(remainingTimeBeforeRetryAfter.count());
        utils::sleep(retryAfterCount);
        return false;
    }
    else
    {
        return true;
    }
}

pplx::task<std::shared_ptr<http_call_response>>
http_call_impl::handle_fast_fail(
    _In_ const http_retry_after_api_state& apiState,
    _In_ const std::shared_ptr<http_call_data>& httpCallData
    )
{
    auto httpCallResponse = get_http_call_response(httpCallData, http_response());

    httpCallResponse->_Set_error_info(apiState.errCode, apiState.errMessage);
    httpCallResponse->_Route_service_call();
    return pplx::task_from_result<std::shared_ptr<http_call_response>>(httpCallResponse);
}

void http_call_impl::set_http_timeout(
    _In_ const std::shared_ptr<http_call_data>& httpCallData,
    _In_ const chrono_clock_t::time_point& currentTime
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
        std::chrono::milliseconds timeElapsedSinceFirstCall = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - httpCallData->firstCallStartTime);
        std::chrono::seconds remainingTimeBeforeTimeout = std::chrono::duration_cast<std::chrono::seconds>(httpCallData->xboxLiveContextSettings->http_timeout_window() - timeElapsedSinceFirstCall);
        uint64_t secondsLeft = __min(DEFAULT_HTTP_TIMEOUT_SECONDS, remainingTimeBeforeTimeout.count());
        uint64_t secondsLeftCapped = __max(MIN_HTTP_TIMEOUT_SECONDS, secondsLeft);
        httpCallData->httpTimeout = std::chrono::seconds(secondsLeftCapped);
    }
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
