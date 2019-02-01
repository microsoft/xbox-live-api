// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "XboxUserProfile_winrt.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN
/// <summary>
/// Services that manage user profile.
/// </summary>

public ref class ProfileService sealed
{
public:
    /// <summary>
    /// Gets current user profile.
    /// </summary>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// The result of the asynchronous operation is an XboxUserProfile object.
    /// </returns>
    /// <remarks>Calls V3 GET /users/me/profile/settings</remarks>
    Windows::Foundation::IAsyncOperation<XboxUserProfile^>^ GetUserProfileAsync();

    /// <summary>
    /// Gets a user profile for a specific Xbox user.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user to get the profile for.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// The result of the asynchronous operation is an XboxUserProfile object.
    /// </returns>
    /// <remarks>Calls V3 GET /users/batch/profile/settings</remarks>
    Windows::Foundation::IAsyncOperation<XboxUserProfile^>^ GetUserProfileAsync(
        _In_ Platform::String^ xboxUserId
        );

    /// <summary>
    /// Gets one or more user profiles for a collection of specified Xbox users.
    /// </summary>
    /// <param name="xboxUserIds">The collection of Xbox User IDs of the users to get profiles for</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// The result of the asynchronous operation is a collection of XboxUserProfile objects.
    /// </returns>
    /// <remarks>Calls V3 GET /users/batch/profile/settings</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<XboxUserProfile^>^>^ GetUserProfilesAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds
        );

    /// <summary>
    /// Gets user profiles for users in a specified social group.
    /// </summary>
    /// <param name="socialGroup">The name of the social group of users to search.
    /// See Microsoft::Xbox::Services::Social::SocialGroupConstants for the latest options.</param>
    /// <returns>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    /// The result of the asynchronous operation is a collection of XboxUserProfile objects.
    /// </returns>
    /// <remarks>Calls V3 GET /users/{userId}/profile/settings/people/{socialGroup}</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<XboxUserProfile^>^>^ GetUserProfilesForSocialGroupAsync(
        _In_ Platform::String^ socialGroup
        );

internal:
    ProfileService(
        _In_ xbox::services::social::profile_service cppObj
        );

private:
    xbox::services::social::profile_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END