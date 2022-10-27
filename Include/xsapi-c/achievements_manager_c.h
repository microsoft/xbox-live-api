// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
    #error C++11 required
#endif

#pragma once

#include "pal.h"
#include "achievements_c.h"

extern "C"
{   

/// <summary>
/// Enumeration values that specify the order we display the results in.
/// </summary>
enum class XblAchievementsManagerSortOrder : uint32_t
{
    /// <summary>
    /// Unsorted sort order will skip the sort operation.
    /// </summary>
    Unsorted = 0,
    
    /// <summary>
    /// Elements in the response are in ascending order of the field specified by XblAchievementsManagerSortValue.
    /// </summary>
    Ascending = 1,
    
    /// <summary>
    /// Elements in the response are in descending order of the field specified by XblAchievementsManagerSortValue.
    /// </summary>
    Descending = 2
};

/// <summary>
/// Defines values used to indicate event types for an achievement. 
/// </summary>
enum class XblAchievementsManagerEventType : uint32_t
{
    /// <summary>
    /// Indicates that a local user was added and has been initialized.
    /// </summary>
    LocalUserInitialStateSynced = 0,

    /// <summary>
    /// Indicates the achievement was unlocked.
    /// </summary>
    AchievementUnlocked = 1,
    
    /// <summary>
    /// Indicates progress has been made on (a requirement of) an achievement.
    /// </summary>
    AchievementProgressUpdated = 2
};

/// <summary>
/// An achievement event that will be returned from <see cref="XblAchievementsManagerDoWork"/>.
/// </summary>
typedef struct XblAchievementsManagerEvent
{
    /// <summary>
    /// Current state of progress for an achievement for AchievementUnlocked and
    /// AchievementProgressUpdated events. The values of this struct are not 
    /// populated for LocalUserInitialStateSynced events.
    /// </summary>
    XblAchievementProgressChangeEntry progressInfo;

    /// <summary>
    /// The ID for the user that has made progress on an achievement.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// Type of the event triggered.
    /// </summary>
    XblAchievementsManagerEventType eventType;
} XblAchievementsManagerEvent;

/// <summary>
/// A handle to an achievement manager result.
/// </summary>
/// <remarks>
/// This handle is used by other APIs to get the achievement objects and to get the 
/// next page of achievements from the service if there is one.  
/// The handle must be closed using <see cref="XblAchievementsManagerResultCloseHandle"/> 
/// when the result is no longer needed.
/// </remarks>
/// <memof><see cref="XblAchievementsManagerResultGetAchievements"/></memof>
/// <memof><see cref="XblAchievementsManagerResultDuplicateHandle"/></memof>
/// <memof><see cref="XblAchievementsManagerResultCloseHandle"/></memof>
typedef struct XblAchievementsManagerResult* XblAchievementsManagerResultHandle;

/// <summary>
/// Get a list of XblAchievement objects.
/// </summary>
/// <param name="resultHandle">Achievement result handle.</param>
/// <param name="achievements">Pointer to an array of XblAchievement objects.  
/// The memory for the returned pointer will remain valid for the life of the 
/// XblAchievementsManagerResultHandle object until it is closed.</param>
/// <param name="achievementsCount">The count of objects in the returned array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The returned array of XblAchievement objects is freed when all outstanding handles 
/// to the object have been closed with <see cref="XblAchievementsManagerResultCloseHandle"/>.
/// However the data might become stale.
/// </remarks>
STDAPI XblAchievementsManagerResultGetAchievements(
    _In_ XblAchievementsManagerResultHandle resultHandle,
    _Out_ const XblAchievement** achievements,
    _Out_ uint64_t* achievementsCount
) XBL_NOEXCEPT;

/// <summary>
/// Duplicates a XblAchievementsManagerResultHandle.
/// </summary>
/// <param name="handle">The XblAchievementsManagerResultHandle to duplicate.</param>
/// <param name="duplicatedHandle">The duplicated handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsManagerResultDuplicateHandle(
    _In_ XblAchievementsManagerResultHandle handle,
    _Out_ XblAchievementsManagerResultHandle* duplicatedHandle
) XBL_NOEXCEPT;

/// <summary>
/// Closes the XblAchievementsManagerResultHandle.
/// </summary>
/// <param name="handle">The XblAchievementsManagerResultHandle to close.</param>
/// <returns></returns>
/// <remarks>
/// When all outstanding handles have been closed, the memory associated with the achievement result will be freed.
/// </remarks>
STDAPI_(void) XblAchievementsManagerResultCloseHandle(
    _In_ XblAchievementsManagerResultHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Generate a local cache of achievements for a user.
/// </summary>
/// <param name="user">Xbox Live User to fetch achievements for.</param>
/// <param name="queue">Queue to be used for background operation for this user (Optional).</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsManagerAddLocalUser(
    _In_ XblUserHandle user, 
    _In_opt_ XTaskQueueHandle queue
) XBL_NOEXCEPT;

/// <summary>
/// Immediately remove the local cache of achievements for a user.
/// </summary>
/// <param name="user">Xbox Live User to clear the cache for.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsManagerRemoveLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT;

/// <summary>
/// Checks whether a specific user has had its initial state synced.
/// </summary>
/// <param name="xboxUserId">Xbox Live User to check.</param>
/// <returns>HRESULT return code for this API operation. If the user is not 
/// initialized, this function will return E_FAIL.</returns>
STDAPI XblAchievementsManagerIsUserInitialized(
    _In_ uint64_t xboxUserId
) XBL_NOEXCEPT;

