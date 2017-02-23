// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "PresenceData_WinRT.h"
#include "PresenceRecord_WinRT.h"
#include "PresenceDetailLevel_WinRT.h"
#include "DevicePresenceChangeSubscription_WinRT.h"
#include "DevicePresenceChangeEventArgs_WinRT.h"
#include "TitlePresenceChangeSubscription_WinRT.h"
#include "TitlePresenceChangeEventArgs_WinRT.h"
#include "xsapi/xbox_live_context_settings.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

ref class PresenceService;

class DevicePresenceChangeEventBind : public std::enable_shared_from_this<DevicePresenceChangeEventBind>
{
public:
    DevicePresenceChangeEventBind(
        _In_ Platform::WeakReference setting,
        _In_ xbox::services::presence::presence_service& cppObj
        );

    void AddDevicePresenceChangeEvent();

    void RemoveDevicePresenceChangeEvent(
        _In_ xbox::services::presence::presence_service& cppObj
        );

    void DevicePresenceChangeRouter(_In_ const xbox::services::presence::device_presence_change_event_args& presenceChangeEventArgs);

private:
    function_context m_functionContext;
    Platform::WeakReference m_setting;
    xbox::services::presence::presence_service m_cppObj;
};

class TitlePresenceChangeEventBind : public std::enable_shared_from_this<TitlePresenceChangeEventBind>
{
public:
    TitlePresenceChangeEventBind(
        _In_ Platform::WeakReference setting,
        _In_ xbox::services::presence::presence_service& cppObj
        );

    void AddTitlePresenceChangeEvent();

    void RemoveTitlePresenceChangeEvent(
        _In_ xbox::services::presence::presence_service& cppObj
        );

    void TitlePresenceChangeRouter(_In_ const xbox::services::presence::title_presence_change_event_args& presenceChangeEventArgs);

private:
    function_context m_functionContext;
    Platform::WeakReference m_setting;
    xbox::services::presence::presence_service m_cppObj;
};

