// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/social_manager_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

// Result of a PeopleHub batch call.
struct PeoplehubBatchResult
{
    Vector<XblSocialManagerUser> users;
    // Users whose relationship (isFriend) the service returned as missing, null, or invalid. Their relationship fields
    // in 'users' are defaults, not the service's answer, so callers that already know the relationship should keep it.
    UnorderedSet<uint64_t> unresolvedRelationshipXuids;
};

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
        _In_ AsyncContext<Result<PeoplehubBatchResult>> async
    ) const noexcept;

    // The configured HTTP timeout window (XblContextSettingsSetHttpTimeoutWindow) that bounds each HTTP
    // attempt made by this service. Used to size the followed-users watchdog relative to how long a
    // healthy call may legitimately take.
    uint32_t HttpTimeoutWindowInSeconds() const noexcept;

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
        _In_ AsyncContext<Result<PeoplehubBatchResult>> async
    ) const noexcept;

    static HRESULT DeserializeUser(
        const JsonValue& json,
        _In_ RelationshipType relationshipType,
        _Out_ XblSocialManagerUser& user,
        _Out_ bool& relationshipResolved
    );
    static HRESULT DeserializePresenceRecord(const JsonValue& json, _Out_ XblSocialManagerPresenceRecord& record);
    static HRESULT DeserializePresenceTitleRecord(const JsonValue& json, _Out_ XblSocialManagerPresenceTitleRecord& record);
    static Result<XblPreferredColor> DeserializePreferredColor(const JsonValue& json);
    static Result<XblTitleHistory> DeserializeTitleHistory(const JsonValue& json);

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_httpSettings;
    uint32_t const m_titleId;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END