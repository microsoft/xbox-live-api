// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary> 
/// Represents a user's Xbox Live profile.
/// </summary>
/// <argof><see cref="XblProfileGetUserProfileResult"/></argof>
/// <argof><see cref="XblProfileGetUserProfilesResult"/></argof>
/// <argof><see cref="XblProfileGetUserProfilesForSocialGroupResult"/></argof>
typedef struct XblUserProfile
{
    /// <summary>
    /// The user's Xbox user ID.
    /// </summary>
    uint64_t xboxUserId;

    /// <summary>
    /// The UTF-8 encoded user's display name to be used in application UI.  
    /// This value is privacy gated and could be a user's real name or their Gamertag.
    /// </summary>
    char appDisplayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded Uri for the user's display picture to be used in application UI.  
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:<br/>
    /// Format: png<br/>
    /// Width   Height<br/>
    /// 64      64<br/>
    /// 208     208<br/>
    /// 424     424<br/>
    /// </summary>
    char appDisplayPictureResizeUri[XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded user's display name to be used in game UI.  
    /// This value is privacy gated and could be a user's real name or their Gamertag.
    /// </summary>
    char gameDisplayName[XBL_DISPLAY_NAME_CHAR_SIZE];

    /// <summary>
    /// UTF-8 encoded Uri for the user's display picture to be used in games.  
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending &apos;&amp;format={format}&amp;w={width}&amp;h={height}:<br/>
    /// Format: png<br/>
    /// Width   Height<br/>
    /// 64      64<br/>
    /// 208     208<br/>
    /// 424     424<br/>
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

    /// <summary>
    /// The UTF-8 encoded modern gamertag for the user.  
    /// Not guaranteed to be unique.
    /// </summary>
    char modernGamertag[XBL_MODERN_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded suffix appended to modern gamertag to ensure uniqueness.  
    /// May be empty in some cases.
    /// </summary>
    char modernGamertagSuffix[XBL_MODERN_GAMERTAG_SUFFIX_CHAR_SIZE];

    /// <summary>
    /// The UTF-8 encoded unique modern gamertag and suffix.  
    /// Format will be "modernGamertag#suffix".  
    /// Guaranteed to be no more than 16 rendered characters.
    /// </summary>
    char uniqueModernGamertag[XBL_UNIQUE_MODERN_GAMERTAG_CHAR_SIZE];
} XblUserProfile;

/// <summary>
/// Gets a user profile for a specific Xbox user.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserId">The Xbox User ID of the user to get the profile for.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If profiles are needed for multiple users, use the batch API instead: <see cref="XblProfileGetUserProfilesAsync"/> 
/// To get the result, call <see cref="XblProfileGetUserProfileResult"/> 
/// inside the AsyncBlock callback or after the AsyncBlock is complete.
/// </remarks>
/// <rest>Calls V2 GET /users/batch/profile/settings</rest>
STDAPI XblProfileGetUserProfileAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblProfileGetUserProfileAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfileAsync.</param>
/// <param name="profile">A caller allocated profile object to write result to.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfileResult(
    _In_ XAsyncBlock* async,
    _Out_ XblUserProfile* profile
) XBL_NOEXCEPT;

/// <summary>
/// Gets one or more user profiles for a collection of specified Xbox users.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="xboxUserIds">C-style array of Xbox User IDs of the users to get profiles for.</param>
/// <param name="xboxUserIdsCount">The number of Xbox User IDs in the array.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, first call <see cref="XblProfileGetUserProfilesResultCount"/> to 
/// get the count of returned profiles and then call <see cref="XblProfileGetUserProfilesResult"/> 
/// inside the AsyncBlock callback or after the AsyncBlock is complete.
/// </remarks>
/// <rest>Calls V2 GET /users/batch/profile/settings</rest>
STDAPI XblProfileGetUserProfilesAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t* xboxUserIds,
    _In_ size_t xboxUserIdsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the number of profiles from a completed get XblProfileGetUserProfilesAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfilesAsync.</param>
/// <param name="profileCount">Passes back the number of profiles.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfilesResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* profileCount
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblProfileGetUserProfilesAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfilesAsync.</param>
/// <param name="profilesCount">The size of the caller allocated profiles array.  
/// Use <see cref="XblProfileGetUserProfilesResultCount"/> to get the count required.</param>
/// <param name="profiles">A caller allocated array that passes back the user profile results.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfilesResult(
    _In_ XAsyncBlock* async,
    _In_ size_t profilesCount,
    _Out_writes_(profilesCount) XblUserProfile* profiles
) XBL_NOEXCEPT;

/// <summary>
/// Gets profiles for users in a specified social group.
/// </summary>
/// <param name="xboxLiveContext">An xbox live context handle created with XblContextCreateHandle.</param>
/// <param name="socialGroup">The UTF-8 encoded name of the social group of users to search. Options are "Favorites" and "People".</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// To get the result, first call <see cref="XblProfileGetUserProfilesForSocialGroupResultCount"/> to 
/// get the count of returned profiles and then call <see cref="XblProfileGetUserProfilesForSocialGroupResult"/> 
/// inside the AsyncBlock callback or after the AsyncBlock is complete.
/// </remarks>
/// <rest>Calls V2 GET /users/{userId}/profile/settings/people/{socialGroup}</rest>
STDAPI XblProfileGetUserProfilesForSocialGroupAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_z_ const char* socialGroup,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the number of profiles from a completed XblProfileGetUserProfilesForSocialGroupAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfilesForSocialGroupAsync.</param>
/// <param name="profileCount">Passes back the number of profiles.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfilesForSocialGroupResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* profileCount
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for a completed XblProfileGetUserProfilesForSocialGroupAsync operation.
/// </summary>
/// <param name="async">The same AsyncBlock that passed to XblProfileGetUserProfilesForSocialGroupAsync.</param>
/// <param name="profilesCount">The size of the caller allocated profiles array.  
/// Use <see cref="XblProfileGetUserProfilesForSocialGroupResultCount"/> to get the count required.</param>
/// <param name="profiles">A caller allocated array that passes back the social group user profile results.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblProfileGetUserProfilesForSocialGroupResult(
    _In_ XAsyncBlock* async,
    _In_ size_t profilesCount,
    _Out_writes_(profilesCount) XblUserProfile* profiles
) XBL_NOEXCEPT;

}