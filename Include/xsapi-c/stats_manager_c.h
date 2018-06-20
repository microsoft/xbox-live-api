// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

/// <summary> 
/// Type of data the stat is
/// </summary>
typedef enum XblStatsManagerStatDataType
{
    /// <summary> Undefined stat type </summary>
    XblStatsManagerStatDataType_Undefined,

    /// <summary> Integer or double stat type </summary>
    XblStatsManagerStatDataType_Number,

    /// <summary> String stat type </summary>
    XblStatsManagerStatDataType_String
} XblStatsManagerStatDataType;

/// <summary> 
/// Type of stat change event
/// </summary>
typedef enum XblStatsManagerStatEventType
{
    /// <summary> A local user has been added </summary>
    XblStatsManager_LocalUserAdded,

    /// <summary> A local user has been removed </summary>
    XblStatsManager_LocalUserRemoved,

    /// <summary> A stat has been updated </summary>
    XblStatsManager_StatUpdateComplete
};

/// <summary> 
/// Represents a simplified stat
/// </summary>
typedef struct XblStatValue
{
    /// <summary> 
    /// UTF-8 encoded name of the stat
    /// </summary>
    _Field_z_ const char* name;
    
    /// <summary> 
    /// The stat value as a double
    /// </summary>
    /// <remarks>Will be 0 if dataType is not XblStatsManagerStatDataType_Number</remarks>
    double asNumber;

    /// <summary> 
    /// The stat value as a 64bit integer
    /// </summary>
    /// <remarks>Will be 0 if dataType is not XblStatsManagerStatDataType_Number</remarks>
    int64_t asInteger;

    /// <summary> 
    /// The stat value as a UTF-8 encoded string
    /// </summary>
    /// <remarks>Will be an empty string if dataType is not XblStatsManagerStatDataType_String</remarks>
    _Field_z_ const char* asString;

    /// <summary> 
    /// Data type of the stat
    /// </summary>
    XblStatsManagerStatDataType dataType;
};

/// <summary> 
/// Represents a stat event returned by XblStatsManagerDoWork()
/// </summary>
typedef struct XblStatEvent
{   
    /// <summary> 
    /// Represents error code of the event
    /// 0 if there is no error
    /// </summary>
    int32_t err;

    /// <summary> 
    /// Represents the error message of the event
    /// Empty string if there is no error
    /// </summary>
    _Field_z_ const char* errMessage;

    /// <summary> 
    /// The type of this event
    /// </summary>
    XblStatsManagerStatEventType eventType;

    /// <summary> 
    /// Local user the event is for
    /// </summary>
    xbl_user_handle localUser;
};

/// <summary> 
/// Adds a local user to the stats manager.
/// Generates a XblStatsManager_LocalUserAdded event from XblStatsManagerDoWork().
/// </summary>
/// <param name="user">The user to add to the statistic manager.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStatsManagerAddLocalUser(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT;

/// <summary> 
/// Removes a local user from the stats manager.
/// Generates a XblStatsManager_LocalUserRemoved event from XblStatsManagerDoWork().
/// </summary>
/// <param name="user">The user to be removed from the statistic manager.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStatsManagerRemoveLocalUser(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT;

/// <summary> 
/// Requests that stats manager sends the stats to the service immediately,
/// with a maximum rate of once every 30 seconds. If you do not call this method, stats
/// are automatically sent to the service every 5 minutes.
/// </summary>
/// <param name="user">The user to flush the stat for.</param>
/// <param name="isHighPriority">Indicates if the flush is a high priority, typically when the game is getting suspended.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Stats are automatically sent to the service every 5 minutes. You can call this method 
/// if you want to update the service with the latest stats sooner, such as when a match or round ends.
/// 
/// This causes the current stat values to be uploaded to the service with
/// a maximum rate of once every 30 seconds.  Set isHighPriority to true when your title 
/// is getting suspended as this will try to update the stats even if it hasn't been 30 seconds 
/// since the last flush. However requests to flush with isHighPriority=true are still limited 
/// to a maximum rate of once every 30 seconds so it can't be used to flood the service.
///
/// Note that you may still be throttled when calling the stats service such as if you are 
/// relaunching the title over and over and send stats at every launch.
/// </remarks>
STDAPI XblStatsManagerRequestFlushToService(
    _In_ xbl_user_handle user,
    _In_ bool isHighPriority = false
    ) XBL_NOEXCEPT;

/// <summary> 
/// Returns any events that have been processed.
/// Must be called every frame for data to be up to date. 
/// The returned array of events is only valid until the next
/// call to XblStatsManagerDoWork().
/// </summary>
/// <param name="statEvents">
/// Pointer to the array of stat events that have occurred since the last call to XblStatsManagerDoWork(). This array is
/// only valid until the next call to XblStatsManagerDoWork(). The internal array will automatically be cleaned
/// up when XblCleanup is called.
/// </param>
/// <param name="statEventsCount">The number of events in statEvents.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStatsManagerDoWork(
    _Outptr_ XblStatEvent** statEvents,
    _Out_ uint32_t* statEventsCount
    ) XBL_NOEXCEPT;

/// <summary> 
/// Replaces the numerical stat by the value. Can be positive or negative.
/// 
/// Stats will be sent to the service automatically every 5 minutes or you can call 
/// XblStatsManagerRequestFlushToService() if you want to update the service with 
/// the latest stats sooner such as when a match or round ends.
/// </summary>
/// <param name="user">The local user whose stats to access.</param>
/// <param name="statName">The name of the statistic to modify.</param>
/// <param name="statValue">The value to replace the stat by.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStatsManagerSetStatAsNumber(
    _In_ xbl_user_handle user,
    _In_z_ const char* statName,
    _In_ double statValue
    ) XBL_NOEXCEPT;

/// <summary> 
/// Replaces the numerical stat by the value. Can be positive or negative.
/// 
/// Stats will be sent to the service automatically every 5 minutes or you can call 
/// XblStatsManagerRequestFlushToService() if you want to update the service with 
/// the latest stats sooner such as when a match or round ends.
/// </summary>
/// <param name="user">The local user whose stats to access.</param>
/// <param name="statName">The name of the statistic to modify.</param>
/// <param name="statValue">The value to replace the stat by.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStatsManagerSetStatAsInteger(
    _In_ xbl_user_handle user,
    _In_z_ const char* statName,
    _In_ int64_t statValue
    ) XBL_NOEXCEPT;

/// <summary> 
/// Replaces a string stat with the given value.
/// 
/// Stats will be sent to the service automatically every 5 minutes or you can call 
/// XblStatsManagerRequestFlushToService() if you want to update the service with 
/// the latest stats sooner such as when a match or round ends.
/// </summary>
/// <param name="user">The local user whose stats to access.</param>
/// <param name="statName">The name of the statistic to modify.</param>
/// <param name="statValue">The value to replace the stat by.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStatsManagerSetStatAsString(
    _In_ xbl_user_handle user,
    _In_z_ const char* statName,
    _In_z_ const char* statValue
    ) XBL_NOEXCEPT;

/// <summary> 
/// Deletes a stat. Will clear stat from service and leaderboard information.
/// </summary>
/// <param name="user">The local user whose stats to access.</param>
/// <param name="statName">The name of the statistic to delete.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblStatsManagerDeleteStat(
    _In_ xbl_user_handle user,
    _In_z_ const char* statName
    ) XBL_NOEXCEPT;