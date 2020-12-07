// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if HC_PLATFORM_IS_MICROSOFT
#include <Rpc.h>
#include <codecvt>
#endif
#if XSAPI_WRL_EVENTS_SERVICE
#include <wrl.h>
#endif
#include "errors_legacy.h"
#include "xsapi-c/pal.h"

HC_DECLARE_TRACE_AREA(XSAPI);

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
    class xbox_live_services_settings;
    class xbox_system_factory;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN
    class stats_manager;
NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN
    class NotificationService;
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN
    class MultiplayerManager;
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN
    class RealTimeActivityServiceFactory;
NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
    class service_call_logging_config;
    class service_call_logger_protocol;
    class service_call_logger;
    class logger;
    class xsapi_telemetry;
    class AppConfig;
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#ifndef __min
#define __min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a,b)            (((a) < (b)) ? (b) : (a))
#endif  

inline bool operator<(const xbox::services::datetime& lhs, const xbox::services::datetime& rhs)
{
    return lhs.to_interval() < rhs.to_interval();
}

struct xsapi_singleton
{
    xsapi_singleton() = default;
    ~xsapi_singleton();

    HRESULT Initialize(_In_ const XblInitArgs* args);

    std::mutex m_rtaActivationCounterLock;
    xsapi_internal_unordered_map<uint64_t, uint32_t> m_rtaActiveSocketCountPerUser;

    std::mutex m_singletonLock;
    std::mutex m_serviceSettingsLock;
    std::mutex m_telemetryLock;
    std::shared_ptr<xbox::services::system::xbox_live_services_settings> m_xboxServiceSettingsSingleton;

#if HC_PLATFORM == HC_PLATFORM_XDK || XSAPI_UNIT_TESTS
    std::mutex m_achievementServiceInitLock;
    bool m_bHasAchievementServiceInitialized{ false };
    std::string m_eventProviderName;
    GUID m_eventPlayerSessionId{};
#endif

    // from Shared\xbox_live_app_config.cpp
    std::shared_ptr<AppConfig> m_appConfigSingleton;

    // from Misc\notification_service.cpp
    std::shared_ptr<xbox::services::notification::NotificationService> m_notificationSingleton;

    // from Shared\service_call_logging_config.cpp
    std::shared_ptr<service_call_logging_config> m_serviceLoggingConfigSingleton;

    // from Shared\xbox_live_app_config.cpp
    std::mutex m_serviceLoggerProtocolSingletonLock;
    std::shared_ptr<service_call_logger_protocol> m_serviceLoggerProtocolSingleton;

    // from Shared\utils_locales.cpp
    xsapi_internal_string m_locales{ "en-US" };
    std::mutex m_locale_lock;
    bool m_custom_locale_override{ false };

    // from Shared\service_call_logger_data.cpp
    uint32_t m_loggerId{ 0 };

    // from Shared\service_call_logger.cpp
    std::shared_ptr<service_call_logger> m_serviceLoggerSingleton;

    // from Services\RealTimeActivity\real_time_activity_service_factory.cpp
    std::shared_ptr<real_time_activity::RealTimeActivityServiceFactory> m_rtaFactoryInstance;

    // from Services\Presence\presence_service_internal.cpp
    std::function<void(int heartBeatDelayInMins)> m_onSetPresenceFinish;

    // from Shared\Logger\log.cpp
    std::shared_ptr<logger> m_logger;

    // from Shared\xbox_system_factory.cpp
    std::shared_ptr<system::xbox_system_factory> m_factoryInstance;

#if XSAPI_DESKTOP_BRIDGE
    // from System\user_impl_idp.cpp
    uint64_t m_build = 0;
    bool m_supportsGetTokenSilently;
#endif

#if HC_PLATFORM == HC_PLATFORM_ANDROID
    // TODO This is a temporary workaround for TCUI. XSAPI C++ TCUI API's accept a xal_user_handle, from which we extract
    // XUID and privileges and call into Java code. Java code then calls back into XSAPI to make an HTTP call, but it does not pass and user
    // context or xal_user_handle but it most likely should. Previously we just remembered the last signed in user, so we can 
    // emulate that behavior again for now.
    std::atomic<XalUserHandle> m_lastSignedInUser{ nullptr };
#endif

    // from Shared\telemetry.cpp
    std::shared_ptr<xsapi_telemetry> m_telemetrySingleton;

    // from Services\Multiplayer\Manager\multiplayer_client_pending_request.cpp
    String m_deviceId;

    XblApiType m_apiType{ XblApiType::XblCApi };
};

std::shared_ptr<xsapi_singleton> get_xsapi_singleton();
XTaskQueueHandle get_xsapi_singleton_async_queue();

