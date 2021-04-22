// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi_utils.h"
#include "xbox_live_app_config_internal.h"
#include <iomanip>
#include <chrono>
#include <time.h>
#include <string>
#if !HC_PLATFORM_IS_MICROSOFT
#include "xbl_guid.h"
#elif defined(_WIN32)
#include <objbase.h>
#endif
#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include "a/utils_a.h"
#include "a/java_interop.h"
#endif
#include "presence_internal.h"
#include "httpClient/httpClient.h"
#include "Logger/log_hc_output.h"
#include "global_state.h"

#ifndef _XTIME_TICKS_PER_TIME_T
#define _XTIME_TICKS_PER_TIME_T 10000000LL
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#define MAKE_HTTP_HRESULT(code) MAKE_HRESULT(1, 0x019, code)

static char const * _sdaPrefix = "AAAAAAAA";

static const uint64_t _msTicks = static_cast<uint64_t>(10000);
static const uint64_t _secondTicks = 1000*_msTicks;

xsapi_internal_string utils::encode_uri(
    _In_ const xsapi_internal_string& data, 
    _In_ xbox::services::uri::components::component component
    )
{
    return xbox::services::uri::encode_uri(data, component);
}

xsapi_internal_string utils::headers_to_string(
    _In_ const xsapi_internal_http_headers& headers
    )
{
    xsapi_internal_stringstream ss;

    for (const auto& header : headers)
    {
        ss << header.first << ": " << header.second << "\r\n";
    }

    return ss.str();
}

xsapi_internal_string 
utils::get_query_from_params(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& params
    )
{
    xsapi_internal_stringstream strQueryString;

    size_t cItems = params.size();
    if (cItems > 0)
    {
        xsapi_internal_string strDelimiter = "&";

        strQueryString << "?";
        strQueryString << params[0];

        size_t i = 0;
        while (++i < cItems)
        {
            strQueryString << strDelimiter;
            strQueryString << params[i];
        }
    }

    return strQueryString.str();
}

void utils::append_paging_info(
    _In_ xbox::services::uri_builder& uriBuilder,
    _In_ unsigned int skipItems,
    _In_ unsigned int maxItems,
    _In_opt_ xsapi_internal_string continuationToken
    )
{
    // add maxItem parameter
    if (maxItems > 0)
    {
        uriBuilder.append_query("maxItems", maxItems);
    }

    if (continuationToken.empty())
    {
        // use skip items value if continuation token is empty
        if (skipItems > 0)
        {
            uriBuilder.append_query("skipItems", skipItems);
        }
    }
    else
    {
        uriBuilder.append_query("continuationToken", continuationToken);
    }
}

#if defined(_WIN32) 
uint32_t utils::convert_timespan_to_days(
    _In_ uint64_t timespan
    )
{
    int64_t days = (timespan / _XTIME_TICKS_PER_TIME_T) / SECONDS_PER_DAY;
    THROW_CPP_INVALIDARGUMENT_IF(days < 0 || days > UINT32_MAX);

    return static_cast<uint32_t>(days);
}

void utils::convert_unix_time_to_filetime(
    _In_ std::time_t t,
    _In_ FILETIME* ft
    )
{
    if (!ft)
    {
        return;
    }

    LONGLONG ll;

#ifdef _USE_32BIT_TIME_T
    ll = Int32x32To64(t * 10000000) + 116444736000000000;
#else
    ll = (t * 10000000) + 116444736000000000;
#endif

    ft->dwLowDateTime = (DWORD)ll;
    ft->dwHighDateTime = ll >> 32;
}
void utils::convert_timepoint_to_filetime(
    _In_ const chrono_clock_t::time_point& time_point,
    _Inout_ uint64_t& largeInt
    )
{
    // time_point to system time
    std::time_t t = convert_timepoint_to_time(time_point);
    // system time to FILETIME
    FILETIME ft = { 0 };
    convert_unix_time_to_filetime(t, &ft);

    if (largeInt)
    {
        ULARGE_INTEGER large;
        large.LowPart = ft.dwLowDateTime;
        large.HighPart = ft.dwHighDateTime;
        largeInt = large.QuadPart;
    }
}
#endif

std::time_t utils::convert_timepoint_to_time(
    _In_ const chrono_clock_t::time_point& time_point
    )
{
#if _MSC_VER <= 1800
    return chrono_clock_t::to_time_t(time_point);
#else
    uint64_t timeDiff = std::chrono::duration_cast<std::chrono::seconds>(time_point.time_since_epoch() - std::chrono::steady_clock::now().time_since_epoch()).count();
    uint64_t timeNow = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return timeNow + timeDiff;
#endif
}

xsapi_internal_string utils::convert_timepoint_to_string(
    _In_ const chrono_clock_t::time_point& time_point
    )
{
    xsapi_internal_string result;
    xsapi_internal_string::value_type buff[FILENAME_MAX];
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());
    time_t t = utils::convert_timepoint_to_time(time_point);
    std::tm time;
#if defined(_WIN32)
    errno_t errorCode = localtime_s(&time, &t);
    if (errorCode != 0)
    {
        return result;
    }
    sprintf_s(buff, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
        time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
        time.tm_hour, time.tm_min, time.tm_sec, static_cast<int>(ms.count() % 1000));
#else
    std::tm* error = localtime_r(&t, &time);
    if (error == nullptr)
    {
        return result;
    }
    sprintf(buff, _T("%04d-%02d-%02dT%02d:%02d:%02d.%03dZ"),
              time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
              time.tm_hour, time.tm_min, time.tm_sec, static_cast<int>(ms.count() % 1000));

#endif
    result = buff;

    return result;
}

xsapi_internal_string utils::escape_special_characters(const xsapi_internal_string& str)
{
    xsapi_internal_string result = str;
    for (auto iter = result.begin(); iter != result.end(); ++iter)
    {
        if (*iter == '\r' || *iter == '\n')
        {
            iter = result.insert(iter, ' ');
            iter = result.erase(iter + 1);
            --iter;
        }
        else if (*iter == '\"')
        {
            iter = result.insert(iter, '\"');
            ++iter;
        }
    }
    return result;
}

