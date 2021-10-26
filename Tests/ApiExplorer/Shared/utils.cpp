// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "utils.h"

#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include "runner.h"
#include "pal.h"
#include "api_explorer.h"
#endif

#if HC_PLATFORM == HC_PLATFORM_GDK
#include <XGameErr.h>
#endif

HRESULT VerifyIsTrue(bool condition, const char* pszCondition)
{
    if (!condition)
    {
        LogToFile("Failure: ");
        LogToFile(pszCondition);
        return E_FAIL;
    }

    return S_OK;
}

#define RETURN_STRING_FROM_HR(e) case (e): return #e
#pragma warning (push)
#pragma warning (disable: 4061)

std::string ConvertHRtoString(HRESULT hr)
{
    switch (hr)
    {
        RETURN_STRING_FROM_HR(S_OK);
        RETURN_STRING_FROM_HR(E_FAIL);
        RETURN_STRING_FROM_HR(E_POINTER);
        RETURN_STRING_FROM_HR(E_INVALIDARG);
        RETURN_STRING_FROM_HR(E_OUTOFMEMORY);
        RETURN_STRING_FROM_HR(E_NOT_SUFFICIENT_BUFFER);
        RETURN_STRING_FROM_HR(E_NOT_SUPPORTED);
        RETURN_STRING_FROM_HR(E_ABORT);
        RETURN_STRING_FROM_HR(E_NOTIMPL);
        RETURN_STRING_FROM_HR(E_ACCESSDENIED);
        RETURN_STRING_FROM_HR(E_PENDING);
        RETURN_STRING_FROM_HR(E_UNEXPECTED);
        RETURN_STRING_FROM_HR(E_TIME_CRITICAL_THREAD);
        RETURN_STRING_FROM_HR(E_NOINTERFACE);
        RETURN_STRING_FROM_HR(E_BOUNDS);
        RETURN_STRING_FROM_HR(E_NO_TASK_QUEUE);

        RETURN_STRING_FROM_HR(HTTP_E_STATUS_AMBIGUOUS);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_BAD_GATEWAY);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_BAD_METHOD);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_BAD_REQUEST);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_CONFLICT);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_DENIED);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_EXPECTATION_FAILED);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_FORBIDDEN);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_GATEWAY_TIMEOUT);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_GONE);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_LENGTH_REQUIRED);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_MOVED);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_NONE_ACCEPTABLE);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_NOT_FOUND);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_NOT_MODIFIED);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_NOT_SUPPORTED);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_PAYMENT_REQ);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_PRECOND_FAILED);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_PROXY_AUTH_REQ);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_RANGE_NOT_SATISFIABLE);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_REDIRECT);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_REDIRECT_KEEP_VERB);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_REDIRECT_METHOD);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_REQUEST_TIMEOUT);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_REQUEST_TOO_LARGE);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_SERVER_ERROR);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_SERVICE_UNAVAIL);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_UNEXPECTED);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_UNSUPPORTED_MEDIA);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_URI_TOO_LONG);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_USE_PROXY);
        RETURN_STRING_FROM_HR(HTTP_E_STATUS_VERSION_NOT_SUP);
        RETURN_STRING_FROM_HR(ONL_E_ACTION_REQUIRED);
        RETURN_STRING_FROM_HR(WEB_E_INVALID_JSON_STRING);
        RETURN_STRING_FROM_HR(WEB_E_UNEXPECTED_CONTENT);

