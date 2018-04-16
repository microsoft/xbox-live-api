// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/achievements_c.h"
#include "Achievements\achievements_internal.h"
#include "xbox_live_context_internal_c.h"
#include "achievements_helper.h"
#include "achievements_state.h"

using namespace xbox::services;
using namespace xbox::services::achievements;

XBL_API bool XBL_CALLING_CONV
XblAchievementsResultHasNext(
    _In_ XBL_ACHIEVEMENTS_RESULT* achievementsResult
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    return achievementsResult->pImpl->cppAchievementsResult()->has_next();
}
CATCH_RETURN_WITH(false)

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementsResultGetNext(
    _In_ XBL_ACHIEVEMENTS_RESULT* achievementsResult,
    _In_ uint32_t maxItems,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* completionRoutineContext,
    _In_ XBL_GET_NEXT_COMPLETION_ROUTINE completionRoutine
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();

    auto result = achievementsResult->pImpl->cppAchievementsResult()->get_next(
        maxItems,
        0, // TODO
        [completionRoutineContext, completionRoutine](xbox_live_result<std::shared_ptr<achievements_result_internal>> result) {
        auto xblResult = utils::create_xbl_result(result.err());
        XBL_ACHIEVEMENTS_RESULT* achievementsResult = nullptr;
        if (!result.err())
        {
            achievementsResult = CreateAchievementsResultFromCpp(result.payload());
        }

        completionRoutine(xblResult, achievementsResult, completionRoutineContext);
    });

    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceUpdateAchievement(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_opt_ uint32_t* titleId,
    _In_opt_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ uint32_t percentComplete,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* completionRoutineContext,
    _In_ XBL_UPDATE_ACVHIEVEMENT_COMPLETION_ROUTINE completionRoutine
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();
    
    auto achievementService = xboxLiveContext->contextImpl->achievement_service_internal();
    xbox_live_result<void> result;

    if (titleId == nullptr)
    {
        result = achievementService->update_achievement(xboxUserId,
            achievementId,
            percentComplete,
            0, // TODO
            [completionRoutine, completionRoutineContext](xbox_live_result<void> result) {
                completionRoutine(utils::create_xbl_result(result.err()), completionRoutineContext);
            });
    }
    else
    {
        result = achievementService->update_achievement(xboxUserId,
            *titleId,
            serviceConfigurationId,
            achievementId,
            percentComplete,
            0, // TODO
            [completionRoutine, completionRoutineContext](xbox_live_result<void> result) {
                completionRoutine(utils::create_xbl_result(result.err()), completionRoutineContext);
            });
    }

    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceGetAchievementsForTitleId(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ uint32_t titleId,
    _In_ XBL_ACHIEVEMENT_TYPE type,
    _In_ bool unlockedOnly,
    _In_ XBL_ACIEVEMENT_ORDER_BY orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* completionRoutineContext,
    _In_ XBL_GET_ACHIEVEMENTS_FOR_TITLE_ID_COMPLETION_ROUTINE completionRoutine
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();

    auto achievementService = xboxLiveContext->contextImpl->achievement_service_internal();

    auto result = achievementService->get_achievements_for_title_id(xboxUserId,
        titleId,
        static_cast<achievement_type>(type),
        unlockedOnly,
        static_cast<achievement_order_by>(orderBy),
        skipItems,
        maxItems,
        0, // TODO
        [completionRoutine, completionRoutineContext](xbox_live_result<std::shared_ptr<achievements_result_internal>> result) {
            completionRoutine(utils::create_xbl_result(result.err()), CreateAchievementsResultFromCpp(result.payload()), completionRoutineContext);
        });

    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceGetAchievement(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ XBL_ASYNC_QUEUE queue,
    _In_opt_ void* completionRoutineContext,
    _In_ XBL_GET_ACHIEVEMENT_COMPLETION_ROUTINE completionRoutine
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();
    
    auto achievementService = xboxLiveContext->contextImpl->achievement_service_internal();

    auto result = achievementService->get_achievement(xboxUserId,
        serviceConfigurationId,
        achievementId,
        0, // TODO
        [completionRoutine, completionRoutineContext](xbox_live_result<std::shared_ptr<achievement_internal>> result) {
            completionRoutine(utils::create_xbl_result(result.err()), CreateAchievementFromCpp(result.payload()), completionRoutineContext);
        });

    return utils::create_xbl_result(result.err());
}
//CATCH_RETURN()


XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceReleaseAchievementsResult(
    _In_ XBL_ACHIEVEMENTS_RESULT* achievementsResult
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();

    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_achievementsState->m_lock);

    size_t erasedItems = singleton->m_achievementsState->m_achievementResults.erase(achievementsResult);
    if (erasedItems > 0 && achievementsResult->pImpl != nullptr)
    {
        delete achievementsResult->pImpl;
    }
    return XBL_RESULT_OK;
}
//CATCH_RETURN()


XBL_API XBL_RESULT XBL_CALLING_CONV
XblAchievementServiceReleaseAchievement(
    _In_ XBL_ACHIEVEMENT* achievement
    ) XBL_NOEXCEPT
//try
{
    verify_global_init();

    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_achievementsState->m_lock);

    size_t erasedItems = singleton->m_achievementsState->m_achievements.erase(achievement);
    if (erasedItems > 0 && achievement->pImpl != nullptr)
    {
        delete achievement->pImpl;
    }
    return XBL_RESULT_OK;
}
//CATCH_RETURN()

void CopyString(buffer_allocator& b, PCSTR* dest, PCSTR val)
{
    auto valLen = std::char_traits<xsapi_internal_string::value_type>::length(val) + 1;
    *dest = (PCSTR)b.alloc(valLen);
    memcpy((void*)(*dest), val, valLen);
}

template <typename T>
uint64_t SizeOfArray(T** arr, uint64_t count, uint64_t(*sizeOfFnc)(const T*))
{
    if (arr == nullptr) return 0;

    uint64_t neededBufferSize = sizeof(T*) * count;

    for (uint64_t i = 0; i < count; ++i)
    {
        neededBufferSize += (*sizeOfFnc)(arr[i]);
    }

    return neededBufferSize;
}

template<typename T>
void CopyArray(
        buffer_allocator& b, 
        uint64_t* bufferSize, 
        T*** dest, 
        T** source, 
        uint64_t count, 
        uint64_t(*sizeOfFnc)(const T*), 
        T*(*copy)(const T*, buffer_allocator&, uint64_t*)
    )
{
    uint64_t neededBufferSize = SizeOfArray(source, count, sizeOfFnc);

    if (neededBufferSize > *bufferSize)
    {
        *bufferSize = neededBufferSize;
        return;
    }

    T** tempArr = (T**)b.alloc(sizeof(T*) * count);
    for (uint64_t i = 0; i < count; ++i)
    {
        auto item = (*copy)(source[i], b, bufferSize);
        tempArr[i] = item;
    }
    *dest = tempArr;
}

uint64_t SizeOfStringArray(PCSTR* arr, uint64_t count)
{
    if (arr == nullptr) return 0;

    uint64_t neededBufferSize = sizeof(PCSTR) * count;

    for (uint64_t i = 0; i < count; ++i)
    {
        neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(arr[i]) + 1;
    }

    return neededBufferSize;
}

void CopyStringArray(
        buffer_allocator& b, 
        uint64_t* bufferSize, 
        PCSTR** dest, 
        PCSTR* source,
        uint64_t count
    )
{
    uint64_t neededBufferSize = SizeOfStringArray(source, count);

    if (neededBufferSize > *bufferSize)
    {
        *bufferSize = neededBufferSize;
        return;
    }

    PCSTR* tempArr = (PCSTR*)b.alloc(sizeof(PCSTR) * count);
    for (uint64_t i = 0; i < count; ++i)
    {
        PCSTR copy;
        CopyString(b, &copy, source[i]);
        tempArr[i] = copy;
    }
    *dest = tempArr;
}


uint64_t SizeOfAchievementTitleAssociation(const XBL_ACHIEVEMENT_TITLE_ASSOCIATION *source)
{
    if (source == nullptr) return 0;

    uint64_t neededBufferSize = sizeof(XBL_ACHIEVEMENT_TITLE_ASSOCIATION);
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->name) + 1;
    return neededBufferSize;
}

