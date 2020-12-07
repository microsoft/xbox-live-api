// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-c/presence_c.h"
#include "xsapi-cpp/real_time_activity.h"

namespace xbox { namespace services {
    class xbox_live_context;
    /// <summary>
    /// Contains classes and enumerations that let you retrieve
    /// information about player's online presence on Xbox Live.
    /// </summary>
    namespace presence {

/// <summary>Defines values used to indicate the device type associate with a PresenceTitleRecord.</summary>
enum class presence_device_type
{
    /// <summary>Unknown device</summary>
    unknown,

    /// <summary>Windows Phone device</summary>
    windows_phone,

    /// <summary>Windows Phone 7 device</summary>
    windows_phone_7,

    /// <summary>Web device, like Xbox.com</summary>
    web,

    /// <summary>Xbox360 device</summary>
    xbox_360,

    /// <summary>Games for Windows LIVE device</summary>
    pc,

    /// <summary>Xbox LIVE for Windows device</summary>
    windows_8,

    /// <summary>Xbox One device</summary>
    xbox_one,

    /// <summary>Windows One Core devices</summary>
    windows_one_core,

    /// <summary>Windows One Core Mobile devices</summary>
    windows_one_core_mobile
};

/// <summary>
/// Defines values used to indicate the state of the user with regard to the presence service.
/// </summary>
enum class user_presence_state
{
    /// <summary>The state is unknown.</summary>
    unknown,

    /// <summary>User is signed in to Xbox LIVE and active in a title.</summary>
    online,

    /// <summary>User is signed-in to Xbox LIVE, but inactive in all titles.</summary>
    away,

    /// <summary>User is not signed in to Xbox LIVE.</summary>
    offline
};

/// <summary>
/// Defines values used to indicate the states of the screen view of presence information.
/// </summary>
enum class presence_title_view_state
{
    /// <summary>Unknown view state.</summary>
    unknown,

    /// <summary>The title's view is using the full screen.</summary>
    full_screen,

    /// <summary>The title's view is using part of the screen with another application snapped.</summary>
    filled,

    /// <summary>The title's view is snapped with another application using a part of the screen.</summary>
    snapped,

    /// <summary>The title's running in the background and is not visible.</summary>
    background
};

/// <summary> Defines values used to set the level of presence detail return from the service. Choosing proper detail level could help the performance of the API.</summary>
enum class presence_detail_level
{
    /// <summary>Default detail level.</summary>
    default_level,

    /// <summary>User detail level. User presence info only, no device, title or rich presence info.</summary>
    user,

    /// <summary>Device detail level. User and device presence info only, no title or rich presence info.</summary>
    device,

    /// <summary>Title detail level. User, device and title presence info only, no rich presence info.</summary>
    title,

    /// <summary>All detail possible. User, device, title and rich presence info will be provided.</summary>
    all
};

/// <summary>Defines values used to indicate the media id types for media presence data.</summary>
enum class presence_media_id_type
{
    /// <summary>Unknown media Id.</summary>
    unknown,

    /// <summary>Bing media Id.</summary>
    bing,

    /// <summary>MediaProvider media Id.</summary>
    media_provider
};

/// <summary>Defines values used to indicate the title presence state for a user.</summary>
enum class title_presence_state
{
    /// <summary>
    /// Indicates this is a Unknown state.
    /// </summary>
    unknown,

    /// <summary>
    /// Indicates the user started playing the title.
    /// </summary>
    started,

    /// <summary>
    /// Indicates the user ended playing the title.
    /// </summary>
    ended
};

/// <summary>
/// Represents data supporting Rich Presence features.
/// </summary>
class presence_data
{
public:
    /// <summary>
    /// Initializes a new instance of the PresenceData class, which identifies where the presence strings are defined and which of the defined strings is used.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) that identifies where the presence strings are defined by Id.</param>
    /// <param name="presenceId">Id of the presence string that should be used.</param>
    inline presence_data(
        _In_ string_t serviceConfigurationId,
        _In_ string_t presenceId
        );