#ifndef _In_reads_bytes_
#define _In_reads_bytes_(s)
#endif

class utils
{
public:
    static int interlocked_increment(volatile long& incrementNum);
    static int interlocked_decrement(volatile long& decrementNum);
    
    static xsapi_internal_string encode_uri(_In_ const xsapi_internal_string& data, _In_ xbox::services::uri::components::component component = xbox::services::uri::components::full_uri);

    static xsapi_internal_string headers_to_string(_In_ const xsapi_internal_http_headers& headers);

    static web::http::http_headers string_to_headers(_In_ const string_t& headers);

    static xsapi_internal_string get_query_from_params(_In_ const xsapi_internal_vector<xsapi_internal_string>& params);

    static xsapi_internal_string create_guid(_In_ bool removeBraces);

#if HC_PLATFORM_IS_MICROSOFT
    static std::error_code guid_from_string(_In_ const string_t& str, _In_ GUID* guid, _In_ bool withBraces);
#endif

    static String format_secure_device_address(String deviceAddress);
    static String parse_secure_device_address(String secureDeviceAddress);

    static void append_paging_info(
        _In_ xbox::services::uri_builder& uriBuilder,
        _In_ unsigned int skipItems,
        _In_ unsigned int maxItems,
        _In_opt_ xsapi_internal_string continuationToken
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

    static inline xbox::services::datetime DatetimeFromTimeT(time_t time)
    {
        uint64_t result = EPOCH_OFFSET + time;
        result *= TICKS_PER_SEC; // convert to 10e-7
        return xbox::services::datetime() + result;
    }

    static inline time_t TimeTFromDatetime(const xbox::services::datetime& datetime)
    {
        uint64_t seconds = datetime.to_interval() / TICKS_PER_SEC;
        if (seconds >= EPOCH_OFFSET)
        {
            return (time_t)(seconds - EPOCH_OFFSET);
        }
        else
        {
            // If time is before epoch, 0 is returned.
            return 0;
        }
    }

    static xsapi_internal_string escape_special_characters(const xsapi_internal_string& str);

    static inline int str_icmp(const string_t &left, const string_t &right)
    {
        return char_t_cmp(left.c_str(), right.c_str());
    }

    static inline int str_icmp_internal(const xsapi_internal_string& left, const xsapi_internal_string& right)
    {
        return str_icmp(left.data(), right.data());
    }

    static inline int str_icmp(const char* left, const char* right)
    {
#if HC_PLATFORM_IS_MICROSOFT
        return _stricmp(left, right);
#else
        return strcasecmp(left, right);
#endif 
    }

    static inline int char_t_cmp(const char_t* left, const char_t* right)
    {
#if HC_PLATFORM_IS_MICROSOFT
        return _wcsicmp(left, right);
#else
        return strcasecmp(left, right);
#endif 
    }

    static std::vector<string_t> string_split(
        _In_ const string_t& string,
        _In_ string_t::value_type seperator
    );

    static xsapi_internal_vector<xsapi_internal_string> string_split(
        _In_ const xsapi_internal_string& string,
        _In_ xsapi_internal_string::value_type seperator
    );

    static xbl_error_code convert_exception_to_xbox_live_error_code();

    static string_t vector_join(
        _In_ const std::vector<string_t>& vector,
        _In_ string_t::value_type seperator
    );

    static xsapi_internal_string vector_join(
        _In_ const std::vector<xsapi_internal_string>& vector,
        _In_ xsapi_internal_string::value_type seperator
    );

#if HC_PLATFORM_IS_MICROSOFT
    static void convert_unix_time_to_filetime(
        _In_ std::time_t t,
        _In_ FILETIME* ft);

    static void convert_timepoint_to_filetime(
        _In_ const chrono_clock_t::time_point& time_point,
        _Inout_ uint64_t& largeInt);
#endif

    static HRESULT convert_exception_to_hresult();
    static HRESULT convert_xbox_live_error_code_to_hresult(_In_ const std::error_code& errCode);

    static xsapi_internal_string convert_hresult_to_error_name(_In_ long hr);
    static HRESULT convert_http_status_to_hresult(_In_ uint32_t httpStatusCode);
    static xbl_error_code convert_http_status_to_xbox_live_error_code(_In_ uint32_t statusCode);

    static xsapi_internal_string create_xboxlive_endpoint(
        _In_ const xsapi_internal_string& subpath,
        _In_ const std::shared_ptr<AppConfig>& appConfig,
        _In_ const xsapi_internal_string& protocol = "https"
    );

#if HC_PLATFORM_IS_MICROSOFT
    static xsapi_internal_string internal_string_from_utf16(_In_z_ const wchar_t* utf16);
#endif
#if XSAPI_WRL_EVENTS_SERVICE
    static Microsoft::WRL::Wrappers::HString HStringFromUtf8(_In_z_ const char* utf8);
#endif
#if __cplusplus_winrt
    static Platform::String^ PlatformStringFromUtf8(_In_z_ const char* utf8);
#endif

    static std::string std_string_from_string_t(_In_ const string_t& stringt);
    static xsapi_internal_string internal_string_from_string_t(_In_ const string_t& stringt);
    static xsapi_internal_string internal_string_from_char_t(_In_ const char_t* char_t);

    static string_t string_t_from_internal_string(_In_ const xsapi_internal_string& internalString);
    static string_t string_t_from_utf8(_In_z_ const char* utf8);

    static int utf8_from_char_t(_In_z_ const char_t* inArray, _Out_writes_z_(cchOutArray) char* outArray, _In_ int cchOutArray);
    static int char_t_from_utf8(_In_z_ const char* inArray, _Out_writes_z_(cchOutArray) char_t* outArray, _In_ int cchOutArray);

    static void generate_locales();
    static xsapi_internal_string get_locales();

    static void set_locales(_In_ const xsapi_internal_string& locale);

#if !XSAPI_NO_PPL

    template<typename T>
    static pplx::task <xbl_result<T>> create_exception_free_task(
        _In_ const pplx::task <xbl_result<T>>& t
    )
    {
        return t.then([](pplx::task <xbox::services::xbl_result<T>> result)
        {
            try
            {
                return result.get();
            }
            catch (const std::exception& e)
            {
                xbl_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code();
                return xbl_result<T>(err, e.what());
            }
#ifdef __cplusplus_winrt
            catch (Platform::Exception^ e)
            {
                xbl_error_code errc = static_cast<xbl_error_code>(e->HResult);
                return xbl_result<T>(errc, xbox::services::convert::to_utf8string(e->Message->Data()));
            }
#endif
        });
    }
#endif // !XSAPI_NO_PPL

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

    static xsapi_internal_vector<xsapi_internal_string> internal_string_vector_from_utf8_array(
        _In_ const char** utf8array,
        _In_ uint32_t arraySize
    )
    {
        xsapi_internal_vector<xsapi_internal_string> internalVector(arraySize);
        for (uint32_t i = 0; i < arraySize; ++i)
        {
            internalVector[i] = utf8array[i];
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

    static string_t replace_sub_string(
        _In_ const string_t& source,
        _In_ const string_t& pattern,
        _In_ const string_t& replacement
    );

    static string_t read_file_to_string(
        _In_ const string_t& filePath
    );

    inline static uint32_t string_t_to_uint32(
        _In_ const string_t& str
    )
    {
#if HC_PLATFORM_IS_MICROSOFT
        return std::stoul(str);
#else
        return (uint32_t)std::strtoul(str.c_str(), nullptr, 0);
#endif
    }

    inline static uint32_t internal_string_to_uint32(
        _In_ const xsapi_internal_string& str
    )
    {
        return (uint32_t)std::strtoul(str.c_str(), nullptr, 0);
    }

    inline static string_t uint32_to_string_t(
        _In_ uint32_t val
    )
    {
        stringstream_t stream;
        stream << val;
        return stream.str();
    }

    inline static xsapi_internal_string uint32_to_internal_string(
        _In_ uint32_t val
    )
    {
        xsapi_internal_stringstream stream;
        stream << val;
        return stream.str();
    }

    inline static xsapi_internal_string uint64_to_internal_string(
        _In_ uint64_t val
    )
    {
        xsapi_internal_stringstream stream;
        stream << val;
        return stream.str();
    }

    inline static uint64_t string_t_to_uint64(
        _In_ const string_t& str
    )
    {
        return uint64_from_char_t(str.data());
    }

    inline static uint64_t uint64_from_char_t(
        _In_ const char_t* str
    )
    {
#if HC_PLATFORM_IS_MICROSOFT
        return _wtoi64(str);
#else
        return strtoull(str, nullptr, 0);
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
#if HC_PLATFORM_IS_MICROSOFT
        return _wtoi(str.c_str());
#else
        return std::atoi(str.c_str());
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
        _In_reads_bytes_(sizeInWords) char_t* destinationCharArr,
        _In_ size_t sizeInWords,
        _In_ const char_t* sourceCharArr
        );
        
    static size_t strcpy(
        _In_ char* destinationCharArr,
        _In_ size_t sizeInWords,
        _In_ const char* sourceCharArr
        );

    static std::vector<string_t> string_array_to_string_vector(
        const char* *stringArray,
        size_t stringArrayCount
        );

    static xsapi_internal_vector<xsapi_internal_string> string_array_to_internal_string_vector(
        const char* *stringArray,
        size_t stringArrayCount
        );
    
    static xsapi_internal_vector<xsapi_internal_string> xuid_array_to_internal_string_vector(
        uint64_t* xuidArray,
        size_t xuidArrayCount
        );

    static xsapi_internal_vector<uint32_t> uint32_array_to_internal_vector(
        uint32_t* intArray,
        size_t intArrayCount
        );

    // date and time constants
    static constexpr uint64_t TICKS_PER_MS = 10000;
    static constexpr uint64_t TICKS_PER_SEC = (1000 * TICKS_PER_MS);
    static constexpr uint64_t EPOCH_OFFSET = 11644473600LL;
    
private:
    template<typename T>
    struct SmartPointerContainer
    {
        virtual ~SmartPointerContainer() {}
        virtual std::shared_ptr<T> GetShared() const = 0;
    };

    template<typename T>
    struct SharedPointerContainer : public SmartPointerContainer<T>
    {
        SharedPointerContainer(std::shared_ptr<T> sharedPtr) : m_sharedPtr(std::move(sharedPtr)) {}

        std::shared_ptr<T> GetShared() const override
        {
            return m_sharedPtr;
        }
    private:
        std::shared_ptr<T> m_sharedPtr;
    };

    template<typename T>
    struct WeakPointerContainer : public SmartPointerContainer<T>
    {
        WeakPointerContainer(std::weak_ptr<T> weakPtr) : m_weakPtr(std::move(weakPtr)) {}

        std::shared_ptr<T> GetShared() const override
        {
            return m_weakPtr.lock();
        }
    private:
        std::weak_ptr<T> m_weakPtr;
    };

public:
    // Returns a handle that can be used to later retrieve the stored shared_ptr.
    // Used when an asynchronous flat-C API requires a shared_ptr as its context.
    template<typename T>
    static void* store_shared_ptr(std::shared_ptr<T> contextSharedPtr)
    {
        return Make<SharedPointerContainer<T>>(contextSharedPtr);
    }

    // Returns a handle that can be used to later retrieve the stored weak_ptr (as a shared_ptr, so
    // it will be null if the object has since been cleaned up).
    template<typename T>
    static void* store_weak_ptr(std::weak_ptr<T> contextWeakPtr)
    {
        return Make<WeakPointerContainer<T>>(contextWeakPtr);
    }

    // Retrieves a shared previouly stored with store_shared_ptr or store_weak_ptr. If releaseContext is true
    // the copy of the smart pointer stored internally is deleted.
    template<typename T>
    static std::shared_ptr<T> get_shared_ptr(void* context, bool releaseContext = true)
    {
        auto smartPtrContainer = reinterpret_cast<SmartPointerContainer<T>*>(context);
        auto sharedPtr = smartPtrContainer->GetShared();
        if (releaseContext)
        {
            Delete(smartPtrContainer);
        }
        return sharedPtr;
    }

    static time_t time_t_from_datetime(const xbox::services::datetime& datetime);

    // Creates an asyncBlock that clean itself up in the completion callback
    static XAsyncBlock* MakeDefaultAsyncBlock(XTaskQueueHandle queue = get_xsapi_singleton_async_queue());

    static XAsyncBlock* MakeAsyncBlock(XTaskQueueHandle queue = get_xsapi_singleton_async_queue(), void* context = nullptr, XAsyncCompletionRoutine* callback = nullptr);

private:
    static xsapi_internal_vector<xsapi_internal_string> get_locale_list();
    
    utils();
    utils(const utils&);
    utils& operator=(const utils&);
};

template<typename K, typename V>
class bimap
{
public:
    typename xsapi_internal_unordered_map<K, V>::iterator find(const K& key)
    {
        return m_map.find(key);
    }

    typename xsapi_internal_unordered_map<K, V>::iterator begin()
    {
        return m_map.begin();
    }

    typename xsapi_internal_unordered_map<K, V>::iterator end()
    {
        return m_map.end();
    }

    typename xsapi_internal_unordered_map<V, K>::iterator reverse_find(const V& value)
    {
        return m_reverseMap.find(value);
    }

    typename xsapi_internal_unordered_map<V, K>::iterator reverse_begin()
    {
        return m_reverseMap.begin();
    }

    typename xsapi_internal_unordered_map<V, K>::iterator reverse_end()
    {
        return m_reverseMap.end();
    }

    void insert(const K& key, const V& value)
    {
        m_map[key] = value;
        m_reverseMap[value] = key;
    }

    void erase(typename xsapi_internal_unordered_map<K, V>::iterator iterator)
    {
        m_reverseMap.erase(iterator->second);
        m_map.erase(iterator);
    }

private:
    xsapi_internal_unordered_map<K, V> m_map;
    xsapi_internal_unordered_map<V, K> m_reverseMap;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