uint32_t
utils::char_t_copy(
    _In_reads_bytes_(sizeInWords) char_t* destinationCharArr,
    _In_ size_t sizeInWords,
    _In_ const char_t* sourceCharArr
    )
{
#if HC_PLATFORM_IS_MICROSOFT
    return wcscpy_s(destinationCharArr, sizeInWords, sourceCharArr);
#else
    return (uint32_t)strlcpy(destinationCharArr, sourceCharArr, (uint32_t)sizeInWords);
#endif
}

size_t
utils::strcpy(
    _In_ char* destinationCharArr,
    _In_ size_t sizeInWords,
    _In_ const char* sourceCharArr
    )

{
#if HC_PLATFORM_IS_MICROSOFT
    return strcpy_s(destinationCharArr, sizeInWords, sourceCharArr);
#else
    return strlcpy(destinationCharArr, sourceCharArr, sizeInWords);
#endif
}

HRESULT
utils::convert_exception_to_hresult()
{
    // Default value, if there is no exception appears, return S_OK
    HRESULT hr = S_OK;

    try
    {
        throw;
    }
    // std exceptions
    catch (const std::bad_alloc&) // is an exception
    {
        hr = E_OUTOFMEMORY;
    }
    catch (const std::bad_cast&) // is an exception
    {
        hr = E_NOINTERFACE;
    }
    catch (const std::invalid_argument&) // is a logic_error
    {
        hr = E_INVALIDARG;
    }
    catch (const std::out_of_range&) // is a logic_error
    {
        hr = E_BOUNDS;
    }
    catch (const std::length_error&) // is a logic_error
    {
        hr = __HRESULT_FROM_WIN32(ERROR_BAD_LENGTH);
    }
    catch (const std::overflow_error&) // is a runtime_error
    {
        hr = __HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
    }
    catch (const std::underflow_error&) // is a runtime_error
    {
        hr = __HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
    }
    catch (const std::range_error&) // is a runtime_error
    {
        hr = E_BOUNDS;
    }
    catch (const std::system_error& ex) // is a runtime_error
    {
        if (ex.code().category() == std::system_category())
        {
            hr = __HRESULT_FROM_WIN32(ex.code().value());
        }
        else
        {
            hr = ex.code().value();
        }
    }
    catch (const std::logic_error&) // is an exception
    {
        hr = E_UNEXPECTED;
    }
    catch (const std::runtime_error&) // is an exception
    {
        hr = E_FAIL;
    }
#if !XSAPI_NO_PPL
    catch (const web::http::http_exception&) // is an exception
    {
        hr = HTTP_E_STATUS_UNEXPECTED;
    }
#endif // !XSAPI_NO_PPL
    catch (const xbox::services::uri_exception&) // is an exception
    {
        hr = WEB_E_UNEXPECTED_CONTENT;
    }
    catch (const std::exception&) // base class for standard C++ exceptions
    {
        hr = E_FAIL;
    }
    catch (HRESULT exceptionHR)
    {
        hr = exceptionHR;
    }
    catch (...) // everything else
    {
        hr = E_FAIL;
    }

    return hr;
}

HRESULT
utils::convert_xbox_live_error_code_to_hresult(
    _In_ const std::error_code& errCode
    )
{
    int err = static_cast<int>(errCode.value());
    xbl_error_code xblErr = static_cast<xbl_error_code>(err);

    if (err == 204)
    {
        return __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND);
    }
    else if (err >= 300 && err <= 505)
    {
        return (HRESULT)convert_http_status_to_hresult(err);
    }
    else if (err >= 1000 && err <= 9999)
    {
        switch (xblErr)
        {
            case xbl_error_code::bad_alloc: return E_OUTOFMEMORY;
            case xbl_error_code::invalid_argument: return E_INVALIDARG;
            case xbl_error_code::runtime_error: return E_XBL_RUNTIME_ERROR;
            case xbl_error_code::length_error: return __HRESULT_FROM_WIN32(ERROR_BAD_LENGTH);
            case xbl_error_code::out_of_range: return E_BOUNDS;
            case xbl_error_code::range_error: return E_BOUNDS;
            case xbl_error_code::bad_cast: return E_NOINTERFACE;
            case xbl_error_code::logic_error: return E_UNEXPECTED;
            case xbl_error_code::json_error: return WEB_E_INVALID_JSON_STRING;
            case xbl_error_code::uri_error: return WEB_E_UNEXPECTED_CONTENT;
            case xbl_error_code::websocket_error: return WEB_E_UNEXPECTED_CONTENT;
            case xbl_error_code::auth_user_interaction_required: return ONL_E_ACTION_REQUIRED;
            case xbl_error_code::rta_generic_error: return E_XBL_RTA_GENERIC_ERROR;
            case xbl_error_code::rta_subscription_limit_reached: return E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED;
            case xbl_error_code::rta_access_denied: return E_XBL_RTA_ACCESS_DENIED;
            case xbl_error_code::auth_unknown_error: return E_XBL_AUTH_UNKNOWN_ERROR;
            case xbl_error_code::auth_runtime_error: return E_XBL_AUTH_RUNTIME_ERROR;
            case xbl_error_code::auth_no_token_error: return E_XBL_AUTH_NO_TOKEN;
            case xbl_error_code::auth_user_not_signed_in: return __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER);
            case xbl_error_code::auth_user_cancel: return __HRESULT_FROM_WIN32(ERROR_CANCELLED);
            case xbl_error_code::auth_user_switched: return __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER);
            case xbl_error_code::invalid_config: return __HRESULT_FROM_WIN32(ERROR_BAD_CONFIGURATION);
            case xbl_error_code::unsupported: return E_NOTIMPL;

            default: return E_FAIL;
        }
    }
    else if ((err & 0x87DD0000) == 0x87D8000)
    {
        return HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR;
    }
    else return err; //return the original error code if can't be translated.
}

xbl_error_code utils::convert_http_status_to_xbox_live_error_code(
    _In_ uint32_t statusCode
    )
{
    if (statusCode < 300 || statusCode >= 600)
    {
        // Treat as success so 
        //      if (!result.err()) 
        // works properly which requires all non-errors to be 0.
        return xbl_error_code::no_error;
    }
    else
    {
        return static_cast<xbl_error_code>(statusCode);
    }
}

