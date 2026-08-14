// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/privacy_c.h"
#include "real_time_activity_subscription.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

typedef Callback<const XblPrivacyMuteListChangeEventArgs&> MuteListChangedHandler;
typedef Callback<const XblPrivacyBlockListChangeEventArgs&> BlockListChangedHandler;

class MuteListChangeSubscription : public real_time_activity::Subscription
{
public:
    MuteListChangeSubscription(_In_ uint64_t xuid) noexcept;

    XblFunctionContext AddHandler(MuteListChangedHandler handler) noexcept;
    size_t RemoveHandler(XblFunctionContext token) noexcept;

protected:
    void OnEvent(const JsonValue& data) noexcept override;

private:
    uint64_t m_xuid;
    Map<XblFunctionContext, MuteListChangedHandler> m_handlers;
    XblFunctionContext m_nextHandlerToken{ 1 };
    mutable std::mutex m_lock;
};

class BlockListChangeSubscription : public real_time_activity::Subscription
{
public:
    BlockListChangeSubscription(_In_ uint64_t xuid) noexcept;

    XblFunctionContext AddHandler(BlockListChangedHandler handler) noexcept;
    size_t RemoveHandler(XblFunctionContext token) noexcept;

protected:
    void OnEvent(const JsonValue& data) noexcept override;

private:
    uint64_t m_xuid;
    Map<XblFunctionContext, BlockListChangedHandler> m_handlers;
    XblFunctionContext m_nextHandlerToken{ 1 };
    mutable std::mutex m_lock;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END