/// <summary>
/// Called whenever the title wants to update the state of achievements and get list of change events.
/// </summary>
/// <param name="achievementsEvents">Passes back a pointer to the array of achievement events that have occurred since the last call to XblAchievementsManagerDoWork.</param>
/// <param name="achievementsEventsCount">Passes back the number of events in the achievement events array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Must be called every frame for data to be up to date.  
/// The array of achievement events that is sent back is only valid until the next call to <see cref="XblAchievementsManagerDoWork"/>.  
/// Make sure to check if there were achievement events sent back.  
/// If the achievement events array is null, no results.  
/// If the achievement events count is 0, no results.  
/// If there were achievement events sent back then handle each <see cref="XblAchievementsManagerEvent"/> 
/// by their respective <see cref="XblAchievementsManagerEventType"/>.
/// </remarks>
STDAPI XblAchievementsManagerDoWork(
    _Outptr_result_maybenull_ const XblAchievementsManagerEvent** achievementsEvents,
    _Out_ size_t* achievementsEventsCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets the current local state of an achievement for a local player with a specific achievement ID.
/// </summary>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="achievementId">The UTF-8 encoded unique identifier of the Achievement as defined by Dev Center.</param>
/// <param name="achievementResult">The handle to the result of AchievementsManager API calls. 
/// This handle is used by other APIs to get the achievement objects matching the 
/// API that was called. 
/// The handle must be closed using <see cref="XblAchievementsManagerResultCloseHandle"/> when
/// the result is no longer needed.
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsManagerGetAchievement(
    _In_ uint64_t xboxUserId,
    _In_ const char* achievementId,
    _Outptr_result_maybenull_ XblAchievementsManagerResultHandle* achievementResult
) XBL_NOEXCEPT;

/// <summary>
/// Gets a list of all achievements for a player.
/// </summary>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="sortField">
/// The field to sort the list of achievements on.
/// </param>
/// <param name="sortOrder">The direction by which to sort the list of achievements.</param>
/// <param name="achievementsResult">The handle to the result of AchievementsManager API calls. 
/// This handle is used by other APIs to get the achievement objects matching the 
/// API that was called. 
/// The handle must be closed using <see cref="XblAchievementsManagerResultCloseHandle"/> when
/// the result is no longer needed.
/// </param>
/// <remarks>
/// Passing in XblAchievementOrderBy::TitleId for sortField yields the same results 
/// as passing in XblAchievementOrderBy::DefaultOrder since all achievements tracked 
/// by achievement manager will have the same TitleId.
/// </remarks>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsManagerGetAchievements(
    _In_ uint64_t xboxUserId,
    _In_ XblAchievementOrderBy sortField,
    _In_ XblAchievementsManagerSortOrder sortOrder,
    _Outptr_result_maybenull_ XblAchievementsManagerResultHandle* achievementsResult
) XBL_NOEXCEPT;

/// <summary>
/// Gets a list of achievements in a specific progress state for a player.
/// </summary>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="sortField">
/// The field to sort the list of achievements on. TitleId will behave
/// the same as DefaultOrder, as AchievementsManager only handles one title
/// at a time.
/// </param>
/// <param name="sortOrder">The direction by which to sort the list of achievements.</param>
/// <param name="achievementState">The achievement state to include in the results.</param>
/// <param name="achievementsResult">The handle to the result of AchievementsManager API calls. 
/// This handle is used by other APIs to get the achievement objects matching the 
/// API that was called. 
/// The handle must be closed using <see cref="XblAchievementsManagerResultCloseHandle"/> when
/// the result is no longer needed.
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblAchievementsManagerGetAchievementsByState(
    _In_ uint64_t xboxUserId,
    _In_ XblAchievementOrderBy sortField,
    _In_ XblAchievementsManagerSortOrder sortOrder,
    _In_ XblAchievementProgressState achievementState,
    _Outptr_result_maybenull_ XblAchievementsManagerResultHandle* achievementsResult
) XBL_NOEXCEPT;

/// <summary>
/// Allow achievement progress to be updated and achievements to be unlocked.
/// </summary>
/// <param name="xboxUserId">The Xbox User ID of the player.</param>
/// <param name="achievementId">The UTF-8 encoded achievement ID as defined by Dev Center.</param>
/// <param name="currentProgress">The completion percentage of the achievement to indicate progress.  
/// Valid values are from 1 to 100. Set to 100 to unlock the achievement.  
/// Progress will be set by the server to the highest value sent</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This API will work even when offline on PC and Xbox consoles. Offline updates will be 
/// posted by the system when connection is re-established even if the title isn't running.  
/// 
/// The result of the operation will not be represented locally immediately. The
/// earliest the update will be reflected will be after the next frame's call to
/// DoWork. Once the change is reflected, the array returned by DoWork 
/// will contain a <see cref="XblAchievementsManagerEvent" /> of with
/// an event type of AchievementProgressUpdated, and potentially an 
/// additional event of type AchievementUnlocked if the new progress
/// resulted in unlocking the achievement.
///
/// If the achievement has already been unlocked or the progress value is less than or 
/// equal to what is cached from the server, this function will return E_NOT_VALID_STATE 
/// or E_INVALIDARG respectively. 
///
/// Only title based achievements may be updated with this function. Event based 
/// achievements cannot be updated with this function. 
/// </remarks>
/// <rest>V2 POST /users/xuid({xuid})/achievements/{scid}/update</rest>
STDAPI XblAchievementsManagerUpdateAchievement(
    _In_ uint64_t xboxUserId, 
    _In_ const char* achievementId, 
    _In_ uint8_t currentProgress
) XBL_NOEXCEPT;

} //end extern "C"