HRESULT utils::convert_http_status_to_hresult(_In_ uint32_t httpStatusCode)
{
    xbl_error_code errCode = static_cast<xbl_error_code>(httpStatusCode);
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
#if HC_PLATFORM_IS_MICROSOFT
// TODO: remove
xsapi_internal_string utils::convert_hresult_to_error_name(_In_ long hr)
{
    switch (hr)
    {
        // Generic errors
    case S_OK: return "S_OK";
    case S_FALSE: return "S_FALSE";
    case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
    case E_ACCESSDENIED: return "E_ACCESSDENIED";
    case E_INVALIDARG: return "E_INVALIDARG";
    case E_UNEXPECTED: return "E_UNEXPECTED";
    case E_ABORT: return "E_ABORT";
    case E_FAIL: return "E_FAIL";
    case E_NOTIMPL: return "E_NOTIMPL";
    case E_ILLEGAL_METHOD_CALL: return "E_ILLEGAL_METHOD_CALL";

        // Authentication specific errors
    case 0x87DD0003: return "AM_E_XASD_UNEXPECTED";
    case 0x87DD0004: return "AM_E_XASU_UNEXPECTED";
    case 0x87DD0005: return "AM_E_XAST_UNEXPECTED";
    case 0x87DD0006: return "AM_E_XSTS_UNEXPECTED";
    case 0x87DD0007: return "AM_E_XDEVICE_UNEXPECTED";
    case 0x87DD0008: return "AM_E_DEVMODE_NOT_AUTHORIZED";
    case 0x87DD0009: return "AM_E_NOT_AUTHORIZED";
    case 0x87DD000A: return "AM_E_FORBIDDEN";
    case 0x87DD000B: return "AM_E_UNKNOWN_TARGET";
    case 0x87DD000C: return "AM_E_INVALID_NSAL_DATA";
    case 0x87DD000D: return "AM_E_TITLE_NOT_AUTHENTICATED";
    case 0x87DD000E: return "AM_E_TITLE_NOT_AUTHORIZED";
    case 0x87DD000F: return "AM_E_DEVICE_NOT_AUTHENTICATED";
    case 0x87DD0010: return "AM_E_INVALID_USER_INDEX";

    case 0x8015DC00: return "XO_E_DEVMODE_NOT_AUTHORIZED";
    case 0x8015DC01: return "XO_E_SYSTEM_UPDATE_REQUIRED";
    case 0x8015DC02: return "XO_E_CONTENT_UPDATE_REQUIRED";
    case 0x8015DC03: return "XO_E_ENFORCEMENT_BAN";
    case 0x8015DC04: return "XO_E_THIRD_PARTY_BAN";
    case 0x8015DC05: return "XO_E_ACCOUNT_PARENTALLY_RESTRICTED";
    case 0x8015DC06: return "XO_E_DEVICE_SUBSCRIPTION_NOT_ACTIVATED";
    case 0x8015DC08: return "XO_E_ACCOUNT_BILLING_MAINTENANCE_REQUIRED";
    case 0x8015DC09: return "XO_E_ACCOUNT_CREATION_REQUIRED";
    case 0x8015DC0A: return "XO_E_ACCOUNT_TERMS_OF_USE_NOT_ACCEPTED";
    case 0x8015DC0B: return "XO_E_ACCOUNT_COUNTRY_NOT_AUTHORIZED";
    case 0x8015DC0C: return "XO_E_ACCOUNT_AGE_VERIFICATION_REQUIRED";
    case 0x8015DC0D: return "XO_E_ACCOUNT_CURFEW";
    case 0x8015DC0E: return "XO_E_ACCOUNT_CHILD_NOT_IN_FAMILY";
    case 0x8015DC0F: return "XO_E_ACCOUNT_CSV_TRANSITION_REQUIRED";
    case 0x8015DC10: return "XO_E_ACCOUNT_MAINTENANCE_REQUIRED";
    case 0x8015DC11: return "XO_E_ACCOUNT_TYPE_NOT_ALLOWED"; // dev account on retail box
    case 0x8015DC12: return "XO_E_CONTENT_ISOLATION (Verify SCID / Sandbox)";
    case 0x8015DC13: return "XO_E_ACCOUNT_NAME_CHANGE_REQUIRED";
    case 0x8015DC14: return "XO_E_DEVICE_CHALLENGE_REQUIRED";
        // case 0x8015DC15: synthetic device type not allowed - does not apply to consoles
    case 0x8015DC16: return "XO_E_SIGNIN_COUNT_BY_DEVICE_TYPE_EXCEEDED";
    case 0x8015DC17: return "XO_E_PIN_CHALLENGE_REQUIRED";
    case 0x8015DC18: return "XO_E_RETAIL_ACCOUNT_NOT_ALLOWED"; // RETAIL account on devkit
    case 0x8015DC19: return "XO_E_SANDBOX_NOT_ALLOWED";
    case 0x8015DC1A: return "XO_E_ACCOUNT_SERVICE_UNAVAILABLE_UNKNOWN_USER";
    case 0x8015DC1B: return "XO_E_GREEN_SIGNED_CONTENT_NOT_AUTHORIZED";
    case 0x8015DC1C: return "XO_E_CONTENT_NOT_AUTHORIZED";

    case 0x8015DC20: return "XO_E_EXPIRED_DEVICE_TOKEN";
    case 0x8015DC21: return "XO_E_EXPIRED_TITLE_TOKEN";
    case 0x8015DC22: return "XO_E_EXPIRED_USER_TOKEN";
    case 0x8015DC23: return "XO_E_INVALID_DEVICE_TOKEN";
    case 0x8015DC24: return "XO_E_INVALID_TITLE_TOKEN";
    case 0x8015DC25: return "XO_E_INVALID_USER_TOKEN";

        // HTTP specific errors
    case WEB_E_UNSUPPORTED_FORMAT: return "WEB_E_UNSUPPORTED_FORMAT";
    case WEB_E_INVALID_XML: return "WEB_E_INVALID_XML";
    case WEB_E_MISSING_REQUIRED_ELEMENT: return "WEB_E_MISSING_REQUIRED_ELEMENT";
    case WEB_E_MISSING_REQUIRED_ATTRIBUTE: return "WEB_E_MISSING_REQUIRED_ATTRIBUTE";
    case WEB_E_UNEXPECTED_CONTENT: return "WEB_E_UNEXPECTED_CONTENT";
    case WEB_E_RESOURCE_TOO_LARGE: return "WEB_E_RESOURCE_TOO_LARGE";
    case WEB_E_INVALID_JSON_STRING: return "WEB_E_INVALID_JSON_STRING";
    case WEB_E_INVALID_JSON_NUMBER: return "WEB_E_INVALID_JSON_NUMBER";
    case WEB_E_JSON_VALUE_NOT_FOUND: return "WEB_E_JSON_VALUE_NOT_FOUND";
    case ERROR_RESOURCE_DATA_NOT_FOUND: return "ERROR_RESOURCE_DATA_NOT_FOUND";

    case HTTP_E_STATUS_UNEXPECTED: return "HTTP_E_STATUS_UNEXPECTED";
    case HTTP_E_STATUS_UNEXPECTED_REDIRECTION: return "HTTP_E_STATUS_UNEXPECTED_REDIRECTION";
    case HTTP_E_STATUS_UNEXPECTED_CLIENT_ERROR: return "HTTP_E_STATUS_UNEXPECTED_CLIENT_ERROR";
    case HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR: return "HTTP_E_STATUS_UNEXPECTED_SERVER_ERROR";
    case HTTP_E_STATUS_AMBIGUOUS: return "HTTP_E_STATUS_AMBIGUOUS";
    case HTTP_E_STATUS_MOVED: return "HTTP_E_STATUS_MOVED";
    case HTTP_E_STATUS_REDIRECT: return "HTTP_E_STATUS_REDIRECT";
    case HTTP_E_STATUS_REDIRECT_METHOD: return "HTTP_E_STATUS_REDIRECT_METHOD";
    case HTTP_E_STATUS_NOT_MODIFIED: return "HTTP_E_STATUS_NOT_MODIFIED";
    case HTTP_E_STATUS_USE_PROXY: return "HTTP_E_STATUS_USE_PROXY";
    case HTTP_E_STATUS_REDIRECT_KEEP_VERB: return "HTTP_E_STATUS_REDIRECT_KEEP_VERB";
    case HTTP_E_STATUS_BAD_REQUEST: return "HTTP_E_STATUS_BAD_REQUEST";
    case HTTP_E_STATUS_DENIED: return "HTTP_E_STATUS_DENIED";
    case HTTP_E_STATUS_PAYMENT_REQ: return "HTTP_E_STATUS_PAYMENT_REQ";
    case HTTP_E_STATUS_FORBIDDEN: return "HTTP_E_STATUS_FORBIDDEN";
    case HTTP_E_STATUS_NOT_FOUND: return "HTTP_E_STATUS_NOT_FOUND";
    case HTTP_E_STATUS_BAD_METHOD: return "HTTP_E_STATUS_BAD_METHOD";
    case HTTP_E_STATUS_NONE_ACCEPTABLE: return "HTTP_E_STATUS_NONE_ACCEPTABLE";
    case HTTP_E_STATUS_PROXY_AUTH_REQ: return "HTTP_E_STATUS_PROXY_AUTH_REQ";
    case HTTP_E_STATUS_REQUEST_TIMEOUT: return "HTTP_E_STATUS_REQUEST_TIMEOUT";
    case HTTP_E_STATUS_CONFLICT: return "HTTP_E_STATUS_CONFLICT";
    case HTTP_E_STATUS_GONE: return "HTTP_E_STATUS_GONE";
    case HTTP_E_STATUS_LENGTH_REQUIRED: return "HTTP_E_STATUS_LENGTH_REQUIRED";
    case HTTP_E_STATUS_PRECOND_FAILED: return "HTTP_E_STATUS_PRECOND_FAILED";
    case HTTP_E_STATUS_REQUEST_TOO_LARGE: return "HTTP_E_STATUS_REQUEST_TOO_LARGE";
    case HTTP_E_STATUS_URI_TOO_LONG: return "HTTP_E_STATUS_URI_TOO_LONG";
    case HTTP_E_STATUS_UNSUPPORTED_MEDIA: return "HTTP_E_STATUS_UNSUPPORTED_MEDIA";
    case HTTP_E_STATUS_RANGE_NOT_SATISFIABLE: return "HTTP_E_STATUS_RANGE_NOT_SATISFIABLE";
    case HTTP_E_STATUS_EXPECTATION_FAILED: return "HTTP_E_STATUS_EXPECTATION_FAILED";

    case MAKE_HTTP_HRESULT(421): return "HTTP_E_STATUS_421_MISDIRECTED_REQUEST";
    case MAKE_HTTP_HRESULT(422): return "HTTP_E_STATUS_422_UNPROCESSABLE_ENTITY";
    case MAKE_HTTP_HRESULT(423): return "HTTP_E_STATUS_423_LOCKED";
    case MAKE_HTTP_HRESULT(424): return "HTTP_E_STATUS_424_FAILED_DEPENDENCY";
    case MAKE_HTTP_HRESULT(426): return "HTTP_E_STATUS_426_UPGRADE_REQUIRED";
    case MAKE_HTTP_HRESULT(428): return "HTTP_E_STATUS_428_PRECONDITION_REQUIRED";
    case MAKE_HTTP_HRESULT(429): return "HTTP_E_STATUS_429_TOO_MANY_REQUESTS";
    case MAKE_HTTP_HRESULT(431): return "HTTP_E_STATUS_431_REQUEST_HEADER_FIELDS_TOO_LARGE";
    case MAKE_HTTP_HRESULT(449): return "HTTP_E_STATUS_449_RETRY_WITH";
    case MAKE_HTTP_HRESULT(451): return "HTTP_E_STATUS_451_UNAVAILABLE_FOR_LEGAL_REASONS";

    case HTTP_E_STATUS_SERVER_ERROR: return "HTTP_E_STATUS_SERVER_ERROR";
    case HTTP_E_STATUS_NOT_SUPPORTED: return "HTTP_E_STATUS_NOT_SUPPORTED";
    case HTTP_E_STATUS_BAD_GATEWAY: return "HTTP_E_STATUS_BAD_GATEWAY";
    case HTTP_E_STATUS_SERVICE_UNAVAIL: return "HTTP_E_STATUS_SERVICE_UNAVAIL";
    case HTTP_E_STATUS_GATEWAY_TIMEOUT: return "HTTP_E_STATUS_GATEWAY_TIMEOUT";
    case HTTP_E_STATUS_VERSION_NOT_SUP: return "HTTP_E_STATUS_VERSION_NOT_SUP";

    case MAKE_HTTP_HRESULT(506): return "HTTP_E_STATUS_506_VARIANT_ALSO_NEGOTIATES";
    case MAKE_HTTP_HRESULT(507): return "HTTP_E_STATUS_507_INSUFFICIENT_STORAGE";
    case MAKE_HTTP_HRESULT(508): return "HTTP_E_STATUS_508_LOOP_DETECTED";
    case MAKE_HTTP_HRESULT(510): return "HTTP_E_STATUS_510_NOT_EXTENDED";
    case MAKE_HTTP_HRESULT(511): return "HTTP_E_STATUS_511_NETWORK_AUTHENTICATION_REQUIRED";

        // WinINet specific errors
    case INET_E_INVALID_URL: return "INET_E_INVALID_URL";
    case INET_E_NO_SESSION: return "INET_E_NO_SESSION";
    case INET_E_CANNOT_CONNECT: return "INET_E_CANNOT_CONNECT";
    case INET_E_RESOURCE_NOT_FOUND: return "INET_E_RESOURCE_NOT_FOUND";
    case INET_E_OBJECT_NOT_FOUND: return "INET_E_OBJECT_NOT_FOUND";
    case INET_E_DATA_NOT_AVAILABLE: return "INET_E_DATA_NOT_AVAILABLE";
    case INET_E_DOWNLOAD_FAILURE: return "INET_E_DOWNLOAD_FAILURE";
    case INET_E_AUTHENTICATION_REQUIRED: return "INET_E_AUTHENTICATION_REQUIRED";
    case INET_E_NO_VALID_MEDIA: return "INET_E_NO_VALID_MEDIA";
    case INET_E_CONNECTION_TIMEOUT: return "INET_E_CONNECTION_TIMEOUT";
    case INET_E_INVALID_REQUEST: return "INET_E_INVALID_REQUEST";
    case INET_E_UNKNOWN_PROTOCOL: return "INET_E_UNKNOWN_PROTOCOL";
    case INET_E_SECURITY_PROBLEM: return "INET_E_SECURITY_PROBLEM";
    case INET_E_CANNOT_LOAD_DATA: return "INET_E_CANNOT_LOAD_DATA";
    case INET_E_CANNOT_INSTANTIATE_OBJECT: return "INET_E_CANNOT_INSTANTIATE_OBJECT";
    case INET_E_INVALID_CERTIFICATE: return "INET_E_INVALID_CERTIFICATE";
    case INET_E_REDIRECT_FAILED: return "INET_E_REDIRECT_FAILED";
    case INET_E_REDIRECT_TO_DIR: return "INET_E_REDIRECT_TO_DIR";
    }

    return "Unknown error";
}
#endif

xbox::services::xbl_error_code
utils::convert_exception_to_xbox_live_error_code()
{
    // Default value, if there is no exception appears, return no_error
    xbox::services::xbl_error_code errCode = xbl_error_code::no_error;

    try
    {
        throw;
    }
    // std exceptions
    catch (const std::bad_alloc&) // is an exception
    {
        errCode = xbl_error_code::bad_alloc;
    }
    catch (const std::bad_cast&) // is an exception
    {
        errCode = xbl_error_code::bad_cast;
    }
    catch (const std::invalid_argument&) // is a logic_error
    {
        errCode = xbl_error_code::invalid_argument;
    }
    catch (const std::out_of_range&) // is a logic_error
    {
        errCode = xbl_error_code::out_of_range;
    }
    catch (const std::length_error&) // is a logic_error
    {
        errCode = xbl_error_code::length_error;
    }
    catch (const std::range_error&) // is a runtime_error
    {
        errCode = xbl_error_code::range_error;
    }
    catch (const std::system_error& ex) // is a runtime_error
    {
        errCode = static_cast<xbl_error_code>(ex.code().value());
    }
    catch (const std::logic_error&) // is an exception
    {
        errCode = xbl_error_code::logic_error;
    }
    catch (const std::runtime_error&) // is an exception
    {
        errCode = xbl_error_code::runtime_error;
    }
#if !XSAPI_NO_PPL
    catch (const web::http::http_exception& ex) // is an exception
    {
        errCode = static_cast<xbl_error_code>(ex.error_code().value());
    }
#endif // !XSAPI_NO_PPL
    catch (const xbox::services::uri_exception&) // is an exception
    {
        errCode = xbl_error_code::uri_error;
    }
    catch (const std::exception&) // base class for standard C++ exceptions
    {
        errCode = xbl_error_code::generic_error;
    }
#if HC_PLATFORM_IS_MICROSOFT
    catch (HRESULT exceptionHR)
    {
        errCode = static_cast<xbl_error_code>(exceptionHR);
    }
#endif
    catch (...) // everything else
    {
        errCode = xbl_error_code::generic_error;
    }

    return errCode;
}

#if HC_PLATFORM_IS_MICROSOFT
std::error_code utils::guid_from_string(
    _In_ const string_t& str, 
    _In_ GUID* guid,
    _In_ bool withBraces
    )
{
    uint32_t data[3] = {0};
    uint32_t charData[8] = {0};

    auto n = swscanf_s(
        str.c_str(), 
        withBraces ? L"{%x-%x-%x-%2x%2x-%2x%2x%2x%2x%2x%2x}" : L"%x-%x-%x-%2x%2x-%2x%2x%2x%2x%2x%2x", 
        &data[0], 
        &data[1], 
        &data[2], 
        &charData[0], 
        &charData[1], 
        &charData[2], 
        &charData[3], 
        &charData[4], 
        &charData[5], 
        &charData[6], 
        &charData[7]
        );
    
    guid->Data1 = data[0];
    guid->Data2 = static_cast<uint8_t>(data[1]);
    guid->Data3 = static_cast<uint8_t>(data[2]);
    for(uint32_t i=0; i<8; i++ )
    {
        guid->Data4[i] = static_cast<uint8_t>(charData[i]);
    }

    return ( n == 11 ) ? xbl_error_code::no_error : xbl_error_code::logic_error;
}
#endif

xsapi_internal_string utils::create_guid(_In_ bool removeBraces)
{
#if HC_PLATFORM_IS_MICROSOFT
    GUID guid = {0};
    THROW_CPP_RUNTIME_IF(FAILED(CoCreateGuid(&guid)), "");

    WCHAR wszGuid[50];
    THROW_CPP_RUNTIME_IF(FAILED(::StringFromGUID2(
        guid,
        wszGuid,
        ARRAYSIZE(wszGuid)
        )), "");

    xsapi_internal_string strGuid = utils::internal_string_from_utf16(wszGuid);
#elif !HC_PLATFORM_IS_MICROSOFT
    xsapi_internal_string strGuid = generate_guid();
#else
    uuid_t uuid;
    uuid_generate_random(uuid);
    char s[37] = { 0 };
    uuid_unparse(uuid, s);
    string_t strGuid = s;
#endif

    if (removeBraces)
    {
        if (strGuid.length() > 3 && strGuid[0] == L'{')
        {
            // Remove the { } 
            strGuid.erase(0, 1);
            strGuid.erase(strGuid.end() - 1, strGuid.end());
        }
    }

    return strGuid;
}
String
utils::format_secure_device_address(String deviceAddress)
{
    if (deviceAddress.empty())
    {
        return "";
    }

    // A secure device address(SDA) is a legacy concept for UWP/Xbox One.
    // SDAs encapsulate the deviceToken which is used by MPSD to identify 
    // a session host and the connection address which is used by the title
    // to connect to the title.

    String formattedDeviceAddress = deviceAddress;
#if !(HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP)
    // SDAs that start with a 1 byte (\001) will be parsed differently in MPSD.
    // Since this platform does not have a valid SDA, we are adding a prefix
    // to ensure that the SDA is handled as a non-valid SDA.
    //
    // MPSD will base64 decode a non-valid SDA and the hashed value will be used
    // as the device token. The SDA can then be parsed by the title while 
    // deserializing the MPSD session to retrieve the connectionAddress.
    formattedDeviceAddress = _sdaPrefix + deviceAddress;
#endif

    Vector<unsigned char> input(formattedDeviceAddress.c_str(), formattedDeviceAddress.c_str() + formattedDeviceAddress.size());
    String sda = xbox::services::convert::to_base64(input);

    return sda;
}

String
utils::parse_secure_device_address(String secureDeviceAddress)
{
    if (secureDeviceAddress.empty())
    {
        return "";
    }

    // A secure device address(SDA) is a legacy concept for UWP/Xbox One.
    // SDAs encapsulate the deviceToken which is used by MPSD to identify 
    // a session host and the connection address which is used by the title
    // to connect to the title.

    std::vector<unsigned char> base64ConnectionAddress(xbox::services::convert::from_base64(secureDeviceAddress.c_str()));
    auto formattedDeviceAddress = String(base64ConnectionAddress.begin(), base64ConnectionAddress.end());

    String deviceAddress = formattedDeviceAddress;
#if !(HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP)
    if (deviceAddress.find(_sdaPrefix) == 0)
    {
        deviceAddress = deviceAddress.substr(strlen(_sdaPrefix));
    }
#endif

    return deviceAddress;
}

std::vector<string_t>
utils::string_split(
    _In_ const string_t& string,
    _In_ string_t::value_type seperator
    )
{
    std::vector<string_t> vSubStrings;

    if (!string.empty())
    {
        size_t posStart = 0, posFound = 0;
        while (posFound != string_t::npos && posStart < string.length())
        {
            posFound = string.find(seperator, posStart);
            if (posFound != string_t::npos)
            {
                if (posFound != posStart)
                {
                    // this substring is not empty
                    vSubStrings.push_back(string.substr(posStart, posFound - posStart));
                }
                posStart = posFound + 1;
            }
            else
            {
                vSubStrings.push_back(string.substr(posStart));
            }
        }
    }

    return vSubStrings;
}

xsapi_internal_vector<xsapi_internal_string> utils::string_split_internal(
    _In_ const xsapi_internal_string& string,
    _In_ xsapi_internal_string::value_type seperator
    )
{
    xsapi_internal_vector<xsapi_internal_string> vSubStrings;

    if (!string.empty())
    {
        size_t posStart = 0, posFound = 0;
        while (posFound != xsapi_internal_string::npos && posStart < string.length())
        {
            posFound = string.find(seperator, posStart);
            if (posFound != string_t::npos)
            {
                if (posFound != posStart)
                {
                    // this substring is not empty
                    vSubStrings.push_back(string.substr(posStart, posFound - posStart));
                }
                posStart = posFound + 1;
            }
            else
            {
                vSubStrings.push_back(string.substr(posStart));
            }
        }
    }

    return vSubStrings;
}

string_t utils::vector_join(
    _In_ const std::vector<string_t>& vector,
    _In_ string_t::value_type seperator
    )
{
    stringstream_t ss;
    
    if (!vector.empty())
    {
        string_t::value_type delimiter[2] = { seperator, 0 };
        std::copy(vector.begin(), vector.end() - 1, std::ostream_iterator<string_t, string_t::value_type>(ss, delimiter));
        ss << vector.back();
    }

    return ss.str();
}

xsapi_internal_string utils::vector_join_internal(
    _In_ const std::vector<xsapi_internal_string>& vector,
    _In_ xsapi_internal_string::value_type seperator
)
{
    xsapi_internal_stringstream ss;

    if (!vector.empty())
    {
        xsapi_internal_string::value_type delimiter[2] = { seperator, 0 };
        std::copy(vector.begin(), vector.end() - 1, std::ostream_iterator<xsapi_internal_string, xsapi_internal_string::value_type>(ss, delimiter));
        ss << vector.back();
    }

    return ss.str();
}

string_t
utils::replace_sub_string(
    _In_ const string_t& source,
    _In_ const string_t& pattern,
    _In_ const string_t& replacement
    )
{
    string_t result = source;
    // Search the string backward for the given pattern first
    size_t nPos = source.rfind(pattern);

    while (nPos != source.npos)
    {
        result.replace(nPos, pattern.length(), replacement);

        if (nPos == 0)
        {
            // There is nothing left to look at, break
            break;
        }

        // Find the next match starting from the last replaced position
        nPos = source.rfind(pattern, nPos - 1);
    }

    return result;
}

xsapi_internal_string_t utils::read_file_to_string(
    _In_ const xsapi_internal_string_t& filePath
    )
{
    std::ifstream in(filePath.c_str(), std::ios::in | std::ios::binary);
    if (in)
    {
        std::vector<char> fileData;
        in.seekg(0, std::ios::end);
        uint32_t fileSizeInBytes = static_cast<uint32_t>(in.tellg());
        if (fileSizeInBytes > 3)
        {
            fileData.resize(fileSizeInBytes);
            in.seekg(0, std::ios::beg);
            if (fileData.size() > 0)
            {
                in.read(&fileData[0], fileData.size());
            }
            in.close();

            bool isUtf16LE = 
                (static_cast<unsigned char>(fileData[0]) == 0xFF &&
                 static_cast<unsigned char>(fileData[1]) == 0xFE); // check for UTF-16 LE BOM

            bool isUtf8 = 
                (static_cast<unsigned char>(fileData[0]) == 0xEF && 
                 static_cast<unsigned char>(fileData[1]) == 0xBB && 
                 static_cast<unsigned char>(fileData[2]) == 0xBF); // check for UTF-8 BOM

            xsapi_internal_string_t fileDataString;
#ifdef WIN32            
            // Convert file data to UTF16 string
            if (isUtf16LE)
            {
                uint32_t byteOrderMarkSizeInBytes = 2;
                uint32_t strLength = (fileSizeInBytes - byteOrderMarkSizeInBytes) / sizeof(WCHAR);
                fileDataString = xsapi_internal_string_t(reinterpret_cast<WCHAR*>(fileData.data() + byteOrderMarkSizeInBytes), strLength);
            }
            else
            {
                int byteOrderMarkSizeInBytes = (isUtf8) ? 3 : 0;
                uint32_t strLength = fileSizeInBytes - byteOrderMarkSizeInBytes;
                xsapi_internal_string utf8FileData = xsapi_internal_string(fileData.data() + byteOrderMarkSizeInBytes, strLength);
                fileDataString = xbox::services::convert::utf8_to_utf16(utf8FileData);
            }
#else
            // Convert file data to UTF8 string
            if (isUtf16LE)
            {
                int byteOrderMarkSizeInBytes = 2;
                uint32_t strLength = (fileSizeInBytes - byteOrderMarkSizeInBytes) / sizeof(wchar_t);
                xsapi_internal_string_t utf16FileData = xsapi_internal_string_t(fileData.data(), strLength);
                fileDataString = utf16FileData;
            }
            else
            {
                int byteOrderMarkSizeInBytes = (isUtf8) ? 3 : 0;
                uint32_t strLength = fileSizeInBytes - byteOrderMarkSizeInBytes;
                fileDataString = xsapi_internal_string(fileData.data(), strLength);
            }
#endif
            return fileDataString;
        }
    }

    return xsapi_internal_string_t();
}

int utils::interlocked_increment(volatile long& incrementNum)
{
#if HC_PLATFORM_IS_MICROSOFT
    return InterlockedIncrement(&incrementNum);
#else
    return static_cast<uint32_t>(__sync_fetch_and_add(&incrementNum, 1));
#endif
}

int utils::interlocked_decrement(volatile long& decrementNum)
{
#if HC_PLATFORM_IS_MICROSOFT
    return InterlockedDecrement(&decrementNum);
#else
    return static_cast<uint32_t>(__sync_fetch_and_sub(&decrementNum, 1));
#endif
}

std::vector<string_t> utils::string_array_to_string_vector(
    const char* *stringArray,
    size_t stringArrayCount
    )
{
    std::vector<xbox::services::string_t> stringVector;
    for (size_t i = 0; i < stringArrayCount; ++i)
    {
        stringVector.push_back(string_t_from_utf8(stringArray[i]));
    }
    return stringVector;
}

xsapi_internal_vector<xsapi_internal_string> utils::string_array_to_internal_string_vector(
    const char* *stringArray,
    size_t stringArrayCount
    )
{
    xsapi_internal_vector<xsapi_internal_string> stringVector;
    stringVector.reserve(stringArrayCount);
    for (size_t i = 0; i < stringArrayCount; ++i)
    {
        stringVector.push_back(stringArray[i]);
    }
    return stringVector;
}

xsapi_internal_vector<xsapi_internal_string> utils::xuid_array_to_internal_string_vector(
    uint64_t* xuidArray,
    size_t xuidArrayCount
    )
{
    xsapi_internal_vector<xsapi_internal_string> stringVector;
    stringVector.reserve(xuidArrayCount);
    for (size_t i = 0; i < xuidArrayCount; ++i)
    {
        stringVector.push_back(utils::uint64_to_internal_string(xuidArray[i]));
    }
    return stringVector;
}

xsapi_internal_vector<uint32_t> utils::uint32_array_to_internal_vector(
    uint32_t* intArray,
    size_t intArrayCount
    )
{
    xsapi_internal_vector<uint32_t> vector;
    vector.reserve(intArrayCount);
    for (size_t i = 0; i < intArrayCount; ++i)
    {
        vector.push_back(intArray[i]);
    }
    return vector;
}

bool utils::EnsureLessThanMaxLength(const char* str, size_t maxLength)
{
    size_t i = 0;
    while (true)
    {
        if (i >= maxLength)
        {
            return false;
        }
        if (str[i] == '\0')
        {
            return true;
        }
        i++;
    }

    return false;
}

String utils::ToLower(String str) noexcept
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
        {
            return static_cast<char>(tolower(c));
        });

    return str;
}