    /// <summary>
    /// Initializes a new instance of the PresenceData class, which identifies where the presence strings are defines, which string is used, and IDs of strings to be used in place of tokens within the presence string.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) that identifies where the presence strings are defined by Id.</param>
    /// <param name="presenceId">Id of the presence string that should be used.</param>
    /// <param name="presenceTokenIds">Ids of the strings that should be used to replace the tokens in the presence string.</param>
    inline presence_data(
        _In_ string_t serviceConfigurationId,
        _In_ string_t presenceId,
        _In_ std::vector<string_t> presenceTokenIds
        );

    /// <summary>
    /// ID of the service configuration containing the presence strings.
    /// </summary>
    inline const string_t& service_configuration_id() const;

    /// <summary>
    /// The ID of a presence string that is defined in the service configuration.
    /// For example, PresenceId = "1" could equal "Playing {0} on {1}" in the service configuration.
    /// The service configuration might map token 0 to Maps and token 1 to MapId.
    /// </summary>
    inline const string_t& presence_id() const;

    /// <summary>
    /// The IDs of the strings to replace the format string tokens found in the presence string.  These strings are also defined in the service configuration.
    /// The ID values in the collection map to the strings associated with the token arguments found in the PresenceId.
    /// For example let's say this vector view contained the values "4" and "1" and PresenceId = "1" equals "Playing {0} on {1}" in the service configuration.
    /// The service configuration might map Token 0 = Maps, where MapId = "4" equals "Hometown".
    /// The service configuration might map Token 1 = Difficulty, where DifficultyId = "1" equals "Casual".
    /// </summary>
    inline const std::vector<string_t>& presence_token_ids() const;

private:
    string_t m_serviceConfigurationId;
    string_t m_presenceId;
    std::vector<string_t> m_presenceTokenIds;
};

class presence_broadcast_record
{
public:
    /// <summary>
    /// Id for this broadcast as defined by the broadcasting service.
    /// </summary>
    inline string_t broadcast_id() const;

    /// <summary>
    /// The GUID uniquely identifying the broadcasting session. 
    /// </summary>
    inline string_t session() const;

    /// <summary>
    /// Name of the streaming provider.
    /// </summary>
    inline string_t provider() const;

    /// <summary>
    /// Approximate number of current viewers. 
    /// </summary>
    inline uint32_t viewer_count() const;

    /// <summary>
    /// UTC timestamp when the broadcast was started.
    /// </summary>
    inline utility::datetime start_time() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline presence_broadcast_record() = default;
    inline presence_broadcast_record(_In_ XblPresenceRecordHandle handle, _In_ const XblPresenceBroadcastRecord* broadcastRecord);
    inline presence_broadcast_record(const presence_broadcast_record& other);
    inline presence_broadcast_record& operator=(presence_broadcast_record other);
    inline ~presence_broadcast_record();

private:
    XblPresenceRecordHandle m_handle{};
    const XblPresenceBroadcastRecord* m_broadcastRecord{ nullptr };
};

class presence_title_record
{
public:

    /// <summary>
    /// The title ID.
    /// </summary>
    inline uint32_t title_id() const;

    /// <summary>
    /// The title name.
    /// </summary>
    inline string_t title_name() const;

    /// <summary>
    /// The UTC timestamp when the record was last updated.
    /// </summary>
    inline utility::datetime last_modified_date() const;

    /// <summary>
    /// The active state for the title.
    /// </summary>
    inline bool is_title_active() const;

    /// <summary>
    /// The formatted and localized presence string.
    /// </summary>
    inline string_t presence() const;

    /// <summary>
    /// The title view state.
    /// </summary>
    inline presence_title_view_state presence_title_view() const;

    /// <summary>
    /// The broadcast information of what the user is broadcasting. 
    /// </summary>
    inline presence_broadcast_record broadcast_record() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline presence_title_record(_In_ XblPresenceRecordHandle handle, _In_ const XblPresenceTitleRecord* titleRecord);
    inline presence_title_record(_In_ const presence_title_record& other);
    inline presence_title_record& operator=(_In_ presence_title_record other);
    inline ~presence_title_record();

private:
    XblPresenceRecordHandle m_handle;
    const XblPresenceTitleRecord* m_titleRecord;
};

class presence_device_record
{
public:
    /// <summary>
    /// The device type associated with this record.
    /// </summary>
    inline presence_device_type device_type() const;

