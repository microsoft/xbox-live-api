//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include <set>

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#endif

namespace xbox { namespace services {

    namespace system {
        class user_impl_idp;
    }

#if UWP_API || UNIT_TEST_SERVICES
class signin_ui_settings
{
public:
    /// <summary>
    /// Enum for current game category, sign in UI will adjust accordingly based on 
    /// your game category.
    /// </summary>
    enum class game_category
    {
        /// <summary>
        /// Enum for setting current title as standard game category.
        /// </summary>
        standard,

        /// <summary>
        /// Enum for setting current title as casual game category
        /// </summary>
        casual
    };

    /// <summary>
    /// Enum for emphasizing Xbox Live features your title has.
    /// </summary>
    enum class emphasis_feature
    {
        /// <summary>
        /// Enum for emphasizing Xbox Live achievement feature.
        /// </summary>
        achievements,

        /// <summary>
        /// Enum for emphasizing Xbox Live connected storage feature.
        /// </summary>
        connected_storage,

        /// <summary>
        /// Enum for emphasizing Xbox Live find players feature.
        /// </summary>
        find_players,

        /// <summary>
        /// Enum for emphasizing Xbox Live game bar feature.
        /// </summary>
        game_bar,

        /// <summary>
        /// Enum for emphasizing Xbox Live game DVR feature.
        /// </summary>
        game_dvr,

        /// <summary>
        /// Enum for emphasizing Xbox Live leaderboards feature.
        /// </summary>
        leaderboards,

        /// <summary>
        /// Enum for emphasizing Xbox Live multiplayer feature.
        /// </summary>
        multiplayer,

        /// <summary>
        /// Enum for emphasizing Xbox Live purchase feature.
        /// </summary>
        purchase,

        /// <summary>
        /// Enum for emphasizing Xbox Live shared content feature.
        /// </summary>
        shared_content,

        /// <summary>
        /// Enum for emphasizing Xbox Live social feature.
        /// </summary>
        social,

        /// <summary>
        /// Enum for emphasizing Xbox Live tournaments feature.
        /// </summary>
        tournaments
    };

    /// <summary>
    /// Get current game_category setting 
    /// </summary>
    _XSAPIIMP game_category title_category() const { return m_gameCategory; }

    /// <summary>
    /// Set your title's category, sign in UI will adjust accordingly based on your game category.
    /// </summary>
    _XSAPIIMP void set_game_category(game_category category) { m_gameCategory = category; }

    /// <summary>
    /// Specify which emphasized Xbox Live feature you want to show on the sign in page, up to 3.
    /// </summary>
    _XSAPIIMP void add_emphasis_feature(emphasis_feature feature) { m_features.insert(feature); }

    /// <summary>
    /// Retrieve the emphasis features added to show on the UI, only first 3 will be displayed.
    /// </summary>
    _XSAPIIMP const std::set<emphasis_feature>& emphasis_features() const { return m_features; }

    /// <summary>
    /// Specify the background image you want to show in the sign in page, in raw binary form.
    /// </summary>
    /// <remarks>
    /// An Image can be one of the following format: jpeg, jpg, png, bmp, gif, tiff and ico. 
    /// The input source will always be stretched to 300X400 pixels , thus the file larger than that 
    /// is not recommended.
    /// </remarks>
    _XSAPIIMP void set_background_image(const std::vector<unsigned char>& image);

    /// <summary>
    /// Retrieve the background color of sign in page the title has set.
    /// </summary>
    _XSAPIIMP const string_t& background_hex_color() const { return m_backgroundColor; }

    /// <summary>
    /// Set the background color of your sign in page, in hex string. Example: "0F2C55".
    /// </summary>
    _XSAPIIMP void set_background_hex_color(const string_t& backgroundColor) { m_backgroundColor = backgroundColor; }

    signin_ui_settings() :
        m_gameCategory(game_category::standard)
    {}

    /// <summary>
    /// Internal function
    /// </summary>
    const string_t& _Background_image_base64_encoded() const { return m_backgroundImageBase64Encoded; }

    /// <summary>
    /// Internal function
    /// </summary>
    static string_t _Feature_to_string(emphasis_feature);

    /// <summary>
    /// Internal function
    /// </summary>
    bool _Enabled();

private:
    // Disable copy, settings must be accessed via reference.
    signin_ui_settings(const signin_ui_settings&); 
    signin_ui_settings& operator=(const signin_ui_settings&);

    game_category m_gameCategory;
    string_t m_backgroundColor;
    string_t m_backgroundImageBase64Encoded;
    std::set<emphasis_feature> m_features;
};

#endif

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
    /// Returns the Xbox Live environment being used, it is empty before you sign in or using prodction.
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

#if UWP_API || UNIT_TEST_SERVICES
public:
    /// <summary>
    /// Returns the reference to sign in UI settings.
    /// </summary>
    _XSAPIIMP signin_ui_settings& app_signin_ui_settings() { return m_signinUISettings; }

private:
    signin_ui_settings m_signinUISettings;
#endif
};


}}
