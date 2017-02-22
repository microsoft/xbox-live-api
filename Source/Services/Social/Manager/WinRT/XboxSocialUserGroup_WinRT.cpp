// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_context.h"
#include "XboxSocialUserGroup_WinRT.h"
#include "Utils_WinRT.h"
#include "user_context.h"

using namespace xbox::services::social::manager;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

XboxSocialUserGroup::XboxSocialUserGroup(
    _In_ std::shared_ptr<xbox_social_user_group> cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_user = xbox::services::user_context::user_convert(m_cppObj->local_user());
}

std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>
XboxSocialUserGroup::GetCppObj() const
{
    return m_cppObj;
}

Windows::Foundation::Collections::IVectorView<XboxSocialUser^>^
XboxSocialUserGroup::Users::get()
{
    auto currentPageResult = m_cppObj->users();
    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<XboxSocialUser>(currentPageResult)->GetView();
}

XboxLiveUser_t
XboxSocialUserGroup::LocalUser::get()
{
    return m_user;
};

Windows::Foundation::Collections::IVectorView<Platform::String^>^
XboxSocialUserGroup::UsersTrackedBySocialUserGroup::get()
{
    auto trackedUsers = m_cppObj->users_tracked_by_social_user_group();
    auto vector = ref new Platform::Collections::Vector<Platform::String^>();
    for (auto& user : trackedUsers)
    {
        vector->Append(ref new Platform::String(user.xbox_user_id()));
    }

    return vector->GetView();
};

Windows::Foundation::Collections::IVectorView<XboxSocialUser^>^
XboxSocialUserGroup::GetUsersFromXboxUserIds(
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds
    )
{
    std::vector<xbox_user_id_container> xboxUserIdContainerVec;
    xboxUserIdContainerVec.reserve(xboxUserIds->Size);
    for (auto str : xboxUserIds)
    {
        xboxUserIdContainerVec.push_back(str->Data());
    }

    auto users = m_cppObj->get_users_from_xbox_user_ids(xboxUserIdContainerVec);
    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<XboxSocialUser>(users)->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END