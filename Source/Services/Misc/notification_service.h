// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/types.h"
#include "xsapi/system.h"

namespace xbox { namespace services { namespace notification  {

enum notification_filter_source_type
{
    media_presence = 1,
    presence_online,
    broadcast,
    message,
    party_invite_360,
    multiplayer
};

struct notification_filter
{
    notification_filter_source_type sourceType;
    uint32_t type;
};

class notification_service : public std::enable_shared_from_this<notification_service>
{
public:
    static std::shared_ptr<notification_service> get_notification_service_singleton();

    virtual pplx::task<xbox_live_result<void>> subscribe_to_notifications(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    virtual pplx::task<xbox_live_result<void>> unsubscribe_from_notifications();
    virtual pplx::task<xbox_live_result<void>> unsubscribe_from_notifications(_In_ const string_t& endpointId);

protected:
    notification_service() = default;
    virtual pplx::task<xbox_live_result<void>> subscribe_to_notifications() = 0;
    virtual pplx::task<xbox_live_result<void>> subscribe_to_notifications_helper(
        _In_ const string_t& applicationInstanceId,
        _In_ const string_t& uriData,
        _In_ const string_t& platform,
        _In_ const string_t& deviceName,
        _In_ const string_t& platformVersion,
        _In_ const std::vector<notification_filter> notificationFilterEnum
        );

    pplx::task<xbox_live_result<void>> unsubscribe_from_notifications_helper(
        _In_ const string_t& endpointId
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    std::unordered_map<string_t, std::shared_ptr<xbox::services::user_context>> m_userContexts;

    const xsapi_internal_string ENDPOINT_ID_CACHE_NAME = "endpointId";
    bool m_isInitialized = false;
    string_t m_endpointId;
};

#if UWP_API || TV_API || UNIT_TEST_SERVICES
class notification_service_windows : public notification_service
{
public:
    notification_service_windows() = default;

    pplx::task<xbox_live_result<void>> subscribe_to_notifications(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) override;

protected:
    pplx::task<xbox_live_result<void>> subscribe_to_notifications() override;

private:
    void on_push_notification_received(
        _In_ Windows::Networking::PushNotifications::PushNotificationChannel ^sender,
        _In_ Windows::Networking::PushNotifications::PushNotificationReceivedEventArgs ^args
        );
};
#elif XSAPI_A
class notification_service_android : public notification_service
{
public:
    static void notificiation_registration_callback(JNIEnv*, jclass, jstring notificationData, jboolean isCached);

    notification_service_android();

    pplx::task<xbox_live_result<void>> subscribe_to_notifications(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        ) override;

protected:
    pplx::task<xbox_live_result<void>> subscribe_to_notifications() override;

private:
    bool m_isInitializing;

    pplx::task<xbox_live_result<void>> subscribe_to_notifications_finish(
        _In_ const string_t& registrationToken,
        _In_ const std::shared_ptr<notification_service_android>& pThis
        );

    static pplx::task_completion_event<string_t> s_notificationRegisterEvent;
};
#elif XSAPI_I
class notification_service_ios : public notification_service
{
public:
    notification_service_ios() = default;
protected:
    pplx::task<xbox_live_result<void>> subscribe_to_notifications() override;
    static pplx::task_completion_event<string_t> s_notificationRegisterEvent;
};
#endif

}}}