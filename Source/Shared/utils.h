// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if defined _WIN32
#include <Rpc.h>
#include <codecvt>
#else
#include <boost/uuid/uuid.hpp>
#endif
#include "xsapi/xbox_live_app_config.h"
#include "http_call_response.h"
#include "xsapi/mem.h"

// Forward
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
    class xbox_live_services_settings;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN


#if !TV_API && !XSAPI_SERVER
namespace presence
{
    class presence_writer;
}
#endif

#ifndef __min
#define __min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a,b)            (((a) < (b)) ? (b) : (a))
#endif  

struct xsapi_singleton
{
    xsapi_singleton();
    ~xsapi_singleton();

    std::mutex s_rtaActivationCounterLock;
    std::unordered_map<string_t, uint32_t> s_rtaActiveSocketCountPerUser;
    std::unordered_map<string_t, uint32_t> s_rtaActiveManagersByUser;

    std::mutex s_singletonLock;
    std::shared_ptr<XBOX_LIVE_NAMESPACE::system::xbox_live_services_settings> s_xboxServiceSettingsSingleton;
    std::shared_ptr<XBOX_LIVE_NAMESPACE::local_config> s_localConfigSingleton;

#if !TV_API && !XSAPI_SERVER
    std::shared_ptr<XBOX_LIVE_NAMESPACE::presence::presence_writer> s_presenceWriterSingleton;
#endif
};

std::shared_ptr<xsapi_singleton> get_xsapi_singleton(_In_ bool createIfRequired = true);

#ifndef _In_reads_bytes_
#define _In_reads_bytes_(s)
#endif

class utils
{
public:
    static string_t extract_json_string(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& stringName,
        _Inout_ std::error_code& error,
        _In_ bool required = false,
        _In_ const string_t& defaultValue = _T("")
    );

    static void extract_json_string_to_char_t_array(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& stringName,
        _Inout_ std::error_code& error,
        _In_reads_bytes_(size) char_t* charArr,
        _In_ size_t size
        );

    static string_t extract_json_as_string(
        _In_ const web::json::value& jsonValue,
        _Inout_ std::error_code& error
    );

    static web::json::array extract_json_as_array(
        _In_ const web::json::value& jsonValue,
        _Inout_ std::error_code& error
    );

    static string_t extract_json_string(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& stringName,
        _In_ bool required = false,
        _In_ const string_t& defaultValue = _T("")
    );

    static bool extract_json_bool(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& stringName,
        _Inout_ std::error_code& error,
        _In_ bool required = false,
        _In_ bool defaultValue = false
    );

    static bool extract_json_bool(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& stringName,
        _In_ bool required = false,
        _In_ bool defaultValue = false
    );

    static int extract_json_int(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _Inout_ std::error_code& error,
        _In_ bool required = false,
        _In_ int defaultValue = 0
    );

    static int extract_json_int(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _In_ bool required = false,
        _In_ int defaultValue = 0
    );

    static uint64_t extract_json_string_to_uint64(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _Inout_ std::error_code& error,
        _In_ bool required = false,
        _In_ uint64_t defaultValue = 0
    );

    static uint64_t extract_json_string_to_uint64(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _In_ bool required = false,
        _In_ uint64_t defaultValue = 0
    );

    static uint64_t extract_json_uint52(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _Inout_ std::error_code& error,
        _In_ bool required = false,
        _In_ uint64_t defaultValue = 0
    );

    static uint64_t extract_json_uint52(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _In_ bool required = false,
        _In_ uint64_t defaultValue = 0
    );

    static utility::datetime extract_json_time(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _Inout_ std::error_code& error,
        _In_ bool required = false
    );

    static utility::datetime extract_json_time(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _In_ bool required = false
    );

    static std::chrono::seconds extract_json_string_timespan_in_seconds(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& stringName,
        _Inout_ std::error_code& error,
        _In_ bool required = false
    );

    static std::chrono::seconds extract_json_string_timespan_in_seconds(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& stringName,
        _In_ bool required = false
    );

    static double extract_json_double(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _Inout_ std::error_code& error,
        _In_ bool required = false,
        _In_ double defaultValue = 0
    );

    static double extract_json_double(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& name,
        _In_ bool required = false,
        _In_ double defaultValue = 0
    );

    static web::json::value extract_json_field(
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _Inout_ std::error_code& error,
        _In_ bool required
    );

    static int interlocked_increment(volatile long& incrementNum);
    static int interlocked_decrement(volatile long& decrementNum);