    /// <summary>
    /// The record containing title presence data.
    /// </summary>
    inline std::vector<presence_title_record> presence_title_records() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline presence_device_record(_In_ XblPresenceRecordHandle handle, _In_ const XblPresenceDeviceRecord* deviceRecord);
    inline presence_device_record(_In_ const presence_device_record& other);
    inline presence_device_record& operator=(_In_ presence_device_record other);
    inline ~presence_device_record();

private:
    XblPresenceRecordHandle m_handle;
    const XblPresenceDeviceRecord* m_deviceRecord;
};

class presence_record
{
public:
    /// <summary>
    /// The Xbox user ID.
    /// </summary>
    inline string_t xbox_user_id() const;

    /// <summary>
    /// The user's presence state.
    /// </summary>
    inline user_presence_state user_state() const;

    /// <summary>
    /// Collection of presence_device_record objects returned by a request.
    /// </summary>
    inline std::vector<presence_device_record> presence_device_records() const;

    /// <summary>
    /// Returns whether the user is playing this title id
    /// </summary>
    inline bool is_user_playing_title(_In_ uint32_t titleId) const;

    presence_record() = default;
    inline presence_record(_In_ XblPresenceRecordHandle handle);
    inline presence_record(_In_ const presence_record& other);
    inline presence_record& operator=(presence_record other);
    inline ~presence_record();

private:
    XblPresenceRecordHandle m_handle{ nullptr };
};

/// <summary> 
/// Used to identify the Xbox user, device, and log-in status presence values.
/// </summary>
class device_presence_change_event_args
{
public:
    /// <summary>
    /// The Xbox user ID.
    /// </summary>
    inline const string_t& xbox_user_id() const;

    /// <summary>
    /// The type of device.
    /// </summary>
    inline presence_device_type device_type() const;

    /// <summary>
    /// Value used to indicate if the Xbox user is logged onto the device.
    /// </summary>
    inline bool is_user_logged_on_device() const;

    inline device_presence_change_event_args(_In_ uint64_t xuid, _In_ XblPresenceDeviceType deviceType, _In_ bool isUserLoggedOn);

private:
    string_t m_xuid;
    XblPresenceDeviceType m_deviceType;
    bool m_isUserLoggedOn;
};

/// <summary>
/// Subscribes to changes to an Xbox user's presence on a device.
/// </summary>
class device_presence_change_subscription : public xbox::services::real_time_activity::real_time_activity_subscription
{
public:
    /// <summary>
    /// The Xbox user ID.
    /// </summary>
    inline const string_t& xbox_user_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline device_presence_change_subscription(_In_ XblRealTimeActivitySubscriptionHandle handle, _In_ const string_t& xuid);

private:
    string_t m_xuid;
    friend class presence_service;
};

/// <summary>
/// Used to identify a Xbox user, title, and presence states that can be subscribed to.
/// </summary>
class title_presence_change_event_args
{
public:
    /// <summary> 
    /// The Xbox user ID.
    /// </summary> 
    inline const string_t& xbox_user_id() const;

    /// <summary>
    /// The title ID.
    /// </summary>
    inline uint32_t title_id() const;

    /// <summary>
    /// Object that defines possible presence states.
    /// </summary>
    inline title_presence_state title_state() const;

    inline title_presence_change_event_args(_In_ uint64_t xuid, _In_ uint32_t titleId, _In_ XblPresenceTitleState titleState);

private:
    string_t m_xuid;
    uint32_t m_titleId;
    XblPresenceTitleState m_titleState;
};

/// <summary>
/// Subscribes to presence change events for the specified title and user.
/// </summary>
class title_presence_change_subscription : public real_time_activity::real_time_activity_subscription
{
public:
    /// <summary>
    /// The Xbox user ID.
    /// </summary>
    inline const string_t& xbox_user_id() const;

    /// <summary>
    /// The title ID.
    /// </summary>
    inline uint32_t title_id() const;

