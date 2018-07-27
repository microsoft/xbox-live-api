// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "local_config.h"
#include "xbox_live_app_config_internal.h"
#include <iomanip>
#include <chrono>
#include <time.h>
#if XSAPI_U
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "a/utils_a.h"
#elif XSAPI_CPP || defined _WIN32
#include <objbase.h>
#endif
#include "xsapi/presence.h"
#include "xsapi/system.h"
#include "presence_internal.h"
#include "initiator.h"
#include "httpClient/httpClient.h"

#if UWP_API
#ifdef _WINRT_DLL
#include "WinRT/User_WinRT.h"
#endif
#endif

HC_DEFINE_TRACE_AREA(XSAPI, HCTraceLevel_Error);

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#define MAKE_HTTP_HRESULT(code) MAKE_HRESULT(1, 0x019, code)
#if UNIT_TEST_SERVICES
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#endif

static const uint64_t _msTicks = static_cast<uint64_t>(10000);
static const uint64_t _secondTicks = 1000*_msTicks;

static std::mutex s_xsapiSingletonLock;
static std::shared_ptr<xsapi_singleton> s_xsapiSingleton;

xsapi_singleton::xsapi_singleton()
#if !TV_API
    : m_presenceWriterSingleton(std::shared_ptr<xbox::services::presence::presence_writer>(new xbox::services::presence::presence_writer()))
#endif
{
#if TV_API || UNIT_TEST_SERVICES
    m_bHasAchievementServiceInitialized = false;
    memset(&m_eventPlayerSessionId, 0, sizeof(m_eventPlayerSessionId));
#endif

    m_locales = "en-US";
    m_custom_locale_override = false;
    m_loggerId = 0;
    m_responseCount = 0;
    m_multiplayerClientPendingRequestUniqueIdentifier = 0;

#if !TV_API
    m_signOutCompletedHandlerIndexer = 0;
    m_signInCompletedHandlerIndexer = 0;
#endif

#if UWP_API
    m_trackingUsers = std::unordered_map<string_t, std::shared_ptr<system::user_impl_idp>>();
#endif
}

XblMemAllocFunction g_pMemAllocHook = nullptr;
XblMemFreeFunction g_pMemFreeHook = nullptr;

void init_mem_hooks()
{
    if (g_pMemAllocHook == nullptr || g_pMemFreeHook == nullptr)
    {
        g_pMemAllocHook = [](size_t size, hc_memory_type memoryType)
        {
            UNREFERENCED_PARAMETER(memoryType);
            if (size > 0)
            {
                return malloc(size);
            }
            return static_cast<void*>(nullptr);
        };

        g_pMemFreeHook = [](void *pointer, hc_memory_type memoryType)
        {
            UNREFERENCED_PARAMETER(memoryType);
            free(pointer);
        };
    }
}

void xsapi_singleton::init()
{
#if UWP_API
#ifdef _WINRT_DLL
    m_userEventBind = std::make_shared<Microsoft::Xbox::Services::System::UserEventBind>();
#endif
#endif

#if _DEBUG && UNIT_TEST_SERVICES
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    HCInitialize(nullptr);
    m_initiator = std::make_shared<initiator>();

    CreateAsyncQueue(AsyncQueueDispatchMode_ThreadPool, AsyncQueueDispatchMode_ThreadPool, &m_asyncQueue);
}

xsapi_singleton::~xsapi_singleton()
{
    CloseAsyncQueue(m_asyncQueue);
}

std::shared_ptr<xsapi_singleton>
get_xsapi_singleton(_In_ bool createIfRequired)
{
    if (createIfRequired && s_xsapiSingleton == nullptr)
    {
        std::lock_guard<std::mutex> guard(s_xsapiSingletonLock);
        if (s_xsapiSingleton == nullptr)
        {
            s_xsapiSingleton = std::make_shared<xsapi_singleton>();
            s_xsapiSingleton->init();
        }
    }

    return s_xsapiSingleton;
}

void cleanup_xsapi_singleton()
{
    {
        std::lock_guard<std::mutex> guard(s_xsapiSingletonLock);
        std::shared_ptr<xsapi_singleton> xsapiSingleton;
        xsapiSingleton = std::atomic_exchange(&s_xsapiSingleton, xsapiSingleton);

        if (xsapiSingleton != nullptr)
        {
            // Wait for all other references to the singleton to go away
            // Note that the use count check here is only valid because we never create
            // a weak_ptr to the singleton. If we did that could cause the use count
            // to increase even though we are the only strong reference
            while (xsapiSingleton.use_count() > 1)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
            }
            // xsapiSingleton will be destroyed on this thread now
        }
    }

    // Don't call HCCleanup until after singleton is destroyed since some of the singleton's
    // state depends on HC state.
    HCCleanup();
}