XBL_ACHIEVEMENT_TITLE_ASSOCIATION* CopyAchievementTitleAssociation(
    const XBL_ACHIEVEMENT_TITLE_ASSOCIATION *source,
    buffer_allocator& b,
    uint64_t *cbBuffer
)
{
    uint64_t neededBufferSize = SizeOfAchievementTitleAssociation(source);

    if (neededBufferSize > *cbBuffer)
    {
        *cbBuffer = neededBufferSize;
        return nullptr;
    }

    if (source == nullptr) return nullptr;

    auto dest = b.alloc<XBL_ACHIEVEMENT_TITLE_ASSOCIATION>(*source);
    CopyString(b, &dest->name, source->name);

    return dest;
}

uint64_t SizeOfAchievementRequirement(const XBL_ACHIEVEMENT_REQUIREMENT *source)
{
    if (source == nullptr) return 0;

    uint64_t neededBufferSize = sizeof(XBL_ACHIEVEMENT_REQUIREMENT);
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->id) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->currentProgressValue) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->targetProgressValue) + 1;
    return neededBufferSize;
}

XBL_ACHIEVEMENT_REQUIREMENT* CopyAchievementRequirement(
    const XBL_ACHIEVEMENT_REQUIREMENT *source,
    buffer_allocator& b,
    uint64_t *cbBuffer
)
{
    uint64_t neededBufferSize = SizeOfAchievementRequirement(source);

    if (neededBufferSize > *cbBuffer)
    {
        *cbBuffer = neededBufferSize;
        return nullptr;
    }

    if (source == nullptr) return nullptr;

    auto dest = b.alloc<XBL_ACHIEVEMENT_REQUIREMENT>(*source);
    CopyString(b, &dest->id, source->id);
    CopyString(b, &dest->currentProgressValue, source->currentProgressValue);
    CopyString(b, &dest->targetProgressValue, source->targetProgressValue);

    return dest;
}

