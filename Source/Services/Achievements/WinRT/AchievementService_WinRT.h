// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/achievements.h"
#include "AchievementsResult_winrt.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents an endpoint that you can use to access the Achievement service.
/// </summary>
public ref class AchievementService sealed
{
public:
    /// <summary>
    /// Allow achievement progress to be updated and achievements to be unlocked.
    /// This API will work even when offline. On PC and Xbox One, updates will be 
    /// posted by the system when connection is re-established even if the title isn't running.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
    /// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
    /// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
    /// Progress will be set by the server to the highest value sent</param>
    /// <remarks>
    /// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
    /// </remarks>
    Windows::Foundation::IAsyncAction^ UpdateAchievementAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ achievementId,
        _In_ uint32 percentComplete
        );

    /// <summary>
    /// Attempts to update achievement progress and unlock achievements.
    /// This API will work even when offline. On PC and Xbox One, updates will be 
    /// posted by the system when connection is re-established even if the title isn't running.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
    /// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
    /// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
    /// Progress will be set by the server to the highest value sent</param>
    /// <remarks>
    /// Returns the HRESULT if an error occured.
    ///
    /// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
    /// </remarks>
    Windows::Foundation::IAsyncOperation<int32>^ TryUpdateAchievementAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ achievementId,
        _In_ uint32 percentComplete
        );

    /// <summary>
    /// Allow achievement progress to be updated and achievements to be unlocked.
    /// This API will work even when offline. On PC and Xbox One, updates will be 
    /// posted by the system when connection is re-established even if the title isn't running.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="titleId">The title ID.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) for the title.</param>
    /// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
    /// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
    /// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
    /// Progress will be set by the server to the highest value sent</param>
    /// <remarks>
    /// Returns a task&lt;T&gt; object that represents the state of the asynchronous operation.
    ///
    /// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
    /// </remarks>
    Windows::Foundation::IAsyncAction^ UpdateAchievementAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ uint32 titleId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ achievementId,
        _In_ uint32 percentComplete
        );

    /// <summary>
    /// Attempts to update achievement progress and unlock achievements.
    /// This API will work even when offline. On PC and Xbox One, updates will be 
    /// posted by the system when connection is re-established even if the title isn't running.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="titleId">The title ID.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) for the title.</param>
    /// <param name="achievementId">The achievement ID as defined by XDP or Dev Center.</param>
    /// <param name="percentComplete">The completion percentage of the achievement to indicate progress.
    /// Valid values are from 1 to 100. Set to 100 to unlock the achievement.
    /// Progress will be set by the server to the highest value sent</param>
    /// <remarks>
    /// Returns the HRESULT if an error occured.
    ///
    /// This method calls V2 POST /users/xuid({xuid})/achievements/{scid}/update
    /// </remarks>
    Windows::Foundation::IAsyncOperation<int32>^ TryUpdateAchievementAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ uint32 titleId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ achievementId,
        _In_ uint32 percentComplete
        );

    /// <summary>
    /// Returns an AchievementsResult object containing the first page of achievements
    /// for a player of the specified title.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="titleId">The title ID.</param>
    /// <param name="type">The achievement type to retrieve.</param>
    /// <param name="unlockedOnly">Indicates whether to return unlocked achievements only.</param>
    /// <param name="orderby">Controls how the list of achievements is ordered.</param>
    /// <param name="skipItems">The number of achievements to skip.</param>
    /// <param name="maxItems">The maximum number of achievements the result can contain.  Pass 0 to attempt
    /// to retrieve all items.</param>
    /// <returns>An AchievementsResult object that contains a list of Achievement objects.</returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// See AchievementsResult::GetNextAsync to page in the next set of results.
    ///
    /// This method calls V2 GET /users/xuid({xuid})/achievements.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<AchievementsResult^>^ GetAchievementsForTitleIdAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ uint32 titleId,
        _In_ AchievementType type,
        _In_ bool unlockedOnly,
        _In_ AchievementOrderBy orderBy,
        _In_ uint32 skipItems,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Returns a specific Achievement object for a specified player.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) for the service.</param>
    /// <param name="achievementId">The achievement ID.</param>
    /// <returns>The requested Achievement object if it exists.
    /// If the achievement does not exist, the method throws a Platform::COMException() exception,
    /// with hr = INET_E_OBJECT_NOT_FOUND (0x800C0006L).</returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// Completion of the asynchronous operation is signaled by using a handler that is passed to the
    /// IAsyncOperation&lt;TResult&gt;.Completed property. When the asynchronous operation is complete, the result of the
    /// operation can be retrieved by using the IAsyncOperation&lt;TResult&gt;.GetResults method.
    ///
    /// This method calls V2 GET /users/xuid({xuid})/achievements/{scid}/{achievementId}.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Achievement^>^ GetAchievementAsync(
        _In_ Platform::String^ xboxUserId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ achievementId
        );

internal:
    AchievementService( 
        _In_ xbox::services::achievements::achievement_service cppObj
        );

private:

    xbox::services::achievements::achievement_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
