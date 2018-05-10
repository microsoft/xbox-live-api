// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/achievements_c.h"
#include "Achievements\achievements_internal.h"
#include "xbox_live_context_internal_c.h"

using namespace xbox::services;
using namespace xbox::services::achievements;
using namespace xbox::services::system;

#pragma region CopyHelpers
void copy_string(buffer_allocator& allocator, UTF8CSTR* dest, xsapi_internal_string val)
{
    *dest = (UTF8CSTR)allocator.alloc(val.size() + 1);
    memcpy((void*)(*dest), val.c_str(), val.size() + 1);
}

template<typename Internal, typename FlatC>
void copy_array(
    FlatC*** dest,
    uint64_t* destCount,
    xsapi_internal_vector<std::shared_ptr<Internal>> source,
    FlatC*(*copy)(std::shared_ptr<Internal>)
)
{
    size_t neededBufferSize = SizeOfArray(source, count, sizeOfFnc);

    if (neededBufferSize > *bufferSize)
    {
        *bufferSize = neededBufferSize;
        return;
    }

    T** tempArr = (T**)b.alloc(sizeof(T*) * count);
    for (size_t i = 0; i < count; ++i)
    {
        auto item = (*copy)(source[i], b, bufferSize);
        tempArr[i] = item;
    }
    *dest = tempArr;
}

template <typename T>
size_t calculate_array_size(size_t size)
{
    size_t neededBufferSize = sizeof(T*) * size;
    neededBufferSize += sizeof(T) * size;
    return neededBufferSize;
}

template <typename T>
size_t calculate_array_size(xsapi_internal_vector<std::shared_ptr<T>> arr, size_t(*sizeOfFnc)(std::shared_ptr<T>))
{
    if (arr.size() == 0) return 0;

    size_t neededBufferSize = sizeof(T*) * arr.size();

    for (size_t i = 0; i < arr.size(); ++i)
    {
        neededBufferSize += (*sizeOfFnc)(arr[i]);
    }

    return neededBufferSize;
}

template<typename Internal, typename FlatC>
void copy_array(
    buffer_allocator& allocator,
    FlatC*** dest,
    uint32_t* destCount,
    xsapi_internal_vector<std::shared_ptr<Internal>> source,
    FlatC*(*copy)(std::shared_ptr<Internal>, buffer_allocator&)
)
{
    FlatC** tempArr = allocator.alloc_array<FlatC*>((uint32_t)source.size());
    for (size_t i = 0; i < source.size(); ++i)
    {
        tempArr[i] = (*copy)(source[i], allocator);
    }
    *dest = tempArr;
    *destCount = (uint32_t)source.size();
}

size_t calculate_string_array_size(xsapi_internal_vector<xsapi_internal_string> arr)
{
    size_t neededBufferSize = sizeof(UTF8CSTR) * arr.size();

    for (size_t i = 0; i < arr.size(); ++i)
    {
        neededBufferSize += arr[i].length() + 1;
    }

    return neededBufferSize;
}

void copy_string_array(
    buffer_allocator& allocator,
    UTF8CSTR** dest,
    uint32_t* destCount,
    xsapi_internal_vector<xsapi_internal_string> source
)
{
    UTF8CSTR* tempArr = allocator.alloc_array<UTF8CSTR>((uint32_t)source.size());
    for (size_t i = 0; i < source.size(); ++i)
    {
        UTF8CSTR copy;
        copy_string(allocator, &copy, source[i]);
        tempArr[i] = copy;
    }
    *dest = tempArr;
    *destCount = (uint32_t)source.size();
}

size_t calculate_achievement_title_association_size(std::shared_ptr<achievement_title_association_internal> source)
{
    if (source == nullptr) return 0;

    size_t neededBufferSize = sizeof(XblAchievementTitleAssociation);
    neededBufferSize += source->name().size() + 1;

    return neededBufferSize;
}

XblAchievementTitleAssociation* copy_achievement_title_association(
    std::shared_ptr<achievement_title_association_internal> source,
    buffer_allocator& allocator
)
{
    if (source == nullptr) return nullptr;

    auto dest = allocator.alloc<XblAchievementTitleAssociation>();
    copy_string(allocator, &dest->name, source->name());
    dest->titleId = source->title_id();

    return dest;
}

size_t calculate_achievement_requirement_size(std::shared_ptr<achievement_requirement_internal> source)
{
    if (source == nullptr) return 0;

    size_t neededBufferSize = sizeof(XblAchievementRequirement);
    neededBufferSize += source->id().size() + 1;
    neededBufferSize += source->current_progress_value().size() + 1;
    neededBufferSize += source->target_progress_value().size() + 1;

    return neededBufferSize;
}

