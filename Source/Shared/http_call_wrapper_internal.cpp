// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_live_context_internal.h"
#include <httpClient/httpProvider.h>

using namespace xbox::services;

#define DEFAULT_USER_AGENT      "XboxServicesAPI/" XBOX_SERVICES_API_VERSION_STRING

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

HttpCall::~HttpCall()
{
    if (m_callHandle)
    {
        HCHttpCallCloseHandle(m_callHandle);
    }
}

HRESULT HttpCall::Init(
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& fullUrl
)
{
    assert(m_step == Step::Uninitialized);
    RETURN_HR_IF_FAILED( HCHttpCallCreate(&m_callHandle) );
    m_step = Step::Pending;

    RETURN_HR_IF_FAILED( HCHttpCallRequestSetUrl(m_callHandle, httpMethod.data(), fullUrl.data()) );

    // Add default User-Agent header
    xsapi_internal_string userAgent = DEFAULT_USER_AGENT;
    return SetHeader(USER_AGENT_HEADER, userAgent);
}

HRESULT HttpCall::SetHeader(
    _In_ const xsapi_internal_string& key,
    _In_ const xsapi_internal_string& value,
    _In_ bool allowTracing
)
{
    assert(m_step == Step::Pending);
    return HCHttpCallRequestSetHeader(m_callHandle, key.data(), value.data(), allowTracing);
}

HRESULT HttpCall::SetRequestBody(const xsapi_internal_vector<uint8_t>& bytes)
{
    assert(m_step == Step::Pending);
    return HCHttpCallRequestSetRequestBodyBytes(m_callHandle, bytes.data(), static_cast<uint32_t>(bytes.size()));
}

HRESULT HttpCall::SetRequestBody(const xsapi_internal_string& bodyString)
{
    assert(m_step == Step::Pending);
    return HCHttpCallRequestSetRequestBodyString(m_callHandle, bodyString.data());
}

HRESULT HttpCall::SetRequestBody(const JsonValue& bodyJson)
{
    assert(m_step == Step::Pending);
    return SetRequestBody(JsonUtils::SerializeJson(bodyJson));
}

HRESULT HttpCall::SetRetryAllowed(bool retryAllowed)
{
    assert(m_step == Step::Pending);
    return HCHttpCallRequestSetRetryAllowed(m_callHandle, retryAllowed);
}

HRESULT HttpCall::SetRetryCacheId(uint32_t retryAfterCacheId)
{
    assert(m_step == Step::Pending);
    return HCHttpCallRequestSetRetryCacheId(m_callHandle, retryAfterCacheId);
}

HRESULT HttpCall::SetRetryDelay(uint32_t retryDelayInSeconds)
{
    assert(m_step == Step::Pending);
    return HCHttpCallRequestSetRetryDelay(m_callHandle, retryDelayInSeconds);
}

HRESULT HttpCall::SetTimeout(uint32_t timeoutInSeconds)
{
    assert(m_step == Step::Pending);
    return HCHttpCallRequestSetTimeout(m_callHandle, timeoutInSeconds);
}

HRESULT HttpCall::SetTimeoutWindow(uint32_t timeoutWindowInSeconds)
{
    assert(m_step == Step::Pending);
    return HCHttpCallRequestSetTimeoutWindow(m_callHandle, timeoutWindowInSeconds);
}

HRESULT HttpCall::Perform(
    AsyncContext<HttpResult> async,
    bool forceRefresh
)
{
    UNREFERENCED_PARAMETER(forceRefresh);
    assert(m_step == Step::Pending);

    m_asyncContext = std::move(async);
    m_asyncBlock.queue = m_asyncContext.Queue().GetHandle();
    m_asyncBlock.context = this;
    m_asyncBlock.callback = &HttpCall::CompletionCallback;

    HRESULT hr = S_OK;
    if (m_performAlreadyCalled)
    {
        hr = CopyHttpCallHandle();
    }

    if (SUCCEEDED(hr))
    {
        m_performAlreadyCalled = true;
        m_step = Step::Running;

        if (XblShouldFaultInject(INJECTION_FEATURE_HTTP))
        {
            LOGS_ERROR << "FAULT INJECTION: HttpCall::Perform ID:" << XblGetFaultCounter();
            hr = E_FAIL;
        }
        else
        {
            hr = HCHttpCallPerformAsync(m_callHandle, &m_asyncBlock);
        }

        if (SUCCEEDED(hr))
        {
            AddRef(); // Keep HttpCall object alive until call completes
        }
        else
        {
            m_step = Step::Done;
        }
    }
    return hr;
}