    inline title_presence_change_subscription(_In_ XblRealTimeActivitySubscriptionHandle handle, _In_ const string_t& xuid, _In_ uint32_t titleId);

private:
    string_t m_xuid;
    uint32_t m_titleId;
    friend class presence_service;
};

#if !XSAPI_NO_PPL
class presence_service
{
public:
    /// <summary>
    /// Sets presence info for the current user context.
    /// </summary>
    /// <param name="isUserActiveInTitle">Indicates if the current user context is currently active or inactive in the title.
    /// The application can choose to set this based on an amount of inactivity.</param>
    /// <param name="presenceData">Current user's presence data.</param>
    /// <remarks>Calls V1 POST /users/xuid({xuid})/devices/current/titles/current</remarks>
    inline pplx::task<xbox_live_result<void>> set_presence(
        _In_ bool isUserActiveInTitle
        );

    /// <summary>
    /// Sets presence info for the current user context.
    /// </summary>
    /// <param name="isUserActiveInTitle">Indicates if the current user context is currently active or inactive in the title.
    /// The application can choose to set this based on an amount of inactivity.</param>
    /// <param name="presenceData">Current user's presence data.</param>
    /// <remarks>Calls V1 POST /users/xuid({xuid})/devices/current/titles/current</remarks>
    inline pplx::task<xbox_live_result<void>> set_presence(
        _In_ bool isUserActiveInTitle,
        _In_ presence_data presenceData
        );

    /// <summary>
    /// Gets presence info for a specific Xbox User Id.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user to get presence for</param>
    /// <remarks>Calls V3 GET /users/xuid({xuid})</remarks>
    inline pplx::task<xbox_live_result<presence_record>> get_presence(
        _In_ const string_t& xboxUserId
        );

    /// <summary>
    /// Gets presence info for multiple users. This returns all possible titles on all device, 
    /// defaults to presence_detail_level::default which is equivalent to
    /// presence_detail_level::title (get basic title level information), 
    /// and does not filter out users who are offline or broadcasting.
    /// </summary>
    /// <param name="xboxUserIds">The name of the users to get presence for.</param>
    /// <remarks>Calls V3 POST /users/batch</remarks>
    inline pplx::task<xbox_live_result<std::vector<presence_record>>> get_presence_for_multiple_users(
        _In_ const std::vector<string_t>& xboxUserIds
        );

    /// <summary>
    /// Gets presence info for multiple users with filters.
    /// </summary>
    /// <param name="xboxUserIds">The name of the users to get presence for.</param>
    /// <param name="deviceTypes">List of device types. If the input is an empty vector, defaults to all possible deviceTypes.</param>
    /// <param name="titleIds">List of titleIds for filtering the result. If the input is an empty vector, defaults to all possible titles.</param>
    /// <param name="presenceDetailLevel">Detail level of the result. Defaults to all details</param>
    /// <param name="onlineOnly">If true, API will filter out records for users that are offline </param>
    /// <param name="broadcastingOnly">If true, API will filter out records for users that are not broadcasting. </param>
    /// <remarks>Calls V3 POST /users/batch</remarks>
    inline pplx::task<xbox_live_result<std::vector<presence_record>>> get_presence_for_multiple_users(
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const std::vector<presence_device_type>& deviceTypes,
        _In_ const std::vector<uint32_t>& titleIds,
        _In_ presence_detail_level presenceDetailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly
        );

    /// <summary>
    /// Gets presence info for a specific group of users.
    /// </summary>
    /// <param name="socialGroup">The name of the group of users to get presence for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <remarks>Calls V3 GET /users/xuid({xuid})/groups/{socialGroup}</remarks>
    inline pplx::task<xbox_live_result<std::vector<presence_record>>> get_presence_for_social_group(
        _In_ const string_t& socialGroup
        );

