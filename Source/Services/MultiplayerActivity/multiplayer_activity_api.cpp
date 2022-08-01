// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_live_context_internal.h"
#include "multiplayer_activity_internal.h"
#if XSAPI_NOTIFICATION_SERVICE
#include "notification_internal.h"
#endif

using namespace xbox::services::multiplayer_activity;

#if XSAPI_NOTIFICATION_SERVICE
using namespace xbox::services::notification;
#endif

STDAPI XblMultiplayerActivityUpdateRecentPlayers(
    _In_ XblContextHandle xblContextHandle,
    _In_reads_(updatesCount) const XblMultiplayerActivityRecentPlayerUpdate* updates,
    _In_ size_t updatesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr);
    return xblContextHandle->MultiplayerActivityService()->UpdateRecentPlayers(updates, updatesCount);
}
CATCH_RETURN()

STDAPI XblMultiplayerActivityFlushRecentPlayersAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerActivityService()->FlushRecentPlayers(data->async));
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

STDAPI XblMultiplayerActivitySetActivityAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const XblMultiplayerActivityInfo* activityInfo,
    _In_ bool allowCrossPlatformJoin,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || activityInfo == nullptr || activityInfo->connectionString ==  nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            info = ActivityInfo{ activityInfo },
            allowCrossPlatformJoin
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerActivityService()->SetActivity(info, allowCrossPlatformJoin, data->async));
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

STDAPI XblMultiplayerActivityGetActivityAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_reads_(xuidsCount) const uint64_t* xuidsPtr,
    _In_ size_t xuidsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || xuidsPtr == nullptr || xuidsCount == 0);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            xuids = Vector<uint64_t>(xuidsPtr, xuidsPtr + xuidsCount),
            activityInfo = Vector<ActivityInfo>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerActivityService()->GetActivity(xuids, { data->async->queue,
                [
                    &activityInfo,
                    async{ data->async }
                ]
            (Result<Vector<ActivityInfo>> result)
            {
                size_t requiredBufferSize{ 0 };
                if (Succeeded(result))
                {
                    activityInfo = result.ExtractPayload();
                    for (const auto& a : activityInfo)
                    {
                        requiredBufferSize += sizeof(XblMultiplayerActivityInfo);
                        if (!a.connectionString.empty())
                        {
                            requiredBufferSize += (a.connectionString.size() + 1);
                        }
                        if (!a.groupId.empty())
                        {
                            requiredBufferSize += (a.groupId.size() + 1);
                        }
                    }

                    // Add padding to store the arraysize
                    requiredBufferSize += sizeof(size_t);
                }
                XAsyncComplete(async, result.Hresult(), requiredBufferSize);
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto activityCountPtr{ static_cast<size_t*>(data->buffer) };
            auto activityInfoPtr{ reinterpret_cast<XblMultiplayerActivityInfo*>(activityCountPtr + 1) };
            auto stringPtr{ reinterpret_cast<char*>(activityInfoPtr + activityInfo.size()) };
            size_t bufferSize{ sizeof(size_t) };

            *activityCountPtr = activityInfo.size();

            for (const auto& a : activityInfo)
            {
                new (activityInfoPtr) XblMultiplayerActivityInfo
                {
                    a.xuid,
                    nullptr,
                    a.joinRestriction,
                    a.maxPlayers,
                    a.currentPlayers,
                    nullptr,
                    a.platform
                };

                if (!a.connectionString.empty())
                {
                    utils::strcpy(stringPtr, a.connectionString.size() + 1, a.connectionString.data());
                    activityInfoPtr->connectionString = stringPtr;
                    stringPtr += (a.connectionString.size() + 1);
                    bufferSize += (a.connectionString.size() + 1);
                }

                if (!a.groupId.empty())
                {
                    utils::strcpy(stringPtr, a.groupId.size() + 1, a.groupId.data());
                    activityInfoPtr->groupId = stringPtr;
                    stringPtr += (a.groupId.size() + 1);
                    bufferSize += (a.groupId.size() + 1);
                }

                bufferSize += sizeof(XblMultiplayerActivityInfo);
                ++activityInfoPtr;
            }
            assert(static_cast<char*>(data->buffer) + bufferSize == stringPtr);

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

STDAPI XblMultiplayerActivityGetActivityResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT
try
{
    return XAsyncGetResultSize(async, resultSizeInBytes);
}
CATCH_RETURN()

STDAPI XblMultiplayerActivityGetActivityResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMultiplayerActivityInfo** results,
    _Out_ size_t* resultCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(results == nullptr || resultCount == nullptr);

    auto hr = XAsyncGetResult(async, nullptr, bufferSize, buffer, bufferUsed);
    if (SUCCEEDED(hr))
    {
        auto resultCountPtr{ static_cast<size_t*>(buffer) };
        *resultCount = *resultCountPtr;
        *results = reinterpret_cast<XblMultiplayerActivityInfo*>(resultCountPtr + 1);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerActivityDeleteActivityAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContextHandle);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerActivityService()->DeleteActivity(data->async));
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

STDAPI XblMultiplayerActivitySendInvitesAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ const uint64_t* xuidsPtr,
    _In_ size_t xuidsCount,
    _In_ bool allowCrossPlatformJoin,
    _In_opt_z_ const char* _connectionString,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || xuidsPtr == nullptr || xuidsCount == 0);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            xuids{ Vector<uint64_t>(xuidsPtr, xuidsPtr + xuidsCount) },
            allowCrossPlatformJoin,
            connectionString = _connectionString ? String{ _connectionString } : String{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerActivityService()->SendInvites(xuids, allowCrossPlatformJoin, connectionString, data->async));
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

#if (HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL) && !XSAPI_UNIT_TESTS
STDAPI_(XblFunctionContext) XblMultiplayerActivityAddInviteHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerActivityInviteHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    if (xblContext == nullptr || handler == nullptr)
    {
        return XblFunctionContext{ 0 };
    }

    auto rtaNotificationService = std::dynamic_pointer_cast<RTANotificationService>(xblContext->NotificationService());
    return rtaNotificationService->AddGameInviteHandler(NotificationSubscription::MultiplayerActivityInviteHandler{
        [
            handler, context
        ]
    (const notification::MultiplayerActivityInviteData& args)
    {
        try
        {
            handler(&args, context);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    } 
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerActivityRemoveInviteHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xblContext);
    auto rtaNotificationService = std::dynamic_pointer_cast<RTANotificationService>(xblContext->NotificationService());
    rtaNotificationService->RemoveNotificationHandler(token);
    return S_OK;
}
CATCH_RETURN()
#endif