struct RAIIHttpCallHandle
{
public:
    ~RAIIHttpCallHandle() { if(h) HCHttpCallCloseHandle(h); }
    HCCallHandle h{ nullptr };
};

HRESULT HttpCall::CopyHttpCallHandle()
{
    RAIIHttpCallHandle newCallHandle;
    RETURN_HR_IF_FAILED(HCHttpCallCreate(&newCallHandle.h));

    const char* method{ nullptr };
    const char* url{ nullptr };
    RETURN_HR_IF_FAILED(HCHttpCallRequestGetUrl(m_callHandle, &method, &url));
    RETURN_HR_IF_FAILED(HCHttpCallRequestSetUrl(newCallHandle.h, method, url));

    bool retryAllowed{ false };
    RETURN_HR_IF_FAILED(HCHttpCallRequestGetRetryAllowed(m_callHandle, &retryAllowed));
    RETURN_HR_IF_FAILED(HCHttpCallRequestSetRetryAllowed(newCallHandle.h, retryAllowed));

    uint32_t retryAfterCacheId{ 0 };
    RETURN_HR_IF_FAILED(HCHttpCallRequestGetRetryCacheId(m_callHandle, &retryAfterCacheId));
    RETURN_HR_IF_FAILED(HCHttpCallRequestSetRetryCacheId(newCallHandle.h, retryAfterCacheId));

    uint32_t retryDelayInSeconds{ 0 };
    RETURN_HR_IF_FAILED(HCHttpCallRequestGetRetryDelay(m_callHandle, &retryDelayInSeconds));
    RETURN_HR_IF_FAILED(HCHttpCallRequestSetRetryDelay(newCallHandle.h, retryDelayInSeconds));

    uint32_t timeoutInSeconds{ 0 };
    RETURN_HR_IF_FAILED(HCHttpCallRequestGetTimeout(m_callHandle, &timeoutInSeconds));
    RETURN_HR_IF_FAILED(HCHttpCallRequestSetTimeout(newCallHandle.h, timeoutInSeconds));

    uint32_t timeoutWindowInSeconds{ 0 };
    RETURN_HR_IF_FAILED(HCHttpCallRequestGetTimeoutWindow(m_callHandle, &timeoutWindowInSeconds));
    RETURN_HR_IF_FAILED(HCHttpCallRequestSetTimeoutWindow(newCallHandle.h, timeoutWindowInSeconds));

    uint32_t headerCount{ 0 };
    RETURN_HR_IF_FAILED(HCHttpCallRequestGetNumHeaders(m_callHandle, &headerCount));

    for (uint32_t i = 0; i < headerCount; ++i)
    {
        const char* headerName{ nullptr };
        const char* headerValue{ nullptr };
        RETURN_HR_IF_FAILED(HCHttpCallRequestGetHeaderAtIndex(m_callHandle, i, &headerName, &headerValue));
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetHeader(newCallHandle.h, headerName, headerValue, false));
    }

    const uint8_t* requestBodyBytes{ nullptr };
    uint32_t requestBodySize{ 0 };
    RETURN_HR_IF_FAILED(HCHttpCallRequestGetRequestBodyBytes(m_callHandle, &requestBodyBytes, &requestBodySize));
    if (requestBodyBytes != nullptr && requestBodySize > 0)
    {
        RETURN_HR_IF_FAILED(HCHttpCallRequestSetRequestBodyBytes(newCallHandle.h, requestBodyBytes, requestBodySize));
    }

    HCHttpCallCloseHandle(m_callHandle);
    m_callHandle = HCHttpCallDuplicateHandle(newCallHandle.h);

    return S_OK;
}


HRESULT xbox::services::HttpCall::ResetAndCopyForRetry()
{
    HRESULT hr = CopyHttpCallHandle();
    if (SUCCEEDED(hr))
    {
        m_step = Step::Pending;
        m_performAlreadyCalled = false;
    }
    return hr;
}

