// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/achievements_c.h"
#include "achievements/achievements_internal.h"
#include "achievements_internal_c.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::achievements;

_Null_terminated_ char* alloc_and_copy_string(string_t src)
{
    auto utf8 = utils::internal_string_from_string_t(src);
    auto copy = static_cast<char*>(xsapi_memory::mem_alloc(utf8.size() + 1));
    if (copy != nullptr)
    {
        strcpy_s(copy, utf8.size() + 1, utf8.data());
    }
    return copy;
}

void create_xbl_achievement(
    _In_ std::shared_ptr<achievement_internal> rhs,
    _Out_ XblAchievement& lhs
    )
{
    ZeroMemory(&lhs, sizeof(XblAchievement));
    lhs.id = rhs->id().data();
    lhs.serviceConfigurationId = rhs->service_configuration_id().data();
    lhs.name = rhs->name().data();
    lhs.titleAssociationsCount = static_cast<uint32_t>(rhs->title_associations().size());
    lhs.titleAssociations = static_cast<XblAchievementTitleAssociation*>(xsapi_memory::mem_alloc(sizeof(XblAchievementTitleAssociation) * lhs.titleAssociationsCount));
    for (uint32_t j = 0; j < lhs.titleAssociationsCount; ++j)
    {
        lhs.titleAssociations[j].name = rhs->title_associations()[j]->name().data();
        lhs.titleAssociations[j].titleId = rhs->title_associations()[j]->title_id();
    }
    lhs.progressState = static_cast<XblAchievementProgressState>(rhs->progress_state());
    lhs.progression.requirementsCount = static_cast<uint32_t>(rhs->progression_internal()->requirements().size());
    lhs.progression.requirements = static_cast<XblAchievementRequirement*>(xsapi_memory::mem_alloc(sizeof(XblAchievementRequirement) * lhs.progression.requirementsCount));
    for (uint32_t j = 0; j < lhs.progression.requirementsCount; ++j)
    {
        lhs.progression.requirements[j].id = rhs->progression_internal()->requirements()[j]->id().data();
        lhs.progression.requirements[j].currentProgressValue = rhs->progression_internal()->requirements()[j]->current_progress_value().data();
        lhs.progression.requirements[j].targetProgressValue = rhs->progression_internal()->requirements()[j]->target_progress_value().data();
    }
    lhs.progression.timeUnlocked = utils::time_t_from_datetime(rhs->progression_internal()->time_unlocked());

    lhs.mediaAssetsCount = static_cast<uint32_t>(rhs->media_assets().size());
    lhs.mediaAssets = static_cast<XblAchievementMediaAsset*>(xsapi_memory::mem_alloc(sizeof(XblAchievementMediaAsset) * lhs.mediaAssetsCount));
    for (uint32_t j = 0; j < lhs.mediaAssetsCount; ++j)
    {
        lhs.mediaAssets[j].name = rhs->media_assets()[j]->name().data();
        lhs.mediaAssets[j].mediaAssetType = static_cast<XblAchievementMediaAssetType>(rhs->media_assets()[j]->media_asset_type());
        lhs.mediaAssets[j].url = alloc_and_copy_string(rhs->media_assets()[j]->url().to_string());
    }

    lhs.platformsAvailableOnCount = static_cast<uint32_t>(rhs->platforms_available_on().size());
    lhs.platformsAvailableOn = static_cast<const char**>(xsapi_memory::mem_alloc(sizeof(const char*) * lhs.platformsAvailableOnCount));
    for (uint32_t j = 0; j < lhs.platformsAvailableOnCount; ++j)
    {
#pragma warning(suppress: 6386)
        lhs.platformsAvailableOn[j] = rhs->platforms_available_on()[j].data();
    }
    lhs.isSecret = rhs->is_secret();
    lhs.unlockedDescription = rhs->unlocked_description().data();
    lhs.lockedDescription = rhs->locked_description().data();
    lhs.productId = rhs->product_id().data();
    lhs.type = static_cast<XblAchievementType>(rhs->type());
    lhs.participationType = static_cast<XblAchievementParticipationType>(rhs->participation_type());
    lhs.available.startDate = utils::time_t_from_datetime(rhs->available().start_date());
    lhs.available.endDate = utils::time_t_from_datetime(rhs->available().end_date());

    lhs.rewardsCount = static_cast<uint32_t>(rhs->rewards().size());
    lhs.rewards = static_cast<XblAchievementReward*>(xsapi_memory::mem_alloc(sizeof(XblAchievementReward) * lhs.rewardsCount));
    if (lhs.rewards != nullptr)
    {
        for (uint32_t j = 0; j < lhs.rewardsCount; ++j)
        {
#pragma warning(suppress: 6386)
            lhs.rewards[j].name = rhs->rewards()[j]->name().data();
            lhs.rewards[j].description = rhs->rewards()[j]->description().data();
            lhs.rewards[j].value = rhs->rewards()[j]->value().data();
            lhs.rewards[j].rewardType = static_cast<XblAchievementRewardType>(rhs->rewards()[j]->reward_type());
            lhs.rewards[j].valueType = rhs->rewards()[j]->value_type().data();
            if (rhs->rewards()[j]->media_asset_internal() != nullptr)
            {
                lhs.rewards[j].mediaAsset = static_cast<XblAchievementMediaAsset*>(xsapi_memory::mem_alloc(sizeof(XblAchievementMediaAsset)));
#pragma warning(suppress: 6385)
                if (lhs.rewards[j].mediaAsset != nullptr)
                {
                    lhs.rewards[j].mediaAsset->url = alloc_and_copy_string(rhs->rewards()[j]->media_asset_internal()->url().to_string());
                    lhs.rewards[j].mediaAsset->mediaAssetType = static_cast<XblAchievementMediaAssetType>(rhs->rewards()[j]->media_asset_internal()->media_asset_type());
                    lhs.rewards[j].mediaAsset->name = rhs->rewards()[j]->media_asset_internal()->name().data();
                }
            }
            else
            {
                lhs.rewards[j].mediaAsset = nullptr;
            }
        }
    }

    lhs.estimatedUnlockTime = rhs->estimated_unlock_time().count();
    lhs.deepLink = rhs->deep_link().data();
    lhs.isRevoked = rhs->is_revoked();
}

