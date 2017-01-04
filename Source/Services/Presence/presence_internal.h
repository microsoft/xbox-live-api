//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "system_internal.h"

namespace xbox { namespace services { namespace presence {

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
        _In_ presence_data presenceData,
        _In_ media_presence_data mediaPresenceData
        );

    bool should_serialize() const;

    web::json::value serialize() const;

private:
    presence_data m_presenceData;
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
        _In_ std::vector<string_t> xboxUserIds,
        _In_ string_t socialGroup,
        _In_ string_t socialGroupOwnerXboxUserId,
        _In_ std::vector<presence_device_type> deviceTypes,
        _In_ std::vector<uint32_t> titleIds,
        _In_ presence_detail_level presenceDetailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly
        );

    static string_t convert_detail_level_to_string(
        _In_ presence_detail_level level
        );
    web::json::value serialize() const;

private:
    std::vector<string_t> m_xboxUserIds;
    string_t m_socialGroup;
    string_t m_socialGroupOwnerXboxUserId;
    std::vector<string_t> m_deviceTypes;
    std::vector<string_t> m_titleIds;
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
        _In_ presence_data presenceData = presence_data(),
        _In_ media_presence_data mediaPresenceData = media_presence_data()
        );

    web::json::value serialize();

private:
    bool m_isUserActive;
    presence_activity_data m_presenceActivityData;
};

class presence_service_impl : public std::enable_shared_from_this<presence_service_impl>
{
public:
    presence_service_impl(
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService,
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    xbox_live_result<std::shared_ptr<title_presence_change_subscription>> subscribe_to_title_presence_change(
        _In_ const string_t& xboxUserId,
        _In_ uint32_t titleId
        );

    xbox_live_result<void> unsubscribe_from_title_presence_change(
        _In_ std::shared_ptr<title_presence_change_subscription> subscription
        );

    function_context add_title_presence_changed_handler(_In_ std::function<void(const title_presence_change_event_args&)> handler);

    void remove_title_presence_changed_handler(_In_ function_context context);

    xbox_live_result<std::shared_ptr<device_presence_change_subscription>> subscribe_to_device_presence_change(
        _In_ const string_t& xboxUserId
        );

    xbox_live_result<void> unsubscribe_from_device_presence_change(
        _In_ std::shared_ptr<device_presence_change_subscription> subscription
        );

    function_context add_device_presence_changed_handler(
        _In_ std::function<void(const device_presence_change_event_args&)> handler
        );

    void remove_device_presence_changed_handler(
        _In_ function_context context
        );

    pplx::task<xbox_live_result<void>> set_presence(
        _In_ bool isUserActiveInTitle
        );

    pplx::task<xbox_live_result<void>> set_presence(
        _In_ bool isUserActiveInTitle,
        _In_ presence_data presenceData
        );

    pplx::task<xbox_live_result<presence_record>> get_presence(
        _In_ const string_t& xboxUserId
        );

    pplx::task<xbox_live_result<std::vector<presence_record>>> get_presence_for_multiple_users(
        _In_ const std::vector<string_t>& xboxUserIds
        );

    pplx::task<xbox_live_result<std::vector<presence_record>>> get_presence_for_multiple_users(
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const std::vector<presence_device_type>& deviceTypes,
        _In_ const std::vector<uint32_t>& titleIds,
        _In_ presence_detail_level presenceDetailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly
        );

    pplx::task<xbox_live_result<std::vector<presence_record>>> get_presence_for_social_group(
        _In_ const string_t& socialGroup
        );

    pplx::task<xbox_live_result<std::vector<presence_record>>> get_presence_for_social_group(
        _In_ const string_t& socialGroup,
        _In_ const string_t& socialGroupOwnerXboxUserId,
        _In_ const std::vector<presence_device_type>& deviceTypes,
        _In_ const std::vector<uint32_t>& titleIds,
        _In_ presence_detail_level peoplehubDetailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly
        );

    std::shared_ptr<xbox_live_context_settings> _Xbox_live_context_settings() { return m_xboxLiveContextSettings; }

    ~presence_service_impl();

    // test hook 
    static void set_presence_set_finished_handler(const std::function<void(int heartBeatDelayInMins)>& onSetPresenceFinish) { s_onSetPresenceFinish = onSetPresenceFinish; }

    static string_t set_presence_sub_path(
        _In_ const string_t& xboxUserId
        );

private:
    string_t get_presence_sub_path(
        _In_ const string_t& xboxUserId
        );

    string_t get_presence_user_batch_subpath();

    string_t get_presence_for_social_group_subpath(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& socialGroup
        );

    pplx::task<xbox_live_result<uint32_t>> set_presence_helper(
        _In_ bool isUserActiveInTitle,
        _In_ presence_data presenceData
        );

    static std::function<void(int heartBeatDelayInMins)> s_onSetPresenceFinish;

    void device_presence_changed(_In_ const device_presence_change_event_args& eventArgs);
    void title_presence_changed(_In_ const title_presence_change_event_args& eventArgs);

    xbox::services::system::xbox_live_mutex m_titlePresenceChangeHandlerLock;
    xbox::services::system::xbox_live_mutex m_devicePresenceChangeHandlerLock;

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_realTimeActivityService;
    std::unordered_map<function_context, std::function<void(const device_presence_change_event_args&)>> m_devicePresenceChangeHandler;
    std::unordered_map<function_context, std::function<void(const title_presence_change_event_args&)>> m_titlePresenceChangeHandler;
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

    void start_writer(_In_ std::shared_ptr<presence_service_impl> presenceServiceImpl);
    void stop_writer(_In_ const string_t& xboxLiveUserId);
    void handle_timer_trigger();

    void set_inactive_in_title(_In_ std::shared_ptr<presence_service_impl> presenceServiceImpl);
    void set_active_in_title();

    static const uint32_t s_defaultHeartBeatDelayInMins = 5;

    size_t tracking_count() { return m_presenceServices.size(); }

private:
    void start_timer(_In_ std::weak_ptr<presence_writer> thisWeakPtr);

    xbox::services::system::xbox_live_mutex m_lock;
    bool m_writerInProgress;
    std::unordered_map<string_t, std::shared_ptr<presence_service_impl>> m_presenceServices;
    int m_heartBeatDelayInMins;

#ifdef  __cplusplus_winrt
    Windows::System::Threading::ThreadPoolTimer^ m_timer;
#else
    bool m_timerComplete;
#endif

    std::atomic<bool> m_isCallInProgress;

};
#endif

}}}