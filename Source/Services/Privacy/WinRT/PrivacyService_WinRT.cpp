// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "PrivacyService_WinRT.h"

using namespace Concurrency;
using namespace xbox::services::privacy;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN

PrivacyService::PrivacyService(
    _In_ xbox::services::privacy::privacy_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

IAsyncOperation<IVectorView<String^>^>^
PrivacyService::GetAvoidListAsync()
{
    auto task = m_cppObj.get_avoid_or_mute_list(_T("avoid"))
    .then([](xbox_live_result<std::vector<string_t>> avoidList)
    {
        THROW_IF_ERR(avoidList);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorString(avoidList.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<String^>^>^ 
PrivacyService::GetMuteListAsync()
{
    auto task = m_cppObj.get_avoid_or_mute_list(_T("mute"))
    .then([](xbox_live_result<std::vector<string_t>> muteList)
    {
        THROW_IF_ERR(muteList);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorString(muteList.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<PermissionCheckResult^>^ 
PrivacyService::CheckPermissionWithTargetUserAsync(
    _In_ Platform::String^ permissionId,
    _In_ Platform::String^ targetXboxUserId
    )
{
    auto task = m_cppObj.check_permission_with_target_user(
        STRING_T_FROM_PLATFORM_STRING(permissionId),
        STRING_T_FROM_PLATFORM_STRING(targetXboxUserId)
        )
    .then([](xbox_live_result<permission_check_result> checkResult)
    {
        THROW_IF_ERR(checkResult)
        return ref new PermissionCheckResult(checkResult.payload());
    });
        
    return ASYNC_FROM_TASK(task);
}


IAsyncOperation<IVectorView<MultiplePermissionsCheckResult^>^>^ 
PrivacyService::CheckMultiplePermissionsWithMultipleTargetUsersAsync(
    _In_ IVectorView<String^>^ permissionIds,
    _In_ IVectorView<String^>^ targetXboxUserIds
    )
{
    auto task = m_cppObj.check_multiple_permissions_with_multiple_target_users(
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(permissionIds),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(targetXboxUserIds)
        )
    .then([](xbox_live_result<std::vector<multiple_permissions_check_result>> multiplePermissionsCheckResult)
    {
        THROW_IF_ERR(multiplePermissionsCheckResult);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplePermissionsCheckResult, multiple_permissions_check_result>(multiplePermissionsCheckResult.payload())->GetView();
    });
        
    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END