uint64_t SizeOfAchievementMediaAsset(const XBL_ACHIEVEMENT_MEDIA_ASSET *source)
{
    if (source == nullptr) return 0;

    uint64_t neededBufferSize = sizeof(XBL_ACHIEVEMENT_MEDIA_ASSET);
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->name) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->url) + 1;
    return neededBufferSize;
}

XBL_ACHIEVEMENT_MEDIA_ASSET* CopyAchievementMediaAsset(
    const XBL_ACHIEVEMENT_MEDIA_ASSET *source,
    buffer_allocator& b,
    uint64_t *cbBuffer
)
{
    uint64_t neededBufferSize = SizeOfAchievementMediaAsset(source);

    if (neededBufferSize > *cbBuffer)
    {
        *cbBuffer = neededBufferSize;
        return nullptr;
    }

    if (source == nullptr) return nullptr;

    auto dest = b.alloc<XBL_ACHIEVEMENT_MEDIA_ASSET>(*source);
    CopyString(b, &dest->name, source->name);
    CopyString(b, &dest->url, source->url);

    return dest;
}

uint64_t SizeOfAchievementReward(const XBL_ACHIEVEMENT_REWARD *source)
{
    if (source == nullptr) return 0;

    uint64_t neededBufferSize = sizeof(XBL_ACHIEVEMENT_REWARD);
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->name) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->description) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->value) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->valueType) + 1;

    neededBufferSize += SizeOfAchievementMediaAsset(source->mediaAsset);

    return neededBufferSize;
}

XBL_ACHIEVEMENT_REWARD* CopyAchievementReward(
    const XBL_ACHIEVEMENT_REWARD *source,
    buffer_allocator& b,
    uint64_t *cbBuffer
)
{
    uint64_t neededBufferSize = SizeOfAchievementReward(source);

    if (neededBufferSize > *cbBuffer)
    {
        *cbBuffer = neededBufferSize;
        return nullptr;
    }

    if (source == nullptr) return nullptr;

    auto dest = b.alloc<XBL_ACHIEVEMENT_REWARD>(*source);
    CopyString(b, &dest->name, source->name);
    CopyString(b, &dest->description, source->description);
    CopyString(b, &dest->value, source->value);
    CopyString(b, &dest->valueType, source->valueType);

    dest->mediaAsset = CopyAchievementMediaAsset(source->mediaAsset, b, cbBuffer);

    return dest;
}

uint64_t SizeOfAchievementProgression(const XBL_ACHIEVEMENT_PROGRESSION *source)
{
    if (source == nullptr) return 0;

    uint64_t neededBufferSize = sizeof(XBL_ACHIEVEMENT_PROGRESSION);
    neededBufferSize += SizeOfArray(source->requirements, source->requirementsCount, SizeOfAchievementRequirement);

    return neededBufferSize;
}

XBL_ACHIEVEMENT_PROGRESSION* CopyAchievementProgression(
    const XBL_ACHIEVEMENT_PROGRESSION *source,
    buffer_allocator& b,
    uint64_t *cbBuffer
)
{
    uint64_t neededBufferSize = SizeOfAchievementProgression(source);

    if (neededBufferSize > *cbBuffer)
    {
        *cbBuffer = neededBufferSize;
        return nullptr;
    }

    if (source == nullptr) return nullptr;

    auto dest = b.alloc<XBL_ACHIEVEMENT_PROGRESSION>(*source);
    CopyArray(b, cbBuffer, &dest->requirements, source->requirements, source->requirementsCount, SizeOfAchievementRequirement, CopyAchievementRequirement);

    return dest;
}

