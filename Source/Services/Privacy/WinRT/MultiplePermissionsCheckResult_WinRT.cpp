// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplePermissionsCheckResult_WinRT.h"

using namespace Platform;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Privacy;
using namespace Windows::Foundation::Collections;
using namespace xbox::services::privacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN

MultiplePermissionsCheckResult::MultiplePermissionsCheckResult(
    _In_ xbox::services::privacy::multiple_permissions_check_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_items = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<PermissionCheckResult, permission_check_result>(m_cppObj.items())->GetView();
}

IVectorView<PermissionCheckResult^>^
MultiplePermissionsCheckResult::Items::get()
{
    return m_items;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END