XAsyncBlock* utils::MakeAsyncBlock(XTaskQueueHandle queue, void* context, XAsyncCompletionRoutine* callback)
{
    auto async = Make<XAsyncBlock>();
    async->queue = queue;
    async->context = context;
    async->callback = callback;
    return async;
}

static void CALLBACK s_defaultAsyncBlockCallback(XAsyncBlock* async)
{
    Delete(async);
}

XAsyncBlock* utils::MakeDefaultAsyncBlock(XTaskQueueHandle queue)
{
    return MakeAsyncBlock(queue, nullptr, s_defaultAsyncBlockCallback);
}

time_t utils::time_t_from_datetime(const xbox::services::datetime& datetime)
{
    uint64_t seconds = datetime.to_interval() / _secondTicks;
    if (seconds >= 11644473600LL)
    {
        return (time_t)(seconds - 11644473600LL);
    }
    else
    {
        // If time is before epoch, 0 is returned.
        return 0;
    }
}

#if HC_PLATFORM_IS_MICROSOFT
xsapi_internal_string utils::internal_string_from_utf16(_In_z_ const wchar_t* utf16)
{
    return internal_string_from_char_t(utf16);
}
#endif

#if XSAPI_WRL_EVENTS_SERVICE
Microsoft::WRL::Wrappers::HString utils::HStringFromUtf8(_In_z_ const char* utf8)
{
    auto cchOutString = char_t_from_utf8(utf8, nullptr, 0);
    xsapi_internal_wstring wstring(cchOutString - 1, '\0');
    char_t_from_utf8(utf8, &wstring[0], cchOutString);

    Microsoft::WRL::Wrappers::HString hstring;
    hstring.Set(wstring.data());
    return hstring;
}
#endif

