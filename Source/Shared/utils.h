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
#include "http_call_response_internal.h"
#include "xsapi/mem.h"
#include "xsapi/system.h"

// Forward decls
class xbl_thread_pool;

class title_storage_state;
struct XBL_XBOX_LIVE_APP_CONFIG;
struct XSAPI_ACHIEVEMENTS_STATE; // TODO use c++ naming conventions for internal classes, make them classes
struct XSAPI_SOCIAL_MANAGER_VARS;
struct XSAPI_STATS_MANAGER_VARS;
struct XBL_XBOX_LIVE_USER;

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

#if !TV_API
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
    std::recursive_mutex m_appConfigLock;
    std::mutex m_mpsdConstantLock;
    std::mutex m_mpsdMemberLock;
    std::mutex m_mpsdPropertyLock;
    std::mutex m_serviceSettingsLock;
    std::shared_ptr<xbox::services::system::xbox_live_services_settings> m_xboxServiceSettingsSingleton;
    std::shared_ptr<xbox::services::local_config> m_localConfigSingleton;

#if !TV_API
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
    std::shared_ptr<xbox_live_app_config_internal> m_internalAppConfigSinglton;

    // from Shared\C\xbox_live_app_config.cpp
    std::shared_ptr<XBL_XBOX_LIVE_APP_CONFIG> m_cAppConfigSingleton;

    // from Misc\notification_service.cpp
    std::shared_ptr<xbox::services::notification::notification_service> m_notificationSingleton;

    // from Shared\service_call_logging_config.cpp
    std::shared_ptr<service_call_logging_config> m_serviceLoggingConfigSingleton;

    // from Shared\xbox_live_app_config.cpp
    std::mutex m_serviceLoggerProtocolSingletonLock;
    std::shared_ptr<service_call_logger_protocol> m_serviceLoggerProtocolSingleton;

    // from Shared\utils_locales.cpp
    xsapi_internal_string m_locales;
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

#if UWP_API || UNIT_TEST_SERVICES
    void start_threadpool();
    std::shared_ptr<xbl_thread_pool> m_threadpool;
#endif

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

    // From xsapi C singleton. Revisit if this is needed after reworking code.
    std::shared_ptr<title_storage_state> m_titleStorageState;

    std::mutex m_usersLock;
    std::unordered_map<std::string, XBL_XBOX_LIVE_USER*> m_signedInUsers;

    std::shared_ptr<XSAPI_ACHIEVEMENTS_STATE> m_achievementsState;
    std::shared_ptr<XSAPI_SOCIAL_MANAGER_VARS> m_socialVars;
    std::shared_ptr<XSAPI_STATS_MANAGER_VARS> m_statsVars;

    std::mutex m_callbackContextsLock;
    xsapi_internal_unordered_map<void *, std::shared_ptr<void>> m_callbackContextPtrs;
};

void init_mem_hooks();
extern XBL_MEM_ALLOC_FUNC g_pMemAllocHook;
extern XBL_MEM_FREE_FUNC g_pMemFreeHook;