xbl_achievements_result::xbl_achievements_result(std::shared_ptr<achievements_result_internal> _internalResult)
    : internalResult(std::move(_internalResult)),
    internalAchievement(nullptr),
    refCount(1)
{
    auto& internalItems = internalResult->items();
    items = xsapi_internal_vector<XblAchievement>(internalItems.size());

    for (uint32_t i = 0; i < items.size(); ++i)
    {
        create_xbl_achievement(internalItems[i], items[i]);
    }
}

xbl_achievements_result::xbl_achievements_result(std::shared_ptr<achievement_internal> _internalAchievement)
    : internalResult(nullptr),
    internalAchievement(std::move(_internalAchievement)),
    refCount(1)
{
    items = xsapi_internal_vector<XblAchievement>(1);
    create_xbl_achievement(internalAchievement, items[0]);
}

xbl_achievements_result::~xbl_achievements_result()
{
    for (auto& item : items)
    {
        xsapi_memory::mem_free(item.titleAssociations);
        xsapi_memory::mem_free(item.progression.requirements);
        for (uint32_t i = 0; i < item.mediaAssetsCount; ++i)
        {
            xsapi_memory::mem_free((void*)item.mediaAssets[i].url);
        }
        xsapi_memory::mem_free(item.mediaAssets);
        xsapi_memory::mem_free(item.platformsAvailableOn);
        for (uint32_t i = 0; i < item.rewardsCount; ++i)
        {
            if (item.rewards[i].mediaAsset != nullptr)
            {
                xsapi_memory::mem_free((void*)item.rewards[i].mediaAsset->url);
                xsapi_memory::mem_free((void*)item.rewards[i].mediaAsset);
            }
        }
        xsapi_memory::mem_free(item.rewards);
    }
}