    template<typename T, typename F>
    static T extract_json_object(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _Inout_ std::error_code& error,
        bool required = false
    )
    {
        web::json::value field(extract_json_field(json, name, error, required));

        auto obj = deserialize(field);
        if (obj.err())
        {
            error = obj.err();
        }

        return obj.payload();
    }

    static web::json::value extract_json_field(
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _In_ bool required
    );

    template<typename T, typename F>
    static std::vector<T> extract_json_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _In_ bool required
    )
    {
        web::json::value field(extract_json_field(json, name, required));
        std::vector<T> result;

        if (!field.is_array() && !required) return result;

        const web::json::array& arr(field.as_array());
        for (const auto& element : arr)
        {
            result.push_back(deserialize(element));
        }

        return result;
    }

    template<typename T, typename F>
    static std::vector<T> extract_json_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _Inout_ std::error_code& errc,
        _In_ bool required
    )
    {
        web::json::value field(extract_json_field(json, name, errc, required));
        std::vector<T> result;

        if ((!field.is_array()) || errc)
        {
            if (required)
            {
                errc = xbox_live_error_code::json_error;
            }

            return result;
        }

        const web::json::array& arr(field.as_array());
        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            auto obj = deserialize(*it);
            if (obj.err())
            {
                errc = obj.err();
                // break here?
            }
            result.push_back(obj.payload());
        }

        return result;
    }

    template<typename T, typename U, typename F>
    static std::vector<T, U> extract_json_internal_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _Inout_ std::error_code& errc,
        _In_ bool required
    )
    {
        web::json::value field(extract_json_field(json, name, errc, required));
        std::vector<T, U> result;

        if ((!field.is_array()) || errc)
        {
            if (required)
            {
                errc = xbox_live_error_code::json_error;
            }

            return result;
        }

        const web::json::array& arr(field.as_array());
        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            auto obj = deserialize(*it);
            if (obj.err())
            {
                errc = obj.err();
                // break here?
            }
            result.push_back(obj.payload());
        }

        return result;
    }

    template<typename T, typename F>
    static xbox_live_result<std::vector<T>> extract_xbox_live_result_json_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _Inout_ std::error_code& errc,
        _In_ bool required
    )
    {
        auto jsonVector = extract_json_vector<T>(
            deserialize,
            json,
            errc,
            required
            );

        return xbox_live_result<std::vector<T>>(
            jsonVector,
            errc,
            ""
            );
    }

    template<typename T, typename F>
    static xbox_live_result<std::vector<T>> extract_xbox_live_result_json_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _Inout_ std::error_code& errc,
        _In_ bool required
    )
    {
        auto jsonVector = extract_json_vector<T>(
            deserialize,
            json,
            name,
            errc,
            required
            );

        return xbox_live_result<std::vector<T>>(
            jsonVector,
            errc,
            ""
            );
    }

    template<typename T, typename F>
    static std::vector<T> extract_json_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _Inout_ std::error_code& errc,
        _In_ bool required
    )
    {
        std::vector<T> result;
        if (!json.is_array())
        {
            if (required)
            {
                errc = xbox_live_error_code::json_error;
            }
            return result;
        }

        auto arrJson = json.as_array();
        for (uint32_t i = 0; i < arrJson.size(); ++i)
        {
            auto it = arrJson[i];
            auto obj = deserialize(it);
            if (obj.err())
            {
                errc = obj.err();
            }
            result.push_back(obj.payload());
        }

        return result;
    }

    template<typename T, typename F>
    static std::vector<T> extract_json_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _In_ bool required
    )
    {
        std::vector<T> result;
        if (!json.is_array() && !required) return result;

        const web::json::array& arr(json.as_array());
        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            result.push_back(deserialize(*it));
        }

        return result;
    }

    static web::json::value json_get_value_from_string(_In_ const string_t& value);

    static xbox_live_result<string_t> json_string_extractor(_In_ const web::json::value& json);

    static web::json::value json_string_serializer(_In_ const string_t& value);

    static xbox_live_result<int> json_int_extractor(_In_ const web::json::value& json);

    static web::json::value json_int_serializer(_In_ int32_t value);

    template<typename T, typename F>
    static web::json::value serialize_vector(
        _In_ F serializer,
        _In_ std::vector<T> inputVector
    )
    {
        int i = 0;
        web::json::value jsonArray = web::json::value::array();
        for (auto& s : inputVector)
        {
            jsonArray[i++] = serializer(s);
        }

        return jsonArray;
    }

    static web::json::value serialize_uint52_to_json(_In_ uint64_t integer);

    static string_t base64_url_encode(_In_ const std::vector<unsigned char>& data);

    static string_t headers_to_string(_In_ const web::http::http_headers& headers);

    static web::http::http_headers string_to_headers(_In_ const string_t& headers);

    static string_t path_and_query_from_uri(_In_ const web::http::uri& uri);

    static string_t get_query_from_params(_In_ const std::vector<string_t>& params);

    static string_t create_guid(_In_ bool removeBraces);

