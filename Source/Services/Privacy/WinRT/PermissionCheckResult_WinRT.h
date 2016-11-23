//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/privacy.h"
#include "PermissionDenyReason_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN
/// <summary>
/// Contains the results of a permission check.
/// </summary>

/// <summary>
/// Contains the result of a permission check.
/// </summary>
public ref class PermissionCheckResult sealed
{
public:
    /// <summary>
    /// Indicates if the user is allowed the requested access.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsAllowed, is_allowed, bool);

    /// <summary>
    /// The permission requested.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(PermissionRequested, permission_requested);

    /// <summary>
    /// If IsAllowed is false, contains the reasons why the permissions were denied.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<PermissionDenyReason^>^ DenyReasons { Windows::Foundation::Collections::IVectorView<PermissionDenyReason^>^ get(); }

internal:
    PermissionCheckResult(
        _In_ xbox::services::privacy::permission_check_result cppObj
        );

private:
    xbox::services::privacy::permission_check_result m_cppObj;
    Windows::Foundation::Collections::IVectorView<PermissionDenyReason^>^ m_denyReasons;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END