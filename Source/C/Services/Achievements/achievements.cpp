// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/achievements_c.h"
#include "achievements_helper.h"
#include "achievements_taskargs.h"
#include "achievements_state.h"
#include "xbox_live_context_impl_c.h"

using namespace xbox::services;
using namespace xbox::services::achievements;

XBL_API bool XBL_CALLING_CONV
AchievementsResultHasNext(
    _In_ XSAPI_ACHIEVEMENTS_RESULT* achievementsResult
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    return achievementsResult->pImpl->cppAchievementsResult().has_next();
}
CATCH_RETURN_WITH(false)

HC_RESULT get_next_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    auto args = reinterpret_cast<get_next_taskargs*>(context);
    xbox_live_result<achievements_result> result = args->achievementsResult->pImpl->cppAchievementsResult().get_next(args->maxItems).get();
    args->copy_xbox_live_result(result);

    if (!result.err())
    {
        args->completionRoutinePayload = CreateAchievementsResultFromCpp(result.payload());
    }

    return HCTaskSetCompleted(taskHandle);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
AchievementsResultGetNext(
    _In_ XSAPI_ACHIEVEMENTS_RESULT* achievementsResult,
    _In_ uint32_t maxItems,
    _In_ XSAPI_GET_NEXT_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto args = new get_next_taskargs();
    args->achievementsResult = achievementsResult;
    args->maxItems = maxItems;

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            get_next_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine_with_payload<get_next_taskargs, XSAPI_GET_NEXT_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()

HC_RESULT update_achievement_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    auto args = reinterpret_cast<update_achievement_taskargs*>(context);
    auto achievementService = args->pXboxLiveContext->pImpl->cppObject()->achievement_service();

    xbox_live_result<void> result;
    
    if (args->titleId == nullptr)
    {
        result = achievementService.update_achievement(args->xboxUserId, 
                                                       args->achievementId, 
                                                       args->percentComplete).get();
    }
    else
    {
        result = achievementService.update_achievement(args->xboxUserId, 
                                                       *args->titleId, 
                                                       args->serviceConfigurationId, 
                                                       args->achievementId, 
                                                       args->percentComplete).get();
    }

    args->copy_xbox_live_result(result);

    return HCTaskSetCompleted(taskHandle);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
AchievementServiceUpdateAchievement(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_opt_ uint32_t* titleId,
    _In_opt_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ uint32_t percentComplete,
    _In_ XSAPI_UPDATE_ACVHIEVEMENT_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto args = new update_achievement_taskargs();
    args->pXboxLiveContext = pContext;
    args->xboxUserId = utils_c::to_utf16string(xboxUserId);
    args->titleId = titleId;
    args->serviceConfigurationId = utils_c::to_utf16string(serviceConfigurationId);
    args->achievementId = utils_c::to_utf16string(achievementId);
    args->percentComplete = percentComplete;

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            update_achievement_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine<update_achievement_taskargs, XSAPI_UPDATE_ACVHIEVEMENT_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()

HC_RESULT get_achievements_for_title_id_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    auto args = reinterpret_cast<get_achievement_for_title_id_taskargs*>(context);
    auto achievementService = args->pXboxLiveContext->pImpl->cppObject()->achievement_service();

    auto result = achievementService.get_achievements_for_title_id(args->xboxUserId, 
                                                                   args->titleId, 
                                                                   args->type, 
                                                                   args->unlockedOnly, 
                                                                   args->orderBy, 
                                                                   args->skipItems, 
                                                                   args->maxItems).get();

    args->copy_xbox_live_result(result);

    if (!result.err())
    {
        args->completionRoutinePayload = CreateAchievementsResultFromCpp(result.payload());
    }

    return HCTaskSetCompleted(taskHandle);
}
XBL_API XSAPI_RESULT XBL_CALLING_CONV
AchievementServiceGetAchievementsForTitleId(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_ uint32_t titleId,
    _In_ XSAPI_ACHIEVEMENT_TYPE type,
    _In_ bool unlockedOnly,
    _In_ XSAPI_ACIEVEMENT_ORDER_BY orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ XSAPI_GET_ACHIEVEMENTS_FOR_TITLE_ID_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto args = new get_achievement_for_title_id_taskargs();
    args->xboxUserId = utils_c::to_utf16string(xboxUserId);
    args->titleId = titleId;
    args->type = static_cast<achievement_type>(type);
    args->unlockedOnly = unlockedOnly;
    args->orderBy = static_cast<achievement_order_by>(orderBy);
    args->skipItems = skipItems;
    args->maxItems = maxItems;

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            update_achievement_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine_with_payload<get_achievement_for_title_id_taskargs, XSAPI_GET_ACHIEVEMENTS_FOR_TITLE_ID_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()

HC_RESULT get_achievement_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    auto args = reinterpret_cast<get_achievement_taskargs*>(context);
    auto achievementService = args->pXboxLiveContext->pImpl->cppObject()->achievement_service();

    auto result = achievementService.get_achievement(args->xboxUserId,
            args->serviceConfigurationId,
            args->achievementId).get();

    args->copy_xbox_live_result(result);

    if (!result.err())
    {
        args->completionRoutinePayload = CreateAchievementFromCpp(result.payload());
    }

    return HCTaskSetCompleted(taskHandle);
}
XBL_API XSAPI_RESULT XBL_CALLING_CONV
AchievementServiceGetAchievement(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_ PCSTR serviceConfigurationId,
    _In_ PCSTR achievementId,
    _In_ XSAPI_GET_ACHIEVEMENT_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    // Task args are cleaned up in write results routine. TODO add similar comment elsewhere.
    auto args = new get_achievement_taskargs();
    args->pXboxLiveContext = pContext;
    args->xboxUserId = utils_c::to_utf16string(xboxUserId);
    args->serviceConfigurationId = utils_c::to_utf16string(serviceConfigurationId);
    args->achievementId = utils_c::to_utf16string(achievementId);

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            update_achievement_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine_with_payload<get_achievement_taskargs, XSAPI_GET_ACHIEVEMENT_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()


XBL_API XSAPI_RESULT XBL_CALLING_CONV
AchievementServiceReleaseAchievementsResult(
    _In_ XSAPI_ACHIEVEMENTS_RESULT* achievementsResult
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_achievementsState->m_lock);

    size_t erasedItems = singleton->m_achievementsState->m_achievementResults.erase(achievementsResult);
    if (erasedItems > 0 && achievementsResult->pImpl != nullptr)
    {
        delete achievementsResult->pImpl;
    }
    return XSAPI_RESULT_OK;
}
CATCH_RETURN()


XBL_API XSAPI_RESULT XBL_CALLING_CONV
AchievementServiceReleaseAchievement(
    _In_ XSAPI_ACHIEVEMENT* achievement
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_achievementsState->m_lock);

    size_t erasedItems = singleton->m_achievementsState->m_achievements.erase(achievement);
    if (erasedItems > 0 && achievement->pImpl != nullptr)
    {
        delete achievement->pImpl;
    }
    return XSAPI_RESULT_OK;
}
CATCH_RETURN()