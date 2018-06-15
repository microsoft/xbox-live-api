// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/achievements_c.h"
#include "achievements/achievements_internal.h"
#include "xbox_live_context_internal_c.h"
#include "achievements_internal_c.h"

using namespace xbox::services;
using namespace xbox::services::achievements;
using namespace xbox::services::system;

STDAPI XblAchievementsGetAchievementsForTitleIdAsync(
    _Inout_ AsyncBlock* async,
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
        xbl_achievements_result_handle resultHandle;
    };
    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context{};
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
        {
            auto result = context->xboxLiveContext->contextImpl->achievement_service_internal()->get_achievements_for_title_id(
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
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                if (SUCCEEDED(hr))
                {
                    context->resultHandle = new (xsapi_memory::mem_alloc(sizeof(xbl_achievements_result))) xbl_achievements_result(result.payload());
                }
                CompleteAsync(data->async, hr, sizeof(xbl_achievements_result_handle));
            });

            if (result.err())
            {
                CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(result.err()), 0);
            }
            return E_PENDING;
        }

        case AsyncOp_GetResult:
            memcpy(data->buffer, &context->resultHandle, sizeof(xbl_achievements_result_handle));
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

STDAPI XblAchievementsGetAchievementsForTitleIdResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_achievements_result_handle* result
    ) XBL_NOEXCEPT
try
{
    return GetAsyncResult(async, nullptr, sizeof(xbl_achievements_result_handle), result, nullptr);
}
CATCH_RETURN()

STDAPI XblAchievementsResultGetAchievements(
    _In_ xbl_achievements_result_handle resultHandle,
    _Out_ XblAchievement** achievements,
    _Out_ uint32_t* achievementsCount
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(achievementsCount == nullptr || achievements == nullptr || resultHandle == nullptr);
    verify_global_init();

    *achievementsCount = static_cast<uint32_t>(resultHandle->items.size());
    *achievements = resultHandle->items.data();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblAchievementsResultHasNext(
    _In_ xbl_achievements_result_handle resultHandle,
    _Out_ bool* hasNext
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(resultHandle == nullptr || hasNext == nullptr);
    verify_global_init();
    *hasNext = resultHandle->internalResult != nullptr && resultHandle->internalResult->has_next();
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblAchievementsResultGetNextAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ xbl_achievements_result_handle resultHandle,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    struct Context
    {
        xbl_achievements_result_handle inputResultHandle;
        xbl_context_handle xboxLiveContext;
        uint32_t maxItems;
        xbl_achievements_result_handle outputResultHandle;
    };
    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context{};
    context->xboxLiveContext = xboxLiveContext;
    context->maxItems = maxItems;
    context->inputResultHandle = resultHandle;

    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
        case AsyncOp_DoWork:
        {
            auto result = context->inputResultHandle->internalResult->get_next(context->maxItems, data->async->queue,
                [data, context](xbox_live_result<std::shared_ptr<achievements_result_internal>> result)
            {
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                if (SUCCEEDED(hr))
                {
                    context->outputResultHandle = new (xsapi_memory::mem_alloc(sizeof(xbl_achievements_result))) xbl_achievements_result(result.payload());
                }
                CompleteAsync(data->async, hr, sizeof(xbl_achievements_result_handle));
            });

            if (result.err())
            {
                CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(result.err()), 0);
            }
            return E_PENDING;
        }

        case AsyncOp_GetResult:
            memcpy(data->buffer, &context->outputResultHandle, sizeof(xbl_achievements_result_handle));
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

STDAPI XblAchievementsResultGetNextResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_achievements_result_handle* result
    ) XBL_NOEXCEPT
{
    return GetAsyncResult(async, nullptr, sizeof(xbl_achievements_result_handle), result, nullptr);
}

STDAPI XblAchievementsUpdateAchievementAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_opt_ const uint32_t* titleId,
    _In_opt_z_ const char* serviceConfigurationId,
    _In_z_ const char* achievementId,
    _In_ uint32_t percentComplete
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    struct Context
    {
        xbl_context_handle xboxLiveContext;
        xsapi_internal_string xboxUserId;
        const uint32_t* titleId;
        xsapi_internal_string serviceConfigurationId;
        xsapi_internal_string achievementId;
        uint32_t percentComplete;
    };

    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context;
    context->xboxLiveContext = xboxLiveContext;
    context->xboxUserId = utils::uint64_to_internal_string(xboxUserId);
    context->titleId = titleId;
    if (serviceConfigurationId != nullptr)
    {
        context->serviceConfigurationId = serviceConfigurationId;
    }
    context->achievementId = achievementId;
    context->percentComplete = percentComplete;

    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
        case AsyncOp_DoWork:
        {
            xbox_live_result<void> result;

            if (context->titleId == nullptr)
            {
                result = context->xboxLiveContext->contextImpl->achievement_service_internal()->update_achievement(
                    context->xboxUserId,
                    context->achievementId,
                    context->percentComplete,
                    data->async->queue,
                    [data, context](xbox_live_result<void> result)
                {
                    CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(result.err()), 0);
                });
            }
            else
            {
                result = context->xboxLiveContext->contextImpl->achievement_service_internal()->update_achievement(
                    context->xboxUserId,
                    *(context->titleId),
                    context->serviceConfigurationId,
                    context->achievementId,
                    context->percentComplete,
                    data->async->queue,
                    [data, context](xbox_live_result<void> result)
                {
                    CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(result.err()), 0);
                });
            }

            if (result.err())
            {
                CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(result.err()), 0);
            }
            return E_PENDING;
        }

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

STDAPI XblAchievementsGetAchievementAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* serviceConfigurationId,
    _In_z_ const char* achievementId
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
        xbl_achievements_result_handle resultHandle;
    };
    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context{};
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
        {
            auto result = context->xboxLiveContext->contextImpl->achievement_service_internal()->get_achievement(
                context->xboxUserId,
                context->serviceConfigurationId,
                context->achievementId,
                data->async->queue,
                [data, context](xbox_live_result<std::shared_ptr<achievement_internal>> result)
            {
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                if (SUCCEEDED(hr))
                {
                    context->resultHandle = new (xsapi_memory::mem_alloc(sizeof(xbl_achievements_result))) xbl_achievements_result(result.payload());
                }
                CompleteAsync(data->async, hr, sizeof(xbl_achievements_result_handle));
            });

            if (result.err())
            {
                CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(result.err()), 0);
            }
            return E_PENDING;
        }

        case AsyncOp_GetResult:
            XSAPI_ASSERT(data->bufferSize == sizeof(xbl_achievements_result_handle));
            memcpy(data->buffer, &context->resultHandle, sizeof(xbl_achievements_result_handle));
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
    _Inout_ AsyncBlock* async,
    _Out_ xbl_achievements_result_handle* result
    ) XBL_NOEXCEPT
try
{
    return GetAsyncResult(async, nullptr, sizeof(xbl_achievements_result_handle), result, nullptr);
}
CATCH_RETURN()

STDAPI_(xbl_achievements_result_handle) XblAchievementsResultDuplicateHandle(
    _In_ xbl_achievements_result_handle handle
    ) XBL_NOEXCEPT
try
{
    if (handle == nullptr)
    {
        return nullptr;
    }

    handle->refCount++;
    return handle;
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(void) XblAchievementsResultCloseHandle(
    _In_ xbl_achievements_result_handle handle
    ) XBL_NOEXCEPT
try
{
    int refCount = --handle->refCount;
    if (refCount <= 0)
    {
        assert(refCount == 0);
        handle->~xbl_achievements_result();
        xsapi_memory::mem_free(handle);
    }
}
CATCH_RETURN_WITH(;)