#include "pch.h"

#define MS_TICKS        (10000)
#define SECOND_TICKS    (1000 * MS_TICKS)
#define MINUTE_TICKS    (60 * SECOND_TICKS)
#define HOUR_TICKS      (60 * MINUTE_TICKS)
#define DAY_TICKS       (24 * HOUR_TICKS)

using namespace xbox::services::cppresturi::utility;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbl_error_code ConvertHrToXblErrorCode(HRESULT hr)
{
    switch (hr)
    {
        case S_OK: return xbl_error_code::no_error;
        case E_OUTOFMEMORY: return xbl_error_code::bad_alloc;
        case E_INVALIDARG: return xbl_error_code::invalid_argument;
        case E_XBL_RUNTIME_ERROR: return xbl_error_code::runtime_error;
        case __HRESULT_FROM_WIN32(ERROR_BAD_LENGTH): return xbl_error_code::length_error;
        case E_BOUNDS: return xbl_error_code::out_of_range;

        case E_NOINTERFACE: return xbl_error_code::bad_cast;
        case E_UNEXPECTED: return xbl_error_code::logic_error;
        case WEB_E_INVALID_JSON_STRING: return xbl_error_code::json_error;
        case WEB_E_UNEXPECTED_CONTENT: return xbl_error_code::uri_error;
        case ONL_E_ACTION_REQUIRED: return xbl_error_code::auth_user_interaction_required;
        case E_XBL_RTA_GENERIC_ERROR: return xbl_error_code::rta_generic_error;
        case E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED: return xbl_error_code::rta_subscription_limit_reached;
        case E_XBL_RTA_ACCESS_DENIED: return xbl_error_code::rta_access_denied;
        case E_XBL_RTA_NOT_ACTIVATED: return xbl_error_code::rta_not_activated;
        case E_XBL_AUTH_UNKNOWN_ERROR: return xbl_error_code::auth_unknown_error;
        case E_XBL_AUTH_RUNTIME_ERROR: return xbl_error_code::auth_runtime_error;
        case E_XBL_AUTH_NO_TOKEN: return xbl_error_code::auth_no_token_error;
        case __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER): return xbl_error_code::auth_user_not_signed_in;
        case __HRESULT_FROM_WIN32(ERROR_CANCELLED): return xbl_error_code::auth_user_cancel;
        case __HRESULT_FROM_WIN32(ERROR_BAD_CONFIGURATION): return xbl_error_code::invalid_config;
        case E_NOTIMPL: return xbl_error_code::unsupported;

            // HTTP errors
        case __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND): return xbl_error_code::http_status_204_resource_data_not_found;
        case HTTP_E_STATUS_AMBIGUOUS: return xbl_error_code::http_status_300_multiple_choices;
        case HTTP_E_STATUS_MOVED: return xbl_error_code::http_status_301_moved_permanently;
        case HTTP_E_STATUS_REDIRECT: return xbl_error_code::http_status_302_found;
        case HTTP_E_STATUS_REDIRECT_METHOD: return xbl_error_code::http_status_303_see_other;
        case HTTP_E_STATUS_NOT_MODIFIED: return xbl_error_code::http_status_304_not_modified;
        case HTTP_E_STATUS_USE_PROXY: return xbl_error_code::http_status_305_use_proxy;
        case HTTP_E_STATUS_REDIRECT_KEEP_VERB: return xbl_error_code::http_status_307_temporary_redirect;

        case HTTP_E_STATUS_BAD_REQUEST: return xbl_error_code::http_status_400_bad_request;
        case HTTP_E_STATUS_DENIED: return xbl_error_code::http_status_401_unauthorized;
        case HTTP_E_STATUS_PAYMENT_REQ: return xbl_error_code::http_status_402_payment_required;
        case HTTP_E_STATUS_FORBIDDEN: return xbl_error_code::http_status_403_forbidden;
        case HTTP_E_STATUS_NOT_FOUND: return xbl_error_code::http_status_404_not_found;
        case HTTP_E_STATUS_BAD_METHOD: return xbl_error_code::http_status_405_method_not_allowed;
        case HTTP_E_STATUS_NONE_ACCEPTABLE: return xbl_error_code::http_status_406_not_acceptable;
        case HTTP_E_STATUS_PROXY_AUTH_REQ: return xbl_error_code::http_status_407_proxy_authentication_required;
        case HTTP_E_STATUS_REQUEST_TIMEOUT: return xbl_error_code::http_status_408_request_timeout;
        case HTTP_E_STATUS_CONFLICT: return xbl_error_code::http_status_409_conflict;
        case HTTP_E_STATUS_GONE: return xbl_error_code::http_status_410_gone;
        case HTTP_E_STATUS_LENGTH_REQUIRED: return xbl_error_code::http_status_411_length_required;
        case HTTP_E_STATUS_PRECOND_FAILED: return xbl_error_code::http_status_412_precondition_failed;
        case HTTP_E_STATUS_REQUEST_TOO_LARGE: return xbl_error_code::http_status_413_request_entity_too_large;
        case HTTP_E_STATUS_URI_TOO_LONG: return xbl_error_code::http_status_414_request_uri_too_long;
        case HTTP_E_STATUS_UNSUPPORTED_MEDIA: return xbl_error_code::http_status_415_unsupported_media_type;
        case HTTP_E_STATUS_RANGE_NOT_SATISFIABLE: return xbl_error_code::http_status_416_requested_range_not_satisfiable;
        case HTTP_E_STATUS_EXPECTATION_FAILED: return xbl_error_code::http_status_417_expectation_failed;
        case MAKE_HTTP_HRESULT(421): return xbl_error_code::http_status_421_misdirected_request;
        case MAKE_HTTP_HRESULT(422): return xbl_error_code::http_status_422_unprocessable_entity;
        case MAKE_HTTP_HRESULT(423): return xbl_error_code::http_status_423_locked;
        case MAKE_HTTP_HRESULT(424): return xbl_error_code::http_status_424_failed_dependency;
        case MAKE_HTTP_HRESULT(426): return xbl_error_code::http_status_426_upgrade_required;
        case MAKE_HTTP_HRESULT(428): return xbl_error_code::http_status_428_precondition_required;
        case MAKE_HTTP_HRESULT(429): return xbl_error_code::http_status_429_too_many_requests;
        case MAKE_HTTP_HRESULT(431): return xbl_error_code::http_status_431_request_header_fields_too_large;
        case MAKE_HTTP_HRESULT(449): return xbl_error_code::http_status_449_retry_with;
        case MAKE_HTTP_HRESULT(451): return xbl_error_code::http_status_451_unavailable_for_legal_reasons;

        case HTTP_E_STATUS_SERVER_ERROR: return xbl_error_code::http_status_500_internal_server_error;
        case HTTP_E_STATUS_NOT_SUPPORTED: return xbl_error_code::http_status_501_not_implemented;
        case HTTP_E_STATUS_BAD_GATEWAY: return xbl_error_code::http_status_502_bad_gateway;
        case HTTP_E_STATUS_SERVICE_UNAVAIL: return xbl_error_code::http_status_503_service_unavailable;
        case HTTP_E_STATUS_GATEWAY_TIMEOUT: return xbl_error_code::http_status_504_gateway_timeout;
        case HTTP_E_STATUS_VERSION_NOT_SUP: return xbl_error_code::http_status_505_http_version_not_supported;
        case MAKE_HTTP_HRESULT(506): return xbl_error_code::http_status_506_variant_also_negotiates;
        case MAKE_HTTP_HRESULT(507): return xbl_error_code::http_status_507_insufficient_storage;
        case MAKE_HTTP_HRESULT(508): return xbl_error_code::http_status_508_loop_detected;
        case MAKE_HTTP_HRESULT(510): return xbl_error_code::http_status_510_not_extended;
        case MAKE_HTTP_HRESULT(511): return xbl_error_code::http_status_511_network_authentication_required;

        default: return xbl_error_code::generic_error;
    }
}