#ifdef _WIN32
    static std::error_code guid_from_string(_In_ const string_t& str, _In_ GUID* guid, _In_ bool withBraces);
#endif

    static void append_paging_info(
        _In_ web::uri_builder& uriBuilder,
        _In_ unsigned int skipItems,
        _In_ unsigned int maxItems,
        _In_opt_ string_t continuationToken
    );

    static uint32_t convert_timespan_to_days(
        _In_ uint64_t timespan
    );

    static std::time_t convert_timepoint_to_time(
        _In_ const chrono_clock_t::time_point& time_point
    );

    static string_t convert_timepoint_to_string(
        _In_ const chrono_clock_t::time_point& time_point
    );

    static string_t escape_special_characters(const string_t& str);

    static inline int str_icmp(const string_t &left, const string_t &right)
    {
        return char_t_cmp(left.c_str(), right.c_str());
    }

    static inline int char_t_cmp(const char_t* left, const char_t* right)
    {
#ifdef  _WIN32
        return _wcsicmp(left, right);
#else
        return strcasecmp(left, right);
#endif 
    }

    static std::vector<string_t> string_split(
        _In_ const string_t& string,
        _In_ string_t::value_type seperator
    );

	static XBOX_LIVE_NAMESPACE::xbox_live_error_code convert_exception_to_xbox_live_error_code();

#ifdef _WIN32
    static void convert_unix_time_to_filetime(
        _In_ std::time_t t,
        _In_ FILETIME* ft);

    static void convert_timepoint_to_filetime(
        _In_ const chrono_clock_t::time_point& time_point,
        _Inout_ uint64_t& largeInt);

    static HRESULT convert_exception_to_hresult();

    static HRESULT convert_xbox_live_error_code_to_hresult(_In_ const std::error_code& errCode);
#endif

    static string_t extract_header_value(
        _In_ const web::http::http_headers& responseHeaders,
        _In_ const string_t& name,
        _In_ const string_t& defaultValue = _T("")
    );

    static string_t convert_hresult_to_error_name(_In_ long hr);
    static long convert_http_status_to_hresult(_In_ uint32_t httpStatusCode);

    static string_t create_xboxlive_endpoint(
        _In_ const string_t& subpath,
        _In_ const std::shared_ptr<xbox_live_app_config>& appConfig,
        _In_ const string_t& protocol = _T("https")
    );

#if defined _WIN32
    static inline std::string convert_wide_string_to_standard_string(_In_ string_t wideString)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.to_bytes(wideString);
    }
