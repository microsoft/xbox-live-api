// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/achievements_c.h"
#include "xsapi/achievements.h"
#include "Achievements\achievements_internal.h"
#include "achievements_helper.h"
#include "achievements_state.h"

using namespace xbox::services::achievements;

std::shared_ptr<xsapi_singleton> get_singleton_for_achievements()
{
    auto singleton = get_xsapi_singleton();

    if (singleton->m_achievementsState == nullptr)
    {
        singleton->m_achievementsState = std::make_shared<XBL_ACHIEVEMENTS_STATE>();
    }
    return singleton;
}

XBL_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL::XBL_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL(
    _In_ std::shared_ptr<achievement_title_association_internal> cppAssociation,
    _In_ XBL_ACHIEVEMENT_TITLE_ASSOCIATION* cAssociation
) : m_cppAssociation(cppAssociation), m_cAssociation(cAssociation)
{
    m_cAssociation->name = m_cppAssociation->name().c_str();

    m_cAssociation->titleId = m_cppAssociation->title_id();
}

XBL_ACHIEVEMENT_REQUIREMENT_IMPL::XBL_ACHIEVEMENT_REQUIREMENT_IMPL(
    _In_ std::shared_ptr<achievement_requirement_internal> cppRequirement,
    _In_ XBL_ACHIEVEMENT_REQUIREMENT* cRequirement
) : m_cppRequirement(cppRequirement), m_cRequirement(cRequirement)
{
    m_cRequirement->id = m_cppRequirement->id().c_str();

    m_cRequirement->currentProgressValue = m_cppRequirement->current_progress_value().c_str();

    m_cRequirement->targetProgressValue = m_cppRequirement->target_progress_value().c_str();
}

XBL_ACHIEVEMENT_PROGRESSION_IMPL::XBL_ACHIEVEMENT_PROGRESSION_IMPL(
    _In_ std::shared_ptr<achievement_progression_internal> cppProgression,
    _In_ XBL_ACHIEVEMENT_PROGRESSION* cProgression
) : m_cppProgression(cppProgression), m_cProgression(cProgression)
{
    m_requirements = std::vector<XBL_ACHIEVEMENT_REQUIREMENT*>();
    for (auto cppRequirement : m_cppProgression->requirements()) {
        // TODO here and elsewhere in C code - is this cleaned up properly? comment to clarify
        auto cRequirement = new XBL_ACHIEVEMENT_REQUIREMENT();
        cRequirement->pImpl = new XBL_ACHIEVEMENT_REQUIREMENT_IMPL(cppRequirement, cRequirement);
        m_requirements.push_back(cRequirement);
    }
    m_cProgression->requirements = m_requirements.data();
    m_cProgression->requirementsCount = (uint32_t)m_requirements.size();

    m_cProgression->timeUnlocked = utils::time_t_from_datetime(m_cppProgression->time_unlocked());
}

XBL_ACHIEVEMENT_MEDIA_ASSET_IMPL::XBL_ACHIEVEMENT_MEDIA_ASSET_IMPL(
    _In_ std::shared_ptr<achievement_media_asset_internal> cppMediaAsset,
    _In_ XBL_ACHIEVEMENT_MEDIA_ASSET* cMediaAsset
) : m_cppMediaAsset(cppMediaAsset), m_cMediaAsset(cMediaAsset)
{
    m_cMediaAsset->name = m_cppMediaAsset->name().c_str();

    m_uri = m_cppMediaAsset->url().to_string();
    m_url = utils::internal_string_from_string_t(m_uri);
    m_cMediaAsset->url = m_url.c_str();

    m_cMediaAsset->mediaAssetType = static_cast<XBL_ACHIEVEMENT_MEDIA_ASSET_TYPE>(m_cppMediaAsset->media_asset_type());
}

XBL_ACHIEVEMENT_REWARD_IMPL::XBL_ACHIEVEMENT_REWARD_IMPL(
    _In_ std::shared_ptr<achievement_reward_internal> cppReward,
    _In_ XBL_ACHIEVEMENT_REWARD* cReward
) : m_cppReward(cppReward), m_cReward(cReward)
{
    m_cReward->name = m_cppReward->name().c_str();

    m_cReward->description = m_cppReward->description().c_str();

    m_cReward->value = m_cppReward->value().c_str();

    m_cReward->valueType = m_cppReward->value_type().c_str();
    
    m_mediaAsset = nullptr;
    if (m_cppReward->media_asset_internal())
    {
        m_mediaAsset = new XBL_ACHIEVEMENT_MEDIA_ASSET();
        m_mediaAsset->pImpl = new XBL_ACHIEVEMENT_MEDIA_ASSET_IMPL(m_cppReward->media_asset_internal(), m_mediaAsset);
    }
    m_cReward->mediaAsset = m_mediaAsset;

    m_cReward->rewardType = static_cast<XBL_ACHIEVEMENT_REWARD_TYPE>(m_cppReward->reward_type());
}

