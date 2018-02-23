// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/achievements_c.h"
#include "xsapi/achievements.h"
#include "achievements_helper.h"
#include "achievements_state.h"

using namespace xbox::services::achievements;

XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL::XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL(
    _In_ achievement_title_association cppAssociation,
    _In_ XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION* cAssociation
) : m_cppAssociation(cppAssociation), m_cAssociation(cAssociation)
{
    m_name = utils::utf8_from_utf16(m_cppAssociation.name());
    m_cAssociation->name = m_name.c_str();

    m_cAssociation->titleId = m_cppAssociation.title_id();
}

XSAPI_ACHIEVEMENT_REQUIREMENT_IMPL::XSAPI_ACHIEVEMENT_REQUIREMENT_IMPL(
    _In_ achievement_requirement cppRequirement,
    _In_ XSAPI_ACHIEVEMENT_REQUIREMENT* cRequirement
) : m_cppRequirement(cppRequirement), m_cRequirement(cRequirement)
{
    m_id = utils::utf8_from_utf16(m_cppRequirement.id());
    m_cRequirement->id = m_id.c_str();

    m_currentProgressValue = utils::utf8_from_utf16(m_cppRequirement.current_progress_value());
    m_cRequirement->currentProgressValue = m_currentProgressValue.c_str();

    m_targetProgressValue = utils::utf8_from_utf16(m_cppRequirement.target_progress_value());
    m_cRequirement->targetProgressValue = m_targetProgressValue.c_str();
}

XSAPI_ACHIEVEMENT_PROGRESSION_IMPL::XSAPI_ACHIEVEMENT_PROGRESSION_IMPL(
    _In_ achievement_progression cppProgression,
    _In_ XSAPI_ACHIEVEMENT_PROGRESSION* cProgression
) : m_cppProgression(cppProgression), m_cProgression(cProgression)
{
    m_requirements = std::vector<XSAPI_ACHIEVEMENT_REQUIREMENT*>();
    for (auto cppRequirement : m_cppProgression.requirements()) {
        // TODO here and elsewhere in C code - is this cleaned up properly? comment to clarify
        auto cRequirement = new XSAPI_ACHIEVEMENT_REQUIREMENT();
        cRequirement->pImpl = new XSAPI_ACHIEVEMENT_REQUIREMENT_IMPL(cppRequirement, cRequirement);
        m_requirements.push_back(cRequirement);
    }
    m_cProgression->requirements = m_requirements.data();
    m_cProgression->requirementsCount = (uint32_t)m_requirements.size();

    m_cProgression->timeUnlocked = utils::time_t_from_datetime(m_cppProgression.time_unlocked());
}

XSAPI_ACHIEVEMENT_MEDIA_ASSET_IMPL::XSAPI_ACHIEVEMENT_MEDIA_ASSET_IMPL(
    _In_ achievement_media_asset cppMediaAsset,
    _In_ XSAPI_ACHIEVEMENT_MEDIA_ASSET* cMediaAsset
) : m_cppMediaAsset(cppMediaAsset), m_cMediaAsset(cMediaAsset)
{
    m_name = utils::utf8_from_utf16(m_cppMediaAsset.name());
    m_cMediaAsset->name = m_name.c_str();

    m_uri = m_cppMediaAsset.url().to_string();
    m_url = utils::utf8_from_utf16(m_uri);
    m_cMediaAsset->name = m_url.c_str();

    m_cMediaAsset->mediaAssetType = static_cast<XSAPI_ACHIEVEMENT_MEDIA_ASSET_TYPE>(m_cppMediaAsset.media_asset_type());
}

XSAPI_ACHIEVEMENT_REWARD_IMPL::XSAPI_ACHIEVEMENT_REWARD_IMPL(
    _In_ achievement_reward cppReward,
    _In_ XSAPI_ACHIEVEMENT_REWARD* cReward
) : m_cppReward(cppReward), m_cReward(cReward)
{
    m_name = utils::utf8_from_utf16(m_cppReward.name());
    m_cReward->name = m_name.c_str();

    m_description = utils::utf8_from_utf16(m_cppReward.description());
    m_cReward->description = m_description.c_str();

    m_value = utils::utf8_from_utf16(m_cppReward.value());
    m_cReward->value = m_value.c_str();

    m_valueType = utils::utf8_from_utf16(m_cppReward.value_type());
    m_cReward->valueType = m_valueType.c_str();

    m_mediaAsset = new XSAPI_ACHIEVEMENT_MEDIA_ASSET();
    m_mediaAsset->pImpl = new XSAPI_ACHIEVEMENT_MEDIA_ASSET_IMPL(m_cppReward.media_asset(), m_mediaAsset);
    m_cReward->mediaAsset = m_mediaAsset;

    m_cReward->rewardType = static_cast<XSAPI_ACHIEVEMENT_REWARD_TYPE>(m_cppReward.reward_type());
}

