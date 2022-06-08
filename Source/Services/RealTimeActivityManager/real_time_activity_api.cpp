// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/real_time_activity_c.h"
#include "real_time_activity_manager.h"
#include "xbox_live_context_internal.h"

std::atomic<uint32_t> XblRealTimeActivitySubscription::s_nextId{ 1 };

STDAPI XblRealTimeActivitySubscriptionGetState(
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle,
    _Out_ XblRealTimeActivitySubscriptionState* state
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(subscriptionHandle == nullptr || state == nullptr);
    *state = subscriptionHandle->state;
    return S_OK;
}
CATCH_RETURN()

STDAPI XblRealTimeActivitySubscriptionGetId(
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle,
    _Out_ uint32_t* id
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(subscriptionHandle == nullptr || id == nullptr);
    *id = subscriptionHandle->id;
    return S_OK;
}
CATCH_RETURN()

STDAPI XblRealTimeActivityActivate(
    _In_ XblContextHandle xboxLiveContext
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__;
    if (auto state{ GlobalState::Get() })
    {
        state->RTAManager()->Activate(xboxLiveContext->User(), true);
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblRealTimeActivityDeactivate(
    _In_ XblContextHandle xboxLiveContext
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__;
    if (auto state{ GlobalState::Get() })
    {
        state->RTAManager()->Deactivate(xboxLiveContext->User());
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblRealTimeActivityAddConnectionStateChangeHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblRealTimeActivityConnectionStateChangeHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__;
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || handler == nullptr);

    if (auto state{ GlobalState::Get() })
    {
        return state->RTAManager()->AddStateChangedHandler(xboxLiveContext->User(),
            [
                handler,
                context
            ]
        (XblRealTimeActivityConnectionState state)
        {
            try
            {
                handler(context, state); 
            }
            catch (...)
            {
                LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
            }
        });
    }
    else
    {
        return E_XBL_NOT_INITIALIZED;
    }
}
CATCH_RETURN()

STDAPI XblRealTimeActivityRemoveConnectionStateChangeHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__;
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);

    if (auto state{ GlobalState::Get() })
    {
        state->RTAManager()->RemoveStateChangedHandler(xboxLiveContext->User(), token);
        return S_OK;
    }
    else
    {
        return E_XBL_NOT_INITIALIZED;
    }
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblRealTimeActivityAddSubscriptionErrorHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblRealTimeActivitySubscriptionErrorHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__ << ": DEPRECATED, No action taken by XSAPI.";
    UNREFERENCED_PARAMETER(xboxLiveContext);
    UNREFERENCED_PARAMETER(handler);
    UNREFERENCED_PARAMETER(context);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblRealTimeActivityRemoveSubscriptionErrorHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__ << ": DEPRECATED, No action taken by XSAPI.";
    UNREFERENCED_PARAMETER(xboxLiveContext);
    UNREFERENCED_PARAMETER(token);
    return S_OK;
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblRealTimeActivityAddResyncHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblRealTimeActivityResyncHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__;
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || handler == nullptr);

    if (auto state{ GlobalState::Get() })
    {
        return state->RTAManager()->AddResyncHandler(xboxLiveContext->User(),
            [
                handler,
                context
            ]
        {
            try
            {
                handler(context);
            }
            catch (...)
            {
                LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
            }
        });
    }
    else
    {
        return E_XBL_NOT_INITIALIZED;
    }
}
CATCH_RETURN()

STDAPI XblRealTimeActivityRemoveResyncHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__;
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);

    if (auto state{ GlobalState::Get() })
    {
        state->RTAManager()->RemoveResyncHandler(xboxLiveContext->User(), token);
        return S_OK;
    }
    else
    {
        return E_XBL_NOT_INITIALIZED;
    }
}
CATCH_RETURN()