    /// <summary>
    /// Gets presence info for a specific group of users.
    /// </summary>
    /// <param name="socialGroup">The name of the group of users to get presence for.</param>
    /// <param name="socialGroupOwnerXboxuserId">The user whose group should be targeted. If the input is null, current user will be used.</param>
    /// <param name="deviceTypes">List of device types. If the input is null; defaults to all possible deviceTypes. (Optional) </param>
    /// <param name="titleIds">List of titleIds for filtering the result. If the input is null, defaults to all possible titles. (Optional) </param>
    /// <param name="peoplehubDetailLevel">Detail level of the result. Defaults to all details. (Optional) </param>
    /// <param name="onlineOnly">If true, API will filter out records for users that are offline. </param>
    /// <param name="broadcastingOnly">If true, API will filter out records for users that are not broadcasting. </param>
    /// <remarks>Calls V3 POST /users/batch</remarks>
    inline pplx::task<xbox_live_result<std::vector<presence_record>>> get_presence_for_social_group(
        _In_ const string_t& socialGroup,
        _In_ const string_t& socialGroupOwnerXboxUserId,
        _In_ const std::vector<presence_device_type>& deviceTypes,
        _In_ const std::vector<uint32_t>& titleIds,
        _In_ presence_detail_level peoplehubDetailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly
        );

    /// <summary>
    /// Subscribes to device presence change notifications via the DevicePresenceChanged event.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the person of the subscription.</param>
    /// <returns>RealTimeActivityDevicePresenceChangeSubscription containing the initial value of the PresenceDeviceRecord.
    /// Register for device presence changes via the DevicePresenceChanged event.</returns>
    inline xbox_live_result<std::shared_ptr<device_presence_change_subscription>> subscribe_to_device_presence_change(
        _In_ const string_t& xboxUserId
        );

    /// <summary>
    /// Unsubscribes a previously created device presence change subscription.
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe.</param>
    inline xbox_live_result<void> unsubscribe_from_device_presence_change(
        _In_ std::shared_ptr<device_presence_change_subscription> subscription
        );

    /// <summary>
    /// Subscribes to title presence change notifications via the TitlePresenceChanged event.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the person of the subscription.</param>
    /// <param name="titleId">The title ID of the subscription.</param>
    /// <returns>RealTimeActivityDevicePresenceChangeSubscription containing the initial value of the PresenceDeviceRecord.
    /// Register for device presence changes via the DevicePresenceChanged event.</returns>
    inline xbox_live_result<std::shared_ptr<title_presence_change_subscription>> subscribe_to_title_presence_change(
        _In_ const string_t& xboxUserId,
        _In_ uint32_t titleId
        );

    /// <summary>
    /// Registers for title presence change notifications.  Event handlers will receive RealTimeActivityTitlePresenceChangeEventArgs^.
    /// </summary>
    inline xbox_live_result<void> unsubscribe_from_title_presence_change(
        _In_ std::shared_ptr<title_presence_change_subscription> subscription
        );

    /// <summary>
    /// Registers an event handler for device presence change notifications.
    /// Event handlers receive a device_presence_change_event_args&amp; object.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    inline function_context add_device_presence_changed_handler(_In_ std::function<void(const device_presence_change_event_args&)> handler);
    
    /// <summary>
    /// Unregisters an event handler for device presence change notifications.
    /// </summary>
    /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
    /// <param name="handler">The callback function that receives notifications.</param>
    inline void remove_device_presence_changed_handler(_In_ function_context context);

    /// <summary>
    /// Registers an event handler for title presence change notifications.
    /// Event handlers receive a title_presence_change_event_args&amp; object.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    inline function_context add_title_presence_changed_handler(_In_ std::function<void(const title_presence_change_event_args&)> handler);

    /// <summary>
    /// Unregisters an event handler for title presence change notifications.
    /// </summary>
    /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
    /// <param name="handler">The callback function that receives notifications.</param>
    inline void remove_title_presence_changed_handler(_In_ function_context context);

    inline presence_service(const presence_service& other);
    inline presence_service& operator=(presence_service other);
    inline ~presence_service();

private:
    inline presence_service(XblContextHandle xblContextHandle);

    XblContextHandle m_xblContextHandle;
    struct HandlerContext;

    friend xbox_live_context;
};
#endif // !XSAPI_NO_PPL

}}}

#if !XSAPI_NO_PPL
#include "impl/presence.hpp"
#endif
