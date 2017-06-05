// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SocialManager_WinRT.h"
#include "user_context.h"
#if UNIT_TEST_SERVICES
#include "MockSocialManager.h"
#endif

using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::social::manager;
using namespace xbox::services;
using namespace Windows::Foundation::Collections;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

SocialManager^
SocialManager::SingletonInstance::get()
{
    auto xsapiSingleton = get_xsapi_singleton();
    if (xsapiSingleton->m_winrt_socialManagerInstance == nullptr)
    {
        SocialManager^ socialManager = ref new SocialManager();

        {
            std::lock_guard<std::mutex> lock(xsapiSingleton->m_singletonLock);
            if (xsapiSingleton->m_winrt_socialManagerInstance == nullptr)
            {
                xsapiSingleton->m_winrt_socialManagerInstance = socialManager;
            }
        }
    }
    return xsapiSingleton->m_winrt_socialManagerInstance;
}

SocialManager::SocialManager() :
#if UNIT_TEST_SERVICES
    m_cppObj(xbox::services::social::manager::MockSocialManager::get_mock_singleton_instance()),
#else
    m_cppObj(xbox::services::social::manager::social_manager::get_singleton_instance()),
#endif
    m_localUserChanged(false),
    m_previousSize(0)
{
}

XboxSocialUserGroup^
SocialManager::CreateSocialUserGroupFromFilters(
    _In_ XboxLiveUser_t user,
    _In_ PresenceFilter presenceFilter,
    _In_ RelationshipFilter relationshipFilter
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto xboxSocialUserGroup = m_cppObj->create_social_user_group_from_filters(
        user_context::user_convert(user),
        static_cast<presence_filter>(presenceFilter),
        static_cast<relationship_filter>(relationshipFilter)
        );

    THROW_IF_ERR(xboxSocialUserGroup);
    return ref new XboxSocialUserGroup(xboxSocialUserGroup.payload());
}

XboxSocialUserGroup^
SocialManager::CreateSocialUserGroupFromList(
    _In_ XboxLiveUser_t user,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIdList
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto viewResult = m_cppObj->create_social_user_group_from_list(
        user_context::user_convert(user),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIdList)
        );

    THROW_IF_ERR(viewResult);
    return ref new XboxSocialUserGroup(viewResult.payload());
}

void
SocialManager::DestroySocialUserGroup(
    _In_ XboxSocialUserGroup^ xboxSocialUserGroup
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(xboxSocialUserGroup);
    auto viewResult = m_cppObj->destroy_social_user_group(
        xboxSocialUserGroup->GetCppObj()
        );

    THROW_IF_ERR(viewResult);
}

void
SocialManager::AddLocalUser(
    _In_ XboxLiveUser_t user,
    _In_ SocialManagerExtraDetailLevel extraDetailLevel
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto userResult = m_cppObj->add_local_user(
        user_context::user_convert(user),
        static_cast<social_manager_extra_detail_level>(extraDetailLevel)
        );

    THROW_IF_ERR(userResult);

    m_localUserChanged = true;
}

void SocialManager::RemoveLocalUser(
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto userResult = m_cppObj->remove_local_user(
        user_context::user_convert(user)
        );

    THROW_IF_ERR(userResult);

    m_localUserChanged = true;
}

Windows::Foundation::Collections::IVectorView<SocialEvent^>^
SocialManager::DoWork()
{
    auto workResult = m_cppObj->do_work();
    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<SocialEvent>(workResult)->GetView();
}

Windows::Foundation::Collections::IVectorView<XboxLiveUser_t>^
SocialManager::LocalUsers::get()
{
    auto localUsers = m_cppObj->local_users();
    auto localUserSize = localUsers.size();
    if (m_localUserChanged || localUserSize != m_previousSize)
    {
        auto platformVec = ref new Platform::Collections::Vector<XboxLiveUser_t>();
        for (auto& localUser : localUsers)
        {
            auto newUser = xbox::services::user_context::user_convert(localUser);
            platformVec->Append(newUser);
        }

        m_localUserVector = platformVec->GetView();
        m_localUserChanged = false;
        m_previousSize = localUserSize;
    }

    return m_localUserVector;
}

void SocialManager::UpdateSocialUserGroup(
    _In_ XboxSocialUserGroup^ socialGroup,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ users
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(socialGroup);

    auto result = m_cppObj->update_social_user_group(
        socialGroup->GetCppObj(),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(users)
        );

    THROW_IF_ERR(result);
}

void SocialManager::SetRichPresencePollingState(
    _In_ XboxLiveUser_t user,
    _In_ bool shouldEnablePolling
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    auto result = m_cppObj->set_rich_presence_polling_status(
        user_context::user_convert(user),
        shouldEnablePolling
        );

    THROW_IF_ERR(result);
}

void SocialManager::LogState()
{
    m_cppObj->_Log_state();
}

std::shared_ptr<xbox::services::social::manager::social_manager>
SocialManager::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END