// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "pch.h"
#include "xsapi/achievements_c.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::achievements;

struct XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL
{
public:
    XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL(
        _In_ achievement_title_association cppAssociation,
        _In_ XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION* cAssociation
    );

private:
    std::string m_name;

    achievement_title_association m_cppAssociation;
    XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION* m_cAssociation;
};

struct XSAPI_ACHIEVEMENT_REQUIREMENT_IMPL
{
public:
    XSAPI_ACHIEVEMENT_REQUIREMENT_IMPL(
        _In_ achievement_requirement cppRequirement,
        _In_ XSAPI_ACHIEVEMENT_REQUIREMENT* cRequirement
    );

private:
    std::string m_id;
    std::string m_currentProgressValue;
    std::string m_targetProgressValue;

    achievement_requirement m_cppRequirement;
    XSAPI_ACHIEVEMENT_REQUIREMENT* m_cRequirement;
};

struct XSAPI_ACHIEVEMENT_PROGRESSION_IMPL
{
public:
    XSAPI_ACHIEVEMENT_PROGRESSION_IMPL(
        _In_ achievement_progression cppProgression,
        _In_ XSAPI_ACHIEVEMENT_PROGRESSION* cProgression
    );

private:
    std::vector<XSAPI_ACHIEVEMENT_REQUIREMENT*> m_requirements;

    achievement_progression m_cppProgression;
    XSAPI_ACHIEVEMENT_PROGRESSION* m_cProgression;
};

struct XSAPI_ACHIEVEMENT_MEDIA_ASSET_IMPL
{
public:
    XSAPI_ACHIEVEMENT_MEDIA_ASSET_IMPL(
        _In_ achievement_media_asset cppMediaAsset,
        _In_ XSAPI_ACHIEVEMENT_MEDIA_ASSET* cMediaAsset
    );

private:
    std::string m_name;
    string_t m_uri;
    std::string m_url;

    achievement_media_asset m_cppMediaAsset;
    XSAPI_ACHIEVEMENT_MEDIA_ASSET* m_cMediaAsset;
};

struct XSAPI_ACHIEVEMENT_REWARD_IMPL
{
public:
    XSAPI_ACHIEVEMENT_REWARD_IMPL(
        _In_ achievement_reward cppReward,
        _In_ XSAPI_ACHIEVEMENT_REWARD* cReward
    );

private:
    std::string m_name;
    std::string m_description;
    std::string m_value;
    std::string m_valueType;
    XSAPI_ACHIEVEMENT_MEDIA_ASSET* m_mediaAsset;

    achievement_reward m_cppReward;
    XSAPI_ACHIEVEMENT_REWARD* m_cReward;
};

struct XSAPI_ACHIEVEMENT_IMPL
{
public:
    XSAPI_ACHIEVEMENT_IMPL(
        _In_ achievement cppAchievement,
        _In_ XSAPI_ACHIEVEMENT* cAchievement
    );

private:
    std::string m_id;
    std::string m_serviceConfigurationId;
    std::string m_name;
    std::vector<XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION*> m_titleAssociations;
    std::vector<XSAPI_ACHIEVEMENT_MEDIA_ASSET*> m_mediaAssets;
    std::vector<std::string> m_platformsAvailableOn;
    std::string m_unlockedDescription;
    std::string m_lockedDescription;
    std::string m_productId;
    XSAPI_ACHIEVEMENT_TIME_WINDOW* m_available;
    std::vector<XSAPI_ACHIEVEMENT_REWARD*> m_rewards;
    std::string m_deepLink;

    achievement m_cppAchievement;
    XSAPI_ACHIEVEMENT* m_cAchievement;
};

struct XSAPI_ACHIEVEMENTS_RESULT_IMPL
{
public:
    XSAPI_ACHIEVEMENTS_RESULT_IMPL(
        _In_ achievements_result cppResult,
        _In_ XSAPI_ACHIEVEMENTS_RESULT* cResult
    );

    achievements_result cppAchievementsResult() const;

private:
    std::vector<XSAPI_ACHIEVEMENT*> m_items;

    achievements_result m_cppResult;
    XSAPI_ACHIEVEMENTS_RESULT* m_cResult;
};


XSAPI_ACHIEVEMENT* CreateAchievementFromCpp(
    _In_ achievement cppAchievement
);

XSAPI_ACHIEVEMENTS_RESULT* CreateAchievementsResultFromCpp(
    _In_ achievements_result cppResult
);