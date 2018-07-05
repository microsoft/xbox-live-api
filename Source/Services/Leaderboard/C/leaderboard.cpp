// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/leaderboard_c.h"
#include "xbox_live_context_internal_c.h"
#include "leaderboard_internal_c.h"
#include "../leaderboard_service_impl.h"

using namespace xbox::services;
using namespace xbox::services::leaderboard;
using namespace xbox::services::system;

// Forward
STDAPI XblLeaderboardResultGetAsyncHelper(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblLeaderboard2017Query* leaderboardQuery2017,
    _In_ XblLeaderboard2013Query* leaderboardQuery2013,
    _In_ xbl_leaderboard_result_handle inputHandle,
    _In_ uint32_t maxItems
    );

STDAPI XblLeaderboardGetLeaderboard2017GetResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_leaderboard_result_handle* handle
    ) XBL_NOEXCEPT
try
{
    return GetAsyncResult(async, nullptr, sizeof(xbl_leaderboard_result_handle), handle, nullptr);
}
CATCH_RETURN()

STDAPI XblLeaderboardGetLeaderboard2013GetResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_leaderboard_result_handle* handle
    ) XBL_NOEXCEPT
try
{
    return GetAsyncResult(async, nullptr, sizeof(xbl_leaderboard_result_handle), handle, nullptr);
}
CATCH_RETURN()

STDAPI XblLeaderboardResultHasNext(
    _In_ xbl_leaderboard_result_handle resultHandle,
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

STDAPI XblLeaderboardResultGetNextResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_leaderboard_result_handle* handle
    ) XBL_NOEXCEPT
try
{
    return GetAsyncResult(async, nullptr, sizeof(xbl_leaderboard_result_handle), handle, nullptr);
}
CATCH_RETURN()

STDAPI XblLeaderboardResultGetRows(
    _In_ xbl_leaderboard_result_handle resultHandle,
    _Out_ XblLeaderboardRow** rows,
    _Out_ uint32_t* rowCount
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(rowCount == nullptr || rows == nullptr || resultHandle == nullptr);
    verify_global_init();

    *rowCount = static_cast<uint32_t>(resultHandle->rows.size());
    *rows = resultHandle->rows.data();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblLeaderboardResultGetColumns(
    _In_ xbl_leaderboard_result_handle resultHandle,
    _Out_ XblLeaderboardColumn** columns,
    _Out_ uint32_t* columnCount
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(columnCount == nullptr || columns == nullptr || resultHandle == nullptr);
    verify_global_init();

    *columnCount = static_cast<uint32_t>(resultHandle->columns.size());
    *columns = resultHandle->columns.data();
    return S_OK;
}
CATCH_RETURN()

STDAPI_(xbl_leaderboard_result_handle) XblLeaderboardResultDuplicateHandle(
    _In_ xbl_leaderboard_result_handle handle
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

STDAPI_(void) XblLeaderboardResultCloseHandle(
    _In_ xbl_leaderboard_result_handle handle
    ) XBL_NOEXCEPT
try
{
    int refCount = --handle->refCount;
    if (refCount <= 0)
    {
        assert(refCount == 0);
        handle->~xbl_leaderboard_result();
        xsapi_memory::mem_free(handle);
    }
}
CATCH_RETURN_WITH(;)


STDAPI XblLeaderboardGetLeaderboard2017Async(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblLeaderboard2017Query leaderboardQuery
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || async == nullptr);
    return XblLeaderboardResultGetAsyncHelper(async, xboxLiveContext, &leaderboardQuery, nullptr, nullptr, 0);
}
CATCH_RETURN()

STDAPI XblLeaderboardGetLeaderboard2013Async(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblLeaderboard2013Query leaderboardQuery
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || async == nullptr);
    return XblLeaderboardResultGetAsyncHelper(async, xboxLiveContext, nullptr, &leaderboardQuery, nullptr, 0);
}
CATCH_RETURN()

STDAPI XblLeaderboardResultGetNextAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ xbl_leaderboard_result_handle resultHandle,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || async == nullptr || resultHandle == nullptr);
    return XblLeaderboardResultGetAsyncHelper(async, xboxLiveContext, nullptr, nullptr, resultHandle, maxItems);
}
CATCH_RETURN()


STDAPI XblLeaderboardResultGetAsyncHelper(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblLeaderboard2017Query* leaderboardQuery2017,
    _In_ XblLeaderboard2013Query* leaderboardQuery2013,
    _In_ xbl_leaderboard_result_handle inputHandle,
    _In_ uint32_t maxItems
    ) 
