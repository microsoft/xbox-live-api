// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/real_time_activity_c.h"

// Support for legacy APIs
struct XblRealTimeActivitySubscription
{
    XblRealTimeActivitySubscription() noexcept = default;
    virtual ~XblRealTimeActivitySubscription() noexcept = default;

    const XblRealTimeActivitySubscriptionState state{ XblRealTimeActivitySubscriptionState::Unknown };
    const uint32_t id{ s_nextId++ };
private:
    static std::atomic<uint32_t> s_nextId;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

class Subscription
{
public:
    Subscription() noexcept = default;
    virtual ~Subscription() noexcept = default;

    String const& ResourceUri() const
    {
        return m_resourceUri;
    }

    virtual void OnSubscribe(const JsonValue& data) noexcept
    {
        UNREFERENCED_PARAMETER(data);
        // If the data is non-null, this should have been handled by the child class
        assert(data.IsNull());
    };
    virtual void OnEvent(const JsonValue& event) noexcept = 0;

protected:
    String m_resourceUri;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END