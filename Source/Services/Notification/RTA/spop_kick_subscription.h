// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-c/game_invite_c.h"
#include "real_time_activity_subscription.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

class SpopKickSubscription : public real_time_activity::Subscription
{
public:
    SpopKickSubscription(
        User user,
        uint32_t titleId
    ) noexcept;

    const String& ResourceUri() const noexcept;

protected:
    void OnEvent(_In_ const JsonValue& event) noexcept override;
    void OnResync() noexcept override;

private:
    User m_user{ nullptr };
};
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
#endif