#endif

    static void generate_locales();
    static const string_t& get_locales();

    static void set_locales(_In_ const string_t& locale);
    template<typename T>
    static XBOX_LIVE_NAMESPACE::xbox_live_result<T> generate_xbox_live_result(
        _Inout_ XBOX_LIVE_NAMESPACE::xbox_live_result<T> deserializationResult,
        _In_ const std::shared_ptr<XBOX_LIVE_NAMESPACE::http_call_response>& response
    )
    {
        if (deserializationResult.err())
        {
            deserializationResult.set_payload(T());
        }

        const std::error_code& httpErrorCode = response->err_code();
        if (httpErrorCode != xbox_live_error_code::no_error)
        {
            deserializationResult._Set_err(httpErrorCode);
            deserializationResult._Set_err_message(response->err_message());
        }

        return deserializationResult;
    }

    template<typename T>
    static pplx::task <XBOX_LIVE_NAMESPACE::xbox_live_result<T>> create_exception_free_task(
        _In_ const pplx::task <XBOX_LIVE_NAMESPACE::xbox_live_result<T>>& t
    )
    {
        return t.then([](pplx::task <XBOX_LIVE_NAMESPACE::xbox_live_result<T>> result)
        {
            try
            {
                return result.get();
            }
            catch (const std::exception& e)
            {
                xbox_live_error_code err = XBOX_LIVE_NAMESPACE::utils::convert_exception_to_xbox_live_error_code();
                return xbox_live_result<T>(err, e.what());
            }
#ifdef __cplusplus_winrt
            catch (Platform::Exception^ e)
            {
                xbox_live_error_code errc = static_cast<xbox_live_error_code>(e->HResult);
                return xbox_live_result<T>(errc, utility::conversions::to_utf8string(e->Message->Data()));
            }
#endif
        });
    }

    template<typename T>
    static std::vector<T> xsapi_vector_to_std_vector(
        _In_ const std::vector<T, xsapi_stl_allocator<T>>& xsapiInternalVector
        )
    {
        auto internalVectorSize = xsapiInternalVector.size();
        std::vector<T> vec(internalVectorSize);
        for (size_t i = 0; i < internalVectorSize; ++i)
        {
            vec[i] = xsapiInternalVector.at(i);
        }

        return vec;
    }

    template<typename T>
    static std::vector<T, xsapi_stl_allocator<T>> std_vector_to_xsapi_vector(
        _In_ const std::vector<T>& xsapiInternalVector
        )
    {
        auto internalVectorSize = xsapiInternalVector.size();
        std::vector<T, xsapi_stl_allocator<T>> vec(internalVectorSize);
        for (size_t i = 0; i < internalVectorSize; ++i)
        {
            vec[i] = xsapiInternalVector.at(i);
        }

        return vec;
    }

    static xsapi_internal_vector(xsapi_internal_string) std_vector_string_to_xsapi_vector_internal_string(
        _In_ const std::vector<string_t>& xsapiInternalVector
    )
    {
        auto internalVectorSize = xsapiInternalVector.size();
        xsapi_internal_vector(xsapi_internal_string) vec(internalVectorSize);
        for (size_t i = 0; i < internalVectorSize; ++i)
        {
            vec[i] = xsapiInternalVector.at(i).c_str();
        }

        return vec;
    }

    static uint32_t try_get_master_title_id();

    static string_t try_get_override_scid();

    static string_t replace_sub_string(
        _In_ const string_t& source,
        _In_ const string_t& pattern,
        _In_ const string_t& replacement
    );

    static string_t read_file_to_string(
        _In_ const string_t& filePath
    );

    static void sleep(
        _In_ uint32_t waitTimeInMilliseconds
    );

    inline static uint32_t string_t_to_uint32(
        _In_ const string_t& str
    )
    {
#if XSAPI_U
        return std::strtoul(str.c_str(), nullptr, 0);
#else
        return std::stoul(str);
#endif
    }

    inline static string_t uint32_to_string_t(
        _In_ const uint32_t& val
    )
    {
        stringstream_t stream;
        stream << val;
        return stream.str();
    }

    inline static uint64_t string_t_to_uint64(
        _In_ const string_t& str
    )
    {
#if XSAPI_U
        return strtoull(str.c_str(), nullptr, 0);
#else
        return _wtoi64(str.c_str());
#endif
    }

    inline static int32_t string_t_to_int32(
        _In_ const string_t& str
    )
    {
#if XSAPI_U
        return std::atoi(str.c_str());
#else
        return _wtoi(str.c_str());
#endif
    }

    #define initialize_char_arr(charArr) \
    { \
        auto charArrSize = sizeof(charArr) / sizeof(*charArr); \
        std::fill(charArr, charArr + charArrSize, _T('\0')); \
    }

    #define initialize_arr(arr) \
    { \
        auto arrSize = sizeof(arr) / sizeof(*arr); \
        std::fill(arr, arr + arrSize, 0); \
    }
    
    inline static string_t uint64_to_string_t(
        _In_ uint64_t num
        )
    {
        stringstream_t stream;
        stream << num;
        auto numStr = stream.str();
        return numStr;
    }

    static uint32_t char_t_copy(
        _In_reads_bytes_(size) char_t* destinationCharArr,
        _In_ size_t sizeInWords,
        _In_ const char_t* sourceCharArr
        );

#if TV_API
    static string_t datetime_to_string(
        _In_ const utility::datetime& datetimeToConvert
    );
#endif

#if UNIT_TEST_SERVICES
    static web::json::value read_test_response_file(_In_ const string_t& filePath);
#endif

private:
    static std::vector<string_t> get_locale_list();
    
    utils();
    utils(const utils&);
    utils& operator=(const utils&);

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
