// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "achievements_internal.h"
#include "xsapi_utils.h"

using namespace xbox::services;
using namespace xbox::services::achievements;

XblAchievementsResult::~XblAchievementsResult()
{
    for (auto& achievement : m_achievements)
    {
        AchievementsService::CleanupAchievement(achievement);
    }
}

Result<std::shared_ptr<XblAchievementsResult>> XblAchievementsResult::Deserialize(
    _In_ const JsonDocument& json,
    _In_ std::shared_ptr<const xbox::services::achievements::AchievementsService> service)
{
    if (json.IsNull())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    auto achievementResult = MakeShared<XblAchievementsResult>(service);

    HRESULT errCode = S_OK;

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblAchievement>(
        AchievementsService::DeserializeAchievement,
        json, "achievements", achievementResult->m_achievements, true
        ));

    if (json.IsObject() && json.HasMember("pagingInfo"))
    {
        const JsonValue& pageInfoJson = json["pagingInfo"];
        if (!pageInfoJson.IsNull())
        {
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(pageInfoJson, "continuationToken", achievementResult->m_continuationToken, true));
        }
    }

    if (FAILED(errCode))
    {
        return Result<std::shared_ptr<XblAchievementsResult>>{ errCode };
    }
    return Result<std::shared_ptr<XblAchievementsResult>>{ achievementResult };
}

XblAchievementsResult::XblAchievementsResult(
    _In_ std::shared_ptr<const xbox::services::achievements::AchievementsService> service
) : 
    m_serviceWeakPointer{ service }
{
}

const xsapi_internal_vector<XblAchievement>& XblAchievementsResult::Achievements() const
{
    return m_achievements;
}

std::shared_ptr<xbox::services::RefCounter> XblAchievementsResult::GetSharedThis()
{
    return shared_from_this();
}

bool XblAchievementsResult::HasNext() const
{
    return !m_continuationToken.empty();
}

HRESULT XblAchievementsResult::GetNext(
    _In_ uint32_t maxItems,
    _In_ AsyncContext<xbox::services::Result<std::shared_ptr<XblAchievementsResult>>> async
) const noexcept
{
    auto service{ m_serviceWeakPointer.lock() };

    if (m_continuationToken.empty() || service == nullptr)
    {
        return E_UNEXPECTED;
    }

    return service->GetAchievements(
        m_xuid,
        m_titleIds,
        m_achievementType,
        m_unlockedOnly,
        m_orderBy,
        0, // use continuationToken, ignore skipItems.
        maxItems,
        m_continuationToken,
        std::move(async)
    );
}

void XblAchievementsResult::SetNextPageQueryParameters(
    _In_ uint64_t xuid,
    _In_ const xsapi_internal_vector<uint32_t>& titleIds,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy
)
{
    m_xuid = xuid;
    m_titleIds = titleIds;
    m_achievementType = type;
    m_unlockedOnly = unlockedOnly;
    m_orderBy = orderBy;
}