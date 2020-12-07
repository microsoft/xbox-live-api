// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-c/achievements_c.h"
#include "real_time_activity_subscription.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

    struct AchievementUnlockEvent : public XblAchievementUnlockEvent
    {
        public:

        AchievementUnlockEvent() = default;
        AchievementUnlockEvent( AchievementUnlockEvent&& event);
        AchievementUnlockEvent(const AchievementUnlockEvent& event);

        static Result<AchievementUnlockEvent> Deserialize( const JsonValue& json ) noexcept;

        private:

        String m_achievementId;
        String m_achievementDescription;
        String m_achievementName;
        String m_achievementIconUri;
        String m_deepLink;
    };

    
    class AchievementUnlockSubscription : public real_time_activity::Subscription
    {
        public:

        AchievementUnlockSubscription(uint64_t xuid, uint32_t titleId) noexcept;

        using EventHandler = Function< void(const AchievementUnlockEvent&) >;
        XblFunctionContext AddHandler(EventHandler handler) noexcept;
        size_t             RemoveHandler(XblFunctionContext id) noexcept;
        const String &     ResourceUri() const noexcept;
        

        protected:

        void           OnEvent(_In_ const JsonValue& data) noexcept override;
        void           OnResync() noexcept override;

        private:

        Map< XblFunctionContext, EventHandler> m_handlers;
        XblFunctionContext                     m_nextId{ 0 };

        std::mutex m_mutex;
    };

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END

#endif