void verify_global_init()
{
    std::lock_guard<std::mutex> guard(s_xsapiSingletonLock);
    if (s_xsapiSingleton == nullptr)
    {
        assert(s_xsapiSingleton != nullptr);
    }
}

web::json::value utils::extract_json_field(
    _In_ const web::json::value& json, 
    _In_ const string_t& name, 
    _Inout_ std::error_code& error,
    _In_ bool required
    )
{
    if (json.is_object())
    {
        auto& jsonObj = json.as_object();
        auto it = jsonObj.find(name);
        if (it != jsonObj.end())
        {
            return it->second;
        }
    }

    if (required)
    {
        error = xbox_live_error_code::json_error;
    }

    return web::json::value::null();
}

web::json::value utils::extract_json_field(
    _In_ const web::json::value& json,
    _In_ const xsapi_internal_string& name,
    _Inout_ std::error_code& error,
    _In_ bool required
    )
{
    if (json.is_object())
    {
        auto& jsonObj = json.as_object();
        auto it = jsonObj.find(utils::string_t_from_internal_string(name));
        if (it != jsonObj.end())
        {
            return it->second;
        }
    }

    if (required)
    {
        error = xbox_live_error_code::json_error;
    }

    return web::json::value::null();
}

web::json::value utils::extract_json_field(
    _In_ const web::json::value& json, 
    _In_ const string_t& name, 
    _In_ bool required
    )
{
    if (json.is_object())
    {
        auto& jsonObj = json.as_object();
        auto it = jsonObj.find(name);
        if (it != jsonObj.end())
        {
            return it->second;
        }
    }

    if (required)
    {
        utility::stringstream_t ss;
        ss << name;
        ss << " not found";
        throw web::json::json_exception(ss.str().c_str());
    }

    return web::json::value::null();
}

web::json::value utils::extract_json_field(
    _In_ const web::json::value& json,
    _In_ const xsapi_internal_string& name,
    _In_ bool required
    )
{
    if (json.is_object())
    {
        auto& jsonObj = json.as_object();
        auto it = jsonObj.find(utils::string_t_from_internal_string(name));
        if (it != jsonObj.end())
        {
            return it->second;
        }
    }

    if (required)
    {
        xsapi_internal_stringstream ss;
        ss << name;
        ss << " not found";
        throw web::json::json_exception(utils::string_t_from_internal_string(ss.str()).c_str());
    }

    return web::json::value::null();
}


std::vector<string_t> utils::extract_json_string_vector(
    _In_ const web::json::value& json,
    _In_ const string_t& name,
    _Inout_ std::error_code& error,
    _In_ bool required
    )
{
    return extract_json_string_vector(
        extract_json_field(json, name, error, required), 
        error, 
        required
        );
}

std::vector<string_t> utils::extract_json_string_vector(
    _In_ const web::json::value& json,
    _In_ const string_t& name,
    _In_ bool required
    )
{
    return extract_json_string_vector(
        extract_json_field(json, name, required),
        required
        );
}

std::vector<string_t> utils::extract_json_string_vector(
    _In_ const web::json::value& json,
    _Inout_ std::error_code& error,
    _In_ bool required
)
{
    std::vector<string_t> result;

    if ((!json.is_array()) || error)
    {
        if (required)
        {
            error = xbox_live_error_code::json_error;
        }

        return result;
    }

    const web::json::array& arr(json.as_array());
    for (const auto& string : arr)
    {
        if (!string.is_string())
        {
            if (required)
            {
                error = xbox_live_error_code::json_error;
            }
            return result;
        }
        result.push_back(string.as_string());
    }
    return result;
}

std::vector<string_t> utils::extract_json_string_vector(
    _In_ const web::json::value& json,
    _In_ bool required
)
{
    std::vector<string_t> result;

    if (!json.is_array() && !required)
    {
        return result;
    }

    const web::json::array& arr(json.as_array());
    for (const auto& string : arr)
    {
        if (!string.is_string() && !required)
        {
            return result;
        }
        result.push_back(string.as_string());
    }
    return result;
}


web::json::value utils::json_get_value_from_string(_In_ const string_t& value)
{
    std::error_code error;
    web::json::value inputObj = web::json::value::parse(value, error);
    if (error.value() > 0)
    {
        inputObj = web::json::value(value);
    }

    return inputObj;
}