#if HC_PLATFORM != HC_PLATFORM_UWP
        RETURN_STRING_FROM_HR(E_XAL_NOTINITIALIZED);
        RETURN_STRING_FROM_HR(E_XAL_ALREADYINITIALIZED);
        RETURN_STRING_FROM_HR(E_XAL_USERSETNOTEMPTY);
        RETURN_STRING_FROM_HR(E_XAL_USERSETFULL);
        RETURN_STRING_FROM_HR(E_XAL_USERSIGNEDOUT);
        RETURN_STRING_FROM_HR(E_XAL_DUPLICATEDUSER);
        RETURN_STRING_FROM_HR(E_XAL_NETWORK);
        RETURN_STRING_FROM_HR(E_XAL_CLIENTERROR);
        RETURN_STRING_FROM_HR(E_XAL_UIREQUIRED);
        RETURN_STRING_FROM_HR(E_XAL_HANDLERALREADYREGISTERED);
        RETURN_STRING_FROM_HR(E_XAL_UNEXPECTEDUSERSIGNEDIN);
        RETURN_STRING_FROM_HR(E_XAL_NOTATTACHEDTOJVM);
        RETURN_STRING_FROM_HR(E_XAL_DEVICEUSER);
        RETURN_STRING_FROM_HR(E_XAL_DEFERRALNOTAVAILABLE);
#if HC_PLATFORM != HC_PLATFORM_GDK
        RETURN_STRING_FROM_HR(E_XAL_MISSINGPLATFORMEVENTHANDLER);
#endif
        RETURN_STRING_FROM_HR(E_XAL_USERNOTFOUND);
        RETURN_STRING_FROM_HR(E_XAL_NOTOKENREQUIRED);
        RETURN_STRING_FROM_HR(E_XAL_NODEFAULTUSER);
        RETURN_STRING_FROM_HR(E_XAL_FAILEDTORESOLVE);
#endif

        RETURN_STRING_FROM_HR(E_XBL_RUNTIME_ERROR);
        RETURN_STRING_FROM_HR(E_XBL_RTA_GENERIC_ERROR);
        RETURN_STRING_FROM_HR(E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED);
        RETURN_STRING_FROM_HR(E_XBL_RTA_ACCESS_DENIED);
        RETURN_STRING_FROM_HR(E_XBL_AUTH_UNKNOWN_ERROR);
        RETURN_STRING_FROM_HR(E_XBL_AUTH_RUNTIME_ERROR);
        RETURN_STRING_FROM_HR(E_XBL_AUTH_NO_TOKEN);
        RETURN_STRING_FROM_HR(E_XBL_ALREADY_INITIALIZED);

        RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
        RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_BAD_CONFIGURATION));
        RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_BAD_LENGTH));
        RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_CANCELLED));
        RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER));
        RETURN_STRING_FROM_HR(__HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND));

        RETURN_STRING_FROM_HR(E_HC_PERFORM_ALREADY_CALLED);
        RETURN_STRING_FROM_HR(E_HC_ALREADY_INITIALISED);
        RETURN_STRING_FROM_HR(E_HC_CONNECT_ALREADY_CALLED);
        RETURN_STRING_FROM_HR(E_HC_NO_NETWORK);

#if HC_PLATFORM == HC_PLATFORM_GDK
        RETURN_STRING_FROM_HR(E_GAMERUNTIME_NOT_INITIALIZED);
        RETURN_STRING_FROM_HR(E_GAMERUNTIME_DLL_NOT_FOUND);
        RETURN_STRING_FROM_HR(E_GAMERUNTIME_VERSION_MISMATCH);
        //RETURN_STRING_FROM_HR(E_GAMEUSER_MAX_USERS_ADDED);
        //RETURN_STRING_FROM_HR(E_GAMEUSER_SIGNED_OUT);
        //RETURN_STRING_FROM_HR(E_GAMEUSER_RESOLVE_USER_ISSUE_REQUIRED);
        //RETURN_STRING_FROM_HR(E_GAMEUSER_DEFERRAL_NOT_AVAILABLE);
        //RETURN_STRING_FROM_HR(E_GAMEUSER_USER_NOT_FOUND);
        //RETURN_STRING_FROM_HR(E_GAMEUSER_NO_TOKEN_REQUIRED);
        //RETURN_STRING_FROM_HR(E_GAMEUSER_NO_DEFAULT_USER);
        //RETURN_STRING_FROM_HR(E_GAMEUSER_FAILED_TO_RESOLVE);
        RETURN_STRING_FROM_HR(E_GAMEUSER_NO_TITLE_ID);
        RETURN_STRING_FROM_HR(E_GAMEUSER_UNKNOWN_GAME_IDENTITY);
        RETURN_STRING_FROM_HR(E_GAMEUSER_NO_PACKAGE_IDENTITY);
        RETURN_STRING_FROM_HR(E_GAMEUSER_FAILED_TO_GET_TOKEN);
        RETURN_STRING_FROM_HR(E_GAMEPACKAGE_APP_NOT_PACKAGED);
        RETURN_STRING_FROM_HR(E_GAMEPACKAGE_NO_INSTALLED_LANGUAGES);
        RETURN_STRING_FROM_HR(E_GAMESTORE_LICENSE_ACTION_NOT_APPLICABLE_TO_PRODUCT);
        RETURN_STRING_FROM_HR(E_GAMESTORE_NETWORK_ERROR);
        RETURN_STRING_FROM_HR(E_GAMESTORE_SERVER_ERROR);
        RETURN_STRING_FROM_HR(E_GAMESTORE_INSUFFICIENT_QUANTITY);
        RETURN_STRING_FROM_HR(E_GAMESTORE_ALREADY_PURCHASED);
