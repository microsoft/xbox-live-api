// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/achievements_c.h"
#include "Achievements/achievements_internal.h"
#include "xbox_live_app_config_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::achievements;
using namespace xbox::services::system;

STDAPI XblAchievementsGetAchievementsForTitleIdAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ uint32_t titleId,
    _In_ XblAchievementType type,
    _In_ bool unlockedOnly,
    _In_ XblAchievementOrderBy orderBy,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() },
            xboxUserId,
            titleId,
            type,
            unlockedOnly,
            orderBy,
            skipItems,
            maxItems,
            achievmentsResult{ std::shared_ptr<XblAchievementsResult>() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->AchievementsService()->GetAchievementsForTitle(
                xboxUserId,
                titleId,
                type,
                unlockedOnly,
                orderBy,
                skipItems,
                maxItems,
                AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>>{ data->async->queue,
                [
                    &achievmentsResult,
                    async{ data->async }
                ]
            (Result<std::shared_ptr<XblAchievementsResult>> result)
            {
                if (Succeeded(result))
                {
                    achievmentsResult = result.ExtractPayload();
                }
                XAsyncComplete(async, result.Hresult(), sizeof(XblAchievementsResultHandle));
            }
            }));

            return E_PENDING;
        }

        case XAsyncOp::GetResult:
        {
            auto resultHandle = static_cast<XblAchievementsResultHandle*>(data->buffer);
            *resultHandle = achievmentsResult.get();
            achievmentsResult->AddRef();
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblAchievementsGetAchievementsForTitleIdResult(
    _In_ XAsyncBlock* async,
    _Out_ XblAchievementsResultHandle* result
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblAchievementsResultHandle), result, nullptr);
}
CATCH_RETURN()