#if __cplusplus_winrt
Platform::String^ utils::PlatformStringFromUtf8(_In_z_ const char* utf8)
{
    auto cchOutString = char_t_from_utf8(utf8, nullptr, 0);
    xsapi_internal_wstring wstr(cchOutString - 1, '\0');
    char_t_from_utf8(utf8, &wstr[0], cchOutString);
    return ref new Platform::String(wstr.data());
}
#endif

std::string utils::std_string_from_string_t(_In_ const string_t& stringt)
{
#if HC_PLATFORM_IS_MICROSOFT
    auto cchOutString = utf8_from_char_t(stringt.data(), nullptr, 0);
    std::string out(static_cast<size_t>(cchOutString) - 1, '\0');
    utf8_from_char_t(stringt.data(), &out[0], cchOutString);
    return out;
#else
    return std::string(stringt.data());
#endif
}

xsapi_internal_string utils::internal_string_from_char_t(_In_ const char_t* char_t)
{
#if HC_PLATFORM_IS_MICROSOFT
    auto cchOutString = utf8_from_char_t(char_t, nullptr, 0);
    xsapi_internal_string out(static_cast<size_t>(cchOutString) - 1, '\0');
    utf8_from_char_t(char_t, &out[0], cchOutString);
    return out;
#else
    return xsapi_internal_string(char_t);
#endif
}

