// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"
#include "xsapi-c/real_time_activity_c.h"

XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

real_time_activity_subscription::real_time_activity_subscription(
    XblRealTimeActivitySubscriptionHandle handle
) :
    m_handle{ handle }
{
}

real_time_activity_subscription_state real_time_activity_subscription::state() const
{
    XblRealTimeActivitySubscriptionState state;
    XblRealTimeActivitySubscriptionGetState(m_handle, &state);
    return static_cast<real_time_activity_subscription_state>(state);
}

const string_t& real_time_activity_subscription::resource_uri() const
{
    return m_resourceUri;
}

uint32_t real_time_activity_subscription::subscription_id() const
{
    uint32_t id;
    XblRealTimeActivitySubscriptionGetId(m_handle, &id);
    return id;
}

real_time_activity_subscription_error_event_args::real_time_activity_subscription_error_event_args(
    XblRealTimeActivitySubscriptionHandle subscriptionHandle,
    HRESULT subscriptionError
) :
    m_subscription{ subscriptionHandle }
{
    m_err = Utils::ConvertHr(subscriptionError);
}

const real_time_activity_subscription& real_time_activity_subscription_error_event_args::subscription()
{
    return m_subscription;
}

std::error_code real_time_activity_subscription_error_event_args::err() const
{
    return m_err;
}

std::string real_time_activity_subscription_error_event_args::err_message() const
{
    return std::string();
}

real_time_activity_service::real_time_activity_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

real_time_activity_service::real_time_activity_service(const real_time_activity_service& other)
{
    XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
}

real_time_activity_service& real_time_activity_service::operator=(real_time_activity_service other)
{
    std::swap(m_xblContext, other.m_xblContext);
    return *this;
}

real_time_activity_service::~real_time_activity_service()
{
    XblContextCloseHandle(m_xblContext);
}

void real_time_activity_service::activate()
{
    XblRealTimeActivityActivate(m_xblContext);
}

void real_time_activity_service::deactivate()
{
    XblRealTimeActivityDeactivate(m_xblContext);
}

struct real_time_activity_service::HandlerContext
{
    XblFunctionContext internalContext;
    std::function<void(real_time_activity_connection_state)> connectionStateChangeHandler;
    std::function<void(const real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler;
    std::function<void()> resyncHandler;
};

function_context real_time_activity_service::add_connection_state_change_handler(
    _In_ std::function<void(real_time_activity_connection_state)> handler
)
{
    auto context = new HandlerContext{};
    context->connectionStateChangeHandler = std::move(handler);

    context->internalContext = XblRealTimeActivityAddConnectionStateChangeHandler(m_xblContext,
        [](_In_ void* context, _In_ XblRealTimeActivityConnectionState connectionState)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->connectionStateChangeHandler(static_cast<real_time_activity_connection_state>(connectionState));
    }, context);

    return context;
}

void real_time_activity_service::remove_connection_state_change_handler(
    _In_ function_context remove
)
{
    auto handlerContext{ static_cast<HandlerContext*>(remove) };
    XblRealTimeActivityRemoveConnectionStateChangeHandler(m_xblContext, handlerContext->internalContext);
    delete handlerContext;
}

function_context real_time_activity_service::add_subscription_error_handler(
    _In_ std::function<void(const real_time_activity_subscription_error_event_args&)> handler
)
{
    auto context = new HandlerContext{};
    context->subscriptionErrorHandler = std::move(handler);

    context->internalContext = XblRealTimeActivityAddSubscriptionErrorHandler(m_xblContext,
        [](_In_ void* context, _In_ XblRealTimeActivitySubscriptionHandle subscription, HRESULT subscriptionError)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->subscriptionErrorHandler(
            real_time_activity_subscription_error_event_args{ subscription, subscriptionError }
        );
    }, context);

    return context;
}

void real_time_activity_service::remove_subscription_error_handler(
    _In_ function_context remove
)
{
    auto handlerContext{ static_cast<HandlerContext*>(remove) };
    XblRealTimeActivityRemoveSubscriptionErrorHandler(m_xblContext, handlerContext->internalContext);
    delete handlerContext;
}

function_context real_time_activity_service::add_resync_handler(
    _In_ std::function<void()> handler
)
{
    auto context = new HandlerContext{};
    context->resyncHandler = std::move(handler);

    context->internalContext = XblRealTimeActivityAddResyncHandler(m_xblContext,
        [](_In_ void* context)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->resyncHandler();
    }, context);

    return context;
}

void real_time_activity_service::remove_resync_handler(
    _In_ function_context remove
)
{
    auto handlerContext{ static_cast<HandlerContext*>(remove) };
    XblRealTimeActivityRemoveResyncHandler(m_xblContext, handlerContext->internalContext);
    delete handlerContext;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END

XBL_WARNING_POP