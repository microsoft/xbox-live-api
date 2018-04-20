// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

/// <summary> 
/// Represents a user's Xbox Live profile.
/// </summary>
typedef struct XblUserProfile
{
    /// <summary>
    /// The user's display name to be used in application UI.  This value is privacy gated and could
    /// be a user's real name or their Gamertag.
    /// </summary>
    char appDisplayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// Encoded Uri for the user's display picture to be used in application UI.
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:
    /// Format: png
    /// Width   Height
    /// 64      64
    /// 208     208
    /// 424     424
    /// </summary>
    char appDisplayPictureResizeUri[XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE];

    /// <summary>
    /// The user's display name to be used in game UI.  This value is privacy gated and could
    /// be a user's real name or their Gamertag.
    /// </summary>
    char gameDisplayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// Encoded Uri for the user's display picture to be used in games.
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:
    /// Format: png
    /// Width   Height
    /// 64      64
    /// 208     208
    /// 424     424
    /// </summary>
    char gameDisplayPictureResizeUri[XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE];

    /// <summary>
    /// The user's Gamerscore.
    /// </summary>
    char gamerscore[XBL_GAMERSCORE_CHAR_SIZE];

    /// <summary>
    /// The user's Gamertag.
    /// </summary>
    char gamertag[XBL_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The user's Xbox user ID.
    /// </summary>
    uint64_t xboxUserId;
} XblUserProfile;

/// <summary>
/// Gets a user profile for a specific Xbox user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the user to get the profile for.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK and E_INVALIDARG.
/// The result of the asynchronous operation is obtained using XblGetProfileResult.
/// </returns>
/// <remarks>Calls V2 GET /users/batch/profile/settings</remarks>
STDAPI XblGetUserProfile(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets one or more user profiles for a collection of specified Xbox users.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserIds">C-style Array of Xbox User IDs of the users to get profiles for.</param>
/// <param name="xboxUserIdsCount">The number of Xbox User IDs in the array.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>
/// The result of the asynchronous operation is obtained using XblGetProfileResult.
/// </returns>
/// <remarks>Calls V2 GET /users/batch/profile/settings</remarks>
STDAPI XblGetUserProfiles(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets a user profile for a specific Xbox user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="socialGroup">The name of the social group of users to search. Options are "Favorites" and "People".</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK and E_INVALIDARG.
/// The result of the asynchronous operation is returned via the callback parameters.
/// </returns>
/// <remarks>Calls V2 GET /users/{userId}/profile/settings/people/{socialGroup}</remarks>
STDAPI XblGetUserProfilesForSocialGroup(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ UTF8CSTR socialGroup,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the number of profiles returned from a completed get profile(s) operation
/// </summary>
/// <param name="async">AsyncBlock from the get profile(s) API.</param>
/// <param name="profileCount">Number of profiles returned.</param>
STDAPI XblGetProfileResultCount(
    _In_ AsyncBlock* async,
    _Out_ uint32_t* profileCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed get profile(s) operation
/// </summary>
/// <param name="async">AsyncBlock from the get profile(s) API.</param>
/// <param name="profilesCount">Size of the profiles array.</param>
/// <param name="profiles">Array of XblUserProfile objects to copy result into.</param>
/// <param name="written">Actual number of profiles written to the array.</param>
STDAPI XblGetProfileResult(
    _In_ AsyncBlock* async,
    _In_ uint32_t profilesCount,
    _Out_writes_to_(profilesCount, written) XblUserProfile* profiles,
    _Out_opt_ uint32_t* written
    ) XBL_NOEXCEPT;