HRESULT HttpCall::ConvertHttpStatusToHRESULT(_In_ uint32_t httpStatusCode)
{
    xbox::services::xbl_error_code errCode = static_cast<xbox::services::xbl_error_code>(httpStatusCode);
    HRESULT hr = HTTP_E_STATUS_UNEXPECTED;

    // 2xx are http success codes
    if ((httpStatusCode >= 200) && (httpStatusCode < 300))
    {
        hr = S_OK;
    }

    // MSXML XHR bug: get_status() returns HTTP/1223 for HTTP/204:
    // http://blogs.msdn.com/b/ieinternals/archive/2009/07/23/the-ie8-native-xmlhttprequest-object.aspx
    // treat it as success code as well
    else if (httpStatusCode == 1223)
    {
        hr = S_OK;
    }
    else
    {
        switch (errCode)
        {
        case xbl_error_code::http_status_300_multiple_choices: hr = HTTP_E_STATUS_AMBIGUOUS; break;
        case xbl_error_code::http_status_301_moved_permanently: hr = HTTP_E_STATUS_MOVED; break;
        case xbl_error_code::http_status_302_found: hr = HTTP_E_STATUS_REDIRECT; break;
        case xbl_error_code::http_status_303_see_other: hr = HTTP_E_STATUS_REDIRECT_METHOD; break;
        case xbl_error_code::http_status_304_not_modified: hr = HTTP_E_STATUS_NOT_MODIFIED; break;
        case xbl_error_code::http_status_305_use_proxy: hr = HTTP_E_STATUS_USE_PROXY; break;
        case xbl_error_code::http_status_307_temporary_redirect: hr = HTTP_E_STATUS_REDIRECT_KEEP_VERB; break;

        case xbl_error_code::http_status_400_bad_request: hr = HTTP_E_STATUS_BAD_REQUEST; break;
        case xbl_error_code::http_status_401_unauthorized: hr = HTTP_E_STATUS_DENIED; break;
        case xbl_error_code::http_status_402_payment_required: hr = HTTP_E_STATUS_PAYMENT_REQ; break;
        case xbl_error_code::http_status_403_forbidden: hr = HTTP_E_STATUS_FORBIDDEN; break;
        case xbl_error_code::http_status_404_not_found: hr = HTTP_E_STATUS_NOT_FOUND; break;
        case xbl_error_code::http_status_405_method_not_allowed: hr = HTTP_E_STATUS_BAD_METHOD; break;
        case xbl_error_code::http_status_406_not_acceptable: hr = HTTP_E_STATUS_NONE_ACCEPTABLE; break;
        case xbl_error_code::http_status_407_proxy_authentication_required: hr = HTTP_E_STATUS_PROXY_AUTH_REQ; break;
        case xbl_error_code::http_status_408_request_timeout: hr = HTTP_E_STATUS_REQUEST_TIMEOUT; break;
        case xbl_error_code::http_status_409_conflict: hr = HTTP_E_STATUS_CONFLICT; break;
        case xbl_error_code::http_status_410_gone: hr = HTTP_E_STATUS_GONE; break;
        case xbl_error_code::http_status_411_length_required: hr = HTTP_E_STATUS_LENGTH_REQUIRED; break;
        case xbl_error_code::http_status_412_precondition_failed: hr = HTTP_E_STATUS_PRECOND_FAILED; break;
        case xbl_error_code::http_status_413_request_entity_too_large: hr = HTTP_E_STATUS_REQUEST_TOO_LARGE; break;
        case xbl_error_code::http_status_414_request_uri_too_long: hr = HTTP_E_STATUS_URI_TOO_LONG; break;
        case xbl_error_code::http_status_415_unsupported_media_type: hr = HTTP_E_STATUS_UNSUPPORTED_MEDIA; break;
        case xbl_error_code::http_status_416_requested_range_not_satisfiable: hr = HTTP_E_STATUS_RANGE_NOT_SATISFIABLE; break;
        case xbl_error_code::http_status_417_expectation_failed: hr = HTTP_E_STATUS_EXPECTATION_FAILED; break;
        case xbl_error_code::http_status_421_misdirected_request: hr = MAKE_HTTP_HRESULT(421); break;
        case xbl_error_code::http_status_422_unprocessable_entity: hr = MAKE_HTTP_HRESULT(422); break;
        case xbl_error_code::http_status_423_locked: hr = MAKE_HTTP_HRESULT(423); break;
        case xbl_error_code::http_status_424_failed_dependency: hr = MAKE_HTTP_HRESULT(424); break;
        case xbl_error_code::http_status_426_upgrade_required: hr = MAKE_HTTP_HRESULT(426); break;
        case xbl_error_code::http_status_428_precondition_required: hr = MAKE_HTTP_HRESULT(428); break;
        case xbl_error_code::http_status_429_too_many_requests: hr = MAKE_HTTP_HRESULT(429); break;
        case xbl_error_code::http_status_431_request_header_fields_too_large: hr = MAKE_HTTP_HRESULT(431); break;
        case xbl_error_code::http_status_449_retry_with:hr = MAKE_HTTP_HRESULT(449); break;
        case xbl_error_code::http_status_451_unavailable_for_legal_reasons: hr = MAKE_HTTP_HRESULT(451); break;

        case xbl_error_code::http_status_500_internal_server_error: hr = HTTP_E_STATUS_SERVER_ERROR; break;
        case xbl_error_code::http_status_501_not_implemented: hr = HTTP_E_STATUS_NOT_SUPPORTED; break;
        case xbl_error_code::http_status_502_bad_gateway: hr = HTTP_E_STATUS_BAD_GATEWAY; break;
        case xbl_error_code::http_status_503_service_unavailable: hr = HTTP_E_STATUS_SERVICE_UNAVAIL; break;
        case xbl_error_code::http_status_504_gateway_timeout: hr = HTTP_E_STATUS_GATEWAY_TIMEOUT; break;
        case xbl_error_code::http_status_505_http_version_not_supported: hr = HTTP_E_STATUS_VERSION_NOT_SUP; break;
        case xbl_error_code::http_status_506_variant_also_negotiates: hr = MAKE_HTTP_HRESULT(506); break;
        case xbl_error_code::http_status_507_insufficient_storage: hr = MAKE_HTTP_HRESULT(507); break;
        case xbl_error_code::http_status_508_loop_detected: hr = MAKE_HTTP_HRESULT(508); break;
        case xbl_error_code::http_status_510_not_extended: hr = MAKE_HTTP_HRESULT(510); break;
        case xbl_error_code::http_status_511_network_authentication_required: hr = MAKE_HTTP_HRESULT(511); break;

        default:
            hr = HTTP_E_STATUS_UNEXPECTED;
            break;
        }
    }

    return hr;
}

