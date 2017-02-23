// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/privacy.h"
#include "PermissionCheckResult_WinRT.h"
#include "MultiplePermissionsCheckResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN

/// <summary>
/// Provides an endpoint for managing privacy settings.
/// </summary>
public ref class PrivacyService sealed
{
public:

    /// <summary>
    /// Get the list of Xbox Live Ids the calling user should avoid during multiplayer matchmaking.
    /// </summary>
    /// <returns>A collection of XboxUserIds that correspond to the calling user's avoid list.</returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /users/xuid({xuid})/people/avoid</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ GetAvoidListAsync();

    /// <summary>
    /// Check a single permission with a single target user.
    /// </summary>
    /// <param name="permissionId">The ID of the permission to check.
    /// See Microsoft::Xbox::Services::Privacy::PermissionIdConstants for the latest options.</param>
    /// <param name="targetXboxUserId">The target user's xbox Live ID for validation</param>
    /// <returns>The permission check result against a single user.</returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /users/{requestorId}/permission/validate
    /// </remarks>
    Windows::Foundation::IAsyncOperation<PermissionCheckResult^>^ CheckPermissionWithTargetUserAsync(
        _In_ Platform::String^ permissionId,
        _In_ Platform::String^ targetXboxUserId
        );

    /// <summary>
    /// Check multiple permissions with multiple target users.
    /// </summary>
    /// <param name="permissionIds">The collection of IDs of the permissions to check.
    /// See Microsoft::Xbox::Services::Privacy::PermissionIdConstants for the latest options.</param>
    /// <param name="targetXboxUserIds">The collection of target Xbox user IDs to check permissions against.</param>
    /// <returns>A multiple permission check result which contains a collection of permission information.</returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 POST /users/{requestorId}/permission/validate
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<MultiplePermissionsCheckResult^>^>^ CheckMultiplePermissionsWithMultipleTargetUsersAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ permissionIds,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ targetXboxUserIds
        );
    
    /// <summary>
    /// Get the list of Xbox Live Ids that the calling user should not hear (mute) during multiplayer matchmaking.
    /// </summary>
    /// <returns>The collection of Xbox user IDs that represent the mute list for a user.</returns>
    /// <remarks>
    /// Returns an IAsyncOperation&lt;TResult&gt; object that represents the state of the asynchronous operation.
    ///
    /// Calls V1 GET /users/xuid({xuid})/people/mute
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<Platform::String^>^>^ GetMuteListAsync();

internal:
    PrivacyService( 
        _In_ xbox::services::privacy::privacy_service cppObj
        );

private:
    xbox::services::privacy::privacy_service m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END