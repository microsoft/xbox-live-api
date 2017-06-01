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

/// <summary>
/// Represents the configuration of an Xbox Live application.
/// </summary>
class xbox_live_app_config
{
public:
    /// <summary>
    /// Gets the app config singleton.
    /// </summary>
    static std::shared_ptr<xbox_live_app_config> get_app_config_singleton();

    /// <summary>
    /// Returns the title id of the app.
    /// </summary>
    _XSAPIIMP uint32_t title_id();

    /// <summary>
    /// Returns the service config id of the app.
    /// </summary>
    _XSAPIIMP string_t scid();

    /// <summary>
    /// Returns the Xbox Live environment being used, it is empty before you sign in or using production.
    /// </summary>
    _XSAPIIMP const string_t& environment() const;

    /// <summary>
    /// Returns the sandbox such as "XDKS.1", it is empty until you sign in.
    /// </summary>
    _XSAPIIMP const string_t& sandbox() const;

    /// <summary>
    /// internal function
    /// </summary>
    uint32_t _Override_title_id_for_multiplayer() const;

    /// <summary>
    /// internal function
    /// </summary>
    const string_t& _Override_scid_for_multiplayer() const;

    /// <summary>
    /// internal function
    /// </summary>
    const string_t& _Apns_Environment() const;

    /// <summary>
    /// internal function
    /// </summary>
    const web::uri& _Proxy() const;
    
    /// <summary>
    /// Set the titles device id that is used for telemetry
    /// </summary>
    _XSAPIIMP void set_title_telemetry_device_id(_In_ const string_t& deviceId);
    
    /// <summary>
    /// Get the titles device id that is used for telemetry
    /// </summary>
    _XSAPIIMP const string_t& title_telemetry_device_id() const;

private:
    /// <summary>
    /// Internal function
    /// </summary>
    xbox_live_app_config();

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
    void set_sandbox(const string_t& sandbox);
    void set_environment(const string_t& environment);
#endif

#if XSAPI_U
    string_t get_proxy_string();
#endif

    string_t m_apnsEnvironment;

    uint32_t m_titleId;
    uint32_t m_overrideTitleId;
    string_t m_environment;
    string_t m_sandbox;
    string_t m_scid;
    string_t m_overrideScid;
    web::uri m_proxy;
    string_t m_titleTelemetryDeviceId;

    friend class system::user_impl_idp;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