#endif

        default: return "Unknown error";
    }
}

std::string ConvertHR(HRESULT hr)
{
    CHAR sz[256];
#if HC_PLATFORM_IS_MICROSOFT
    sprintf_s(sz, "%s (0x%0.8x)", ConvertHRtoString(hr).c_str(), hr);
#else
    sprintf(sz, "%s (0x%0.8x)", ConvertHRtoString(hr).c_str(), hr);
#endif
    return sz;
}

std::string ReadFile(std::string fileName)
{

    std::string filePath = pal::FindFile(fileName);
    if (filePath.empty())
    {
        std::string testPath = "Tests\\" + fileName;
        filePath = pal::FindFile(testPath);
    }

    if (filePath.empty())
    {
        return std::string();
    }

    std::ifstream inputStream(filePath.c_str());

    return std::string(std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>());
}

uint64_t ConvertStringToUInt64(std::string str)
{
    return std::stoull(str);
}

HRESULT CreateQueueIfNeeded()
{
    HRESULT hr = S_OK;
    if (Data()->queue == nullptr)
    {
        hr = XTaskQueueCreate(
            XTaskQueueDispatchMode::ThreadPool,
            XTaskQueueDispatchMode::ThreadPool,
            &Data()->queue);

        assert(SUCCEEDED(hr));
    }

    return hr;
}

#if CPP_TESTS_ENABLED

using namespace xbox::services;