HRESULT HttpCall::Result() const
{
    HRESULT hrNetworkError{ S_OK };
    uint32_t platformNetworkResult{ 0 };
  
    RETURN_HR_IF_FAILED(HCHttpCallResponseGetNetworkErrorCode(m_callHandle, &hrNetworkError, &platformNetworkResult));

    if (XblShouldFaultInject(INJECTION_FEATURE_HTTP))
    {
        LOGS_ERROR << "FAULT INJECTION: HttpCall::Result" << XblGetFaultCounter();
        hrNetworkError = E_FAIL;
    }

    if (SUCCEEDED(hrNetworkError))
    {
        HRESULT hrHttpStatus = ConvertHttpStatusToHRESULT(HttpStatus());
        return hrHttpStatus;
    }
    else
    {
        LOGS_ERROR << "HttpCall failed due to network error " << platformNetworkResult;
        return hrNetworkError;
    }
}

HRESULT HttpCall::GetErrorMessage(const char** errorMessage) const
{
    return HCHttpCallResponseGetPlatformNetworkErrorMessage(m_callHandle, errorMessage);
}

uint32_t HttpCall::HttpStatus() const
{
    assert(m_step == Step::Done);

    uint32_t status{ 0 };
    auto hr = HCHttpCallResponseGetStatusCode(m_callHandle, &status);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    return status;
}

