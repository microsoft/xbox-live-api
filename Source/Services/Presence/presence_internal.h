// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "system_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
namespace presence {

class presence_data_internal
{
public:
    _XSAPIIMP presence_data_internal(
        _In_ xsapi_internal_string serviceConfigurationId,
        _In_ xsapi_internal_string presenceId
        );

    _XSAPIIMP presence_data_internal(
        _In_ xsapi_internal_string serviceConfigurationId,
        _In_ xsapi_internal_string presenceId,
        _In_ xsapi_internal_vector<xsapi_internal_string> presenceTokenIds
        );

    _XSAPIIMP presence_data_internal(
        _In_ const presence_data& presenceData
        );

    _XSAPIIMP const xsapi_internal_string& service_configuration_id() const;

    _XSAPIIMP const xsapi_internal_string& presence_id() const;

    _XSAPIIMP const xsapi_internal_vector<xsapi_internal_string>& presence_token_ids() const;

    presence_data_internal();

    bool should_serialize() const;

    web::json::value serialize() const;

private:
    xsapi_internal_string m_serviceConfigurationId;
    xsapi_internal_string m_presenceId;
    bool m_shouldSerialize;
    xsapi_internal_vector<xsapi_internal_string> m_presenceTokenIds;
};

/// <summary>
/// Represents media supporting Rich Presence features.
/// </summary>
class media_presence_data
{
public:
    /// <summary>
    /// Initializes a new instance of the MediaPresenceData class.
    /// </summary>
    /// <param name="mediaId">Id of the media used by the Bing catalog or the MediaProvider catalog.</param>
    /// <param name="mediaIdType">Type of the mediaId parameter.  The type identifies how the service should interpret the Id.</param>
    media_presence_data(
        _In_ string_t mediaId,
        _In_ presence_media_id_type mediaIdType
        );

    /// <summary>
    /// ID of the media being consumed.
    /// </summary>
    const string_t& media_id() const;

    /// <summary>
    /// The type of MediaId.  This will be MediaIdType::Bing for Xbox media, or MediaIdType::MediaPlatform for others.
    /// </summary>
    const presence_media_id_type media_id_type() const;

    media_presence_data();

    bool should_serialize() const;
    web::json::value serialize() const;

private:
    string_t m_mediaId;
    presence_media_id_type m_presenceMediaIdType;
    bool m_shouldSerialize;
};

class presence_activity_data
{
public:
    presence_activity_data();

    /// <summary>
    /// Represents activity data supporting Rich Presence features.
    /// </summary>
    ///<param name="presenceData">Identifies a string used to indicate presence.</param>
    ///<param name="mediapresenceData">Identifies a string used to indicate media presence.</param>
    presence_activity_data(
        _In_ presence_data_internal presenceData,
        _In_ media_presence_data mediaPresenceData
        );

    bool should_serialize() const;

    web::json::value serialize() const;

private:
    presence_data_internal m_presenceData;
    media_presence_data m_mediaPresenceData;
    bool m_shouldSerialize;
};

class presence_user_batch_request
{
public:
    /// <summary> 
    /// Retrieves the current presence data for multiple users or a specific social group with additional options to scope results.
    /// </summary>
    /// <param name="xboxUserIds">A list of Xbox user IDs.</param>
    /// <param name="socialGroup">A specific social group.</param>
    /// <param name="socialGroupOwnerXboxUserId">The Xbox user ID of the social group owner.</param>
    /// <param name="deviceTypes">The device types to return presence data for.</param>
    /// <param name="titleIds">The title IDs to return presence data for.</param>
    /// <param name="presenceDetailLevel">Value indicating the level of granularity in returned presence data</param>
    /// <param name="onlineOnly">Indicates if returned values are scoped to users that are online.</param>
    /// <param name="broadcastingOnly">Indicates if returned values are scoped to users that are currently broadcasting.</param>
    presence_user_batch_request(
        _In_ xsapi_internal_vector<xsapi_internal_string> xboxUserIds,
        _In_ xsapi_internal_string socialGroup,
        _In_ xsapi_internal_string socialGroupOwnerXboxUserId,
        _In_ xsapi_internal_vector<presence_device_type> deviceTypes,
        _In_ xsapi_internal_vector<uint32_t> titleIds,
        _In_ presence_detail_level presenceDetailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly
        );

    static xsapi_internal_string convert_detail_level_to_string(
        _In_ presence_detail_level level
        );