HRESULT ConvertHttpStatusToHresult(_In_ uint32_t httpStatusCode)
{
    xbox::services::xbox_live_error_code errCode = static_cast<xbox::services::xbox_live_error_code>(httpStatusCode);
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
        case xbox_live_error_code::http_status_300_multiple_choices: hr = HTTP_E_STATUS_AMBIGUOUS; break;
        case xbox_live_error_code::http_status_301_moved_permanently: hr = HTTP_E_STATUS_MOVED; break;
        case xbox_live_error_code::http_status_302_found: hr = HTTP_E_STATUS_REDIRECT; break;
        case xbox_live_error_code::http_status_303_see_other: hr = HTTP_E_STATUS_REDIRECT_METHOD; break;
        case xbox_live_error_code::http_status_304_not_modified: hr = HTTP_E_STATUS_NOT_MODIFIED; break;
        case xbox_live_error_code::http_status_305_use_proxy: hr = HTTP_E_STATUS_USE_PROXY; break;
        case xbox_live_error_code::http_status_307_temporary_redirect: hr = HTTP_E_STATUS_REDIRECT_KEEP_VERB; break;

        case xbox_live_error_code::http_status_400_bad_request: hr = HTTP_E_STATUS_BAD_REQUEST; break;
        case xbox_live_error_code::http_status_401_unauthorized: hr = HTTP_E_STATUS_DENIED; break;
        case xbox_live_error_code::http_status_402_payment_required: hr = HTTP_E_STATUS_PAYMENT_REQ; break;
        case xbox_live_error_code::http_status_403_forbidden: hr = HTTP_E_STATUS_FORBIDDEN; break;
        case xbox_live_error_code::http_status_404_not_found: hr = HTTP_E_STATUS_NOT_FOUND; break;
        case xbox_live_error_code::http_status_405_method_not_allowed: hr = HTTP_E_STATUS_BAD_METHOD; break;
        case xbox_live_error_code::http_status_406_not_acceptable: hr = HTTP_E_STATUS_NONE_ACCEPTABLE; break;
        case xbox_live_error_code::http_status_407_proxy_authentication_required: hr = HTTP_E_STATUS_PROXY_AUTH_REQ; break;
        case xbox_live_error_code::http_status_408_request_timeout: hr = HTTP_E_STATUS_REQUEST_TIMEOUT; break;
        case xbox_live_error_code::http_status_409_conflict: hr = HTTP_E_STATUS_CONFLICT; break;
        case xbox_live_error_code::http_status_410_gone: hr = HTTP_E_STATUS_GONE; break;
        case xbox_live_error_code::http_status_411_length_required: hr = HTTP_E_STATUS_LENGTH_REQUIRED; break;
        case xbox_live_error_code::http_status_412_precondition_failed: hr = HTTP_E_STATUS_PRECOND_FAILED; break;
        case xbox_live_error_code::http_status_413_request_entity_too_large: hr = HTTP_E_STATUS_REQUEST_TOO_LARGE; break;
        case xbox_live_error_code::http_status_414_request_uri_too_long: hr = HTTP_E_STATUS_URI_TOO_LONG; break;
        case xbox_live_error_code::http_status_415_unsupported_media_type: hr = HTTP_E_STATUS_UNSUPPORTED_MEDIA; break;
        case xbox_live_error_code::http_status_416_requested_range_not_satisfiable: hr = HTTP_E_STATUS_RANGE_NOT_SATISFIABLE; break;
        case xbox_live_error_code::http_status_417_expectation_failed: hr = HTTP_E_STATUS_EXPECTATION_FAILED; break;
        case xbox_live_error_code::http_status_421_misdirected_request: hr = MAKE_HTTP_HRESULT(421); break;
        case xbox_live_error_code::http_status_422_unprocessable_entity: hr = MAKE_HTTP_HRESULT(422); break;
        case xbox_live_error_code::http_status_423_locked: hr = MAKE_HTTP_HRESULT(423); break;
        case xbox_live_error_code::http_status_424_failed_dependency: hr = MAKE_HTTP_HRESULT(424); break;
        case xbox_live_error_code::http_status_426_upgrade_required: hr = MAKE_HTTP_HRESULT(426); break;
        case xbox_live_error_code::http_status_428_precondition_required: hr = MAKE_HTTP_HRESULT(428); break;
        case xbox_live_error_code::http_status_429_too_many_requests: hr = MAKE_HTTP_HRESULT(429); break;
        case xbox_live_error_code::http_status_431_request_header_fields_too_large: hr = MAKE_HTTP_HRESULT(431); break;
        case xbox_live_error_code::http_status_449_retry_with:hr = MAKE_HTTP_HRESULT(449); break;
        case xbox_live_error_code::http_status_451_unavailable_for_legal_reasons: hr = MAKE_HTTP_HRESULT(451); break;

        case xbox_live_error_code::http_status_500_internal_server_error: hr = HTTP_E_STATUS_SERVER_ERROR; break;
        case xbox_live_error_code::http_status_501_not_implemented: hr = HTTP_E_STATUS_NOT_SUPPORTED; break;
        case xbox_live_error_code::http_status_502_bad_gateway: hr = HTTP_E_STATUS_BAD_GATEWAY; break;
        case xbox_live_error_code::http_status_503_service_unavailable: hr = HTTP_E_STATUS_SERVICE_UNAVAIL; break;
        case xbox_live_error_code::http_status_504_gateway_timeout: hr = HTTP_E_STATUS_GATEWAY_TIMEOUT; break;
        case xbox_live_error_code::http_status_505_http_version_not_supported: hr = HTTP_E_STATUS_VERSION_NOT_SUP; break;
        case xbox_live_error_code::http_status_506_variant_also_negotiates: hr = MAKE_HTTP_HRESULT(506); break;
        case xbox_live_error_code::http_status_507_insufficient_storage: hr = MAKE_HTTP_HRESULT(507); break;
        case xbox_live_error_code::http_status_508_loop_detected: hr = MAKE_HTTP_HRESULT(508); break;
        case xbox_live_error_code::http_status_510_not_extended: hr = MAKE_HTTP_HRESULT(510); break;
        case xbox_live_error_code::http_status_511_network_authentication_required: hr = MAKE_HTTP_HRESULT(511); break;

        default:
            hr = HTTP_E_STATUS_UNEXPECTED;
            break;
        }
    }

    return hr;
}

