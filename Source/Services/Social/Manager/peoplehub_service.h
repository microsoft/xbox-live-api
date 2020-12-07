// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/social_manager_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

class PeoplehubService
{
public:
    PeoplehubService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> httpCallSettings,
        _In_ uint32_t titleId
    ) noexcept;

    HRESULT GetSocialGraph(
        _In_ uint64_t xuid,
        _In_ XblSocialManagerExtraDetailLevel decorations,
        _In_ AsyncContext<Result<Vector<XblSocialManagerUser>>> async
    ) const noexcept;

    HRESULT GetSocialUsers(
        _In_ uint64_t xuid,
        _In_ XblSocialManagerExtraDetailLevel decorations,
        _In_ const Vector<uint64_t>& xuids,
        _In_ AsyncContext<Result<Vector<XblSocialManagerUser>>> async
    ) const noexcept;

private:
    enum class RelationshipType
    {
        Social,
        Batch
    };

    HRESULT MakeServiceCall(
        _In_ uint64_t xuid,
        _In_ XblSocialManagerExtraDetailLevel decorations,
        _In_ RelationshipType relationshipType,
        _In_opt_ const Vector<uint64_t>& batchUsers,
        _In_ AsyncContext<Result<Vector<XblSocialManagerUser>>> async
    ) const noexcept;

    static Result<XblSocialManagerUser> DeserializeUser(const JsonValue& json);
    static Result<XblSocialManagerPresenceRecord> DeserializePresenceRecord(const JsonValue& json);
    static Result<XblSocialManagerPresenceTitleRecord> DeserializePresenceTitleRecord(const JsonValue& json);
    static Result<XblPreferredColor> DeserializePreferredColor(const JsonValue& json);
    static Result<XblTitleHistory> DeserializeTitleHistory(const JsonValue& json);

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_httpSettings;
    uint32_t const m_titleId;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END