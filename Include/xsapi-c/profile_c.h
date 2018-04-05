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
    UTF8CSTR appDisplayName;

    /// <summary>
    /// Encoded Uri for the user's display picture to be used in application UI.
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:
    /// Format: png
    /// Width   Height
    /// 64      64
    /// 208     208
    /// 424     424
    /// </summary>
    UTF8CSTR appDisplayPictureResizeUri;

    /// <summary>
    /// The user's display name to be used in game UI.  This value is privacy gated and could
    /// be a user's real name or their Gamertag.
    /// </summary>
    UTF8CSTR gameDisplayName;

    /// <summary>
    /// Encoded Uri for the user's display picture to be used in games.
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:
    /// Format: png
    /// Width   Height
    /// 64      64
    /// 208     208
    /// 424     424
    /// </summary>
    UTF8CSTR gameDisplayPictureResizeUri;

    /// <summary>
    /// The user's Gamerscore.
    /// </summary>
    UTF8CSTR gamerscore;

    /// <summary>
    /// The user's Gamertag.
    /// </summary>
    UTF8CSTR gamertag;

    /// <summary>
    /// The user's Xbox user ID.
    /// </summary>
    UTF8CSTR xboxUserId;
} XblUserProfile;

typedef struct XblUserProfiles
{
    XblUserProfile* items;
    uint32_t count;
} XblUserProfiles;


/// <summary>
/// Gets a user profile for a specific Xbox user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the user to get the profile for.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK and E_INVALIDARG.
/// The result of the asynchronous operation is returned via the callback parameters.
/// </returns>
/// <remarks>Calls V2 GET /users/batch/profile/settings</remarks>
STDAPI XblGetUserProfile(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ UTF8CSTR xboxUserId,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the results from XblGetUserProfile
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="sizeInBytes">Size in bytes of buffer to allocate for result.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_OUTOFMEMORY
/// </returns>
STDAPI XblGetUserProfileResultSize(
    _In_ AsyncBlock* async,
    _Out_ size_t* sizeInBytes
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the results from XblGetUserProfile
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="profile">A pointer to store the result.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_OUTOFMEMORY
/// </returns>
STDAPI XblGetUserProfileResult(
    _In_ AsyncBlock* async,
    _Out_ XblUserProfile** profile,
    _In_ size_t sizeInBytes
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets one or more user profiles for a collection of specified Xbox users.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserIds">C-style Array of Xbox User IDs of the users to get profiles for.</param>
/// <param name="xboxUserIdsCount">The number of Xbox User IDs in the array.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>
/// <returns>
/// Result code for this API operation. Possible values are S_OK and E_INVALIDARG.
/// The result of the asynchronous operation is returned via the callback parameters.
/// </returns>
/// <remarks>Calls V2 GET /users/batch/profile/settings</remarks>
STDAPI XblGetUserProfiles(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ UTF8CSTR *xboxUserIds,
    _In_ uint32_t xboxUserIdsCount,
    _In_ AsyncBlock* async
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the results from XblGetUserProfile
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="sizeInBytes">Size in bytes of buffer to allocate for result.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_OUTOFMEMORY
/// </returns>
STDAPI XblGetUserProfilesResultSize(
    _In_ AsyncBlock* async,
    _Out_ size_t* sizeInBytes
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the results from XblGetUserProfile
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="profile">A pointer to store the result.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_OUTOFMEMORY
/// </returns>
STDAPI XblGetUserProfilesResult(
    _In_ AsyncBlock* async,
    _Out_ XblUserProfiles** profiles,
    _In_ size_t sizeInBytes
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
/// Gets the results from XblGetUserProfile
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="sizeInBytes">Size in bytes of buffer to allocate for result.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_OUTOFMEMORY
/// </returns>
STDAPI XblGetUserProfilesForSocialGroupResultSize(
    _In_ AsyncBlock* async,
    _Out_ size_t* sizeInBytes
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the results from XblGetUserProfile
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="profile">A pointer to store the result.</param>
/// <returns>
/// Result code for this API operation. Possible values are S_OK, E_INVALIDARG, or E_OUTOFMEMORY
/// </returns>
STDAPI XblGetUserProfilesForSocialGroupResult(
    _In_ AsyncBlock* async,
    _Out_ XblUserProfiles** profiles,
    _In_ size_t sizeInBytes
    ) XBL_NOEXCEPT;

