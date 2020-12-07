// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbox_user_profile::xbox_user_profile(
    const XblUserProfile& profile
) :
    m_profile{ profile }
{
}

string_t xbox_user_profile::app_display_name() const
{
    return Utils::StringTFromUtf8(m_profile.appDisplayName);
}

web::uri xbox_user_profile::app_display_picture_resize_uri() const
{
    return web::uri{ Utils::StringTFromUtf8(m_profile.appDisplayPictureResizeUri) };
}

string_t xbox_user_profile::game_display_name() const
{
    return Utils::StringTFromUtf8(m_profile.gameDisplayName);
}

web::uri xbox_user_profile::game_display_picture_resize_uri() const
{
    return web::uri{ Utils::StringTFromUtf8(m_profile.gameDisplayPictureResizeUri) };
}

string_t xbox_user_profile::gamerscore() const
{
    return Utils::StringTFromUtf8(m_profile.gamerscore);
}

string_t xbox_user_profile::gamertag() const
{
    return Utils::StringTFromUtf8(m_profile.gamertag);
}

string_t xbox_user_profile::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_profile.xboxUserId);
}

profile_service::profile_service(XblContextHandle xblContextHandle)
{
    XblContextDuplicateHandle(xblContextHandle, &m_xblContextHandle);
}

profile_service::profile_service(const profile_service& other)
{
    XblContextDuplicateHandle(other.m_xblContextHandle, &m_xblContextHandle);
}

profile_service& profile_service::operator=(profile_service other)
{
    std::swap(m_xblContextHandle, other.m_xblContextHandle);
    return *this;
}

profile_service::~profile_service()
{
    XblContextCloseHandle(m_xblContextHandle);
}

pplx::task<xbox::services::xbox_live_result<xbox_user_profile>> profile_service::get_user_profile(
    _In_ string_t xboxUserId
)
{
    auto asyncWrapper = new AsyncWrapper<xbox_user_profile>{
        [](XAsyncBlock* async, xbox_user_profile& result)
    {
        XblUserProfile profile{};
        auto hr = XblProfileGetUserProfileResult(async, &profile);
        if (SUCCEEDED(hr))
        {
            result = xbox_user_profile{ profile };
        }
        return hr;
    }
    };

    auto hr = XblProfileGetUserProfileAsync(m_xblContextHandle, Utils::Uint64FromStringT(std::move(xboxUserId)), &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox::services::xbox_live_result<std::vector<xbox_user_profile>>> profile_service::get_user_profiles(
    _In_ const std::vector<string_t>& xboxUserIds
)
{
    auto asyncWrapper = new AsyncWrapper<std::vector<xbox_user_profile>>{
        [](XAsyncBlock* async, std::vector<xbox_user_profile>& result)
    {
        size_t resultCount{ 0 };
        auto hr = XblProfileGetUserProfilesResultCount(async, &resultCount);
        if (SUCCEEDED(hr))
        {
            auto profiles = new XblUserProfile[resultCount];
            hr = XblProfileGetUserProfilesResult(async, resultCount, profiles);
            if (SUCCEEDED(hr))
            {
                for (size_t i = 0; i < resultCount; ++i)
                {
                    result.push_back(profiles[i]);
                }
            }
            delete[] profiles;
        }
        return hr;
    }
    };

    auto xuids{ Utils::Transform<uint64_t>(xboxUserIds, Utils::Uint64FromStringT) };

    auto hr = XblProfileGetUserProfilesAsync(m_xblContextHandle, xuids.data(), xuids.size(), &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox::services::xbox_live_result< std::vector< xbox_user_profile>>> profile_service::get_user_profiles_for_social_group(
    _In_ const string_t& socialGroup
)
{
    auto asyncWrapper = new AsyncWrapper<std::vector<xbox_user_profile>>{
        [](XAsyncBlock* async, std::vector<xbox_user_profile>& result)
    {
        size_t resultCount{ 0 };
        auto hr = XblProfileGetUserProfilesForSocialGroupResultCount(async, &resultCount);
        if (SUCCEEDED(hr))
        {
            auto profiles = new XblUserProfile[resultCount];
            hr = XblProfileGetUserProfilesForSocialGroupResult(async, resultCount, profiles);
            if (SUCCEEDED(hr))
            {
                for (size_t i = 0; i < resultCount; ++i)
                {
                    result.push_back(profiles[i]);
                }
            }
            delete[] profiles;
        }
        return hr;
    }
    };

    auto hr = XblProfileGetUserProfilesForSocialGroupAsync(m_xblContextHandle, Utils::StringFromStringT(socialGroup).data(), &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END