STDAPI XblAchievementsResultGetAchievements(
    _In_ XblAchievementsResultHandle result,
    _Out_ const XblAchievement** achievements,
    _Out_ size_t* achievementsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(achievementsCount == nullptr || achievements == nullptr || result == nullptr);
    VERIFY_XBL_INITIALIZED();

    *achievementsCount = result->Achievements().size();
    *achievements = result->Achievements().data();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblAchievementsResultHasNext(
    _In_ XblAchievementsResultHandle result,
    _Out_ bool* hasNext
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(result == nullptr || hasNext == nullptr);
    VERIFY_XBL_INITIALIZED();
    *hasNext = result->HasNext();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblAchievementsResultGetNextAsync(
    _In_ XblAchievementsResultHandle result,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    VERIFY_XBL_INITIALIZED();

    return RunAsync(async, __FUNCTION__,
        [
            inputResult{ result->shared_from_this() },
            maxItems,
            outputResult{ std::shared_ptr<XblAchievementsResult>() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(inputResult->GetNext(
                maxItems,
                AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>>{ data->async->queue,
                [
                    &outputResult,
                    async{ data->async }
                ]
            (Result<std::shared_ptr<XblAchievementsResult>> result)
            {
                if (Succeeded(result))
                {
                    outputResult = result.ExtractPayload();
                }
                XAsyncComplete(async, result.Hresult(), sizeof(XblAchievementsResultHandle));
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto resultHandle = static_cast<XblAchievementsResultHandle*>(data->buffer);
            *resultHandle = outputResult.get();
            outputResult->AddRef();
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblAchievementsResultGetNextResult(
    _In_ XAsyncBlock* async,
    _Out_ XblAchievementsResultHandle* result
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblAchievementsResultHandle), result, nullptr);
}
CATCH_RETURN()

STDAPI XblAchievementsUpdateAchievementAsyncInternal(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ uint64_t xboxUserId,
    _In_ uint32_t titleId,
    _In_opt_z_ const char* serviceConfigurationId,
    _In_z_ const char* achievementId,
    _In_ uint32_t percentComplete,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(achievementId);
    VERIFY_XBL_INITIALIZED();

    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext{ xboxLiveContextHandle->shared_from_this() },
            xboxUserId,
            titleId,
            scid = String{ serviceConfigurationId ? serviceConfigurationId : "" },
            achievementId = String{ achievementId },
            percentComplete
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            if (titleId == 0)
            {
                RETURN_HR_IF_FAILED(xboxLiveContext->AchievementsService()->UpdateAchievement(
                    xboxUserId,
                    achievementId,
                    percentComplete,
                    data->async
                ));
            }
            else
            {
                RETURN_HR_IF_FAILED(xboxLiveContext->AchievementsService()->UpdateAchievement(
                    xboxUserId,
                    titleId,
                    scid,
                    achievementId,
                    percentComplete,
                    data->async
                ));
            }

            return E_PENDING;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblAchievementsUpdateAchievementAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* achievementId,
    _In_ uint32_t percentComplete,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(achievementId);

    return XblAchievementsUpdateAchievementAsyncInternal(
        xboxLiveContext, 
        xboxUserId, 
        AppConfig::Instance()->TitleId(),
        AppConfig::Instance()->Scid().c_str(),
        achievementId, 
        percentComplete, 
        async
    );
}
CATCH_RETURN()

STDAPI XblAchievementsUpdateAchievementForTitleIdAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ const uint32_t titleId,
    _In_z_ const char* serviceConfigurationId,
    _In_z_ const char* achievementId,
    _In_ uint32_t percentComplete,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(achievementId);

    return XblAchievementsUpdateAchievementAsyncInternal(xboxLiveContext, xboxUserId, titleId, serviceConfigurationId, achievementId, percentComplete, async);
}
CATCH_RETURN()


STDAPI XblAchievementsGetAchievementAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ uint64_t xboxUserId,
    _In_z_ const char* serviceConfigurationId,
    _In_z_ const char* achievementId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    VERIFY_XBL_INITIALIZED();
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContextHandle);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(achievementId);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);

    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext{ xboxLiveContextHandle->shared_from_this() },
            xboxUserId,
            scid = String{ serviceConfigurationId },
            achievementId = String{ achievementId },
            achievementResult = std::shared_ptr<XblAchievementsResult>{ nullptr }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xboxLiveContext->AchievementsService()->GetAchievement(
                xboxUserId,
                scid,
                achievementId,
                AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>>{ data->async->queue,
                [
                    &achievementResult,
                    async{ data->async }
                ]
            (Result<std::shared_ptr<XblAchievementsResult>> result)
            {
                if (Succeeded(result))
                {
                    achievementResult = result.ExtractPayload();
                }
                XAsyncComplete(async, result.Hresult(), sizeof(XblAchievementsResultHandle));
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto resultHandle = static_cast<XblAchievementsResultHandle*>(data->buffer);
            *resultHandle = achievementResult.get();
            achievementResult->AddRef();
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblAchievementsGetAchievementResult(
    _In_ XAsyncBlock* async,
    _Out_ XblAchievementsResultHandle* result
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblAchievementsResultHandle), result, nullptr);
}
CATCH_RETURN()

STDAPI XblAchievementsResultDuplicateHandle(
    _In_ XblAchievementsResultHandle handle,
    _Out_ XblAchievementsResultHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || duplicatedHandle == nullptr);

    handle->AddRef();
    *duplicatedHandle = handle;

    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblAchievementsResultCloseHandle(
    _In_ XblAchievementsResultHandle handle
) XBL_NOEXCEPT
try
{
    if (handle)
    {
        handle->DecRef();
    }
}
CATCH_RETURN_WITH(;)

STDAPI_(XblFunctionContext) XblAchievementsAddAchievementProgressChangeHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblAchievementsProgressChangeHandler handler,
    _In_opt_ void* handlerContext
) XBL_NOEXCEPT
try
{
    if (xblContextHandle == nullptr || handler == nullptr)
    {
        return XblFunctionContext{ 0 };
    }

    return xblContextHandle->AchievementsService()->AddAchievementProgressChangeHandler(
        [
            handler,
            handlerContext
        ]
    (const XblAchievementProgressChangeEventArgs& args)
    {
        try
        {
            handler(&args, handlerContext);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    });
}
CATCH_RETURN()

STDAPI XblAchievementsRemoveAchievementProgressChangeHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext functionContext
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    xboxLiveContext->AchievementsService()->RemoveAchievementProgressChangeHandler(functionContext);
    return S_OK;
}
CATCH_RETURN()