XblAchievementRequirement* copy_achievement_requirement(
    std::shared_ptr<achievement_requirement_internal> source,
    buffer_allocator& allocator
)
{
    if (source == nullptr) return nullptr;

    auto dest = allocator.alloc<XblAchievementRequirement>();
    copy_string(allocator, &dest->id, source->id());
    copy_string(allocator, &dest->currentProgressValue, source->current_progress_value());
    copy_string(allocator, &dest->targetProgressValue, source->target_progress_value());

    return dest;
}

size_t calculate_achievement_media_asset_size(std::shared_ptr<achievement_media_asset_internal> source)
{
    if (source == nullptr) return 0;

    size_t neededBufferSize = sizeof(XblAchievementMediaAsset);
    neededBufferSize += source->name().size() + 1;
    neededBufferSize += source->url().to_string().size() + 1;

    return neededBufferSize;
}

XblAchievementMediaAsset* copy_achievement_media_asset(
    std::shared_ptr<achievement_media_asset_internal> source,
    buffer_allocator& allocator
)
{
    if (source == nullptr) return nullptr;

    auto dest = allocator.alloc<XblAchievementMediaAsset>();
    copy_string(allocator, &dest->name, source->name());
    dest->mediaAssetType = static_cast<XblAchievementMediaAssetType>(source->media_asset_type());
    copy_string(allocator, &dest->url, utils::internal_string_from_string_t(source->url().to_string()));

    return dest;
}

size_t calculate_achievement_reward_size(std::shared_ptr<achievement_reward_internal>source)
{
    if (source == nullptr) return 0;

    size_t neededBufferSize = sizeof(XblAchievementReward);
    neededBufferSize += source->name().size() + 1;
    neededBufferSize += source->description().size() + 1;
    neededBufferSize += source->value().size() + 1;
    neededBufferSize += source->value_type().size() + 1;

    if (source->media_asset_internal())
    {
        neededBufferSize += calculate_achievement_media_asset_size(source->media_asset_internal());
    }

    return neededBufferSize;
}

XblAchievementReward* copy_achievement_reward(
    std::shared_ptr<achievement_reward_internal> source,
    buffer_allocator& allocator
)
{
    if (source == nullptr) return nullptr;

    auto dest = allocator.alloc<XblAchievementReward>();
    copy_string(allocator, &dest->name, source->name());
    copy_string(allocator, &dest->description, source->description());
    copy_string(allocator, &dest->value, source->value());
    dest->rewardType = static_cast<XblAchievementRewardType>(source->reward_type());
    copy_string(allocator, &dest->valueType, source->value_type());
    dest->mediaAsset = copy_achievement_media_asset(source->media_asset_internal(), allocator);

    return dest;
}

size_t calculate_achievement_progression_size(std::shared_ptr<achievement_progression_internal> source)
{
    if (source == nullptr) return 0;

    size_t neededBufferSize = sizeof(XblAchievementProgression);
    neededBufferSize += calculate_array_size(source->requirements(), calculate_achievement_requirement_size);

    return neededBufferSize;
}

XblAchievementProgression* copy_achievement_progression(
    std::shared_ptr<achievement_progression_internal> source,
    buffer_allocator& allocator
)
{
    if (source == nullptr) return nullptr;

    auto dest = allocator.alloc<XblAchievementProgression>();
    copy_array(allocator, &dest->requirements, &dest->requirementsCount, source->requirements(), copy_achievement_requirement);
    dest->timeUnlocked = utils::time_t_from_datetime(source->time_unlocked());

    return dest;
}

size_t calculate_achievement_size(
    _In_ std::shared_ptr<achievement_internal> source
)
{
    if (source == nullptr) return 0;

    size_t neededBufferSize = sizeof(XblAchievement);

    neededBufferSize += source->id().size() + 1;
    neededBufferSize += source->service_configuration_id().size() + 1;
    neededBufferSize += source->name().size() + 1;
    neededBufferSize += calculate_array_size(source->title_associations(), calculate_achievement_title_association_size);
    neededBufferSize += calculate_achievement_progression_size(source->progression_internal());
    neededBufferSize += calculate_array_size(source->media_assets(), calculate_achievement_media_asset_size);
    neededBufferSize += calculate_string_array_size(source->platforms_available_on());
    neededBufferSize += source->unlocked_description().size() + 1;
    neededBufferSize += source->locked_description().size() + 1;
    neededBufferSize += source->product_id().size() + 1;
    neededBufferSize += sizeof(XblAchievementTimeWindow);
    neededBufferSize += calculate_array_size(source->rewards(), calculate_achievement_reward_size);
    neededBufferSize += source->deep_link().size() + 1;

    return neededBufferSize;
}