    web::json::value serialize() const;

private:
    xsapi_internal_vector<xsapi_internal_string> m_xboxUserIds;
    xsapi_internal_string m_socialGroup;
    xsapi_internal_string m_socialGroupOwnerXboxUserId;
    xsapi_internal_vector<xsapi_internal_string> m_deviceTypes;
    xsapi_internal_vector<xsapi_internal_string> m_titleIds;
    presence_detail_level m_presenceDetailLevel;
    bool m_onlineOnly;
    bool m_broadcastingOnly;
};

class presence_title_request
{
public:
    /// <summary>
    /// Retrieves the current user presence data.
    /// </summary>
    /// <param name="isUserActive">Indicates if the user is active.</param>
    /// <param name="presenceData">Contains current user presence data.</param>
    /// <param name="mediaPresenceData">Contains current media presence data.</param>
    presence_title_request(
        _In_ bool isUserActive,
        _In_ presence_data_internal presenceData = presence_data_internal(),
        _In_ media_presence_data mediaPresenceData = media_presence_data()
        );

    web::json::value serialize();

private:
    bool m_isUserActive;
    presence_activity_data m_presenceActivityData;
};

class title_presence_change_event_args_internal
{
public:
    _XSAPIIMP const xsapi_internal_string& xbox_user_id() const;

    _XSAPIIMP uint32_t title_id() const;

    _XSAPIIMP title_presence_state title_state() const;

    title_presence_change_event_args_internal();

    title_presence_change_event_args_internal(
        _In_ xsapi_internal_string xboxUserId,
        _In_ uint32_t titleId,
        _In_ title_presence_state titleState
        );

private:
    xsapi_internal_string m_xboxUserId;
    uint32_t m_titleId;
    title_presence_state m_titleState;
};

class title_presence_change_subscription_internal : public real_time_activity::real_time_activity_subscription
{
public:
    _XSAPIIMP const xsapi_internal_string& xbox_user_id() const;

    _XSAPIIMP uint32_t title_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    title_presence_change_subscription_internal(
        _In_ xsapi_internal_string xboxUserId,
        _In_ uint32_t titleId,
        _In_ xbox_live_callback<std::shared_ptr<title_presence_change_event_args_internal>> handler,
        _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
        );

protected:
    void on_subscription_created(_In_ uint32_t id, _In_ const web::json::value& data) override;
    void on_event_received(_In_ const web::json::value& data) override;

private:
    xsapi_internal_string m_xboxUserId;
    uint32_t m_titleId;
    xbox_live_callback<std::shared_ptr<title_presence_change_event_args_internal>> m_handler;
};

class device_presence_change_event_args_internal
{
public:
    _XSAPIIMP const xsapi_internal_string& xbox_user_id() const;

    _XSAPIIMP presence_device_type device_type() const;

    _XSAPIIMP bool is_user_logged_on_device() const;

    device_presence_change_event_args_internal();

    device_presence_change_event_args_internal(
        _In_ xsapi_internal_string xboxUserId,
        _In_ presence_device_type deviceType,
        _In_ bool isUserLoggedOnDevice
        );

private:
    xsapi_internal_string m_xboxUserId;
    presence_device_type m_deviceType;
    bool m_isUserLoggedOnDevice;
};

class device_presence_change_subscription_internal : public xbox::services::real_time_activity::real_time_activity_subscription
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    device_presence_change_subscription_internal(
        _In_ xsapi_internal_string xboxUserId,
        _In_ xbox_live_callback<std::shared_ptr<device_presence_change_event_args_internal>> handler,
        _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
        );

    _XSAPIIMP const xsapi_internal_string& xbox_user_id() const;

protected:
    void on_subscription_created(_In_ uint32_t id, _In_ const web::json::value& data) override;
    void on_event_received(_In_ const web::json::value& data) override;

private:
    xsapi_internal_string m_xboxUserId;
    xbox_live_callback<std::shared_ptr<device_presence_change_event_args_internal>> m_devicePresenceChangeHandler;
};


class presence_broadcast_record_internal
{
public:
    _XSAPIIMP const xsapi_internal_string& broadcast_id() const;

    _XSAPIIMP const xsapi_internal_string& session() const;

    _XSAPIIMP const xsapi_internal_string& provider() const;

    _XSAPIIMP uint32_t viewer_count() const;

    _XSAPIIMP const utility::datetime& start_time() const;

    presence_broadcast_record_internal();

    bool operator!=(_In_ const presence_broadcast_record_internal& rhs) const;

    static xbox_live_result<std::shared_ptr<presence_broadcast_record_internal>> deserialize(_In_ const web::json::value& inputJson);

private:
    xsapi_internal_string m_broadcastId;
    xsapi_internal_string m_session;
    xsapi_internal_string m_provider;
    uint32_t m_viewerCount;
    utility::datetime m_startTime;
};

class presence_title_record_internal
{
public:
    _XSAPIIMP uint32_t title_id() const;

    _XSAPIIMP const xsapi_internal_string& title_name() const;

    _XSAPIIMP const utility::datetime& last_modified_date() const;

    _XSAPIIMP bool is_title_active() const;

    _XSAPIIMP const xsapi_internal_string& presence() const;

