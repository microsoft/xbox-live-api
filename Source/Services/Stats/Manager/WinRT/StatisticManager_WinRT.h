// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/stats_manager.h"
#include "StatisticEvent_WinRT.h"
#include "StatisticValue_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

public ref class StatisticManager sealed
{
public:
    /// <summary> 
    /// Instantiates and returns an instance of simplified stats
    /// </summary>
    static property StatisticManager^ SingletonInstance
    {
        StatisticManager^ get();
    }
    /// <summary> 
    /// Adds a local user to the simplified stats manager
    /// Returns a local_user_added event from do_work
    /// </summary>
    /// <param name="user">The user to add to the statistic manager</param>
    void AddLocalUser(
        _In_ XboxLiveUser_t user
        );

    /// <summary> 
    /// Removes a local user from the simplified stats manager
    /// Returns a local_user_removed event from do_work
    /// </summary>
    /// <param name="user">The user to be removed from the statistic manager</param>
    void RemoveLocalUser(
        _In_ XboxLiveUser_t user
        );

    /// <summary> 
    /// Force the current stat values to be uploaded to the service
    /// This will send immediately instead of automatically during a 30 second window
    /// </summary>
    /// <param name="user">The user to flush the stat for</param>
    /// <remarks>This can be throttled if called too often</remarks>
    void RequestFlushToService(
        _In_ XboxLiveUser_t user
        );

    /// <summary> 
    /// Force the current stat values to be uploaded to the service
    /// This will send immediately instead of automatically during a 30 second window
    /// </summary>
    /// <param name="user">The user to flush the stat for</param>
    /// <remarks>This can be throttled if called too often</remarks>
    void RequestFlushToService(
        _In_ XboxLiveUser_t user,
        _In_ bool isHighPriority
        );

    /// <summary> 
    /// Returns any events that have been processed
    /// </summary>
    /// <return>A list of events that have happened since previous do_work</return>
    Windows::Foundation::Collections::IVectorView<StatisticEvent^>^ DoWork();

    /// <summary> 
    /// Replaces the numerical stat by the value. Can be positive or negative
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <param name="value">Value to replace the stat by</param>
    /// <return>Whether or not the setting was successful. Can fail if stat is not of numerical type. Will return updated stat</return>
    void SetStatisticNumberData(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name,
        _In_ double value
        );

        /// <summary> 
    /// Replaces the numerical stat by the value. Can be positive or negative
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <param name="value">Value to replace the stat by</param>
    /// <return>Whether or not the setting was successful. Can fail if stat is not of numerical type. Will return updated stat</return>
    void SetStatisticIntegerData(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name,
        _In_ int64_t value
        );


    /// <summary> 
    /// Replaces the string stat by the value.
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <param name="value">Value to replace the stat by</param>
    /// <return>Whether or not the setting was successful. Can fail if stat is not of string type. Will return updated stat</return>
    void SetStatisticStringData(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name,
        _In_ Platform::String^ value
        );

    /// <summary> 
    /// Gets all stat names in the stat document.
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <return>The list of statistic anmes.</return>
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ GetStatisticNames(
        _In_ XboxLiveUser_t user
        );

    /// <summary> 
    /// Gets a stat value
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    /// <return>The updated stat</return>
    StatisticValue^ GetStatistic(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name
        );

    /// <summary> 
    /// Deletes a stat value
    /// </summary>
    /// <param name="user">The local user whose stats to access</param>
    /// <param name="name">The name of the statistic to modify</param>
    void DeleteStatistic(
        _In_ XboxLiveUser_t user,
        _In_ Platform::String^ name
        );

internal:
    StatisticManager();

private:
   std::shared_ptr<xbox::services::stats::manager::stats_manager> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END