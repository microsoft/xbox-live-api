// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

class xbl_xbox_user_profiles_wrapper
{
public:
    xbl_xbox_user_profiles_wrapper(const xsapi_internal_vector<std::shared_ptr<xbox::services::social::xbox_user_profile_internal>>& profiles);
    xbl_xbox_user_profiles_wrapper(std::shared_ptr<xbox::services::social::xbox_user_profile_internal> profile);
    ~xbl_xbox_user_profiles_wrapper();

    const XblUserProfile *xbl_xbox_user_profile() const;
    uint32_t profiles_count() const;

private:
    xsapi_internal_vector<XblUserProfile> m_profiles;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END