xsapi_internal_vector<uint8_t> HttpCall::GetResponseBodyBytes() const
{
    assert(m_step == Step::Done);

    size_t bodySize{ 0 };
    xsapi_internal_vector<uint8_t> body;

    HRESULT hr = HCHttpCallResponseGetResponseBodyBytesSize(m_callHandle, &bodySize);
    if (SUCCEEDED(hr))
    {
        body.resize(bodySize);
        hr = HCHttpCallResponseGetResponseBodyBytes(m_callHandle, body.size(), body.data(), nullptr);
        assert(SUCCEEDED(hr));
    }
    return body;
}

xsapi_internal_string HttpCall::GetResponseBodyString() const
{
    assert(m_step == Step::Done);

    const char* bodyString{ nullptr };
    auto hr = HCHttpCallResponseGetResponseString(m_callHandle, &bodyString);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    return bodyString != nullptr ? xsapi_internal_string{ bodyString } : xsapi_internal_string();
}

//TODO: consider keeping a JsonDocument member instead of parsing every time
JsonDocument HttpCall::GetResponseBodyJson() const
{
    assert(m_step == Step::Done);

    const char* bodyString{ nullptr };
    auto hr = HCHttpCallResponseGetResponseString(m_callHandle, &bodyString);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    JsonDocument json;
    if (bodyString != nullptr)
    {
        json.Parse(bodyString);
        if (!json.HasParseError())
        {
            return json;
        }
    }
    return JsonDocument(rapidjson::kNullType);
}

xsapi_internal_string HttpCall::GetResponseHeader(const xsapi_internal_string& key) const
{
    assert(m_step == Step::Done);

    const char* headerValue{ nullptr };
    auto hr = HCHttpCallResponseGetHeader(m_callHandle, key.data(), &headerValue);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    return headerValue ? xsapi_internal_string{ headerValue } : xsapi_internal_string{};
}

HRESULT HttpCall::SetRequestBody(
    _In_reads_bytes_(requestBodySize) const uint8_t* requestBodyBytes,
    _In_ uint32_t requestBodySize
)
{
    return HCHttpCallRequestSetRequestBodyBytes(m_callHandle, requestBodyBytes, requestBodySize);
}

HRESULT HttpCall::SetRequestBody(
    _In_z_ const char* requestBodyString
)
{
    return HCHttpCallRequestSetRequestBodyString(m_callHandle, requestBodyString);
}

HRESULT HttpCall::GetResponseString(
    _Out_ const char** responseString
)
{
    return HCHttpCallResponseGetResponseString(m_callHandle, responseString);
}

HRESULT HttpCall::GetResponseBodyBytesSize(
    _Out_ size_t* bufferSize
)
{
    return HCHttpCallResponseGetResponseBodyBytesSize(m_callHandle, bufferSize);
}

HRESULT HttpCall::GetResponseBodyBytes(
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) uint8_t* buffer,
    _Out_opt_ size_t* bufferUsed
)
{
    return HCHttpCallResponseGetResponseBodyBytes(m_callHandle, bufferSize, buffer, bufferUsed);
}

HRESULT HttpCall::GetNetworkErrorCode(
    _Out_ HRESULT* networkErrorCode,
    _Out_ uint32_t* platformNetworkErrorCode
)
{
    return HCHttpCallResponseGetNetworkErrorCode(m_callHandle, networkErrorCode, platformNetworkErrorCode);
}

HRESULT HttpCall::GetPlatformNetworkErrorMessage(
    _Out_ const char** platformNetworkErrorMessage
)
{
    return HCHttpCallResponseGetPlatformNetworkErrorMessage(m_callHandle, platformNetworkErrorMessage);
}

HRESULT HttpCall::ResponseGetHeader(
    _In_z_ const char* headerName,
    _Out_ const char** headerValue
)
{
    return HCHttpCallResponseGetHeader(m_callHandle, headerName, headerValue);
}

HRESULT HttpCall::ResponseGetNumHeaders(
    _Out_ uint32_t* numHeaders
)
{
    return HCHttpCallResponseGetNumHeaders(m_callHandle, numHeaders);
}

HRESULT HttpCall::ResponseGetHeaderAtIndex(
    _In_ uint32_t headerIndex,
    _Out_ const char** headerName,
    _Out_ const char** headerValue
)
{
    return HCHttpCallResponseGetHeaderAtIndex(m_callHandle, headerIndex, headerName, headerValue);
}

HRESULT HttpCall::SetTracing(bool traceCall)
{
    return HCHttpCallSetTracing(m_callHandle, traceCall);
}