    _XSAPIIMP presence_title_view_state presence_title_view() const;

    _XSAPIIMP const std::shared_ptr<presence_broadcast_record_internal> broadcast_record() const;

    presence_title_record_internal();

    presence_title_record_internal(
        _In_ uint32_t titleId,
        _In_ title_presence_state titlePresenceState
        );

    bool operator!=(_In_ const presence_title_record_internal& rhs) const;

    static xbox_live_result<std::shared_ptr<presence_title_record_internal>> deserialize(_In_ const web::json::value& json);

    static presence_title_view_state convert_string_to_presence_title_view_state(
        _In_ const xsapi_internal_string& value
        );

private:
    uint32_t m_titleId;
    xsapi_internal_string m_titleName;
    utility::datetime m_lastModifiedDate;
    bool m_isTitleActive;
    xsapi_internal_string m_presence;
    presence_title_view_state m_titleViewState;
    std::shared_ptr<presence_broadcast_record_internal> m_broadcastRecord;
};

class presence_device_record_internal
{
public:
    _XSAPIIMP presence_device_type device_type() const;

    _XSAPIIMP const xsapi_internal_vector<std::shared_ptr<presence_title_record_internal>>& presence_title_records() const;

    presence_device_record_internal();

    presence_device_record_internal(
        _In_ presence_device_type deviceType
        );

    bool operator!=(_In_ const presence_device_record_internal& rhs);

    static presence_device_type convert_string_to_presence_device_type(_In_ const xsapi_internal_string& value);

    static xsapi_internal_string convert_presence_device_type_to_string(_In_ presence_device_type deviceType);

    void add_title_record(
        _In_ uint32_t titleId,
        _In_ title_presence_state titlePresenceState
        );

    static xbox_live_result<std::shared_ptr<presence_device_record_internal>> deserialize(
        _In_ const web::json::value& inputJson
        );

private:
    xsapi_internal_unordered_map<uint32_t, std::shared_ptr<presence_title_record_internal>> create_map_from_title_records(
        _In_ const xsapi_internal_vector<std::shared_ptr<presence_title_record_internal>>& titleRecords
        ) const;

    xsapi_internal_vector<std::shared_ptr<presence_title_record_internal>> m_presenceTitleRecords;
    presence_device_type m_deviceType;
};

class presence_media_record_internal
{
public:
    _XSAPIIMP const xsapi_internal_string& media_id() const;

    _XSAPIIMP presence_media_id_type media_id_type() const;

    _XSAPIIMP const xsapi_internal_string& name() const;

    presence_media_record_internal();

    static xbox_live_result<std::shared_ptr<presence_media_record_internal>> deserialize(_In_ const web::json::value& inputJson);

    static presence_media_id_type convert_string_to_media_id_type(
        _In_ const xsapi_internal_string& value
        );

private:
    xsapi_internal_string m_mediaId;
    presence_media_id_type m_mediaIdType;
    xsapi_internal_string m_name;
};

class presence_record_internal
{
public:
    _XSAPIIMP presence_record_internal();

    _XSAPIIMP const xsapi_internal_string& xbox_user_id() const;

    _XSAPIIMP user_presence_state user_state() const;

    _XSAPIIMP const xsapi_internal_vector<std::shared_ptr<presence_device_record_internal>>& presence_device_records() const;

    _XSAPIIMP bool is_user_playing_title(_In_ uint32_t titleId) const;

    void update_title(
        _In_ uint32_t titleId,
        _In_ title_presence_state state
        );

    void update_device(
        _In_ presence_device_type deviceType,
        _In_ bool isUserLoggedIn
        );

    static xbox_live_result<std::shared_ptr<presence_record_internal>> deserialize(_In_ const web::json::value& json);

    static user_presence_state convert_string_to_user_presence_state(_In_ const xsapi_internal_string& value);

private:
    static xsapi_internal_unordered_map<presence_device_type, presence_device_record_internal> create_map_from_device_list(_In_ const xsapi_internal_vector<presence_device_record_internal>& deviceRecords);

    xsapi_internal_string m_xboxUserId;
    user_presence_state m_userState;
    xsapi_internal_vector<std::shared_ptr<presence_device_record_internal>> m_presenceDeviceRecords;
};

class presence_service_internal : public std::enable_shared_from_this<presence_service_internal>
{
public:
    presence_service_internal(
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService,
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig
        );

    xbox_live_result<std::shared_ptr<title_presence_change_subscription_internal>> subscribe_to_title_presence_change(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ uint32_t titleId
        );

    xbox_live_result<void> unsubscribe_from_title_presence_change(
        _In_ std::shared_ptr<title_presence_change_subscription_internal> subscription
        );

    function_context add_title_presence_changed_handler(
        _In_ xbox_live_callback<std::shared_ptr<title_presence_change_event_args_internal>> handler
        );