XBL_ACHIEVEMENT_IMPL::XBL_ACHIEVEMENT_IMPL(
    _In_ std::shared_ptr<achievement_internal> cppAchievement,
    _In_ XBL_ACHIEVEMENT* cAchievement
) : m_cppAchievement(cppAchievement), m_cAchievement(cAchievement)
{
    m_cAchievement->id = m_cppAchievement->id().c_str();

    m_cAchievement->serviceConfigurationId = m_cppAchievement->service_configuration_id().c_str();

    m_cAchievement->name = m_cppAchievement->name().c_str();

    for (auto cppTitleAssociation : m_cppAchievement->title_associations())
    {
        auto titleAssociation = new XBL_ACHIEVEMENT_TITLE_ASSOCIATION();
        titleAssociation->pImpl = new XBL_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL(cppTitleAssociation, titleAssociation);
        m_titleAssociations.push_back(titleAssociation);
    }
    m_cAchievement->titleAssociations = m_titleAssociations.data();
    m_cAchievement->titleAssociationsCount = (uint32_t)m_titleAssociations.size();

    m_cAchievement->progressState = static_cast<XBL_ACHIEVEMENT_PROGRESS_STATE>(m_cppAchievement->progress_state());
    
    m_progression = new XBL_ACHIEVEMENT_PROGRESSION();
    m_progression->pImpl = new XBL_ACHIEVEMENT_PROGRESSION_IMPL(m_cppAchievement->progression_internal(), m_progression);
    m_cAchievement->progression = m_progression;

    for (auto cppMediaAsset : m_cppAchievement->media_assets())
    {
        auto mediaAsset = new XBL_ACHIEVEMENT_MEDIA_ASSET();
        mediaAsset->pImpl = new XBL_ACHIEVEMENT_MEDIA_ASSET_IMPL(cppMediaAsset, mediaAsset);
        m_mediaAssets.push_back(mediaAsset);
    }
    m_cAchievement->mediaAssets = m_mediaAssets.data();
    m_cAchievement->mediaAssetsCount = (uint32_t)m_mediaAssets.size();

    for (size_t i = 0; i < m_cppAchievement->platforms_available_on().size(); i++)
    {
        m_cPlatforms.push_back(m_cppAchievement->platforms_available_on()[i].c_str());
    }
    m_cAchievement->platformsAvailableOn = m_cPlatforms.data();
    m_cAchievement->platformsAvailableOnCount = (uint32_t)m_cPlatforms.size();

    m_cAchievement->isSecret = m_cppAchievement->is_secret();

    m_cAchievement->unlockedDescription = m_cppAchievement->unlocked_description().c_str();

    m_cAchievement->lockedDescription = m_cppAchievement->locked_description().c_str();

    m_cAchievement->productId = m_cppAchievement->product_id().c_str();

    m_cAchievement->type = static_cast<XBL_ACHIEVEMENT_TYPE>(m_cppAchievement->type());

    m_cAchievement->participationType = static_cast<XBL_ACHIEVEMENT_PARTICIPATION_TYPE>(m_cppAchievement->participation_type());

    m_available = new XBL_ACHIEVEMENT_TIME_WINDOW();
    m_available->startDate = utils::time_t_from_datetime(m_cppAchievement->available().start_date());
    m_available->endDate = utils::time_t_from_datetime(m_cppAchievement->available().end_date());
    m_cAchievement->available = m_available;
    
    for (auto cppReward : m_cppAchievement->rewards())
    {
        auto reward = new XBL_ACHIEVEMENT_REWARD();
        reward->pImpl = new XBL_ACHIEVEMENT_REWARD_IMPL(cppReward, reward);
        m_rewards.push_back(reward);
    }
    m_cAchievement->rewards = m_rewards.data();
    m_cAchievement->rewardsCount = (uint32_t)m_rewards.size();

    m_cAchievement->estimatedUnlockTime = m_cppAchievement->estimated_unlock_time().count();

    m_cAchievement->deepLink = m_cppAchievement->deep_link().c_str();

    m_cAchievement->isRevoked = m_cppAchievement->is_revoked();
}

XBL_ACHIEVEMENTS_RESULT_IMPL::XBL_ACHIEVEMENTS_RESULT_IMPL(
    _In_ std::shared_ptr<achievements_result_internal> cppResult,
    _In_ XBL_ACHIEVEMENTS_RESULT* cResult
) : m_cppResult(cppResult), m_cResult(cResult)
{
    for (auto achievement : m_cppResult->items())
    {
        m_items.push_back(CreateAchievementFromCpp(achievement));
    }

    m_cResult->items = m_items.data();
    m_cResult->itemsCount = (uint32_t)m_items.size();
}
std::shared_ptr<achievements_result_internal> XBL_ACHIEVEMENTS_RESULT_IMPL::cppAchievementsResult() const
{
    return m_cppResult;
}

XBL_ACHIEVEMENT* CreateAchievementFromCpp(
    _In_ std::shared_ptr<achievement_internal> cppAchievement
)
{
    auto singleton = get_singleton_for_achievements();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_achievementsState->m_lock);

    auto achievement = new XBL_ACHIEVEMENT();
    achievement->pImpl = new XBL_ACHIEVEMENT_IMPL(cppAchievement, achievement);

    singleton->m_achievementsState->m_achievements.insert(achievement);

    return achievement;
}

XBL_ACHIEVEMENTS_RESULT* CreateAchievementsResultFromCpp(
    _In_ std::shared_ptr<achievements_result_internal> cppResult
)
{
    auto singleton = get_singleton_for_achievements();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_achievementsState->m_lock);

    auto result = new XBL_ACHIEVEMENTS_RESULT();
    result->pImpl = new XBL_ACHIEVEMENTS_RESULT_IMPL(cppResult, result);

    singleton->m_achievementsState->m_achievementResults.insert(result);
    return result;
}