string_t utils::string_t_from_internal_string(_In_ const xsapi_internal_string& internalString)
{
#if HC_PLATFORM_IS_MICROSOFT
    return string_t_from_utf8(internalString.data());
#else
    return string_t(internalString.c_str());
#endif
}

string_t utils::string_t_from_utf8(_In_z_ const char* utf8)
{
#if HC_PLATFORM_IS_MICROSOFT
    auto cchOutString = char_t_from_utf8(utf8, nullptr, 0);
    string_t out(static_cast<size_t>(cchOutString) - 1, '\0');
    char_t_from_utf8(utf8, &out[0], cchOutString);
    return out;
#else
    return string_t(utf8);
#endif
}

xsapi_internal_string utils::internal_string_from_string_t(_In_ const string_t& externalString)
{
#if HC_PLATFORM_IS_MICROSOFT
    return internal_string_from_utf16(externalString.c_str());
#else
    return xsapi_internal_string(externalString.c_str());
#endif
}

int utils::utf8_from_char_t(
    _In_z_ const char_t* inArray, 
    _Out_writes_z_(cchOutArray) char* outArray,
    _In_ int cchOutArray
    )
{
#if HC_PLATFORM_IS_MICROSOFT
    // query for the buffer size
    auto queryResult = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        inArray, -1,
        nullptr, 0,
        nullptr, nullptr
    );

    if (queryResult > cchOutArray && cchOutArray == 0)
    {
        return queryResult;
    }
    else if (queryResult == 0 || queryResult > cchOutArray)
    {
        throw std::exception("utf8_from_char_t failed");
    }

    auto conversionResult = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        inArray, -1,
        outArray, cchOutArray,
        nullptr, nullptr
    );
    if (conversionResult == 0)
    {
        throw std::exception("utf8_from_char_t failed");
    }

    return conversionResult;
