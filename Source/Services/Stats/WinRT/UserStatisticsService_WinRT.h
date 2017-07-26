// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

#include "xsapi/user_statistics.h"
#include "xsapi/xbox_live_context_settings.h"
#include "RequestedStatistics_WinRT.h"
#include "UserStatisticsResult_WinRT.h"
#include "StatisticChangeSubscription_WinRT.h"
#include "StatisticChangeEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

ref class UserStatisticsService;

class UserStatisticsServiceEventBind : public std::enable_shared_from_this<UserStatisticsServiceEventBind>
{
public:
    UserStatisticsServiceEventBind(
        _In_ Platform::WeakReference setting,
        _In_ xbox::services::user_statistics::user_statistics_service& cppObj
        );

    void AddUserStatisticsServiceEvent();

    void RemoveUserStatisticChangeEvent(
        _In_ xbox::services::user_statistics::user_statistics_service& cppObj
        );

    void StatisticChangeRouter(_In_ const xbox::services::user_statistics::statistic_change_event_args& statisticChangeEventArgs);

private:
    function_context m_functionContext;
    Platform::WeakReference m_setting;
    xbox::services::user_statistics::user_statistics_service m_cppObj;
};

/// <summary>
/// Represents an endpoint that you can use to access the user statistic service.
/// </summary>
public ref class UserStatisticsService sealed
{
public:
    /// <summary>
    /// Registers for statistic change notifications.  Event handlers receive a StatisticChangeEventArgs^ object.
    /// </summary>
    event Windows::Foundation::EventHandler<StatisticChangeEventArgs^>^ StatisticChanged;

    /// <summary>
    /// Get a specified statistic for a specified user.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player to get statistics for.</param>
    /// <param name="serviceConfigurationId">The service configuration ID for the service holding the statistic.</param>
    /// <param name="statisticName">The name of the statistic to return.</param>
    /// <returns>An interface for tracking the progress of the asynchronous call. The result is the requested statistic.</returns>
    /// <remarks>Calls V1 GET /users/xuid({xuid})/scids/{scid}/stats/{statname1}</remarks>
    [Windows::Foundation::Metadata::DefaultOverloadAttribute]
    Windows::Foundation::IAsyncOperation<UserStatisticsResult^>^ 
    GetSingleUserStatisticsAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ statisticName
        );

    /// <summary>
    /// Get specified statistics for a single user.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player to get statistics for.</param>
    /// <param name="serviceConfigurationId">The service configuration ID of the service holding the statistics.</param>
    /// <param name="statisticNames">A collection of statistic names to lookup.</param>
    /// <returns>An interface for tracking the progress of the asynchronous call. The result is the requested statistics.
    /// Only statistics with values are returned. For example, if you ask for 3 statistic names and only 2 have values,
    /// only 2 statistics are returned by the service.</returns>
    /// <remarks>Calls V1 GET /users/xuid({xuid})/scids/{scid}/stats/{statname1},...,{statnameN}</remarks>
    Windows::Foundation::IAsyncOperation<UserStatisticsResult^>^ 
    GetSingleUserStatisticsAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ statisticNames
        );

    /// <summary>
    /// Get statistics for multiple users.
    /// </summary>
    /// <param name="xboxUserIds">A list of the user Xbox user IDs to get stats for.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticNames">A collection of statistic names to lookup.</param>
    /// <returns>An interface for tracking the progress of the asynchronous call. The result is a collection
    /// of user statistics results.
    /// Only statistics with values are returned. For example, if you ask for 3 statistic names and only 2 have values,
    /// only 2 statistics are returned by the service.</returns>
    /// <remarks>Calls V1 POST /batch</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<UserStatisticsResult^>^>^ 
    GetMultipleUserStatisticsAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ statisticNames
        );

    /// <summary>
    /// Get statistics for users across different Service configurations.
    /// </summary>
    /// <param name="xboxUserIds">A list of the user Xbox user ID to get stats for.</param>
    /// <param name="requestedServiceConfigurationStatisticsCollection">A list of the service config IDs and its associated array of statistics.</param>
    /// <returns>An interface for tracking the progress of the asynchronous call. The result is a collection of user statistics results.</returns>
    /// <remarks>Calls V1 POST /batch</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<UserStatisticsResult^>^>^ 
    GetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
        _In_ Windows::Foundation::Collections::IVectorView<RequestedStatistics^>^ requestedServiceConfigurationStatisticsCollection
        );

    /// <summary>
    /// Subscribes to statistic update notifications via the StatisticChanged event.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player requesting the subscription.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the subscription.</param>
    /// <param name="statisticName">The name of the statistic to subscribe to.</param>
    /// <returns>A subscription object that contains the state of the subscription. 
    /// You can register an event handler for statistic changes by using the StatisticChanged event.</returns>
    StatisticChangeSubscription^ SubscribeToStatisticChange(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ statisticName
        );

    /// <summary>
    /// Unsubscribes a previously created statistic change subscription.
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe.</param>
    void UnsubscribeFromStatisticChange(
        _In_ StatisticChangeSubscription^ subscription
        );

internal:
    UserStatisticsService(
        _In_ xbox::services::user_statistics::user_statistics_service cppObj
        );

    void RaiseStatisticChange(_In_ StatisticChangeEventArgs^ args);
private:
    ~UserStatisticsService();
    xbox::services::user_statistics::user_statistics_service m_cppObj;
    std::shared_ptr<UserStatisticsServiceEventBind> m_userStatisticsServiceEventBind;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END