try
{
    verify_global_init();
 
    struct Context
    {
        xbl_context_handle xboxLiveContext;

        // 2013 & 2017
        leaderboard_version version;
        uint64_t xboxUserId;
        xsapi_internal_string scid;
        xsapi_internal_string leaderboardName;
        xsapi_internal_string statName;
        XblSocialGroupType socialGroup;
        xsapi_internal_vector<xsapi_internal_string> additionalColumnleaderboardNames;
        XblLeaderboardSortOrder order;
        uint32_t maxItems;
        uint64_t skipToXboxUserId;
        uint32_t skipResultToRank;
        xsapi_internal_string continuationToken;

        // Input handle
        xbl_leaderboard_result_handle inputHandle;

        xbl_leaderboard_result_handle resultHandle;
    };
    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context{};
    context->xboxLiveContext = xboxLiveContext;
    if (leaderboardQuery2013 != nullptr)
    {
        context->version = leaderboard_version::leaderboard_version_2013;
        context->xboxUserId = leaderboardQuery2013->xboxUserId;
        context->scid = leaderboardQuery2013->scid;
        context->leaderboardName = leaderboardQuery2013->leaderboardName;
        context->statName = leaderboardQuery2013->statName;
        context->socialGroup = leaderboardQuery2013->socialGroup;
        context->additionalColumnleaderboardNames =
            utils::string_array_to_internal_string_vector(
                leaderboardQuery2013->additionalColumnleaderboardNames,
                leaderboardQuery2013->additionalColumnleaderboardNameCount);
        context->order = leaderboardQuery2013->order;
        context->maxItems = leaderboardQuery2013->maxItems;
        context->skipToXboxUserId = leaderboardQuery2013->skipToXboxUserId;
        context->skipResultToRank = leaderboardQuery2013->skipResultToRank;
        context->continuationToken = leaderboardQuery2013->continuationToken;
    }
    else if (leaderboardQuery2017 != nullptr)
    {
        context->version = leaderboard_version::leaderboard_version_2017;
        context->xboxUserId = leaderboardQuery2017->xboxUserId;
        context->scid = leaderboardQuery2017->scid;
        context->statName = leaderboardQuery2017->statName;
        context->socialGroup = leaderboardQuery2017->socialGroup;
        context->order = leaderboardQuery2017->order;
        context->maxItems = leaderboardQuery2017->maxItems;
        context->skipToXboxUserId = leaderboardQuery2017->skipToXboxUserId;
        context->skipResultToRank = leaderboardQuery2017->skipResultToRank;
        context->continuationToken = leaderboardQuery2017->continuationToken;
    }
    else
    {
        context->inputHandle = inputHandle;
        context->maxItems = maxItems;
    }

    HRESULT hr1 = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
            case AsyncOp_DoWork:
            {
                HRESULT hr2 = context->xboxLiveContext->contextImpl->leaderboard_service_impl()->get_leaderboard_internal(
                    context->scid,
                    context->leaderboardName,
                    context->skipResultToRank,
                    context->skipToXboxUserId,
                    context->xboxUserId,
                    context->socialGroup,
                    context->maxItems,
                    context->continuationToken,
                    context->additionalColumnleaderboardNames,
                    context->version,
                    data->async->queue,
                    [data, context](xbox_live_result<std::shared_ptr<leaderboard_result_internal>> result)
                {
                    auto hr3 = utils::convert_xbox_live_error_code_to_hresult(result.err());
                    if (SUCCEEDED(hr3))
                    {
                        context->resultHandle = new (xsapi_memory::mem_alloc(sizeof(xbl_leaderboard_result))) xbl_leaderboard_result(result.payload());
                    }
                    CompleteAsync(data->async, hr3, sizeof(xbl_leaderboard_result_handle));
                });

                if (FAILED(hr2))
                {
                    return hr2;
                }
                return E_PENDING;
            }

            case AsyncOp_GetResult:
            {
                memcpy(data->buffer, &context->resultHandle, sizeof(xbl_leaderboard_result_handle));
                break;
            }

            case AsyncOp_Cleanup:
            {
                context->~Context();
                xsapi_memory::mem_free(context);
                break;
            }
        }
        return S_OK;
    });

    if (FAILED(hr1))
    {
        return hr1;
    }
    return ScheduleAsync(async, 0);
}
CATCH_RETURN()