namespace legacy
{
    string_t StringTFromUtf8(_In_z_ const char* utf8)
    {
        if (utf8 == nullptr)
        {
            return string_t();
        }

#if HC_PLATFORM_IS_MICROSOFT
        auto cchOutString = CharTFromUft8(utf8, nullptr, 0);
        string_t out(static_cast<size_t>(cchOutString) - 1, '\0');
        CharTFromUft8(utf8, &out[0], cchOutString);
        return out;
#else
        return string_t(utf8);
#endif
    }

    std::string StringFromStringT(_In_ const string_t& stringt)
    {
#if HC_PLATFORM_IS_MICROSOFT
        auto cchOutString = Utf8FromCharT(stringt.data(), nullptr, 0);
        std::string out(static_cast<size_t>(cchOutString) - 1, '\0');
        Utf8FromCharT(stringt.data(), &out[0], cchOutString);
        return out;
#else
        return std::string(stringt.data());
#endif
    }

    int Utf8FromCharT(
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
            strlcpy(outArray, inArray, len + 1);
        }
        else if (cchOutArray > 0)
        {
            return 0;
        }
        return len + 1;
#endif
    }

    int CharTFromUft8(
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
            strlcpy(outArray, inArray, len + 1);
        }
        else if (cchOutArray > 0)
        {
            return 0;
        }
        return len + 1;