HRESULT ConvertXboxLiveErrorCodeToHresult(_In_ const std::error_code& errCode)
{
    int err = static_cast<int>(errCode.value());
    xbox_live_error_code xblErr = static_cast<xbox_live_error_code>(err);

    if (err == 204)
    {
        return __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND);
    }
    else if (err >= 300 && err <= 505)
    {
        return (HRESULT)ConvertHttpStatusToHresult(static_cast<uint32_t>(err));
    }
    else if (err >= 1000 && err <= 9999)
    {
        switch (xblErr)
        {
        case xbox_live_error_code::bad_alloc: return E_OUTOFMEMORY;
        case xbox_live_error_code::invalid_argument: return E_INVALIDARG;
        case xbox_live_error_code::runtime_error: return E_XBL_RUNTIME_ERROR;
        case xbox_live_error_code::length_error: return __HRESULT_FROM_WIN32(ERROR_BAD_LENGTH);
        case xbox_live_error_code::out_of_range: return E_BOUNDS;
        case xbox_live_error_code::range_error: return E_BOUNDS;
        case xbox_live_error_code::bad_cast: return E_NOINTERFACE;
        case xbox_live_error_code::logic_error: return E_UNEXPECTED;
        case xbox_live_error_code::json_error: return WEB_E_INVALID_JSON_STRING;
        case xbox_live_error_code::uri_error: return WEB_E_UNEXPECTED_CONTENT;
        case xbox_live_error_code::websocket_error: return WEB_E_UNEXPECTED_CONTENT;
        case xbox_live_error_code::auth_user_interaction_required: return ONL_E_ACTION_REQUIRED;
        case xbox_live_error_code::rta_generic_error: return E_XBL_RTA_GENERIC_ERROR;
        case xbox_live_error_code::rta_subscription_limit_reached: return E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED;
        case xbox_live_error_code::rta_access_denied: return E_XBL_RTA_ACCESS_DENIED;
        case xbox_live_error_code::auth_unknown_error: return E_XBL_AUTH_UNKNOWN_ERROR;
        case xbox_live_error_code::auth_runtime_error: return E_XBL_AUTH_RUNTIME_ERROR;
        case xbox_live_error_code::auth_no_token_error: return E_XBL_AUTH_NO_TOKEN;
        case xbox_live_error_code::auth_user_not_signed_in: return __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER);
        case xbox_live_error_code::auth_user_cancel: return __HRESULT_FROM_WIN32(ERROR_CANCELLED);
        case xbox_live_error_code::auth_user_switched: return __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER);
        case xbox_live_error_code::invalid_config: return __HRESULT_FROM_WIN32(ERROR_BAD_CONFIGURATION);
        case xbox_live_error_code::unsupported: return E_NOTIMPL;

        default: return E_FAIL;
        }
    }
    else if ((err & 0x87DD0000) == 0x87D8000)
    {
        return HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR;
    }
    else return err; //return the original error code if can't be translated.
}
#endif

#pragma warning (pop)
