// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/privacy.h"
#include "PermissionCheckResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN

/// <summary>
/// Contains the results of multiple permission checks.
/// </summary>
public ref class MultiplePermissionsCheckResult sealed
{
public:
    /// <summary>
    /// Xbox User Id for the target user of the permission request.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// Contains a collection of results returned when checking multiple permissions for a user.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<PermissionCheckResult^>^ Items { Windows::Foundation::Collections::IVectorView<PermissionCheckResult^>^ get(); }

internal:
    MultiplePermissionsCheckResult(
        _In_ xbox::services::privacy::multiple_permissions_check_result cppObj
        );

private:
    xbox::services::privacy::multiple_permissions_check_result m_cppObj;

    Windows::Foundation::Collections::IVectorView<PermissionCheckResult^>^ m_items;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END