    void remove_title_presence_changed_handler(_In_ function_context context);

    xbox_live_result<std::shared_ptr<device_presence_change_subscription_internal>> subscribe_to_device_presence_change(
        _In_ const xsapi_internal_string& xboxUserId
        );

    xbox_live_result<void> unsubscribe_from_device_presence_change(
        _In_ std::shared_ptr<device_presence_change_subscription_internal> subscription
        );

    function_context add_device_presence_changed_handler(
        _In_ xbox_live_callback<std::shared_ptr<device_presence_change_event_args_internal>> handler
        );

    void remove_device_presence_changed_handler(
        _In_ function_context context
        );

    xbox_live_result<void> set_presence(
        _In_ bool isUserActiveInTitle,
        _In_ presence_data_internal presenceData,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<uint32_t>> callback
        );

    xbox_live_result<void> get_presence(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<presence_record_internal>>> callback
        );

    xbox_live_result<void> get_presence_for_multiple_users(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>> callback
        );

    xbox_live_result<void> get_presence_for_multiple_users(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
        _In_ const xsapi_internal_vector<presence_device_type>& deviceTypes,
        _In_ const xsapi_internal_vector<uint32_t>& titleIds,
        _In_ presence_detail_level presenceDetailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>> callback
        );

    xbox_live_result<void> get_presence_for_social_group(
        _In_ const xsapi_internal_string& socialGroup,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>> callback
        );

    xbox_live_result<void> get_presence_for_social_group(
        _In_ const xsapi_internal_string& socialGroup,
        _In_ const xsapi_internal_string& socialGroupOwnerXboxUserId,
        _In_ const xsapi_internal_vector<presence_device_type>& deviceTypes,
        _In_ const xsapi_internal_vector<uint32_t>& titleIds,
        _In_ presence_detail_level peoplehubDetailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>> callback
        );

    ~presence_service_internal();

    std::shared_ptr<xbox_live_context_settings> xbox_live_context_settings();

    // test hook 
    static void set_presence_set_finished_handler(const std::function<void(int heartBeatDelayInMins)>& onSetPresenceFinish);

    static xsapi_internal_string set_presence_sub_path(
        _In_ const xsapi_internal_string& xboxUserId
        );

private:
    xsapi_internal_string get_presence_sub_path(
        _In_ const xsapi_internal_string& xboxUserId
        );

    xsapi_internal_string get_presence_user_batch_subpath();

    xsapi_internal_string get_presence_for_social_group_subpath(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ const xsapi_internal_string& socialGroup
        );

    void device_presence_changed(_In_ std::shared_ptr<device_presence_change_event_args_internal> eventArgs);
    void title_presence_changed(_In_ std::shared_ptr<title_presence_change_event_args_internal> eventArgs);

    xbox::services::system::xbox_live_mutex m_titlePresenceChangeHandlerLock;
    xbox::services::system::xbox_live_mutex m_devicePresenceChangeHandlerLock;

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config_internal> m_appConfig;

    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_realTimeActivityService;
    xsapi_internal_unordered_map<function_context, xbox_live_callback<std::shared_ptr<device_presence_change_event_args_internal>>> m_devicePresenceChangeHandler;
    xsapi_internal_unordered_map<function_context, xbox_live_callback<std::shared_ptr<title_presence_change_event_args_internal>>> m_titlePresenceChangeHandler;
    function_context m_devicePresenceChangeHandlerCounter;
    function_context m_titlePresenceChangeHandlerCounter;

    friend class presence_writer;
};

#if !TV_API

class presence_writer : public std::enable_shared_from_this <presence_writer>
{
public:
    static std::shared_ptr<presence_writer> get_presence_writer_singleton();
    presence_writer();

    void start_writer(_In_ std::shared_ptr<presence_service_internal> presenceServiceInternal);
    void stop_writer(_In_ const xsapi_internal_string& xboxLiveUserId);
    void handle_timer_trigger();

    void set_inactive_in_title(_In_ std::shared_ptr<presence_service_internal> presenceServiceInternal);
    void set_active_in_title();

    static const uint32_t s_defaultHeartBeatDelayInMins = 5;

    size_t tracking_count() { return m_presenceServices.size(); }

private:
    void start_timer(_In_ std::weak_ptr<presence_writer> thisWeakPtr);

    xbox::services::system::xbox_live_mutex m_lock;
    bool m_writerInProgress;
    std::unordered_map<xsapi_internal_string, std::shared_ptr<presence_service_internal>> m_presenceServices;
    int m_heartBeatDelayInMins;

#if UWP_API || UNIT_TEST_SERVICES
    Windows::System::Threading::ThreadPoolTimer^ m_timer;
#else
    bool m_timerComplete;
#endif

    std::atomic<bool> m_isCallInProgress;
};
#endif

} // namespace presence
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END