// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/profile_c.h"
#include "xsapi/profile.h"
#include "profile_helpers.h"
#include "profile_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbl_xbox_user_profiles_wrapper::xbl_xbox_user_profiles_wrapper(
    const xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>& profiles
    )
{
    for (auto& profile : profiles)
    {
        XblUserProfile cProfile;
        cProfile.appDisplayName = profile->app_display_name().data();
        cProfile.appDisplayPictureResizeUri = utils::alloc_string(profile->app_display_picture_resize_uri().to_string());  // TODO change after we have new URI class
        cProfile.gameDisplayName = profile->game_display_name().data();
        cProfile.gameDisplayPictureResizeUri = utils::alloc_string(profile->game_display_picture_resize_uri().to_string());
        cProfile.gamerscore = profile->gamerscore().data();
        cProfile.gamertag = profile->gamertag().data();
        cProfile.xboxUserId = profile->xbox_user_id().data();
        m_profiles.push_back(cProfile);
    }
}

xbl_xbox_user_profiles_wrapper::xbl_xbox_user_profiles_wrapper(
    std::shared_ptr<xbox_user_profile_internal> profile
    ) 
    : xbl_xbox_user_profiles_wrapper(xsapi_internal_vector<std::shared_ptr<xbox_user_profile_internal>>(1, profile))
{
}

xbl_xbox_user_profiles_wrapper::~xbl_xbox_user_profiles_wrapper()
{
    for (auto& p : m_profiles)
    {
        utils::free_string(p.appDisplayPictureResizeUri);
        utils::free_string(p.gameDisplayPictureResizeUri);
    }
}

const XblUserProfile *xbl_xbox_user_profiles_wrapper::xbl_xbox_user_profile() const
{
    return &m_profiles[0];
}

uint32_t xbl_xbox_user_profiles_wrapper::profiles_count() const
{
    return static_cast<uint32_t>(m_profiles.size());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END