size_t calculate_achievements_result_size(
    _In_ std::shared_ptr<achievements_result_internal> source
)
{
    size_t neededBufferSize = sizeof(XblAchievementsResult);
    neededBufferSize += calculate_array_size(source->items(), calculate_achievement_size);
    neededBufferSize += sizeof(uint32_t) * source->title_ids().size();
    neededBufferSize += source->continuation_token().size() + 1;
    return neededBufferSize;
}

XblAchievement* copy_achievement(
    std::shared_ptr<achievement_internal> internal,
    buffer_allocator& allocator
)
{
    if (internal == nullptr) return nullptr;

    auto result = allocator.alloc<XblAchievement>();
    copy_string(allocator, &result->id, internal->id());
    copy_string(allocator, &result->serviceConfigurationId, internal->service_configuration_id());
    copy_string(allocator, &result->name, internal->name());
    copy_array(allocator, &result->titleAssociations, &result->titleAssociationsCount, internal->title_associations(), copy_achievement_title_association);
    result->progressState = static_cast<XblAchievementProgressState>(internal->progress_state());
    result->progression = copy_achievement_progression(internal->progression_internal(), allocator);
    copy_array(allocator, &result->mediaAssets, &result->mediaAssetsCount, internal->media_assets(), copy_achievement_media_asset);
    copy_string_array(allocator, &result->platformsAvailableOn, &result->platformsAvailableOnCount, internal->platforms_available_on());
    result->isSecret = internal->is_secret();
    copy_string(allocator, &result->unlockedDescription, internal->unlocked_description());
    copy_string(allocator, &result->lockedDescription, internal->locked_description());
    copy_string(allocator, &result->productId, internal->product_id());
    result->type = static_cast<XblAchievementType>(internal->type());
    result->participationType = static_cast<XblAchievementParticipationType>(internal->participation_type());
    result->available = allocator.alloc<XblAchievementTimeWindow>();
    result->available->startDate = utils::time_t_from_datetime(internal->available().start_date());
    result->available->endDate = utils::time_t_from_datetime(internal->available().end_date());
    copy_array(allocator, &result->rewards, &result->rewardsCount, internal->rewards(), copy_achievement_reward);
    result->estimatedUnlockTime = internal->estimated_unlock_time().count();
    copy_string(allocator, &result->deepLink, internal->deep_link());
    result->isSecret = internal->is_secret();;

    return result;
}

XblAchievement* xbl_copy_achievement(
    _In_ std::shared_ptr<achievement_internal> internal,
    _In_ size_t bufferSize,
    _Out_ XblAchievement* achievement
)
{
    buffer_allocator allocator(achievement, bufferSize);
    return copy_achievement(internal, allocator);
}

void xbl_copy_achievements_result(
    _In_ std::shared_ptr<achievements_result_internal> internal,
    _In_ size_t bufferSize,
    _Out_ XblAchievementsResult* achievementsResult
)
{
    buffer_allocator allocator(achievementsResult, bufferSize);

    auto result = allocator.alloc<XblAchievementsResult>();
    copy_array(allocator, &result->items, &result->itemsCount, internal->items(), copy_achievement);
    result->hasNext = internal->has_next();

    result->xboxUserId = utils::internal_string_to_uint64(internal->xbox_user_id());
    result->titleIds = allocator.alloc_array<uint32_t>((uint32_t)internal->title_ids().size());
    result->titleIdsCount = (uint32_t)internal->title_ids().size();
    for (size_t i = 0; i < internal->title_ids().size(); i++)
    {
        result->titleIds[i] = internal->title_ids()[i];
    }    
    result->type = static_cast<XblAchievementType>(internal->type());
    result->unlockedOnly = internal->unlocked_only();
    result->orderBy = static_cast<XblAchievementOrderBy>(internal->order_by());
    copy_string(allocator, &result->continuationToken, internal->continuation_token());
}
#pragma endregion 