/// <summary>
/// Represents a service for Rich Presence features.
/// </summary>
public ref class PresenceService sealed
{
public:
    /// <summary>
    /// Registers for device presence change notifications.  Event handlers will receive DevicePresenceChangeEventArgs^.
    /// </summary>
    event Windows::Foundation::EventHandler<DevicePresenceChangeEventArgs^>^ DevicePresenceChanged;

    /// <summary>
    /// Registers for title presence change notifications.  Event handlers will receive TitlePresenceChangeEventArgs^.
    /// </summary>
    event Windows::Foundation::EventHandler<TitlePresenceChangeEventArgs^>^ TitlePresenceChanged;

    /// <summary>
    /// Sets presence info for the current user context.
    /// </summary>
    /// <param name="isUserActiveInTitle">Indicates if the current user context is currently active or inactive in the title.
    /// The application can choose to set this based on an amount of inactivity.</param>
    /// <remarks>Calls V1 POST /users/xuid({xuid})/devices/current/titles/current</remarks>
    Windows::Foundation::IAsyncAction^ SetPresenceAsync(
        _In_ bool isUserActiveInTitle
        );

    /// <summary>
    /// Sets presence info for the current user context.
    /// </summary>
    /// <param name="isUserActiveInTitle">Indicates if the current user context is currently active or inactive in the title.
    /// The application can choose to set this based on an amount of inactivity.</param>
    /// <param name="presenceData">Current user's presence data. (Optional)</param>
    /// <remarks>Calls V1 POST /users/xuid({xuid})/devices/current/titles/current</remarks>
    Windows::Foundation::IAsyncAction^ SetPresenceAsync(
        _In_ bool isUserActiveInTitle,
        _In_opt_ PresenceData^ presenceData
        );

    /// <summary>
    /// Gets presence info for a specific Xbox User Id.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user to get presence for</param>
    /// <remarks>Calls V3 GET /users/xuid({xuid})</remarks>
    Windows::Foundation::IAsyncOperation<PresenceRecord^>^ GetPresenceAsync(
        _In_ Platform::String^ xboxUserId
        );

    /// <summary>
    /// Gets presence info for multiple users. This returns all possible titles on all device, defaults to all detail, 
    /// and does not filter out users who are offline or broadcasting.
    /// </summary>
    /// <param name="xboxUserIds">The name of the users to get presence for.</param>
    /// <remarks>Calls V3 POST /users/batch</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<PresenceRecord^>^>^ GetPresenceForMultipleUsersAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds
        );

    /// <summary>
    /// Gets presence info for multiple users with filters.
    /// </summary>
    /// <param name="xboxUserIds">The name of the users to get presence for.</param>
    /// <param name="deviceTypes">List of device types. If the input is null; defaults to all possible deviceTypes. (Optional) </param>
    /// <param name="titleIds">List of titleIds for filtering the result. If the input is null, defaults to all possible titles. (Optional) </param>
    /// <param name="detailLevel">Detail level of the result. Defaults to all details</param>
    /// <param name="onlineOnly">If true, API will filter out records for users that are offline </param>
    /// <param name="broadcastingOnly">If true, API will filter out records for users that are not broadcasting. </param>
    /// <remarks>Calls V3 POST /users/batch</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<PresenceRecord^>^>^ GetPresenceForMultipleUsersAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
        _In_opt_ Windows::Foundation::Collections::IVectorView<PresenceDeviceType>^ deviceTypes,
        _In_opt_ Windows::Foundation::Collections::IVectorView<uint32>^ titleIds,
        _In_ PresenceDetailLevel detailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly
        );

    /// <summary>
    /// Gets presence info for a specific group of users.
    /// </summary>
    /// <param name="socialGroup">The name of the group of users to get presence for.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <remarks>Calls V3 GET /users/xuid({xuid})/groups/{socialGroup}</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<PresenceRecord^>^>^ GetPresenceForSocialGroupAsync(
        _In_ Platform::String^ socialGroup
        );

    /// <summary>
    /// Gets presence info for a specific group of users.
    /// </summary>
    /// <param name="socialGroup">The name of the group of users to get presence for.</param>
    /// <param name="socialGroupOwnerXboxuserId">The user whose group should be targeted. If the input is null, current user will be used. (Optional) </param>
    /// <param name="deviceTypes">List of device types. If the input is null; defaults to all possible deviceTypes. (Optional) </param>
    /// <param name="titleIds">List of titleIds for filtering the result. If the input is null, defaults to all possible titles. (Optional) </param>
    /// <param name="detailLevel">Detail level of the result. Defaults to all details</param>
    /// <param name="onlineOnly">If true, API will filter out records for users that are offline </param>
    /// <param name="broadcastingOnly">If true, API will filter out records for users that are not broadcasting. </param>
    /// <remarks>Calls V3 POST /users/batch</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<PresenceRecord^>^>^ GetPresenceForSocialGroupAsync(
        _In_ Platform::String^ socialGroup,
        _In_opt_ Platform::String^ socialGroupOwnerXboxuserId,
        _In_opt_ Windows::Foundation::Collections::IVectorView<PresenceDeviceType>^ deviceTypes,
        _In_opt_ Windows::Foundation::Collections::IVectorView<uint32>^ titleIds,
        _In_ PresenceDetailLevel detailLevel,
        _In_ bool onlineOnly,
        _In_ bool broadcastingOnly
        );

    /// <summary>
    /// Subscribes to device presence change notifications via the DevicePresenceChanged event
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the person of the subscription</param>
    /// <returns>RealTimeActivityDevicePresenceChangeSubscription containing the initial value of the PresenceDeviceRecord
    /// Register for device presence changes via the DevicePresenceChanged event</returns>
    DevicePresenceChangeSubscription^ SubscribeToDevicePresenceChange(
        _In_ Platform::String^ xboxUserId
        );

    /// <summary>
    /// Unsubscribes a previously created device presence change subscription
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe</param>
    void UnsubscribeFromDevicePresenceChange(
        _In_ DevicePresenceChangeSubscription^ subscription
        );

    /// <summary>
    /// Subscribes to title presence change notifications via the TitlePresenceChanged event
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the person of the subscription</param>
    /// <param name="titleId">The title ID of the subscription</param>
    /// <returns>RealTimeActivityDevicePresenceChangeSubscription containing the initial value of the PresenceDeviceRecord
    /// Register for device presence changes via the DevicePresenceChanged event</returns>
    TitlePresenceChangeSubscription^ SubscribeToTitlePresenceChange(
        _In_ Platform::String^ xboxUserId,
        _In_ uint32 titleId
        );

    /// <summary>
    /// Unsubscribes a previously created title presence change subscription
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe</param>
    void UnsubscribeFromTitlePresenceChange(
        _In_ TitlePresenceChangeSubscription^ subscription
        );

internal:
    PresenceService(_In_ xbox::services::presence::presence_service cppObj);
    void RaiseDevicePresenceChange(_In_ DevicePresenceChangeEventArgs^ args);
    void RaiseTitlePresenceChange(_In_ TitlePresenceChangeEventArgs^ args);

    const xbox::services::presence::presence_service& GetCppObj() const;

private:
    ~PresenceService();
    xbox::services::presence::presence_service m_cppObj;
    std::shared_ptr<DevicePresenceChangeEventBind> m_devicePresenceChangeEventBind;
    std::shared_ptr<TitlePresenceChangeEventBind> m_titlePresenceChangeEventBind;
};
NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END