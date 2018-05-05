// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/stats_manager.h"
#include "StatisticEvent_WinRT.h"
#include "StatisticValue_WinRT.h"
#include "LeaderboardQuery_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

/// <summary> 
/// Statistic Manager handles and writes a local user's stats to the simplified stats service.
/// </summary>
public ref class StatisticManager sealed
{
public:
    /// <summary> 
    /// Instantiates and returns an instance of simplified stats.
    /// </summary>
    static property StatisticManager^ SingletonInstance
    {
        StatisticManager^ get();
    }
    /// <summary> 
    /// Adds a local user to the simplified stats manager.
    /// Returns a LocalUserAdded event from DoWork().
    /// </summary>
    /// <param name="user">The user to add to the statistic manager.</param>
    void AddLocalUser(
        _In_ XboxLiveUser_t user
        );

    /// <summary> 
    /// Removes a local user from the simplified stats manager.
    /// Returns a LocalUserRemoved event from DoWork().
    /// </summary>
    /// <param name="user">The user to be removed from the statistic manager.</param>
    void RemoveLocalUser(
        _In_ XboxLiveUser_t user
        );

	/// <summary> 
	/// Requests that StatisticManager sends the stats to the service immediately,
	/// with a maximum rate of once every 30 seconds. If you do not call this method, stats
	/// are automatically sent to the service every 5 minutes.
	/// </summary>
	/// <param name="user">The user to flush the stat for.</param>
	/// <remarks>
	/// Stats are automatically sent to the service every 5 minutes. You can call this method 
	/// if you want to update the service with the latest stats sooner such as when a match or round ends.
	/// 
	/// This causes the current stat values to be uploaded to the service with
	/// a maximum rate of once every 30 seconds.
	///
	/// Note that you may still be throttled when calling the stats service too often, 
	/// such as if you are relaunching the title over and over and send stats at every launch.
	/// </remarks>
    void RequestFlushToService(
        _In_ XboxLiveUser_t user
        );

	/// <summary> 
	/// Requests that StatisticManager sends the stats to the service immediately,
	/// with a maximum rate of once every 30 seconds. If you do not call this method, stats
	/// are automatically sent to the service every 5 minutes.
	/// </summary>
    /// <param name="user">The user to flush the stat for.</param>
    /// <param name="isHighPriority">Indicates if the flush is a high priority, typically when the game is getting suspended.</param>
    /// <remarks>
    /// Stats are automatically sent to the service every 5 minutes. You can call this method 
    /// if you want to update the service with the latest stats sooner such as when a match or round ends.
    /// 
	/// This causes the current stat values to be uploaded to the service with
	/// a maximum rate of once every 30 seconds.  Set isHighPriority to true when your title 
	/// is getting suspended as this will try to update the stats even if it hasn't been 30 seconds 
	/// since the last flush. However requests to flush with isHighPriority=true are still limited 
	/// to a maximum rate of once every 30 seconds so it can't be used to flood the service.
	///
	/// Note that you may still be throttled when calling the stats service too often, 
	/// such as if you are relaunching the title over and over and send stats at every launch.
    /// </remarks>
    void RequestFlushToService(
        _In_ XboxLiveUser_t user,
        _In_ bool isHighPriority
        );

    /// <summary> 
    /// Returns any events that have been processed.
    /// </summary>
    /// <return>A list of events that have happened since previous DoWork().</return>
    Windows::Foundation::Collections::IVectorView<StatisticEvent^>^ DoWork();

    /// <summary> 
    /// Replaces the numerical stat with the specified double precision floating point value. Can be positive or negative.
    /// </summary>
    /// <param name="user">The local user whose stats to access.</param>
    /// <param name="name">The name of the statistic to modify.</param>
    /// <param name="value">Value to replace the stat with.</param>
    /// <remarks>
    /// Stats will be sent to the service automatically every 5 minutes or you can call 
    /// RequestFlushToService() if you want to update the service with the latest stats sooner
    /// such as when a match or round ends.
    /// </remarks>
    void SetStatisticNumberData(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name,
        _In_ double value
        );

    /// <summary> 
    /// Replaces the numerical stat with the specified integer value. Can be positive or negative.
    /// </summary>
    /// <param name="user">The local user whose stats to access.</param>
    /// <param name="name">The name of the statistic to modify.</param>
    /// <param name="value">Value to replace the stat with.</param>
	/// <remarks>
	/// Stats will be sent to the service automatically every 5 minutes or you can call 
	/// RequestFlushToService() if you want to update the service with the latest stats sooner
	/// such as when a match or round ends.
	/// </remarks>
    void SetStatisticIntegerData(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name,
        _In_ int64_t value
        );


    /// <summary> 
    /// Replaces the string stat with the specified string value.
    /// </summary>
    /// <param name="user">The local user whose stats to access.</param>
    /// <param name="name">The name of the statistic to modify.</param>
    /// <param name="value">Value to replace the stat with.</param>
    /// <remarks>
	/// Stats will be sent to the service automatically every 5 minutes or you can call 
	/// RequestFlushToService() if you want to update the service with the latest stats sooner
	/// such as when a match or round ends.
	/// </remarks>
    void SetStatisticStringData(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name,
        _In_ Platform::String^ value
        );

    /// <summary> 
    /// Gets all stat names in the stat document.
    /// </summary>
    /// <param name="user">The local user whose stats to access.</param>
    /// <return>The list of statistic names.</return>
    /// <remarks>
    /// These are the names for the stats that the user already has values for.
    /// This call won't return all the stat names configured for the title.
    ///
    /// For example
    ///
    /// A title has stat1, stat2, and stat3 configured in the developer portal.
    /// The user has previously set a value for stat2.
    /// GetStatisticNames() will only return "stat2".
    /// 
    /// Note that if the service can't be reached then this will return an empty list.
    /// </remarks>
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ GetStatisticNames(
        _In_ XboxLiveUser_t user
        );

    /// <summary> 
    /// Gets a stat value.
    /// </summary>
    /// <param name="user">The local user whose stats to access.</param>
    /// <param name="name">The name of the statistic to retrieve.</param>
    /// <return>The updated stat</return>
    /// <remarks>
    /// The title is responsible for tracking user stats. For example with connected storage.
    /// This method will return the value for a stat stored in the service; however, this value 
    /// returned will not be valid if the service couldn't be reached.
    /// </remarks>
    StatisticValue^ GetStatistic(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name
        );

    /// <summary> 
    /// Deletes a stat value. This clears the stat from the service and social leaderboards.
    /// </summary>
    /// <param name="user">The local user whose stats to access.</param>
    /// <param name="name">The name of the statistic to delete.</param>
    void DeleteStatistic(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name
        );

    /// <summary> 
    /// Starts a request for a global leaderboard. You can retrieve the resulting data by checking
    /// the events returned from DoWork() for an event of type GetLeaderboardComplete.
    /// </summary>
    /// <param name="user">The local user whose stats to access.</param>
    /// <param name="statName">The name of the statistic to get the leaderboard of.</param>
    /// <param name="query">The query parameters of the leaderboard request.</param>
    void GetLeaderboard(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ statName,
        _In_ Leaderboard::LeaderboardQuery^ query
        );

    /// <summary> 
    /// Starts a request for a social leaderboard. You can retrieve the resulting data by checking
	/// the events returned from DoWork() for an event of type GetLeaderboardComplete.
    /// </summary>
    /// <param name="user">The local user whose stats to access.</param>
    /// <param name="statName">The name of the statistic to get the leaderboard of.</param>
    /// <param name="socialGroup">The name of the social group.</param>
    /// <param name="query">The query parameters of the leaderboard request.</param>
    void GetSocialLeaderboard(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ statName,
        _In_ Platform::String^ socialGroup,
        _In_ Leaderboard::LeaderboardQuery^ query
        );

internal:
    StatisticManager();

private:
   std::shared_ptr<xbox::services::stats::manager::stats_manager> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END