std::shared_ptr<xsapi_singleton> get_xsapi_singleton(_In_ bool createIfRequired = true);
void verify_global_init();

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

    static xsapi_internal_string extract_json_string(
        _In_ const web::json::value& jsonValue,
        _In_ const xsapi_internal_string& stringName,
        _Inout_ std::error_code& error,
        _In_ bool required = false,
        _In_ const xsapi_internal_string& defaultValue = ""
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

    static xsapi_internal_string extract_json_string(
        _In_ const web::json::value& jsonValue,
        _In_ const xsapi_internal_string& stringName,
        _In_ bool required = false,
        _In_ const xsapi_internal_string& defaultValue = ""
    );

    static web::json::array extract_json_array(
        _In_ const web::json::value& jsonValue,
        _In_ const string_t& arrayName,
        _In_ bool required
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
        _In_ const xsapi_internal_string& stringName,
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
        _In_ const xsapi_internal_string& name,
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
        _In_ const xsapi_internal_string& name,
        _Inout_ std::error_code& error,
        _In_ bool required = false,
        _In_ uint64_t defaultValue = 0
    );

    static uint64_t extract_json_uint52(
        _In_ const web::json::value& jsonValue,
        _In_ const xsapi_internal_string& name,
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

    static web::json::value extract_json_field(
        _In_ const web::json::value& json,
        _In_ const xsapi_internal_string& name,
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

    static web::json::value extract_json_field(
        _In_ const web::json::value& json,
        _In_ const xsapi_internal_string& name,
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

    template<typename T, typename F>
    static xsapi_internal_vector<T> extract_json_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _In_ const xsapi_internal_string& name,
        _Inout_ std::error_code& errc,
        _In_ bool required
    )
    {
        web::json::value field(extract_json_field(json, name, errc, required));
        xsapi_internal_vector<T> result;

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
    static xbox_live_result<xsapi_internal_vector<T>> extract_xbox_live_result_json_vector(
        _In_ F deserialize,
        _In_ const web::json::value& json,
        _In_ const xsapi_internal_string& name,
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

        return xbox_live_result<xsapi_internal_vector<T>>(
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

    static xbox_live_result<xsapi_internal_string> json_internal_string_extractor(_In_ const web::json::value& json);

    static web::json::value json_string_serializer(_In_ const string_t& value);

    static web::json::value json_internal_string_serializer(_In_ const xsapi_internal_string& value);

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

    template<typename T, typename F>
    static web::json::value serialize_vector(
        _In_ F serializer,
        _In_ xsapi_internal_vector<T> inputVector
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

    static xsapi_internal_string headers_to_string(_In_ const http_headers& headers);

    static web::http::http_headers string_to_headers(_In_ const string_t& headers);

    static string_t path_and_query_from_uri(_In_ const web::http::uri& uri);

    static string_t get_query_from_params(_In_ const std::vector<string_t>& params);

    static xsapi_internal_string create_guid(_In_ bool removeBraces);

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

    static xsapi_internal_string convert_timepoint_to_string(
        _In_ const chrono_clock_t::time_point& time_point
    );

    static xsapi_internal_string escape_special_characters(const xsapi_internal_string& str);

    static inline int str_icmp(const string_t &left, const string_t &right)
    {
        return char_t_cmp(left.c_str(), right.c_str());
    }

    static inline int str_icmp(const xsapi_internal_string& left, const xsapi_internal_string& right)
    {
        return _stricmp(left.c_str(), right.c_str());
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

    static xsapi_internal_string convert_hresult_to_error_name(_In_ long hr);
    static long convert_http_status_to_hresult(_In_ uint32_t httpStatusCode);

    // TODO should not be needed eventually
    static string_t create_xboxlive_endpoint(
        _In_ const string_t& subpath,
        _In_ const std::shared_ptr<xbox_live_app_config>& appConfig,
        _In_ const string_t& protocol = _T("https")
    );

    static xsapi_internal_string create_xboxlive_endpoint(
        _In_ const xsapi_internal_string& subpath,
        _In_ const std::shared_ptr<xbox_live_app_config_internal>& appConfig,
        _In_ const xsapi_internal_string& protocol = "https"
    );

#ifdef _WIN32
    static inline std::string convert_wide_string_to_standard_string(_In_ string_t wideString)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.to_bytes(wideString);
    }
#endif

#ifdef UWP_API // TODO add definitions for other platforms

    static xsapi_internal_string internal_string_from_string_t(_In_ const string_t& externalString);
    static xsapi_internal_string internal_string_from_utf16(_In_reads_(size) PCWSTR utf16, size_t size);
    static xsapi_internal_string internal_string_from_utf16(_In_z_ PCWSTR utf16);

    static string_t string_t_from_internal_string(_In_ const xsapi_internal_string& internalString);
    static string_t string_t_from_utf8(_In_reads_(size) PCSTR utf8, size_t size);
    static string_t string_t_from_utf8(_In_z_ PCSTR utf8);

    // TODO remove these after migrating all APIs
    static std::string utf8_from_utf16(std::wstring const& utf16);
    static std::wstring utf16_from_utf8(std::string const& utf8);

    static std::string utf8_from_utf16(_In_z_ PCWSTR utf16);
    static std::wstring utf16_from_utf8(_In_z_ PCSTR utf8);

    static std::string utf8_from_utf16(_In_reads_(size) PCWSTR utf16, size_t size);
    static std::wstring utf16_from_utf8(_In_reads_(size) PCSTR utf8, size_t size);
#endif

    static void generate_locales();
    static const xsapi_internal_string& get_locales();

    static void set_locales(_In_ const xsapi_internal_string& locale);
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
    static xbox::services::xbox_live_result<T> generate_xbox_live_result(
        _Inout_ xbox::services::xbox_live_result<T> deserializationResult,
        _In_ const std::shared_ptr<xbox::services::http_call_response_internal>& response
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
            deserializationResult._Set_err_message(std::string(response->err_message().data()));
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

    static xsapi_internal_vector<xsapi_internal_string> internal_string_vector_from_std_string_vector(
        _In_ const std::vector<string_t>& stdVector
        )
    {
        auto size = stdVector.size();
        xsapi_internal_vector<xsapi_internal_string> internalVector(size);
        for (size_t i = 0; i < size; ++i)
        {
            internalVector[i] = utils::internal_string_from_string_t(stdVector[i]);
        }
        return internalVector;
    }

    static std::vector<string_t> std_string_vector_from_internal_string_vector(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& internalVector
        )
    {
        auto size = internalVector.size();
        std::vector<string_t> vector(size);
        for (size_t i = 0; i < size; ++i)
        {
            vector[i] = utils::string_t_from_internal_string(internalVector[i]);
        }
        return vector;
    }

    template<typename T>
    static std::vector<T> std_vector_from_internal_vector(
        _In_ const xsapi_internal_vector<T>& internalVector
    )
    {
        return std::vector<T>(internalVector.begin(), internalVector.end());
    }

    template<typename External, typename Internal>
    static std::vector<External> std_vector_external_from_internal_vector(
        _In_ const xsapi_internal_vector<Internal>& internalVector
        )
    {
        return std::vector<External>(internalVector.begin(), internalVector.end());
    }

    template<typename T>
    static xsapi_internal_vector<T> internal_vector_from_std_vector(
        _In_ const std::vector<T>& vector
        )
    {
        return xsapi_internal_vector<T>(vector.begin(), vector.end());
    }

    template<typename Internal, typename External> 
    static xsapi_internal_vector<Internal> internal_vector_from_std_vector(
        _In_ const std::vector<External>& vector
        )
    {
        return xsapi_internal_vector<Internal>(vector.begin(), vector.end());
    }

    static uint32_t try_get_master_title_id();

    static xsapi_internal_string try_get_override_scid();

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

    inline static uint64_t internal_string_to_uint64(
        _In_ const xsapi_internal_string& str
    )
    {
        return strtoull(str.c_str(), nullptr, 0);
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

    static std::vector<string_t> string_array_to_string_vector(
        PCSTR *stringArray,
        size_t stringArrayCount
        );

    // These both might not be needed
    static xsapi_internal_vector<xsapi_internal_string> string_array_to_internal_string_vector(
        PCSTR *stringArray,
        size_t stringArrayCount
        );

    template<typename T>
    static void *store_shared_ptr(std::shared_ptr<T> contextSharedPtr)
    {
        auto singleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(singleton->m_callbackContextsLock);
        void *rawVoidPtr = contextSharedPtr.get();
        std::shared_ptr<void> voidSharedPtr(contextSharedPtr, rawVoidPtr);
        singleton->m_callbackContextPtrs.insert(std::make_pair(rawVoidPtr, voidSharedPtr));
        return rawVoidPtr;
    }

    template<typename T>
    static std::shared_ptr<T> remove_shared_ptr(void *rawContextPtr, bool deleteShared = true)
    {
        auto singleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(singleton->m_callbackContextsLock);

        auto iter = singleton->m_callbackContextPtrs.find(rawContextPtr);
        if (iter != singleton->m_callbackContextPtrs.end())
        {
            auto returnPtr = std::shared_ptr<T>(iter->second, reinterpret_cast<T*>(iter->second.get()));
            if (deleteShared)
            {
                singleton->m_callbackContextPtrs.erase(iter);
            }
            return returnPtr;
        }
        else
        {
            XSAPI_ASSERT(false && "Context not found!");
            return std::shared_ptr<T>();
        }
    }

#if XSAPI_C
    static PCSTR alloc_string(const string_t& str);
    static void free_string(PCSTR str);

    static time_t time_t_from_datetime(const utility::datetime& datetime);

    static utility::datetime datetime_from_time_t(const time_t* time);

    static XBL_RESULT create_xbl_result(std::error_code errc);
    static XBL_RESULT create_xbl_result(HC_RESULT hcResult);

    static XBL_RESULT std_bad_alloc_to_xbl_result(std::bad_alloc const& e);
    static XBL_RESULT std_exception_to_xbl_result(std::exception const& e);
    static XBL_RESULT unknown_exception_to_xbl_result();
#endif

private:
    static xsapi_internal_vector<xsapi_internal_string> get_locale_list();
    
    utils();
    utils(const utils&);
    utils& operator=(const utils&);
};

static const uint64_t XSAPI_DEFAULT_TASKGROUP = 99;

template<typename... Args>
class xbox_live_callback
{
public:
    xbox_live_callback() : m_callable(nullptr) {}
    xbox_live_callback(nullptr_t) : m_callable(nullptr) {}

    template <typename Functor>
    xbox_live_callback(Functor functor)
    {
        m_callable = xsapi_unique_ptr<ICallable>(xsapi_allocate_unique<callable<Functor>>(functor).release());
    }

    xbox_live_callback(const xbox_live_callback& rhs)
    {
        *this = rhs;
    }

    xbox_live_callback(xbox_live_callback&& rhs)
    {
        *this = std::move(rhs);
    }

    template <typename Functor>
    xbox_live_callback& operator=(Functor f)
    {
        m_callable = xsapi_unique_ptr<ICallable>(xsapi_allocate_unique<callable<Functor>>(f).release());
        return *this;
    }

    xbox_live_callback& operator=(const xbox_live_callback& rhs)
    {
        if (rhs.m_callable != nullptr)
        {
            m_callable = rhs.m_callable->copy();
        }
        else
        {
            m_callable = nullptr;
        }
        return *this;
    }

    xbox_live_callback& operator=(xbox_live_callback&& rhs)
    {
        m_callable = std::move(rhs.m_callable);
        return *this;
    }

    xbox_live_callback& operator=(nullptr_t)
    {
        m_callable = nullptr;
        return *this;
    }

    void operator()(Args... args) const
    {
        if (m_callable != nullptr)
        {
            (*m_callable)(args...);
        }
    }

    bool operator==(std::nullptr_t) noexcept
    {
        return m_callable == nullptr;
    }

    bool operator!=(std::nullptr_t) noexcept
    {
        return m_callable != nullptr;
    }

private:
    struct ICallable
    {
        virtual ~ICallable() = default;
        virtual void operator()(Args...) = 0;
        virtual xsapi_unique_ptr<ICallable> copy() = 0;
    };

    template <typename Functor>
    struct callable : public ICallable
    {
        callable(const Functor& functor) : m_functor(functor) { }
        ~callable() override = default;

        void operator()(Args... args) override
        {
            m_functor(args...);
        }

        xsapi_unique_ptr<ICallable> copy() override
        {
            return xsapi_unique_ptr<ICallable>(xsapi_allocate_unique<callable<Functor>>(m_functor).release());
        }

        Functor m_functor;
    };

    xsapi_unique_ptr<ICallable> m_callable;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
