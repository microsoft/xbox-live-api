// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

/// <summary> 
/// Represents a user's Xbox Live profile.
/// </summary>
typedef struct XblUserProfile
{
    /// <summary>
    /// The user's Xbox user ID.
    /// </summary>
    uint64_t xboxUserId;
    
    /// <summary>
    /// The UTF-8 encoded user's display name to be used in application UI.  This value is privacy gated and could
    /// be a user's real name or their Gamertag.
    /// </summary>
    char appDisplayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded Uri for the user's display picture to be used in application UI.
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:
    /// Format: png
    /// Width   Height
    /// 64      64
    /// 208     208
    /// 424     424
    /// </summary>
    char appDisplayPictureResizeUri[XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded user's display name to be used in game UI.  This value is privacy gated and could
    /// be a user's real name or their Gamertag.
    /// </summary>
    char gameDisplayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded Uri for the user's display picture to be used in games.
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:
    /// Format: png
    /// Width   Height
    /// 64      64
    /// 208     208
    /// 424     424
    /// </summary>
    char gameDisplayPictureResizeUri[XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded user's Gamerscore.
    /// </summary>
    char gamerscore[XBL_GAMERSCORE_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded user's Gamertag.
    /// </summary>
    char gamertag[XBL_GAMERTAG_CHAR_SIZE];
} XblUserProfile;

/// <summary>
/// Gets a user profile for a specific Xbox user.
/// To get the result, call XblProfileGetUserProfileResult inside the AsyncBlock callback
/// or after the AsyncBlock is complete.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the user to get the profile for.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Calls V2 GET /users/batch/profile/settings</remarks>
STDAPI XblProfileGetUserProfileAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblProfileGetUserProfileAsync operation
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfileAsync.</param>
/// <param name="profile">A caller allocated profile object to write result to.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfileResult(
    _Inout_ AsyncBlock* async,
    _Out_ XblUserProfile* profile
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets one or more user profiles for a collection of specified Xbox users.
/// To get the result, first call XblProfileGetUserProfilesResultCount to 
/// get the count of returned profiles and then call XblProfileGetUserProfilesResult 
/// inside the AsyncBlock callback or after the AsyncBlock is complete.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserIds">C-style array of Xbox User IDs of the users to get profiles for.</param>
/// <param name="xboxUserIdsCount">The number of Xbox User IDs in the array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Calls V2 GET /users/batch/profile/settings</remarks>
STDAPI XblProfileGetUserProfilesAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the number of profiles returned from a completed get XblProfileGetUserProfilesAsync operation
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfilesAsync.</param>
/// <param name="profileCount">Number of profiles returned.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfilesResultCount(
    _Inout_ AsyncBlock* async,
    _Out_ uint32_t* profileCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblProfileGetUserProfilesAsync operation.
/// The number of profiles returned can be obtained with XblGetProfileResultCount.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfilesAsync.</param>
/// <param name="profilesCount">Size of the caller allocated profiles array.</param>
/// <param name="profiles">A caller allocated array of XblUserProfile objects to copy the result into.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfilesResult(
    _Inout_ AsyncBlock* async,
    _In_ uint32_t profilesCount,
    _Out_writes_(profilesCount) XblUserProfile* profiles
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets profiles for users in a specified social group.
/// To get the result, first call XblProfileGetUserProfilesForSocialGroupResultCount to 
/// get the count of returned profiles and then call XblProfileGetUserProfilesForSocialGroupResult 
/// inside the AsyncBlock callback or after the AsyncBlock is complete.
/// </summary>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="socialGroup">The UTF-8 encoded name of the social group of users to search. Options are "Favorites" and "People".</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Calls V2 GET /users/{userId}/profile/settings/people/{socialGroup}</remarks>
STDAPI XblProfileGetUserProfilesForSocialGroupAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_z_ const char* socialGroup
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the number of profiles returned from a completed get XblProfileGetUserProfilesForSocialGroupAsync operation
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfilesForSocialGroupAsync.</param>
/// <param name="profileCount">Number of profiles returned.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfilesForSocialGroupResultCount(
    _Inout_ AsyncBlock* async,
    _Out_ uint32_t* profileCount
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblProfileGetUserProfilesForSocialGroupAsync operation.
/// The number of profiles returned can be obtained with XblGetProfileResultCount.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfilesForSocialGroupAsync.</param>
/// <param name="profilesCount">Size of the caller allocated profiles array.</param>
/// <param name="profiles">A caller allocated array of XblUserProfile objects to copy the result into.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfilesForSocialGroupResult(
    _Inout_ AsyncBlock* async,
    _In_ uint32_t profilesCount,
    _Out_writes_(profilesCount) XblUserProfile* profiles
    ) XBL_NOEXCEPT;