HRESULT HttpCall::GetRequestUrl(const char** url) const
{
    return HCHttpCallGetRequestUrl(m_callHandle, url);
}

HttpHeaders HttpCall::GetResponseHeaders() const
{
    assert(m_step == Step::Done);

    uint32_t headerCount{ 0 };
    auto hr = HCHttpCallResponseGetNumHeaders(m_callHandle, &headerCount);
    assert(SUCCEEDED(hr));

    HttpHeaders headers{};
    for (uint32_t i = 0; i < headerCount; ++i)
    {
        const char* headerName{ nullptr };
        const char* headerValue{ nullptr };
        hr = HCHttpCallResponseGetHeaderAtIndex(m_callHandle, i, &headerName, &headerValue);
        assert(SUCCEEDED(hr));
        headers[headerName] = headerValue;
    }
    UNREFERENCED_PARAMETER(hr);

    return headers;
}

void HttpCall::CompletionCallback(_In_ XAsyncBlock* async)
{
    auto sharedThis{ static_cast<HttpCall*>(async->context)->shared_from_this() };
    sharedThis->DecRef();

    sharedThis->m_step = Step::Done;
    sharedThis->m_asyncContext.Complete(HttpResult{ sharedThis });
}

std::shared_ptr<RefCounter> HttpCall::GetSharedThis()
{
    return shared_from_this();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

using namespace xbox::services;

HRESULT XblHttpCall::SetXblServiceContractVersion(uint32_t contractVersion)
{
    xsapi_internal_stringstream ss;
    ss << contractVersion;
    return SetHeader(CONTRACT_VERSION_HEADER, ss.str());
}

void XblHttpCall::SetLongHttpCall(_In_ bool longHttpCall)
{
    m_longHttpCall = longHttpCall;
}

XblHttpCall::XblHttpCall(_In_ User&& user)
    : m_user{ std::move(user) }
{
}

HRESULT XblHttpCall::Init(
    _In_ std::shared_ptr<XboxLiveContextSettings> contextSettings,
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& fullUrl,
    _In_ xbox_live_api xboxLiveApi
)
{
    m_httpMethod = httpMethod;
    m_fullUrl = fullUrl;
    m_longHttpTimeout = contextSettings->LongHttpTimeout();

    RETURN_HR_IF_FAILED(HttpCall::Init(httpMethod, fullUrl));
    RETURN_HR_IF_FAILED(SetRetryCacheId(static_cast<uint32_t>(xboxLiveApi)));
    m_httpTimeoutWindowInSeconds = contextSettings->HttpTimeoutWindow();
    RETURN_HR_IF_FAILED(SetTimeoutWindow(contextSettings->HttpTimeoutWindow()));
    RETURN_HR_IF_FAILED(SetRetryDelay(contextSettings->HttpRetryDelay()));
    RETURN_HR_IF_FAILED(SetHeader(CONTRACT_VERSION_HEADER, "1"));
    RETURN_HR_IF_FAILED(SetHeader(CONTENT_TYPE_HEADER, "application/json; charset=utf-8"));
    RETURN_HR_IF_FAILED(SetHeader(ACCEPT_LANGUAGE_HEADER, utils::get_locales()));
    RETURN_HR_IF_FAILED(SetUserAgent(contextSettings->HttpUserAgent()));

    return S_OK;
}

HRESULT XblHttpCall::CalcHttpTimeout()
{
    if (m_longHttpCall)
    {
        // Long calls such as Title Storage upload/download ignore http_timeout_window so they act as expected with 
        // requiring the game developer to manually adjust http_timeout_window before calling them.
        return SetTimeout(m_longHttpTimeout);
    }
    else
    {
        // For all other calls, set the timeout to be how much time left before hitting the http_timeout_window setting with a min of 5 second
        std::chrono::milliseconds timeElapsedSinceFirstCall = std::chrono::duration_cast<std::chrono::milliseconds>(m_requestStartTime - m_firstCallStartTime);
        std::chrono::seconds remainingTimeBeforeTimeout = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(m_httpTimeoutWindowInSeconds) - timeElapsedSinceFirstCall);
        uint64_t secondsLeft = __min(DEFAULT_HTTP_TIMEOUT_SECONDS, static_cast<uint64_t>(remainingTimeBeforeTimeout.count()));
        uint64_t secondsLeftCapped = __max(MIN_HTTP_TIMEOUT_SECONDS, secondsLeft);
        return SetTimeout(static_cast<uint32_t>(secondsLeftCapped));
    }
}

