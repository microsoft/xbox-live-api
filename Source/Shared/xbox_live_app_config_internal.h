// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <set>

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

namespace system {
    class user_impl_idp;
}

class xbox_live_app_config_internal : public std::enable_shared_from_this<xbox_live_app_config_internal>
{
public:
    static std::shared_ptr<xbox_live_app_config_internal> get_app_config_singleton();

    _XSAPIIMP uint32_t title_id();

    _XSAPIIMP const xsapi_internal_string& scid();

    _XSAPIIMP const xsapi_internal_string& environment() const;

    _XSAPIIMP const xsapi_internal_string& sandbox() const;

    uint32_t override_title_id_for_multiplayer() const;

    const xsapi_internal_string& override_scid_for_multiplayer() const;

    const xsapi_internal_string& apns_Environment() const;

    const web::uri& proxy() const;

    _XSAPIIMP void set_title_telemetry_device_id(_In_ const xsapi_internal_string& deviceId);

    _XSAPIIMP const xsapi_internal_string& title_telemetry_device_id() const;

    void disable_asserts_for_xbox_live_throttling_in_dev_sandboxes(_In_ xbox_live_context_throttle_setting setting);

    bool is_disable_asserts_for_xbox_live_throttling_in_dev_sandboxes() const;

    void disable_asserts_for_maximum_number_of_websockets_activated(_In_ xbox_live_context_recommended_setting setting);

    bool is_disable_asserts_for_maximum_number_of_websockets_activated() const;

private:
    xbox_live_app_config_internal();

    /// <summary>
    /// Reads the configuration data.
    /// </summary>
    xbox_live_result<void> read();
#if XSAPI_U
public:
#ifdef __OBJC__
    /// Set the view controller that your app wants to launch any xbox live services ui like sign in, profile
    /// card, etc from. This will set a weak pointer to this view controller. If at any point this view controller
    /// is no longer the desired presenting view controller, then set this to nil. If the weak pointer that backs
    /// this view controller ever is nil, then the fallback view controller for the app is the application's
    /// key window's root view controller.
    ///
    /// @param viewController The view controller to present any xbox live services ui from. Can be nil.
    ///
    /// Usage for using a view controller as the presenting view controller only once:
    ///
    ///     xbox_live_app_config::set_launch_view_controller(viewController)
    ///     user->signin().then...
    ///     // Then when done with sign in or any ui
    ///     xbox_live_app_config::set_launch_view_controller(nil)
    ///
    /// Usage for setting the view controller that will always be used:
    ///
    ///     xbox_live_app_config::set_launch_view_controller(viewController)
    ///     // Some time later in your application
    ///     title_callable_ui::show_profile_card_ui(xuid)
    ///
    _XSAPIIMP static void set_launch_view_controller(_In_ UIViewController *viewController);
    _XSAPIIMP static void set_notification_registration_token(_In_ NSString *registrationToken);
#endif

    /// <summary>
    /// Sets the Xbox Live Sandbox
    /// </summary>
    _XSAPIIMP void set_sandbox(_In_ const string_t& sandbox);

    /// <summary>
    /// Sets the Xbox Live environment
    /// </summary>
    _XSAPIIMP void set_environment(_In_ const string_t& environment);
private:
#else
    void set_sandbox(_In_ const xsapi_internal_string& sandbox);
    void set_environment(_In_ const xsapi_internal_string& environment);
#endif

#if XSAPI_U
    string_t get_proxy_string();
#endif

    xsapi_internal_string m_apnsEnvironment;

    uint32_t m_titleId;
    uint32_t m_overrideTitleId;
    xsapi_internal_string m_environment;
    xsapi_internal_string m_sandbox;
    xsapi_internal_string m_scid;
    xsapi_internal_string m_overrideScid;
    web::uri m_proxy;
    xsapi_internal_string m_titleTelemetryDeviceId;
    bool m_disableAssertsForXboxLiveThrottlingInDevSandboxes;
    bool m_disableAssertsForMaxNumberOfWebsocketsActivated;

    friend class system::user_impl_idp;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