#else
    int len = (int)strlen(inArray);
    if (len < cchOutArray && outArray != nullptr)
    {
        strcpy(outArray, len + 1, inArray);
    }
    else if (cchOutArray > 0)
    {
        return 0;
    }
    return len + 1;
#endif
}

int utils::char_t_from_utf8(
    _In_z_ const char* inArray,
    _Out_writes_z_(cchOutArray) char_t* outArray,
    _In_ int cchOutArray
    )
{
#if HC_PLATFORM_IS_MICROSOFT
    // query for the buffer size
    auto queryResult = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        inArray, -1,
        nullptr, 0
    );

    if (queryResult > cchOutArray && cchOutArray == 0)
    {
        return queryResult;
    }
    else if (queryResult == 0 || queryResult > cchOutArray)
    {
        throw std::exception("char_t_from_utf8 failed");
    }

    auto conversionResult = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        inArray, -1,
        outArray, cchOutArray
    );
    if (conversionResult == 0)
    {
        throw std::exception("char_t_from_utf8 failed");
    }

    return conversionResult;
#else
    int len = (int)strlen(inArray);
    if (len < cchOutArray && outArray != nullptr)
    {
        strcpy(outArray, len + 1, inArray);
    }
    else if (cchOutArray > 0)
    {
        return 0;
    }
    return len + 1;
#endif
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
