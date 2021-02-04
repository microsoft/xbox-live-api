// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <set>

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class AppConfig : public std::enable_shared_from_this<AppConfig>
{
public:
    AppConfig() = default;

    // TODO Remove. Unsafe method - returns null if GlobalState doesn't exist and is unchecked by callers
    static std::shared_ptr<AppConfig> Instance();

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK
    HRESULT Initialize();
#else
    HRESULT Initialize(xsapi_internal_string scid);
#endif

    uint32_t TitleId();
    const xsapi_internal_string& Scid() const;
    const xsapi_internal_string& Sandbox() const;
#if HC_PLATFORM == HC_PLATFORM_UWP
    // Remove after switch to Xal auth on UWP
    void SetSandbox(const xsapi_internal_string& sandbox);

    std::shared_ptr<local_config> LocalConfig() const { return m_localConfig; }
#endif

    const xsapi_internal_string& EndpointId() const;
    void SetEndpointId(const xsapi_internal_string& endpointId);

    uint32_t OverrideTitleId() const;
    void SetOverrideTitleId(uint32_t overrideTitleId);
    const xsapi_internal_string& OverrideScid() const;
    void SetOverrideScid(const xsapi_internal_string& overrideScid);

    void DisableAssertsForXboxLiveThrottlingInDevSandboxes();
    bool IsDisableAssertsForXboxLiveThrottlingInDevSandboxes() const;

#if HC_PLATFORM == HC_PLATFORM_IOS
    const xsapi_internal_string& APNSEnvironment() const;
    void SetAPNSEnvironment(const xsapi_internal_string& apnsEnvironment);
#endif

#if !HC_PLATFORM_IS_MICROSOFT && defined(__OBJC__)
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
#endif

#if HC_PLATFORM_IS_EXTERNAL
    xsapi_internal_string const& AppId() const;
    xsapi_internal_string const& AppVer() const;
    xsapi_internal_string const& OsName() const;
    xsapi_internal_string const& OsLocale() const;
    xsapi_internal_string const& OsVersion() const;
    xsapi_internal_string const& DeviceClass() const;
    xsapi_internal_string const& DeviceId() const;

    void SetAppId(xsapi_internal_string&& v);
    void SetAppVer(xsapi_internal_string&& v);
    void SetOsName(xsapi_internal_string&& v);
    void SetOsLocale(xsapi_internal_string&& v);
    void SetOsVersion(xsapi_internal_string&& v);
    void SetDeviceClass(xsapi_internal_string&& v);
    void SetDeviceId(xsapi_internal_string&& v);
#endif

private:
    uint32_t m_titleId{ 0 };
    uint32_t m_overrideTitleId{ 0 };
    xsapi_internal_string m_sandbox;
    xsapi_internal_string m_scid;
    xsapi_internal_string m_overrideScid;
    xsapi_internal_string m_endpointId;
    bool m_disableAssertsForXboxLiveThrottlingInDevSandboxes{ false };

#if HC_PLATFORM == HC_PLATFORM_IOS
    xsapi_internal_string m_apnsEnvironment{ "apnsProduction" };
    xsapi_internal_string m_registrationToken;
#endif

#if HC_PLATFORM_IS_EXTERNAL
    xsapi_internal_string m_telemetryAppId;
    xsapi_internal_string m_telemetryAppVer;
    xsapi_internal_string m_telemetryOsName;
    xsapi_internal_string m_telemetryOsLocale;
    xsapi_internal_string m_telemetryOsVersion;
    xsapi_internal_string m_telemetryDeviceClass;
    xsapi_internal_string m_telemetryDeviceId;
#endif

#if HC_PLATFORM == HC_PLATFORM_UWP
    std::shared_ptr<local_config> m_localConfig;
#endif
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