HRESULT XblHttpCall::SetHeader(
    _In_ const xsapi_internal_string& key,
    _In_ const xsapi_internal_string& value,
    _In_ bool allowTracing
)
{
    m_requestHeaders[key] = value;
    return HttpCall::SetHeader(key, value, allowTracing);
}

HRESULT XblHttpCall::SetUserAgent(_In_ HttpCallAgent userAgent)
{
    String headerValue{ DEFAULT_USER_AGENT };
    if (userAgent != HttpCallAgent::Title)
    {
        headerValue += EnumName(userAgent);
    }

    XblApiType apiType{ XblApiType::XblCApi };
    {
        auto state{ GlobalState::Get() };
        if (state)
        {
            apiType = state->ApiType;
        }
    }

    switch (apiType)
    {
    case XblApiType::XblCApi:
    {
        headerValue += " c";
        break;
    }
    case XblApiType::XblCPPApi:
    {
        headerValue += " cpp";
        break;
    }
    }

    return SetHeader(USER_AGENT_HEADER, headerValue);
}

HRESULT XblHttpCall::SetRequestBody(const xsapi_internal_vector<uint8_t>& bytes)
{
    m_requestBody = bytes;
    return HttpCall::SetRequestBody(xsapi_internal_vector<uint8_t>{ bytes });
}

HRESULT XblHttpCall::SetRequestBody(_In_reads_bytes_(requestBodySize) const uint8_t* requestBodyBytes, _In_ uint32_t requestBodySize)
{
    m_requestBody = xsapi_internal_vector<uint8_t>{ requestBodyBytes, requestBodyBytes + requestBodySize };
    return HttpCall::SetRequestBody(requestBodyBytes, requestBodySize);
}

HRESULT XblHttpCall::SetRequestBody(_In_z_ const char* requestBodyString)
{
    xsapi_internal_string requestBody{ requestBodyString };
    return XblHttpCall::SetRequestBody(requestBody);
}

HRESULT XblHttpCall::SetRequestBody(const xsapi_internal_string& bodyString)
{
    m_requestBody = xsapi_internal_vector<uint8_t>{ bodyString.begin(), bodyString.end() };
    return HttpCall::SetRequestBody(bodyString);
}

HRESULT XblHttpCall::SetRequestBody(const JsonValue& bodyJson)
{
    return SetRequestBody(JsonUtils::SerializeJson(bodyJson));
}

void XblHttpCall::SetAuthRetryAllowed(bool authRetryAllowed)
{
    m_authRetryExplicitlyAllowed = authRetryAllowed;
}

HRESULT XblHttpCall::Perform(
    AsyncContext<HttpResult> async,
    bool forceRefresh
)
{
    m_asyncContext = std::move(async);

    std::shared_ptr<XblHttpCall> sharedThis = { std::dynamic_pointer_cast<XblHttpCall>(shared_from_this()) };

    auto now = chrono_clock_t::now();
    if (m_iterationNumber == 0)
    {
        m_firstCallStartTime = now;
    }
    m_iterationNumber++;
    m_requestStartTime = now;

    if (forceRefresh)
    {
        m_user.SetTokenExpired(m_user.Xuid());
    }

    return m_user.GetTokenAndSignature(
        m_httpMethod,
        m_fullUrl,
        m_requestHeaders,
        m_requestBody.data(),
        m_requestBody.size(),
        false, // allUsersAuthRequired
        AsyncContext<xbox::services::Result<TokenAndSignature>>{ m_asyncContext.Queue(),
        [
            sharedThis
        ]
    (xbox::services::Result<TokenAndSignature> authResult)
    {
        if (Failed(authResult))
        {
            sharedThis->IntermediateHttpCallCompleteCallback(authResult.Hresult());
        }
        else
        {
            const auto& authPayload = authResult.Payload();
            HRESULT hr = S_OK;

            if (!authPayload.token.empty())
            {
                hr = sharedThis->HttpCall::SetHeader(AUTH_HEADER, authPayload.token, false);
            }

            if (SUCCEEDED(hr) && !authPayload.signature.empty())
            {
                hr = sharedThis->HttpCall::SetHeader(SIG_HEADER, authPayload.signature, false);
            }

            if (SUCCEEDED(hr))
            {
                hr = sharedThis->CalcHttpTimeout();
                if (SUCCEEDED(hr))
                {
                    hr = sharedThis->HttpCall::Perform(AsyncContext<HttpResult>{
                        sharedThis->m_asyncContext.Queue(),
                            [
                                // Don't store a shared ref here since this lambda will be stored in HttpCall object, creating a self reference that would never
                                // be cleaned up. HttpCallPerform already guarantees lifetime until this callback is called.
                                rawThis{ sharedThis.get() }
                            ]
                        (HttpResult result)
                        {
                            rawThis->IntermediateHttpCallCompleteCallback(result);
                        }});
                }
            }

            if (FAILED(hr))
            {
                sharedThis->IntermediateHttpCallCompleteCallback(HttpResult{ hr });
            }
        }
    }
    });
}