uint64_t SizeOfAchievement(const XBL_ACHIEVEMENT *source)
{
    if (source == nullptr) return 0;

    uint64_t neededBufferSize = sizeof(XBL_ACHIEVEMENT);
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->id) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->serviceConfigurationId) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->name) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->unlockedDescription) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->lockedDescription) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->productId) + 1;
    neededBufferSize += std::char_traits<xsapi_internal_string::value_type>::length(source->deepLink) + 1;

    neededBufferSize += SizeOfArray(source->titleAssociations, source->titleAssociationsCount, SizeOfAchievementTitleAssociation);
    neededBufferSize += SizeOfAchievementProgression(source->progression);
    neededBufferSize += SizeOfArray(source->mediaAssets, source->mediaAssetsCount, SizeOfAchievementMediaAsset);
    neededBufferSize += SizeOfStringArray(source->platformsAvailableOn, source->platformsAvailableOnCount);
    neededBufferSize += sizeof(XBL_ACHIEVEMENT_TIME_WINDOW);
    neededBufferSize += SizeOfArray(source->rewards, source->rewardsCount, SizeOfAchievementReward);

    return neededBufferSize;
}

XBL_ACHIEVEMENT* CopyAchievement(
    const XBL_ACHIEVEMENT *source,
    buffer_allocator& b,
    uint64_t *cbBuffer
)
{
    uint64_t neededBufferSize = SizeOfAchievement(source);

    if (neededBufferSize > *cbBuffer)
    {
        *cbBuffer = neededBufferSize;
        return nullptr;
    }

    if (source == nullptr) return nullptr;

    auto dest = b.alloc<XBL_ACHIEVEMENT>(*source);

    CopyString(b, &dest->id, source->id);
    CopyString(b, &dest->serviceConfigurationId, source->serviceConfigurationId);
    CopyString(b, &dest->name, source->name);
    CopyArray(b, cbBuffer, &dest->titleAssociations, source->titleAssociations, source->titleAssociationsCount, SizeOfAchievementTitleAssociation, CopyAchievementTitleAssociation);
    dest->progression = CopyAchievementProgression(source->progression, b, cbBuffer);
    CopyArray(b, cbBuffer, &dest->mediaAssets, source->mediaAssets, source->mediaAssetsCount, SizeOfAchievementMediaAsset, CopyAchievementMediaAsset);
    CopyStringArray(b, cbBuffer, &dest->platformsAvailableOn, source->platformsAvailableOn, source->platformsAvailableOnCount);
    CopyString(b, &dest->unlockedDescription, source->unlockedDescription);
    CopyString(b, &dest->lockedDescription, source->lockedDescription);
    CopyString(b, &dest->productId, source->productId);
    dest->available = b.alloc<XBL_ACHIEVEMENT_TIME_WINDOW>(*(source->available));
    CopyArray(b, cbBuffer, &dest->rewards, source->rewards, source->rewardsCount, SizeOfAchievementReward, CopyAchievementReward);
    CopyString(b, &dest->deepLink, source->deepLink);

    return dest;
}

XBL_API XBL_ACHIEVEMENTS_RESULT* XBL_CALLING_CONV
XblCopyAchievementsResult(
    _In_ CONST XBL_ACHIEVEMENTS_RESULT *source,
    _In_ void *buffer,
    _Inout_ uint64_t *cbBuffer
    ) XBL_NOEXCEPT
{
    uint64_t neededBufferSize = sizeof(XBL_ACHIEVEMENTS_RESULT);
    neededBufferSize += SizeOfArray(source->items, source->itemsCount, SizeOfAchievement);

    if (neededBufferSize > *cbBuffer)
    {
        *cbBuffer = neededBufferSize;
        return nullptr;
    }

    buffer_allocator b(buffer, *cbBuffer);
    auto dest = b.alloc<XBL_ACHIEVEMENTS_RESULT>(*source);

    CopyArray(b, cbBuffer, &dest->items, source->items, source->itemsCount, SizeOfAchievement, CopyAchievement);

    return dest;
}

XBL_API XBL_ACHIEVEMENT* XBL_CALLING_CONV
XblCopyAchievement(
    _In_ CONST XBL_ACHIEVEMENT *source,
    _In_ void *buffer,
    _Inout_ uint64_t *cbBuffer
    ) XBL_NOEXCEPT
{
    uint64_t neededBufferSize = SizeOfAchievement(source);

    if (neededBufferSize > *cbBuffer)
    {
        *cbBuffer = neededBufferSize;
        return nullptr;
    }

    buffer_allocator b(buffer, *cbBuffer);
    return CopyAchievement(source, b, cbBuffer);
}