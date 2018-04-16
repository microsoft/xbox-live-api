// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::achievements;

struct XBL_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL
{
public:
    XBL_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL(
        _In_ std::shared_ptr<achievement_title_association_internal> cppAssociation,
        _In_ XBL_ACHIEVEMENT_TITLE_ASSOCIATION* cAssociation
    );

private:
    std::shared_ptr<achievement_title_association_internal> m_cppAssociation;
    XBL_ACHIEVEMENT_TITLE_ASSOCIATION* m_cAssociation;
};

struct XBL_ACHIEVEMENT_REQUIREMENT_IMPL
{
public:
    XBL_ACHIEVEMENT_REQUIREMENT_IMPL(
        _In_ std::shared_ptr<achievement_requirement_internal> cppRequirement,
        _In_ XBL_ACHIEVEMENT_REQUIREMENT* cRequirement
    );

private:
    std::shared_ptr<achievement_requirement_internal> m_cppRequirement;
    XBL_ACHIEVEMENT_REQUIREMENT* m_cRequirement;
};

struct XBL_ACHIEVEMENT_PROGRESSION_IMPL
{
public:
    XBL_ACHIEVEMENT_PROGRESSION_IMPL(
        _In_ std::shared_ptr<achievement_progression_internal> cppProgression,
        _In_ XBL_ACHIEVEMENT_PROGRESSION* cProgression
    );

private:
    std::vector<XBL_ACHIEVEMENT_REQUIREMENT*> m_requirements;

    std::shared_ptr<achievement_progression_internal> m_cppProgression;
    XBL_ACHIEVEMENT_PROGRESSION* m_cProgression;
};

struct XBL_ACHIEVEMENT_MEDIA_ASSET_IMPL
{
public:
    XBL_ACHIEVEMENT_MEDIA_ASSET_IMPL(
        _In_ std::shared_ptr<achievement_media_asset_internal> cppMediaAsset,
        _In_ XBL_ACHIEVEMENT_MEDIA_ASSET* cMediaAsset
    );

private:
    string_t m_uri;
    xsapi_internal_string m_url;

    std::shared_ptr<achievement_media_asset_internal> m_cppMediaAsset;
    XBL_ACHIEVEMENT_MEDIA_ASSET* m_cMediaAsset;
};

struct XBL_ACHIEVEMENT_REWARD_IMPL
{
public:
    XBL_ACHIEVEMENT_REWARD_IMPL(
        _In_ std::shared_ptr<achievement_reward_internal> cppReward,
        _In_ XBL_ACHIEVEMENT_REWARD* cReward
    );

private:
    XBL_ACHIEVEMENT_MEDIA_ASSET* m_mediaAsset;

    std::shared_ptr<achievement_reward_internal> m_cppReward;
    XBL_ACHIEVEMENT_REWARD* m_cReward;
};

struct XBL_ACHIEVEMENT_IMPL
{
public:
    XBL_ACHIEVEMENT_IMPL(
        _In_ std::shared_ptr<achievement_internal> cppAchievement,
        _In_ XBL_ACHIEVEMENT* cAchievement
    );

private:
    std::vector<XBL_ACHIEVEMENT_TITLE_ASSOCIATION*> m_titleAssociations;
    XBL_ACHIEVEMENT_PROGRESSION* m_progression;
    std::vector<XBL_ACHIEVEMENT_MEDIA_ASSET*> m_mediaAssets;
    std::vector<PCSTR> m_cPlatforms;
    XBL_ACHIEVEMENT_TIME_WINDOW* m_available;
    std::vector<XBL_ACHIEVEMENT_REWARD*> m_rewards;

    std::shared_ptr<achievement_internal> m_cppAchievement;
    XBL_ACHIEVEMENT* m_cAchievement;
};

struct XBL_ACHIEVEMENTS_RESULT_IMPL
{
public:
    XBL_ACHIEVEMENTS_RESULT_IMPL(
        _In_ std::shared_ptr<achievements_result_internal> cppResult,
        _In_ XBL_ACHIEVEMENTS_RESULT* cResult
    );

    std::shared_ptr<achievements_result_internal> cppAchievementsResult() const;

private:
    std::vector<XBL_ACHIEVEMENT*> m_items;

    std::shared_ptr<achievements_result_internal> m_cppResult;
    XBL_ACHIEVEMENTS_RESULT* m_cResult;
};


XBL_ACHIEVEMENT* CreateAchievementFromCpp(
    _In_ std::shared_ptr<achievement_internal> cppAchievement
);

XBL_ACHIEVEMENTS_RESULT* CreateAchievementsResultFromCpp(
    _In_ std::shared_ptr<achievements_result_internal> cppResult
);