xbox_live_result<string_t> utils::json_string_extractor(_In_ const web::json::value& json)
{
    if (!json.is_string())
    {
        return xbox_live_result<string_t>(xbox_live_error_code::json_error, "JSON being deserialized is not a string");
    }
    return xbox_live_result<string_t>(json.as_string());
}

xbox_live_result<xsapi_internal_string> utils::json_internal_string_extractor(_In_ const web::json::value& json)
{
    if (!json.is_string())
    {
        return xbox_live_result<xsapi_internal_string>(xbox_live_error_code::json_error, "JSON being deserialized is not a string");
    }
    return xbox_live_result<xsapi_internal_string>(utils::internal_string_from_string_t(json.as_string()));
}

web::json::value utils::json_string_serializer(_In_ const string_t& value)
{
    return web::json::value::string(value);
}

web::json::value utils::json_internal_string_serializer(_In_ const xsapi_internal_string& value)
{
    auto v = utils::string_t_from_internal_string(value);
    return web::json::value::string(v);
}

xbox_live_result<int>
utils::json_int_extractor(_In_ const web::json::value& json)
{
    if (!json.is_integer())
    {
        return xbox_live_result<int>(xbox_live_error_code::json_error, "JSON being deserialized is not an integer");
    }
    return xbox_live_result<int>(json.as_integer());
}

web::json::value
utils::json_int_serializer(_In_ int32_t value)
{
    return web::json::value(value);
}

string_t utils::extract_json_string(
    _In_ const web::json::value& jsonValue, 
    _In_ const string_t& stringName,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ const string_t& defaultValue
    )
{
    web::json::value field(utils::extract_json_field(jsonValue, stringName, error, required));
    if ((!field.is_string() && !required) || field.is_null()) { return defaultValue; }
    return field.as_string();
}

xsapi_internal_string utils::extract_json_string(
    _In_ const web::json::value& jsonValue,
    _In_ const xsapi_internal_string& stringName,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ const xsapi_internal_string& defaultValue
    )
{
    web::json::value field(utils::extract_json_field(jsonValue, stringName, error, required));
    if ((!field.is_string() && !required) || field.is_null()) { return defaultValue; }
    return utils::internal_string_from_string_t(field.as_string());
}

void
utils::extract_json_string_to_char_t_array(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& stringName,
    _Inout_ std::error_code& error,
    _In_reads_bytes_(size) char_t* charArr,
    _In_ size_t size
    )
{
    auto jsonStr = utils::extract_json_string(jsonValue, stringName, error);
    uint32_t errorCode = 0;
    
    auto strSize = __min(size - 1, jsonStr.size());
#ifdef _WIN32
    wcsncpy_s(&charArr[0], size, jsonStr.c_str(), strSize);
#else
    strncpy(&charArr[0], jsonStr.c_str(), size);
#endif
    if (errorCode)
    {
        error = xbox_live_error_code::json_error;
    }
}

string_t utils::extract_json_string(
    _In_ const web::json::value& jsonValue, 
    _In_ const string_t& stringName, 
    _In_ bool required,
    _In_ const string_t& defaultValue
    )
{
    web::json::value field(utils::extract_json_field(jsonValue, stringName, required));
    if ((!field.is_string() && !required) || field.is_null()) { return defaultValue; }
    return field.as_string();
}

xsapi_internal_string utils::extract_json_string(
    _In_ const web::json::value& jsonValue,
    _In_ const xsapi_internal_string& stringName,
    _In_ bool required,
    _In_ const xsapi_internal_string& defaultValue
    )
{
    web::json::value field(utils::extract_json_field(jsonValue, stringName, required));
    if ((!field.is_string() && !required) || field.is_null()) { return defaultValue; }
    return internal_string_from_string_t(field.as_string());
}

web::json::array utils::extract_json_array(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& arrayName,
    _In_ bool required
    )
{
    web::json::value field(utils::extract_json_field(jsonValue, arrayName, required));
    if ((!field.is_array() && !required) || field.is_null()) { return web::json::value::array().as_array(); }
    return field.as_array();
}

string_t utils::extract_json_as_string(
    _In_ const web::json::value& jsonValue,
    _Inout_ std::error_code& error
    )
{
    if (jsonValue.is_string())
    {
        error = xbox_live_error_code::no_error;
        return jsonValue.as_string();
    }
    
    error = xbox_live_error_code::json_error;
    return string_t();
}

web::json::array utils::extract_json_as_array(
    _In_ const web::json::value& jsonValue,
    _Inout_ std::error_code& error
    )
{
    web::json::value arr;
    if (jsonValue.is_array())
    {
        error = xbox_live_error_code::no_error;
        return jsonValue.as_array();
    }

    error = xbox_live_error_code::json_error;
    return jsonValue.array().as_array();
}