XBL_API bool XBL_CALLING_CONV
XblAchievementsResultHasNext(
    _In_ XblAchievementsResult* achievementsResult
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    return achievementsResult->continuationToken != nullptr && std::char_traits<xsapi_internal_string::value_type>::length(achievementsResult->continuationToken) > 0;
}
CATCH_RETURN_WITH(false)

STDAPI
XblAchievementsResultGetNext(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblAchievementsResult* achievementsResult,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    struct Context
    {
        XblAchievementsResult* achievementsResult;
        xbl_context_handle xboxLiveContext;
        uint32_t maxItems;
        std::shared_ptr<achievements_result_internal> result;
    };
    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context;
    context->xboxLiveContext = xboxLiveContext;
    context->maxItems = maxItems;
    context->achievementsResult = achievementsResult;

    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
        case AsyncOp_DoWork:
            context->xboxLiveContext->contextImpl->achievement_service_internal()->get_achievements(
                utils::uint64_to_internal_string(context->achievementsResult->xboxUserId),
                utils::uint32_array_to_internal_vector(context->achievementsResult->titleIds, context->achievementsResult->titleIdsCount),
                static_cast<achievement_type>(context->achievementsResult->type),
                context->achievementsResult->unlockedOnly,
                static_cast<achievement_order_by>(context->achievementsResult->orderBy),
                0, // use continuationToken, ignore skipItems.
                context->maxItems,
                context->achievementsResult->continuationToken,
                data->async->queue,
                [data, context](xbox_live_result<std::shared_ptr<achievements_result_internal>> result)
            {
                context->result = std::move(result.payload());
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                CompleteAsync(data->async, hr, calculate_achievements_result_size(context->result));
            });
            return E_PENDING;

        case AsyncOp_GetResult:
            xbl_copy_achievements_result(context->result, data->bufferSize, static_cast<XblAchievementsResult*>(data->buffer));
            break;

        case AsyncOp_Cleanup:
            context->~Context();
            xsapi_memory::mem_free(context);
            break;
        }
        return S_OK;
    });

    if (FAILED(hr))
    {
        return hr;
    }
    return ScheduleAsync(async, 0);
}
CATCH_RETURN()

STDAPI XblAchievementsUpdateAchievement(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_opt_ uint32_t* titleId,
    _In_opt_ UTF8CSTR serviceConfigurationId,
    _In_ UTF8CSTR achievementId,
    _In_ uint32_t percentComplete
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    struct Context
    {
        xbl_context_handle xboxLiveContext;
        xsapi_internal_string xboxUserId;
        uint32_t* titleId;
        xsapi_internal_string serviceConfigurationId;
        xsapi_internal_string achievementId;
        uint32_t percentComplete;
        xbox_live_result<void> result;
    };

    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context;
    context->xboxLiveContext = xboxLiveContext;
    context->xboxUserId = utils::uint64_to_internal_string(xboxUserId);
    context->titleId = titleId;
    context->serviceConfigurationId = serviceConfigurationId;
    context->achievementId = achievementId;
    context->percentComplete = percentComplete;

    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
        case AsyncOp_DoWork:

            if (context->titleId == nullptr)
            {
                context->xboxLiveContext->contextImpl->achievement_service_internal()->update_achievement(
                    context->xboxUserId,
                    context->achievementId,
                    context->percentComplete,
                    data->async->queue,
                    [data, context](xbox_live_result<void> result) 
                {
                    context->result = std::move(result);
                    auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                    CompleteAsync(data->async, hr, 0);
                });
            }
            else
            {
                context->xboxLiveContext->contextImpl->achievement_service_internal()->update_achievement(
                    context->xboxUserId,
                    *(context->titleId),
                    context->serviceConfigurationId,
                    context->achievementId,
                    context->percentComplete,
                    data->async->queue,
                    [data, context](xbox_live_result<void> result)
                {
                    context->result = std::move(result);
                    auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                    CompleteAsync(data->async, hr, 0);
                });
            }
            return E_PENDING;

        case AsyncOp_GetResult:
            break;

        case AsyncOp_Cleanup:
            context->~Context();
            xsapi_memory::mem_free(context);
            break;
        }
        return S_OK;
    });

    if (FAILED(hr))
    {
        return hr;
    }
    return ScheduleAsync(async, 0);
}
CATCH_RETURN()

