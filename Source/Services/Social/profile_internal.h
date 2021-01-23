// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/profile_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

class ProfileService : public std::enable_shared_from_this<ProfileService>
{
public:
    ProfileService(
        _In_ User&& user,
        _In_ std::shared_ptr<XboxLiveContextSettings> xboxLiveContextSettings,
        _In_ std::shared_ptr<AppConfig> appConfig
    ) noexcept;

    HRESULT GetUserProfiles(
        _In_ xsapi_internal_vector<uint64_t>&& xuids,
        _In_ AsyncContext<Result<xsapi_internal_vector<XblUserProfile>>> async
    ) const noexcept;

    HRESULT GetUserProfilesForSocialGroup(
        _In_ xsapi_internal_string&& socialGroup,
        _In_ AsyncContext<Result<xsapi_internal_vector<XblUserProfile>>> async
    ) const noexcept;

private:
    static Result<xsapi_internal_vector<XblUserProfile>> DeserializeUserProfiles(
        _In_ const JsonValue& json
    ) noexcept;

    static Result<XblUserProfile> DeserializeUserProfile(
        _In_ const JsonValue& json
    ) noexcept;

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::AppConfig> m_appConfig;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END