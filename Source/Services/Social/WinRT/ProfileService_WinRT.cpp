// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ProfileService_winrt.h"
#include "Utils_WinRT.h"

using namespace pplx;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace Platform::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::social;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

ProfileService::ProfileService(
    _In_ profile_service cppObj
    ):
    m_cppObj(std::move(cppObj))
{
}

IAsyncOperation<XboxUserProfile^>^
ProfileService::GetUserProfileAsync(
    _In_ String^ xboxUserId
    )
{
    auto task = m_cppObj.get_user_profile(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId)
        )
    .then([](xbox::services::xbox_live_result<xbox_user_profile> cppUserProfile)
    {
        THROW_HR_IF_ERR(cppUserProfile.err());
        return ref new XboxUserProfile(cppUserProfile.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<XboxUserProfile^>^>^
ProfileService::GetUserProfilesAsync(
    _In_ IVectorView<String^>^ xboxUserIds
    )
{
    std::vector<string_t> vecXboxUserIds = UtilsWinRT::CovertVectorViewToStdVectorString(xboxUserIds);

    auto task = m_cppObj.get_user_profiles(vecXboxUserIds)
    .then([](xbox::services::xbox_live_result<std::vector<xbox_user_profile>> cppUserProfiles)
    {
        THROW_HR_IF_ERR(cppUserProfiles.err());
        Vector<XboxUserProfile^>^ responseVector = ref new Vector<XboxUserProfile^>();
        const auto& result = cppUserProfiles.payload();
        for (auto& cppUserProfile : result)
        {
            auto userProfile = ref new XboxUserProfile(cppUserProfile);
            responseVector->Append(userProfile);
        }
        return responseVector->GetView(); 
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<XboxUserProfile^>^>^
ProfileService::GetUserProfilesForSocialGroupAsync(
    _In_ Platform::String^ socialGroup
    )
{
    auto task = m_cppObj.get_user_profiles_for_social_group(STRING_T_FROM_PLATFORM_STRING(socialGroup))
    .then([](xbox_live_result<std::vector<xbox_user_profile>> cppUserProfileList)
    {
        THROW_IF_ERR(cppUserProfileList);
        Vector<XboxUserProfile^>^ responseVector = ref new Vector<XboxUserProfile^>();
        for (auto& cppUserProfile : cppUserProfileList.payload())
        {
            XboxUserProfile^ userProfile = ref new XboxUserProfile(cppUserProfile);
            responseVector->Append(userProfile);
        }
        return responseVector->GetView(); 
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END