#endif
    }

    size_t CopyUtf8(
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

    string_t StringTFromUint64(_In_ uint64_t val)
    {
        stringstream_t ss;
        ss << val;
        return ss.str();
    }

    std::string StringFromUint64(_In_ uint64_t val)
    {
        std::stringstream ss;
        ss << val;
        return ss.str();
    }

    uint64_t Uint64FromStringT(_In_ const string_t& str)
    {
#if HC_PLATFORM_IS_MICROSOFT
        return _wtoi64(str.data());
#else
        return strtoull(str.data(), nullptr, 0);
#endif
    }

    int Stricmp(const char* left, const char* right) noexcept
    {
#if HC_PLATFORM_IS_MICROSOFT
        return _stricmp(left, right);
#else
        return strcasecmp(left, right);
#endif
    }

    int Stricmp(const string_t& left, const string_t& right)
    {
#if HC_PLATFORM_IS_MICROSOFT
        return _wcsicmp(left.data(), right.data());
#else
        return strcasecmp(left.data(), right.data());
#endif 
    }

    std::string SerializeJson(const rapidjson::Value& json)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);

        return buffer.GetString();
    }

    const rapidjson::Value& ExtractJsonField(
        _In_ const rapidjson::Value& json,
        _In_ const std::string& name,
        _In_ bool required
    )
    {
        if (json.IsObject())
        {
            auto it = json.FindMember(name.c_str());
            if (it != json.MemberEnd())
            {
                return it->value;
            }
        }

        if (required)
        {
            //TODO: Throw exception
        }

        return json;
    }

    uint64_t ExtractJsonUint64(
        _In_ const rapidjson::Value& jsonValue,
        _In_ const std::string& name,
        _In_ bool required,
        _In_ uint64_t defaultValue
    )
    {
        const rapidjson::Value& field(ExtractJsonField(jsonValue, name, required));
        if (!field.IsNumber() && !required)
        {
            return defaultValue;
        }
        return field.GetUint64();
    }

    std::vector<string_t> XuidStringVectorFromXuidArray(const uint64_t* xuids, size_t xuidsCount)
    {
        return Transform<string_t>(xuids, xuidsCount, StringTFromUint64);
    }

    std::vector<uint64_t> XuidVectorFromXuidStringVector(const std::vector<string_t>& xuidStrings)
    {
        return Transform<uint64_t>(xuidStrings, Uint64FromStringT);
    }

    std::vector<string_t> StringTVectorFromCStringArray(const char** stringArray, size_t arrayCount)
    {
        return Transform<string_t>(stringArray, arrayCount, StringTFromUtf8);
    }

    xbox::services::cppresturi::utility::datetime DatetimeFromTimeT(time_t time)
    {
        const uint64_t epoch_offset = 11644473600LL;
        uint64_t result = epoch_offset + time;
        result *= SECOND_TICKS; // convert to 10e-7
        return xbox::services::cppresturi::utility::datetime() + result;
    }

    time_t TimeTFromDatetime(const xbox::services::cppresturi::utility::datetime& datetime)
    {
        const uint64_t epoch_offset = 11644473600LL;
        uint64_t seconds = datetime.to_interval() / SECOND_TICKS;
        if (seconds >= epoch_offset)
        {
            return (time_t)(seconds - epoch_offset);
        }
        else
        {
            // If time is before epoch, 0 is returned.
            return 0;
        }
    }

    char_t ToLower(char_t c)
    {
        return std::tolower<char_t>(c, std::locale());
    }

    std::error_code ConvertHr(HRESULT hr)
    {
        return make_error_code(ConvertHrToXblErrorCode(hr));
    }

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END