void XblHttpCall::IntermediateHttpCallCompleteCallback(HttpResult result)
{
    auto httpCall = result.Payload();
    if (httpCall)
    {
        bool wasHandled{ false };
        HRESULT hr = HandleAuthError(httpCall, wasHandled);
        if (wasHandled)
        {
            return;
        }
        if (FAILED(hr))
        {
            m_asyncContext.Complete(HttpResult{ hr });
            return;
        }

        HandleThrottleError(httpCall);
    }

    m_asyncContext.Complete(std::move(result));
}

HRESULT XblHttpCall::HandleAuthError(_In_ std::shared_ptr<class HttpCall> httpCall, _Out_ bool& wasHandled)
{
    if (httpCall->HttpStatus() != 401)
    {
        wasHandled = false;
        return S_OK;
    }

    bool retryAllowed{ false };
    HCHttpCallRequestGetRetryAllowed(m_callHandle, &retryAllowed);
    if (!retryAllowed && !m_authRetryExplicitlyAllowed)
    {
        wasHandled = false;
        return S_OK;
    }

    if (m_hasPerformedRetryOn401)
    {
        // Ignore 401 retrying if we already have retried a 401
        wasHandled = false;
        return S_OK;
    }
    m_hasPerformedRetryOn401 = true;

    HRESULT hr = ResetAndCopyForRetry();
    if (SUCCEEDED(hr))
    {
        hr = XblHttpCall::Perform(m_asyncContext, true);
    }
    if (SUCCEEDED(hr))
    {
        wasHandled = true;
        return S_OK;
    }

    wasHandled = false;
    return hr;
}

void XblHttpCall::HandleThrottleError(_In_ std::shared_ptr<class HttpCall> httpCall)
{
    if (httpCall->HttpStatus() != 429)
    {
        return;
    }

    // Assert if we were throttled by the service so the game dev knows that they are calling Xbox Live to agressively
    auto appConfig = AppConfig::Instance();
    if (appConfig && utils::str_icmp_internal(appConfig->Sandbox(), "RETAIL") != 0)
    {
        if (!appConfig->IsDisableAssertsForXboxLiveThrottlingInDevSandboxes())
        {
            LOGS_ERROR << "Xbox Live service call to " << m_fullUrl << " was throttled";
            LOGS_ERROR << GetResponseBodyString();
            LOGS_ERROR << "You can temporarily disable the assert by calling";
            LOGS_ERROR << "XblDisableAssertsForXboxLiveThrottlingInDevSandboxes()";
            LOGS_ERROR << "Note that this will only disable this assert.  You will still be throttled in all sandboxes.";
#ifndef XSAPI_UNIT_TESTS
            assert(false && "Xbox Live service call was throttled.  See Output for more detail");
#endif
        }
    }
}

xsapi_internal_string XblHttpCall::BuildUrl(
    xsapi_internal_string&& serviceName,
    const xsapi_internal_string& pathQueryFragment
)
{
    xsapi_internal_stringstream source;
    source << "https://" << serviceName << ".xboxlive.com" << pathQueryFragment;
    return source.str();
}
