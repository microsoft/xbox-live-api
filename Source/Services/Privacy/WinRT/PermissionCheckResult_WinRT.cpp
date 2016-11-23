//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "PermissionCheckResult_WinRT.h"

using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::privacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN

PermissionCheckResult::PermissionCheckResult(
    _In_ xbox::services::privacy::permission_check_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_denyReasons = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<PermissionDenyReason, permission_deny_reason>(m_cppObj.deny_reasons())->GetView();
}

IVectorView<PermissionDenyReason^>^
PermissionCheckResult::DenyReasons::get()
{
    return m_denyReasons;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END