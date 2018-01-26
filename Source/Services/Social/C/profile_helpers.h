// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

class xbl_xbox_user_profiles_wrapper
{
public:
    xbl_xbox_user_profiles_wrapper(const xsapi_internal_vector<xbox::services::social::xbox_user_profile>& profiles);
    xbl_xbox_user_profiles_wrapper(const xbox::services::social::xbox_user_profile& profile);
    ~xbl_xbox_user_profiles_wrapper();

    const XBL_XBOX_USER_PROFILE *xbl_xbox_user_profile() const;
    uint32_t profiles_count() const;

private:
    xsapi_internal_vector<XBL_XBOX_USER_PROFILE> m_profiles;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END