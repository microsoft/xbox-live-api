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

// Forward decls
class xbl_thread_pool;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
    class xbox_live_services_settings;
    class xbox_system_factory;
    #if UWP_API
        class user_impl_idp;
    #endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN
    class social_manager;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN
    class stats_manager;
NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN
    class notification_service;
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN
    class multiplayer_manager;
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN
    class real_time_activity_service_factory;
NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
    class service_call_logging_config;
    class service_call_logger_protocol;
    class service_call_logger;
    class http_retry_after_manager;
    class logger;
    class perf_tester;
    class initiator;
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#if !TV_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
    class sign_out_completed_event_args;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

#if !TV_API && !XSAPI_SERVER
NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN
    class presence_writer;
NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END
#endif

#if _WINRT_DLL || UNIT_TEST_SERVICES
    NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN
        ref class MultiplayerManager;
    NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END

    NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN
        ref class SocialManager;
    NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END

    NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN
        ref class ServiceCallLoggingConfig;
    NAMESPACE_MICROSOFT_XBOX_SERVICES_END

    NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN
        class UserEventBind;
    NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END

    NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN
        ref class StatisticManager;
    NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END
#endif


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

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

    void init();

    std::mutex m_rtaActivationCounterLock;
    std::unordered_map<string_t, uint32_t> m_rtaActiveSocketCountPerUser;
    std::unordered_map<string_t, uint32_t> m_rtaActiveManagersByUser;

    std::mutex m_singletonLock;
    std::mutex m_appConfigLock;
    std::mutex m_mpsdConstantLock;
    std::mutex m_mpsdMemberLock;
    std::mutex m_mpsdPropertyLock;
    std::mutex m_serviceSettingsLock;
    std::shared_ptr<xbox::services::system::xbox_live_services_settings> m_xboxServiceSettingsSingleton;
    std::shared_ptr<xbox::services::local_config> m_localConfigSingleton;

#if !TV_API && !XSAPI_SERVER
    std::shared_ptr<xbox::services::presence::presence_writer> m_presenceWriterSingleton;
#endif

#if TV_API || UNIT_TEST_SERVICES
    std::mutex m_achievementServiceInitLock;
    bool m_bHasAchievementServiceInitialized;
    std::string m_eventProviderName;
    GUID m_eventPlayerSessionId;
#endif

    // from Shared\xbox_live_app_config.cpp
    std::shared_ptr<xbox_live_app_config> m_appConfigSingleton;

    // from Misc\notification_service.cpp
    std::shared_ptr<xbox::services::notification::notification_service> m_notificationSingleton;

    // from Shared\service_call_logging_config.cpp
    std::shared_ptr<service_call_logging_config> m_serviceLoggingConfigSingleton;

    // from Shared\xbox_live_app_config.cpp
    std::mutex m_serviceLoggerProtocolSingletonLock;
    std::shared_ptr<service_call_logger_protocol> m_serviceLoggerProtocolSingleton;

    // from Shared\utils_locales.cpp
    string_t m_locales;
    std::mutex m_locale_lock;
    bool m_custom_locale_override;

    // from Shared\service_call_logger_data.cpp
    uint32_t m_loggerId;

    // from Shared\service_call_logger.cpp
    std::shared_ptr<service_call_logger> m_serviceLoggerSingleton;

    // from Shared\http_call_response.cpp
    volatile long m_responseCount;

    // from Services\Multiplayer\Manager\multiplayer_manager.cpp
    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_manager> m_multiplayerManagerInstance;

    // from Social\Manager\social_manager.cpp
    std::shared_ptr<xbox::services::social::manager::social_manager> m_socialManagerInstance;
    std::shared_ptr<xbox::services::perf_tester> m_perfTester;

    // from Stats\Manager\stats_manager.cpp
    std::shared_ptr<xbox::services::stats::manager::stats_manager> m_statsManagerInstance;

    // from Services\RealTimeActivity\real_time_activity_service_factory.cpp
    std::shared_ptr<real_time_activity::real_time_activity_service_factory> m_rtaFactoryInstance;

    // from Multiplayer\Manager\multiplayer_client_pending_request.cpp
    volatile long m_multiplayerClientPendingRequestUniqueIdentifier;

    // from Shared\http_call_impl.cpp
    std::shared_ptr<http_retry_after_manager> m_httpRetryPolicyManagerSingleton;

    // from Services\Presence\presence_service_impl.cpp
    std::function<void(int heartBeatDelayInMins)> m_onSetPresenceFinish;

    // from Shared\Logger\log.cpp
    std::shared_ptr<logger> m_logger;

    // from Shared\xbox_system_factory.cpp
    std::shared_ptr<system::xbox_system_factory> m_factoryInstance;

    std::shared_ptr<initiator> m_initiator;

    std::shared_ptr<xbl_thread_pool> m_threadpool;

#if _WINRT_DLL || UNIT_TEST_SERVICES
    // from Services\Multiplayer\Manager\WinRT\MultiplayerManager_WinRT.cpp
    Microsoft::Xbox::Services::Multiplayer::Manager::MultiplayerManager^ m_winrt_multiplayerManagerInstance;
    Microsoft::Xbox::Services::Social::Manager::SocialManager^ m_winrt_socialManagerInstance;
    Microsoft::Xbox::Services::ServiceCallLoggingConfig^ m_winrt_serviceCallLoggingConfigInstance;
    Microsoft::Xbox::Services::Statistics::Manager::StatisticManager^ m_winrt_statisticManagerInstance;

    // from System\WinRT\User_WinRT.cpp
    std::shared_ptr<Microsoft::Xbox::Services::System::UserEventBind> m_userEventBind;
