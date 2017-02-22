// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "PermissionDenyReason_WinRT.h"

using namespace xbox::services::privacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN

PermissionDenyReason::PermissionDenyReason(
    _In_ xbox::services::privacy::permission_deny_reason cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END