bool utils::extract_json_bool(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& stringName,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ bool defaultValue
    )
{
    web::json::value field(utils::extract_json_field(jsonValue, stringName, error, required));
    if (!field.is_boolean() && !required) { return defaultValue; }
    return field.as_bool();
}

bool utils::extract_json_bool(
    _In_ const web::json::value& jsonValue,
    _In_ const xsapi_internal_string& stringName,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ bool defaultValue
    )
{
    web::json::value field(utils::extract_json_field(jsonValue, stringName, error, required));
    if (!field.is_boolean() && !required) { return defaultValue; }
    return field.as_bool();
}

bool utils::extract_json_bool(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& stringName,
    _In_ bool required,
    _In_ bool defaultValue
    )
{
    web::json::value field(utils::extract_json_field(jsonValue, stringName, required));
    if (!field.is_boolean() && !required) { return defaultValue; } 
    return field.as_bool();
}

int utils::extract_json_int(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ int defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, error, required));
    if ((!field.is_integer() && !required) || error) { return defaultValue; }
    return field.as_integer();
}

int utils::extract_json_int(
    _In_ const web::json::value& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ int defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, error, required));
    if ((!field.is_integer() && !required) || error) { return defaultValue; }
    return field.as_integer();
}

int utils::extract_json_int(
    _In_ const web::json::value& jsonValue, 
    _In_ const string_t& name, 
    _In_ bool required, 
    _In_ int defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, required));
    if (!field.is_integer() && !required) { return defaultValue; }
    return field.as_integer();
}

uint64_t utils::extract_json_string_to_uint64(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ uint64_t defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, error, required));
    if ((!field.is_string() && !required) || error) { return defaultValue; }
    return string_t_to_uint64(field.as_string());
}

uint64_t utils::extract_json_string_to_uint64(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _In_ bool required,
    _In_ uint64_t defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, required));
    if (!field.is_string() && !required) { return defaultValue; }
    return string_t_to_uint64(field.as_string());
}

uint64_t utils::extract_json_uint52(
    _In_ const web::json::value& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ uint64_t defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, error, required));
    if ((!field.is_number() && !required) || error){ return defaultValue; }
    return field.as_number().to_uint64();
}

uint64_t utils::extract_json_uint52(
    _In_ const web::json::value& jsonValue,
    _In_ const xsapi_internal_string& name,
    _In_ bool required,
    _In_ uint64_t defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, required));
    if (!field.is_number() && !required){ return defaultValue; }
    return field.as_number().to_uint64();
}

utility::datetime utils::extract_json_time(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _Inout_ std::error_code& error,
    _In_ bool required
    )
{
    utility::datetime result;
    web::json::value field(extract_json_field(jsonValue, name, error, required));
    if ((!field.is_string() && !required) || error) { return result; }

    result = utility::datetime::from_string(field.as_string(), utility::datetime::date_format::ISO_8601);

    return result;
}

utility::datetime utils::extract_json_time(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _In_ bool required)
{
    utility::datetime result;
    web::json::value field(extract_json_field(jsonValue, name, required));
    if (!field.is_string() && !required) { return result; }
    
    result = utility::datetime::from_string(field.as_string(), utility::datetime::date_format::ISO_8601);

    return result;
}

utility::datetime utils::extract_json_time(
    _In_ const web::json::value& jsonValue,
    _In_ const xsapi_internal_string& name,
    _Inout_ std::error_code& error,
    _In_ bool required
    )
{
    return extract_json_time(jsonValue, utils::string_t_from_internal_string(name), error, required);
}

utility::datetime utils::extract_json_time(
    _In_ const web::json::value& jsonValue,
    _In_ const xsapi_internal_string& name,
    _In_ bool required
    )
{
    return extract_json_time(jsonValue, utils::string_t_from_internal_string(name), required);
}

std::chrono::seconds utils::extract_json_string_timespan_in_seconds(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _Inout_ std::error_code& error,
    _In_ bool required)
{
    web::json::value field(extract_json_field(jsonValue, name, error, required));
    if ((!field.is_string() && !required) || error) { return std::chrono::seconds(); }

    char_t delimiter;
    int hour = 0, min = 0, sec = 0;
    stringstream_t ss(field.as_string());
    ss >> hour >> delimiter >> min >> delimiter >> sec;

    return std::chrono::hours(hour) + std::chrono::minutes(min) + std::chrono::seconds(sec);
}