#endif

#if UWP_API
    // from System\user_impl_idp.cpp
    std::unordered_map<string_t, std::shared_ptr<system::user_impl_idp>> m_trackingUsers;
    Windows::System::UserWatcher^ m_userWatcher;
#endif

#if !TV_API
    // from System\user_impl.cpp
    std::unordered_map<function_context, std::function<void(const system::sign_out_completed_event_args&)>> m_signOutCompletedHandlers;
    std::unordered_map<function_context, std::function<void(const string_t&)>> m_signInCompletedHandlers;
    function_context m_signOutCompletedHandlerIndexer;
    function_context m_signInCompletedHandlerIndexer;
    std::mutex m_trackingUsersLock;
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

    static std::vector<string_t> extract_json_string_vector(
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _Inout_ std::error_code& error,
        _In_ bool required
    );

    static std::vector<string_t> extract_json_string_vector(
        _In_ const web::json::value& json,
        _In_ const string_t& name,
        _In_ bool required
    );

    static std::vector<string_t> extract_json_string_vector(
        _In_ const web::json::value& json,
        _Inout_ std::error_code& error,
        _In_ bool required
    );

    static std::vector<string_t> extract_json_string_vector(
        _In_ const web::json::value& json,
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

    static xbox::services::xbox_live_error_code convert_exception_to_xbox_live_error_code();

    static string_t vector_join(
        _In_ const std::vector<string_t>& vector,
        _In_ string_t::value_type seperator
    );

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

#ifdef _WIN32
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
    static xbox::services::xbox_live_result<T> generate_xbox_live_result(
        _Inout_ xbox::services::xbox_live_result<T> deserializationResult,
        _In_ const std::shared_ptr<xbox::services::http_call_response>& response
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
    static pplx::task <xbox::services::xbox_live_result<T>> create_exception_free_task(
        _In_ const pplx::task <xbox::services::xbox_live_result<T>>& t
    )
    {
        return t.then([](pplx::task <xbox::services::xbox_live_result<T>> result)
        {
            try
            {
                return result.get();
            }
            catch (const std::exception& e)
            {
                xbox_live_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code();
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

struct client_callback_info
{
    client_callback_info(
        void *_completionFunction,
        void *_clientContext
        ) :
        completionFunction(_completionFunction),
        clientContext(_clientContext)
    {
    }

    void *completionFunction;
    void *clientContext;
};

class callback_context_helper
{
public:
    template<typename T>
    static void *store_shared_ptr(std::shared_ptr<T> contextSharedPtr)
    {
        std::lock_guard<std::mutex> lock(m_contextsLock);
        void *rawVoidPtr = contextSharedPtr.get();
        std::shared_ptr<void> voidSharedPtr(contextSharedPtr, rawVoidPtr);
        m_sharedPtrs.insert(std::make_pair(rawVoidPtr, voidSharedPtr));
        return rawVoidPtr;
    }

    template<typename T>
    static std::shared_ptr<T> remove_shared_ptr(void *rawContextPtr)
    {
        std::lock_guard<std::mutex> lock(m_contextsLock);

        auto iter = m_sharedPtrs.find(rawContextPtr);
        if (iter != m_sharedPtrs.end())
        {
            auto returnPtr = std::shared_ptr<T>(iter->second, reinterpret_cast<T*>(iter->second.get()));
            m_sharedPtrs.erase(iter);
            return returnPtr;
        }
        else
        {
            XSAPI_ASSERT(false && "Context not found!");
            return std::shared_ptr<T>();
        }
    }

    static void *store_client_callback_info(void *clientCallbackFunction, void *clientContext)
    {
        std::lock_guard<std::mutex> lock(m_contextsLock);
        void *index = (void*)m_clientCallbackInfoIndexer++;
        m_clientCallbackInfoMap.insert(std::make_pair(index, client_callback_info(clientCallbackFunction, clientContext)));
        return index;
    }

    static client_callback_info remove_client_callback_info(void * context)
    {
        std::lock_guard<std::mutex> lock(m_contextsLock);

        auto iter = m_clientCallbackInfoMap.find(context);
        if (iter != m_clientCallbackInfoMap.end())
        {
            auto callbackInfo = iter->second;
            m_clientCallbackInfoMap.erase(iter);
            return callbackInfo;
        }
        else
        {
            XSAPI_ASSERT(false && "Context not found!");
            return client_callback_info(nullptr, nullptr);
        }
    }

private:
    static std::mutex m_contextsLock;
    static std::unordered_map<void *, std::shared_ptr<void>> m_sharedPtrs;
    static uint32_t m_clientCallbackInfoIndexer;
    static std::unordered_map<void *, client_callback_info> m_clientCallbackInfoMap;

    callback_context_helper();
    callback_context_helper(const callback_context_helper&);
    callback_context_helper& operator=(const callback_context_helper&);
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
