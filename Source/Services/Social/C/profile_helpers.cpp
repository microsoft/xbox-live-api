// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/profile_c.h"
#include "xsapi/profile.h"
#include "profile_helpers.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

xbl_xbox_user_profiles_wrapper::xbl_xbox_user_profiles_wrapper(
    const xsapi_internal_vector<xbox_user_profile>& profiles
    )
{
    for (auto& profile : profiles)
    {
        XBL_XBOX_USER_PROFILE cProfile;
        cProfile.appDisplayName = utils::alloc_string(profile.app_display_name());
        cProfile.appDisplayPictureResizeUri = utils::alloc_string(profile.app_display_picture_resize_uri().to_string());
        cProfile.gameDisplayName = utils::alloc_string(profile.game_display_name());
        cProfile.gameDisplayPictureResizeUri = utils::alloc_string(profile.game_display_picture_resize_uri().to_string());
        cProfile.gamerscore = utils::alloc_string(profile.gamerscore());
        cProfile.gamertag = utils::alloc_string(profile.gamertag());
        cProfile.xboxUserId = utils::alloc_string(profile.xbox_user_id());
        m_profiles.push_back(cProfile);
    }
}

xbl_xbox_user_profiles_wrapper::xbl_xbox_user_profiles_wrapper(
    const xbox_user_profile& profile
    ) 
    : xbl_xbox_user_profiles_wrapper(xsapi_internal_vector<xbox_user_profile>(1, profile))
{
}

xbl_xbox_user_profiles_wrapper::~xbl_xbox_user_profiles_wrapper()
{
    for (auto& p : m_profiles)
    {
        utils::free_string(p.appDisplayName);
        utils::free_string(p.appDisplayPictureResizeUri);
        utils::free_string(p.gameDisplayName);
        utils::free_string(p.gameDisplayPictureResizeUri);
        utils::free_string(p.gamertag);
        utils::free_string(p.gamerscore);
        utils::free_string(p.xboxUserId);
    }
}

const XBL_XBOX_USER_PROFILE *xbl_xbox_user_profiles_wrapper::xbl_xbox_user_profile() const
{
    return &m_profiles[0];
}

uint32_t xbl_xbox_user_profiles_wrapper::profiles_count() const
{
    return static_cast<uint32_t>(m_profiles.size());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END