std::chrono::seconds utils::extract_json_string_timespan_in_seconds(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _In_ bool required)
{
    web::json::value field(extract_json_field(jsonValue, name, required));
    if (!field.is_string() && !required) { return std::chrono::seconds(); }

    char_t delimiter;
    int hour = 0, min = 0, sec = 0;
    stringstream_t ss(field.as_string());
    ss >> hour >> delimiter >> min >> delimiter >> sec;

    return std::chrono::hours(hour) + std::chrono::minutes(min) + std::chrono::seconds(sec);
}

double utils::extract_json_double(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _Inout_ std::error_code& error,
    _In_ bool required,
    _In_ double defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, error, required));
    if ((!field.is_double() && !required) || error) { return defaultValue; }
    return field.as_double();
}

double utils::extract_json_double(
    _In_ const web::json::value& jsonValue,
    _In_ const string_t& name,
    _In_ bool required,
    _In_ double defaultValue
    )
{
    web::json::value field(extract_json_field(jsonValue, name, required));
    if (!field.is_double() && !required) { return defaultValue; }
    return field.as_double();
}

string_t utils::base64_url_encode(
    _In_ const std::vector<unsigned char>& data
    )
{
    string_t base64 = utility::conversions::to_base64(data);
    std::replace(base64.begin(), base64.end(), '+', '-');
    std::replace(base64.begin(), base64.end(), '/', '_');
    std::size_t end = base64.find('=', base64.length() - 3);
    if (end != base64.length())
    {
        base64.resize(end);
    }

    return base64;
}

xsapi_internal_string utils::headers_to_string(
    _In_ const http_headers& headers
    )
{
    xsapi_internal_stringstream ss;

    for (const auto& header : headers)
    {
        ss << header.first << ": " << header.second << "\r\n";
    }

    return ss.str();
}

web::http::http_headers utils::string_to_headers(
    _In_ const string_t& headers
    )
{
    web::http::http_headers headerMap;
    std::vector<string_t> lines = utils::string_split(headers, '\n');

    for (const auto& line : lines)
    {
        static regex_t headerLineRegex(_T("\\s*(\\S+):\\s*(.*\\S)\\s*"));
        smatch_t match;
        bool succeeded = std::regex_match(line, match, headerLineRegex);
        if (succeeded)
        {
            // match[0] is the full matched string
            // match[1] is the header name
            // match[2] is the header value
            headerMap[match[1]] = match[2];
        }
        else if (!line.empty())
        {
            LOGS_ERROR << "Parsing string to headers failed" << line;
        }
    }

    return headerMap;
}

string_t utils::path_and_query_from_uri(
    _In_ const web::http::uri& uri
    )
{
    stringstream_t ss;
    ss << uri.path();
    if (!uri.query().empty())
    {
        ss << _T("?") << uri.query();
    }

    return ss.str();
}

