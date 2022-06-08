// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "notification_internal.h"
#include "multiplayer_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::notification;
#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID
STDAPI XblNotificationSubscribeToNotificationsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XAsyncBlock* asyncBlock,
    _In_ const char* registrationToken
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(registrationToken == nullptr || strlen(registrationToken) <= 1);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(asyncBlock);

    return RunAsync(asyncBlock, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() },
            registrationToken
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
             auto result = xblContext->NotificationService()->RegisterWithNotificationService(
                registrationToken,
                {
                    data->async->queue,
                    [
                        async{ data->async }
                    ]
                    (HRESULT hr)
                    {
                        XAsyncComplete(async, hr, 0);
                    }
             });
            
             if (FAILED(result))
             {
                 return result;
             }
             return E_PENDING;
        }
        default: return S_OK;
        }
    });
}
CATCH_RETURN()

/// <summary>
/// Unsubscribes title from push notifications.
/// </summary>
STDAPI XblNotificationUnsubscribeFromNotificationsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(asyncBlock);

    return RunAsync(asyncBlock, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            auto result = xblContext->NotificationService()->UnregisterFromNotificationService(
                {
                       data->async->queue,
                       [
                           async{ data->async }
                       ]
                       (HRESULT hr)
                       {
                           XAsyncComplete(async, hr, 0);
                       }
                });

            if (FAILED(result))
            {
                return result;
            }
            return E_PENDING;
        }
        default: return S_OK;
        }
    });
}
CATCH_RETURN()

#elif HC_PLATFORM == HC_PLATFORM_UWP

STDAPI XblNotificationSubscribeToNotificationsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(asyncBlock);

    return RunAsync(asyncBlock, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            auto result = xblContext->NotificationService()->RegisterWithNotificationService({
                data->async->queue,
                [
                    async{ data->async }
                ]
                (HRESULT hr)
                {
                    XAsyncComplete(async, hr, 0);
                }
            });

            if (FAILED(result))
            {
                return result;
            }
            return E_PENDING;
        }
        default: return S_OK;
        }
    });
}
CATCH_RETURN()

/// <summary>
/// Ubsubscribes title from push notifications.
/// </summary>
STDAPI XblNotificationUnsubscribeFromNotificationsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(asyncBlock);

    return RunAsync(asyncBlock, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            auto result = xblContext->NotificationService()->UnregisterFromNotificationService({
                data->async->queue,
                [
                    async{ data->async }
                ]
                (HRESULT hr)
                {
                    XAsyncComplete(async, hr, 0);
                }
            });

            if (FAILED(result))
            {
                return result;
            }

            return E_PENDING;
        }
        default: return S_OK;
        }
    });
}
CATCH_RETURN()

#elif HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

STDAPI XblGameInviteRegisterForEventAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::Begin:
        {
            XAsyncComplete(data->async, S_OK, sizeof(XblRealTimeActivitySubscriptionHandle));
            return S_OK;
        }
        case XAsyncOp::GetResult:
        {
            // Return a dummy subscription handle that is cleaned up when client unsubscribes
            auto handlePtr = static_cast<XblRealTimeActivitySubscriptionHandle*>(data->buffer);
            *handlePtr = Make<XblRealTimeActivitySubscription>();
            return S_OK;
        }
        default: return S_OK;
        }
    });
}
CATCH_RETURN()

STDAPI XblGameInviteRegisterForEventResult(
    _In_ XAsyncBlock* async,
    _Out_ XblRealTimeActivitySubscriptionHandle* handle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(handle);

    XblRealTimeActivitySubscriptionHandle handleCopy{ nullptr };
    auto hr = XAsyncGetResult(async, nullptr, sizeof(XblRealTimeActivitySubscriptionHandle), &handleCopy, nullptr);

    if (SUCCEEDED(hr))
    {
        *handle = handleCopy;
    }
    else
    {
        *handle = nullptr;
    }

    return hr;
}
CATCH_RETURN()

STDAPI XblGameInviteUnregisterForEventAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContextHandle == nullptr || subscriptionHandle == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            subscriptionHandle
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::Begin:
        {
            Delete(subscriptionHandle);
            XAsyncComplete(data->async, S_OK, 0);
            return S_OK;
        }
        default: return S_OK;
        }
    });
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblGameInviteAddNotificationHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblGameInviteHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    // TODO really should be returning HRESULT E_INVALIDARG here
    if (xblContextHandle == nullptr || handler == nullptr)
    {
        return XblFunctionContext{ 0 };
    }

    auto rtaNotificationService = std::dynamic_pointer_cast<RTANotificationService>(xblContextHandle->NotificationService());
    return rtaNotificationService->AddGameInviteHandler(NotificationSubscription::MPSDInviteHandler{
        [
            handler, context
        ]
    (const GameInviteNotificationEventArgs& args)
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

STDAPI_(void) XblGameInviteRemoveNotificationHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    if (xblContextHandle)
    {
        auto rtaNotificationService = std::dynamic_pointer_cast<RTANotificationService>(xblContextHandle->NotificationService());
        rtaNotificationService->RemoveNotificationHandler(token);
    }
}
CATCH_RETURN_WITH(;)

STDAPI_(XblFunctionContext) XblAchievementUnlockAddNotificationHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblAchievementUnlockHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    // TODO really should be returning HRESULT E_INVALIDARG here
    if (xblContextHandle == nullptr || handler == nullptr)
    {
        return XblFunctionContext{ 0 };
    }

    auto rtaNotificationService = std::dynamic_pointer_cast<RTANotificationService>(xblContextHandle->NotificationService());
    return rtaNotificationService->AddAchievementUnlockNotificationHandler(
        [handler, context]
        (const AchievementUnlockEvent& args)
        {
            try
            {
                handler(&args, context);
            }
            catch (...)
            {
                LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
            }
        });
}
CATCH_RETURN()

STDAPI_(void) XblAchievementUnlockRemoveNotificationHandler(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblFunctionContext functionContext
) XBL_NOEXCEPT
try
{
    auto rtaNotificationService = std::dynamic_pointer_cast<RTANotificationService>(xblContextHandle->NotificationService());
    rtaNotificationService->RemoveNotificationHandler(functionContext);
}
CATCH_RETURN_WITH(;)

#endif