XSAPI_ACHIEVEMENT_IMPL::XSAPI_ACHIEVEMENT_IMPL(
    _In_ achievement cppAchievement,
    _In_ XSAPI_ACHIEVEMENT* cAchievement
) : m_cppAchievement(cppAchievement), m_cAchievement(cAchievement)
{
    m_id = utils::utf8_from_utf16(m_cppAchievement.id());
    m_cAchievement->id = m_id.c_str();

    m_serviceConfigurationId = utils::utf8_from_utf16(m_cppAchievement.service_configuration_id());
    m_cAchievement->serviceConfigurationId = m_serviceConfigurationId.c_str();

    m_name = utils::utf8_from_utf16(m_cppAchievement.name());
    m_cAchievement->name = m_name.c_str();

    for (auto cppTitleAssociation : m_cppAchievement.title_associations())
    {
        auto titleAssociation = new XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION();
        titleAssociation->pImpl = new XSAPI_ACHIEVEMENT_TITLE_ASSOCIATION_IMPL(cppTitleAssociation, titleAssociation);
        m_titleAssociations.push_back(titleAssociation);
    }
    m_cAchievement->titleAssociations = m_titleAssociations.data();
    m_cAchievement->titleAssociationsCount = (uint32_t)m_titleAssociations.size();

    for (auto cppMediaAsset : m_cppAchievement.media_assets())
    {
        auto mediaAsset = new XSAPI_ACHIEVEMENT_MEDIA_ASSET();
        mediaAsset->pImpl = new XSAPI_ACHIEVEMENT_MEDIA_ASSET_IMPL(cppMediaAsset, mediaAsset);
        m_mediaAssets.push_back(mediaAsset);
    }
    m_cAchievement->mediaAssets = m_mediaAssets.data();
    m_cAchievement->mediaAssetsCount = (uint32_t)m_mediaAssets.size();

    std::vector<PCSTR> cPlatforms;
    for (auto cppPlatform : m_cppAchievement.platforms_available_on())
    {
        auto platform = utils::utf8_from_utf16(cppPlatform);
        m_platformsAvailableOn.push_back(platform);
        cPlatforms.push_back(platform.c_str());
    }
    m_cAchievement->platformsAvailableOn = cPlatforms.data();
    m_cAchievement->platformsAvailableOnCount = (uint32_t)cPlatforms.size();

    m_unlockedDescription = utils::utf8_from_utf16(m_cppAchievement.unlocked_description());
    m_cAchievement->unlockedDescription = m_unlockedDescription.c_str();

    m_lockedDescription = utils::utf8_from_utf16(m_cppAchievement.locked_description());
    m_cAchievement->lockedDescription = m_lockedDescription.c_str();

    m_productId = utils::utf8_from_utf16(m_cppAchievement.product_id());
    m_cAchievement->productId = m_productId.c_str();

    m_available = new XSAPI_ACHIEVEMENT_TIME_WINDOW();
    m_available->startDate = utils::time_t_from_datetime(m_cppAchievement.available().start_date());
    m_available->endDate = utils::time_t_from_datetime(m_cppAchievement.available().end_date());

    for (auto cppReward : m_cppAchievement.rewards())
    {
        auto reward = new XSAPI_ACHIEVEMENT_REWARD();
        reward->pImpl = new XSAPI_ACHIEVEMENT_REWARD_IMPL(cppReward, reward);
        m_rewards.push_back(reward);
    }
    m_cAchievement->rewards = m_rewards.data();
    m_cAchievement->rewardsCount = (uint32_t)m_rewards.size();

    m_deepLink = utils::utf8_from_utf16(m_cppAchievement.deep_link());
    m_cAchievement->deepLink = m_deepLink.c_str();
}

XSAPI_ACHIEVEMENTS_RESULT_IMPL::XSAPI_ACHIEVEMENTS_RESULT_IMPL(
    _In_ achievements_result cppResult,
    _In_ XSAPI_ACHIEVEMENTS_RESULT* cResult
) : m_cppResult(cppResult), m_cResult(cResult)
{
    for (auto achievement : m_cppResult.items())
    {
        m_items.push_back(CreateAchievementFromCpp(achievement));
    }

    m_cResult->items = m_items.data();
    m_cResult->itemsCount = (uint32_t)m_items.size();
}
achievements_result XSAPI_ACHIEVEMENTS_RESULT_IMPL::cppAchievementsResult() const
{
    return m_cppResult;
}

XSAPI_ACHIEVEMENT* CreateAchievementFromCpp(
    _In_ achievement cppAchievement
)
{
    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_achievementsState->m_lock);

    auto achievement = new XSAPI_ACHIEVEMENT();
    achievement->pImpl = new XSAPI_ACHIEVEMENT_IMPL(cppAchievement, achievement);

    singleton->m_achievementsState->m_achievements.insert(achievement);

    return achievement;
}

XSAPI_ACHIEVEMENTS_RESULT* CreateAchievementsResultFromCpp(
    _In_ achievements_result cppResult
)
{
    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_achievementsState->m_lock);

    auto result = new XSAPI_ACHIEVEMENTS_RESULT();
    result->pImpl = new XSAPI_ACHIEVEMENTS_RESULT_IMPL(cppResult, result);

    singleton->m_achievementsState->m_achievementResults.insert(result);
    return result;
}