string_t 
utils::get_query_from_params(
    _In_ const std::vector<string_t>& params
    )
{
    stringstream_t strQueryString;

    size_t cItems = params.size();
    if (cItems > 0)
    {
        string_t strDelimiter = _T("&");

        strQueryString << _T("?");
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

web::json::value
utils::serialize_uint52_to_json(
    _In_ uint64_t integer
    )
{
    if ((integer & 0xFFF0000000000000) != 0)
    {
        return web::json::value::null();
    }

    return web::json::value::number(static_cast<double>(integer));
}

void utils::append_paging_info(
    _In_ web::uri_builder& uriBuilder,
    _In_ unsigned int skipItems,
    _In_ unsigned int maxItems,
    _In_opt_ string_t continuationToken
    )
{
    // add maxItem parameter
    if (maxItems > 0)
    {
        uriBuilder.append_query(_T("maxItems"), maxItems);
    }

    if (continuationToken.empty())
    {
        // use skip items value if continuation token is empty
        if (skipItems > 0)
        {
            uriBuilder.append_query(_T("skipItems"), skipItems);
        }
    }
    else
    {
        uriBuilder.append_query(_T("continuationToken"), continuationToken);
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
#if _WIN32
    return wcscpy_s(destinationCharArr, sizeInWords, sourceCharArr);
#else
    return strlcpy(destinationCharArr, sourceCharArr, sizeInWords);
#endif
}

#ifdef _WIN32
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
            hr = HRESULT_FROM_WIN32(ex.code().value());
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
    catch (const web::json::json_exception&) // is an exception
    {
        hr = WEB_E_INVALID_JSON_STRING;
    }
    catch (const web::http::http_exception&) // is an exception
    {
        hr = HTTP_E_STATUS_UNEXPECTED;
    }
    catch (const web::uri_exception&) // is an exception
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
    xbox_live_error_code xblErr = static_cast<xbox_live_error_code>(err);

    if (err == 204)
    {
        return __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND);
    }
    else if (err >= 300 && err <= 505)
    {
        return convert_http_status_to_hresult(err);
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

long utils::convert_http_status_to_hresult(_In_ uint32_t httpStatusCode)
{
    xbox::services::xbox_live_error_code errCode = static_cast<xbox::services::xbox_live_error_code>(httpStatusCode);
    long hr = HTTP_E_STATUS_UNEXPECTED;

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

xbox::services::xbox_live_error_code
utils::convert_exception_to_xbox_live_error_code()
{
    // Default value, if there is no exception appears, return no_error
    xbox::services::xbox_live_error_code errCode = xbox_live_error_code::no_error;

    try
    {
        throw;
    }
    // std exceptions
    catch (const std::bad_alloc&) // is an exception
    {
        errCode = xbox_live_error_code::bad_alloc;
    }
    catch (const std::bad_cast&) // is an exception
    {
        errCode = xbox_live_error_code::bad_cast;
    }
    catch (const std::invalid_argument&) // is a logic_error
    {
        errCode = xbox_live_error_code::invalid_argument;
    }
    catch (const std::out_of_range&) // is a logic_error
    {
        errCode = xbox_live_error_code::out_of_range;
    }
    catch (const std::length_error&) // is a logic_error
    {
        errCode = xbox_live_error_code::length_error;
    }
    catch (const std::range_error&) // is a runtime_error
    {
        errCode = xbox_live_error_code::range_error;
    }
    catch (const std::system_error& ex) // is a runtime_error
    {
        errCode = static_cast<xbox_live_error_code>(ex.code().value());
    }
    catch (const std::logic_error&) // is an exception
    {
        errCode = xbox_live_error_code::logic_error;
    }
    catch (const std::runtime_error&) // is an exception
    {
        errCode = xbox_live_error_code::runtime_error;
    }
    catch (const web::json::json_exception&) // is an exception
    {
        errCode = xbox_live_error_code::json_error;
    }
    catch (const web::http::http_exception& ex) // is an exception
    {
        errCode = static_cast<xbox_live_error_code>(ex.error_code().value());
    }
    catch (const web::uri_exception&) // is an exception
    {
        errCode = xbox_live_error_code::uri_error;
    }
    catch (const std::exception&) // base class for standard C++ exceptions
    {
        errCode = xbox_live_error_code::generic_error;
    }
#ifdef _WIN32
    catch (HRESULT exceptionHR)
    {
        errCode = static_cast<xbox_live_error_code>(exceptionHR);
    }
#endif
    catch (...) // everything else
    {
        errCode = xbox_live_error_code::generic_error;
    }

    return errCode;
}

#ifdef _WIN32
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

    return ( n == 11 ) ? xbox_live_error_code::no_error : xbox_live_error_code::logic_error;
}
#endif

xsapi_internal_string utils::create_guid(_In_ bool removeBraces)
{
#ifdef _WIN32
    GUID guid = {0};
    THROW_CPP_RUNTIME_IF(FAILED(CoCreateGuid(&guid)), "");

    WCHAR wszGuid[50];
    THROW_CPP_RUNTIME_IF(FAILED(::StringFromGUID2(
        guid,
        wszGuid,
        ARRAYSIZE(wszGuid)
        )), "");

    xsapi_internal_string strGuid = utils::internal_string_from_utf16(wszGuid);
#elif XSAPI_U
    boost::uuids::uuid uuid;
    auto uuidGenerator = boost::uuids::random_generator();
    uuid = uuidGenerator();
    std::stringstream ss;
    ss << uuid;

    std::string strGuid = ss.str();
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

xsapi_internal_vector<xsapi_internal_string> utils::string_split(
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

string_t utils::create_xboxlive_endpoint(
    _In_ const string_t& subpath,
    _In_ const std::shared_ptr<xbox_live_app_config>& appConfig,
    _In_ const string_t& protocol
    )
{
    stringstream_t source;
    source << protocol; // eg. https or wss
    source << _T("://");
    source << subpath; // eg. "achievements"
#if !TV_API
    if (appConfig)
    {
        source << appConfig->environment(); // eg. "" or ".dnet"
    }
#else
    UNREFERENCED_PARAMETER(appConfig);    
#endif
    source << _T(".xboxlive.com");
    return source.str();
}

xsapi_internal_string utils::create_xboxlive_endpoint(
    _In_ const xsapi_internal_string& subpath,
    _In_ const std::shared_ptr<xbox_live_app_config_internal>& appConfig,
    _In_ const xsapi_internal_string& protocol
    )
{
    xsapi_internal_stringstream source;
    source << protocol; // eg. https or wss
    source << "://";
    source << subpath; // eg. "achievements"
#if !TV_API
    if (appConfig)
    {
        source << appConfig->environment(); // eg. "" or ".dnet"
    }
#else
    UNREFERENCED_PARAMETER(appConfig);
#endif
    source << ".xboxlive.com";
    return source.str();
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

void utils::sleep(
    _In_ uint32_t waitTimeInMilliseconds
    )
{
#if XSAPI_U
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeInMilliseconds));
#else
    concurrency::wait(waitTimeInMilliseconds);
#endif
}

string_t utils::read_file_to_string(
    _In_ const string_t& filePath
    )
{
    std::ifstream in(filePath, std::ios::in | std::ios::binary);
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

            string_t fileDataString;
#ifdef WIN32            
            // Convert file data to UTF16 string
            if (isUtf16LE)
            {
                int byteOrderMarkSizeInBytes = 2;
                uint32_t strLength = (fileSizeInBytes - byteOrderMarkSizeInBytes) / sizeof(WCHAR);
                fileDataString = std::wstring(reinterpret_cast<WCHAR*>(fileData.data() + byteOrderMarkSizeInBytes), strLength);
            }
            else
            {
                int byteOrderMarkSizeInBytes = (isUtf8) ? 3 : 0;
                uint32_t strLength = fileSizeInBytes - byteOrderMarkSizeInBytes;
                std::string utf8FileData = std::string(fileData.data() + byteOrderMarkSizeInBytes, strLength);
                fileDataString = utility::conversions::utf8_to_utf16(utf8FileData);
            }
#else
            // Convert file data to UTF8 string
            if (isUtf16LE)
            {
                int byteOrderMarkSizeInBytes = 2;
                uint32_t strLength = (fileSizeInBytes - byteOrderMarkSizeInBytes) / sizeof(wchar_t);
                string_t utf16FileData = string_t(fileData.data(), strLength);
                fileDataString = utf16FileData = utility::conversions::to_string_t(utf16FileData);
            }
            else
            {
                int byteOrderMarkSizeInBytes = (isUtf8) ? 3 : 0;
                uint32_t strLength = fileSizeInBytes - byteOrderMarkSizeInBytes;
                fileDataString = std::string(fileData.data(), strLength);
            }
#endif
            return fileDataString;
        }
    }

    return string_t();
}

#if TV_API
// This function only supports date_format::ISO_8601
string_t utils::datetime_to_string(
    _In_ const utility::datetime& datetimeToConvert
    )
{
    ULARGE_INTEGER largeInt;
    largeInt.QuadPart = datetimeToConvert.to_interval();

    FILETIME ft;
    ft.dwHighDateTime = largeInt.HighPart;
    ft.dwLowDateTime = largeInt.LowPart;

    SYSTEMTIME systemTime;
    if (!FileTimeToSystemTime((const FILETIME *)&ft, &systemTime))
    {
        throw utility::details::create_system_error(GetLastError());
    }

    std::wostringstream outStream;
    outStream.imbue(std::locale::classic());

    const size_t buffSize = 64;
    wchar_t dateStr[buffSize] = {0};
    swprintf_s(dateStr, buffSize, L"%4d-%02d-%02d",
        systemTime.wYear,
        systemTime.wMonth,
        systemTime.wDay
    );

    wchar_t timeStr[buffSize] = {0};
    swprintf_s(timeStr, buffSize, L"%02d:%02d:%02d",
        systemTime.wHour,
        systemTime.wMinute,
        systemTime.wSecond
    );

    outStream << dateStr << "T" << timeStr;
    uint64_t frac_sec = largeInt.QuadPart % _secondTicks;
    if (frac_sec > 0)
    {
        // Append fractional second, which is a 7-digit value with no trailing zeros
        // This way, '1200' becomes '00012'
        char buf[9] = { 0 };
        sprintf_s(buf, sizeof(buf), ".%07ld", (long int)frac_sec);
        // trim trailing zeros
        for (int i = 7; buf[i] == '0'; i--) buf[i] = '\0';
        outStream << buf;
    }
    outStream << "Z";

    return outStream.str();
}

#endif

uint32_t
utils::try_get_master_title_id()
{
    auto titleId = xbox::services::xbox_live_app_config_internal::get_app_config_singleton()->override_title_id_for_multiplayer();
    if (titleId == 0)
    {
        titleId = xbox::services::xbox_live_app_config_internal::get_app_config_singleton()->title_id();
    }
    return titleId;
}

xsapi_internal_string
utils::try_get_override_scid()
{
    auto scid = xbox::services::xbox_live_app_config_internal::get_app_config_singleton()->override_scid_for_multiplayer();
    if (scid.empty())
    {
        scid = xbox::services::xbox_live_app_config_internal::get_app_config_singleton()->scid();
    }
    return scid;
}

int utils::interlocked_increment(volatile long& incrementNum)
{
#ifdef _WIN32
    return InterlockedIncrement(&incrementNum);
#else
    return static_cast<uint32_t>(__sync_fetch_and_add(&incrementNum, 1));
#endif
}

int utils::interlocked_decrement(volatile long& decrementNum)
{
#ifdef _WIN32
    return InterlockedDecrement(&decrementNum);
#else
    return static_cast<uint32_t>(__sync_fetch_and_sub(&decrementNum, 1));
#endif
}

#if UNIT_TEST_SERVICES
web::json::value
utils::read_test_response_file(_In_ const string_t& filePath)
{

    WCHAR modulePath[MAX_PATH] = { 0 };
    if (0 != GetModuleFileNameW((HINSTANCE)&__ImageBase, modulePath, _countof(modulePath)))
    {
        WCHAR* lastSlash = wcsrchr(modulePath, L'\\');
        if (lastSlash != nullptr)
        {
            *lastSlash = 0;
        }
    }
    
    string_t jsonFilePath = modulePath + filePath;
    std::ifstream fileStream(jsonFilePath, std::ifstream::binary);

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    web::json::value jsonResponse;
    jsonResponse = web::json::value::parse(buffer);
    return jsonResponse;
}
#endif

std::vector<string_t> utils::string_array_to_string_vector(
    PCSTR *stringArray,
    size_t stringArrayCount
    )
{
    std::vector<utility::string_t> stringVector;
    for (size_t i = 0; i < stringArrayCount; ++i)
    {
        stringVector.push_back(string_t_from_utf8(stringArray[i]));
    }
    return stringVector;
}

xsapi_internal_vector<xsapi_internal_string> utils::string_array_to_internal_string_vector(
    PCSTR* stringArray,
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

#ifdef _WIN32
time_t utils::time_t_from_datetime(const utility::datetime& datetime)
{
    static const uint64_t ut_msTicks = static_cast<uint64_t>(10000);
    static const uint64_t ut_secondTicks = 1000 * ut_msTicks;
    static const uint64_t ut_minuteTicks = 60 * ut_secondTicks;
    static const uint64_t ut_hourTicks = 60 * 60 * ut_secondTicks;
    static const uint64_t ut_dayTicks = 24 * 60 * 60 * ut_secondTicks;

    uint64_t seconds = datetime.to_interval() / ut_secondTicks;
    if (seconds >= 11644473600LL)
    {
        return seconds - 11644473600LL;
    }
    else
    {
        return 0;
    }
}
#endif // _WIN32

#ifdef _WIN32 

xsapi_internal_string utils::internal_string_from_string_t(_In_ const string_t& externalString)
{
    return internal_string_from_utf16(externalString.c_str());
}

xsapi_internal_string utils::internal_string_from_utf16(_In_z_ PCWSTR utf16)
{
    return internal_string_from_char_t(utf16);
}

xsapi_internal_string utils::internal_string_from_char_t(_In_ const char_t* char_t)
{
    auto cchOutString = utf8_from_char_t(char_t, nullptr, 0);
    xsapi_internal_string out(cchOutString - 1, '\0');
    utf8_from_char_t(char_t, &out[0], cchOutString);
    return out;
}

string_t utils::string_t_from_internal_string(_In_ const xsapi_internal_string& internalString)
{
    return string_t_from_utf8(internalString.data());
}

string_t utils::string_t_from_utf8(_In_z_ PCSTR utf8)
{
    auto cchOutString = char_t_from_utf8(utf8, nullptr, 0);
    string_t out(cchOutString - 1, '\0');
    char_t_from_utf8(utf8, &out[0], cchOutString);
    return out;
}

int utils::utf8_from_char_t(
    _In_z_ const char_t* inArray, 
    _Out_writes_z_(cchOutArray) char* outArray,
    _In_ int cchOutArray
    )
{
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
}

int utils::char_t_from_utf8(
    _In_z_ const char* inArray,
    _Out_writes_z_(cchOutArray) char_t* outArray,
    _In_ int cchOutArray
    )
{
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
}

#endif // _WIN32

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