STDAPI XblAchievementsGetAchievementsForTitleId(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ uint32_t titleId,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    struct Context
    {
        xbl_context_handle xboxLiveContext;
        xsapi_internal_string xboxUserId;
        uint32_t titleId;
        XblAchievementType type;
        bool unlockedOnly;
        XblAchievementOrderBy orderBy;
        uint32_t skipItems;
        uint32_t maxItems;
        std::shared_ptr<achievements_result_internal> result;
    };
    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context;
    context->xboxLiveContext = xboxLiveContext;
    context->xboxUserId = utils::uint64_to_internal_string(xboxUserId);
    context->titleId = titleId;
    context->type = type;
    context->unlockedOnly = unlockedOnly;
    context->orderBy = orderBy;
    context->skipItems = skipItems;
    context->maxItems = maxItems;

    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
        case AsyncOp_DoWork:
            context->xboxLiveContext->contextImpl->achievement_service_internal()->get_achievements_for_title_id(
                context->xboxUserId,
                context->titleId,
                static_cast<achievement_type>(context->type),
                context->unlockedOnly,
                static_cast<achievement_order_by>(context->orderBy),
                context->skipItems,
                context->maxItems,
                data->async->queue,
                [data, context](xbox_live_result<std::shared_ptr<achievements_result_internal>> result)
            {
                context->result = std::move(result.payload());
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                CompleteAsync(data->async, hr, calculate_achievements_result_size(context->result));
            });
            return E_PENDING;

        case AsyncOp_GetResult:
            xbl_copy_achievements_result(context->result, data->bufferSize, static_cast<XblAchievementsResult*>(data->buffer));
            break;

        case AsyncOp_Cleanup:
            context->~Context();
            xsapi_memory::mem_free(context);
            break;
        }
        return S_OK;
    });

    if (FAILED(hr))
    {
        return hr;
    }
    return ScheduleAsync(async, 0);
}
CATCH_RETURN()

STDAPI XblAchievementsGetAchievement(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ UTF8CSTR serviceConfigurationId,
    _In_ UTF8CSTR achievementId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    struct Context
    {
        xbl_context_handle xboxLiveContext;
        xsapi_internal_string xboxUserId;
        xsapi_internal_string serviceConfigurationId;
        xsapi_internal_string achievementId;
        std::shared_ptr<achievement_internal> result;
    };
    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context;
    context->xboxLiveContext = xboxLiveContext;
    context->xboxUserId = utils::uint64_to_internal_string(xboxUserId);
    context->serviceConfigurationId = serviceConfigurationId;
    context->achievementId = achievementId;

    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
        case AsyncOp_DoWork:
            context->xboxLiveContext->contextImpl->achievement_service_internal()->get_achievement(
                context->xboxUserId,
                context->serviceConfigurationId,
                context->achievementId,
                data->async->queue,
                [data, context](xbox_live_result<std::shared_ptr<achievement_internal>> result) 
            {
                context->result = std::move(result.payload());
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                CompleteAsync(data->async, hr, calculate_achievement_size(context->result));
            });
            return E_PENDING;

        case AsyncOp_GetResult:
            xbl_copy_achievement(context->result, data->bufferSize, static_cast<XblAchievement*>(data->buffer));
            break;

        case AsyncOp_Cleanup:
            context->~Context();
            xsapi_memory::mem_free(context);
            break;
        }
        return S_OK;
    });

    if (FAILED(hr))
    {
        return hr;
    }
    return ScheduleAsync(async, 0);
}
CATCH_RETURN()

STDAPI XblAchievementsGetAchievementResult(
    _In_ AsyncBlock* async,
    _In_ size_t resultSize,
    _Out_writes_bytes_to_opt_(resultSize, *bufferUsed) XblAchievement* result,
    _Out_opt_ size_t* bufferUsed
    ) XBL_NOEXCEPT
{
    return GetAsyncResult(async, nullptr, resultSize, result, bufferUsed);
}

STDAPI XblAchievementsGetAchievementsForTitleIdResult(
    _In_ AsyncBlock* async,
    _In_ size_t resultSize,
    _Out_writes_bytes_to_opt_(resultSize, *bufferUsed) XblAchievementsResult* result,
    _Out_opt_ size_t* bufferUsed
    ) XBL_NOEXCEPT
{
    return GetAsyncResult(async, nullptr, resultSize, result, bufferUsed);
}

STDAPI XblAchievementsResultGetNextResult(
    _In_ AsyncBlock* async,
    _In_ size_t resultSize,
    _Out_writes_bytes_to_opt_(resultSize, *bufferUsed) XblAchievementsResult* result,
    _Out_opt_ size_t* bufferUsed
    ) XBL_NOEXCEPT
{
    return GetAsyncResult(async, nullptr, resultSize, result, bufferUsed);
}