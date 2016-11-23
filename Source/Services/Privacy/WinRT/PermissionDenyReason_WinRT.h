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

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN

/// <summary>
/// Contains the reason why permission was denied. 
/// </summary>
public ref class PermissionDenyReason sealed
{
public:
    /// <summary>
    /// The reason why permission was denied.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Reason, reason);

    /// <summary>
    /// If the deny reason is previlege check, this indicates which privilege failed.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(RestrictedSetting, restricted_setting);

internal:
    PermissionDenyReason(
        _In_ xbox::services::privacy::permission_deny_reason cppObj
        );

private:
    xbox::services::privacy::permission_deny_reason m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END