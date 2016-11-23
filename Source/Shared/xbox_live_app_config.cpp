//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "shared_macros.h"
#include "xsapi/xbox_live_app_config.h"
#include "local_config.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN


static std::mutex g_appConfigSingletonLock;
static std::shared_ptr<xbox_live_app_config> g_appConfigSingleton;

#if UWP_API || UNIT_TEST_SERVICES
void signin_ui_settings::set_background_image(const std::vector<unsigned char>& image)
{
    m_backgroundImageBase64Encoded = utility::conversions::to_base64(image);
}

bool signin_ui_settings::_Enabled()
{
    // If any of these settings are being set, enable UI customization 
    return !m_backgroundColor.empty() ||
        !m_backgroundImageBase64Encoded.empty() ||
        m_features.size() > 0 ||
        m_gameCategory != signin_ui_settings::game_category::standard;
}

string_t signin_ui_settings::_Feature_to_string(emphasis_feature feature)
{
    switch (feature)
    {
    case signin_ui_settings::emphasis_feature::achievements:
        return _T("Achievements");
    case signin_ui_settings::emphasis_feature::connected_storage:
        return _T("ConnectedStorage");
    case signin_ui_settings::emphasis_feature::find_players:
        return _T("FindPlayers");
    case signin_ui_settings::emphasis_feature::game_bar:
        return _T("Gamebar");
    case signin_ui_settings::emphasis_feature::game_dvr:
        return _T("GameDVR");
    case signin_ui_settings::emphasis_feature::leaderboards:
        return _T("Leaderboards");
    case signin_ui_settings::emphasis_feature::multiplayer:
        return _T("Multiplayer");
    case signin_ui_settings::emphasis_feature::purchase:
        return _T("Purchase");
    case signin_ui_settings::emphasis_feature::shared_content:
        return _T("SharedContent");
    case signin_ui_settings::emphasis_feature::social:
        return _T("Social");
    case signin_ui_settings::emphasis_feature::tournaments:
        return _T("Tournaments");
    default:
        return _T("");
    }
}

#endif

std::shared_ptr<xbox_live_app_config> 
xbox_live_app_config::get_app_config_singleton()
{
    std::lock_guard<std::mutex> guard(g_appConfigSingletonLock);
    if (g_appConfigSingleton == nullptr)
    {
        g_appConfigSingleton = std::shared_ptr<xbox_live_app_config>(new xbox_live_app_config());
    }

    return g_appConfigSingleton;
}


xbox_live_app_config::xbox_live_app_config() :
    m_titleId(0),
    m_overrideTitleId(0)
{
    auto servicesConfigFileReadResult = read();

    if (servicesConfigFileReadResult.err())
    {
        LOG_ERROR(servicesConfigFileReadResult.err_message());
#if !defined(XSAPI_U) 
        assert(!servicesConfigFileReadResult.err());
#endif
    }

#if XSAPI_U
    m_proxy = get_proxy_string();
#endif
}

xbox_live_result<void> xbox_live_app_config::read()
{
    xbox::services::system::xbox_system_factory::get_factory();

#if TV_API
    m_sandbox = Windows::Xbox::Services::XboxLiveConfiguration::SandboxId->Data();
    m_scid = Windows::Xbox::Services::XboxLiveConfiguration::PrimaryServiceConfigId->Data();
    m_titleId = std::stoi(Windows::Xbox::Services::XboxLiveConfiguration::TitleId->Data());
#else
    std::shared_ptr<xbox::services::local_config> localConfig = xbox::services::system::xbox_system_factory::get_factory()->create_local_config();
    m_titleId = localConfig->title_id();
    m_scid = localConfig->scid();
    m_environment = localConfig->environment();
    m_overrideScid = localConfig->override_scid();
    m_overrideTitleId = localConfig->override_title_id();
#if XSAPI_I
    m_apnsEnvironment = localConfig->apns_environment();
#endif

    if(m_titleId == 0)
    {
        return xbox_live_result<void>(
            std::make_error_code(xbox::services::xbox_live_error_code::invalid_config),
            "ERROR: Could not read \"titleId\" in xboxservices.config.  Must be a JSON number"
            );
    }

    if(m_scid.empty())
    {
        return xbox_live_result<void>(
            std::make_error_code(xbox::services::xbox_live_error_code::invalid_config),
            "ERROR: Could not read \"PrimaryServiceConfigId\" in xboxservices.config.  Must be a JSON string"
            );
    }

#if XSAPI_SERVER || XSAPI_U
    m_sandbox = localConfig->sandbox();
#endif

#endif

    return xbox_live_result<void>();
}

uint32_t xbox_live_app_config::title_id()
{
    return m_titleId;
}

uint32_t xbox_live_app_config::_Override_title_id_for_multiplayer() const
{
    return m_overrideTitleId;
}

string_t xbox_live_app_config::scid()
{
    return m_scid;
}

const string_t& xbox_live_app_config::_Override_scid_for_multiplayer() const
{
    return m_overrideScid;
}

const string_t& xbox_live_app_config::environment() const
{
    return m_environment;
}

void xbox_live_app_config::set_environment(_In_ const string_t& environment)
{
    m_environment = environment;
}

const string_t& xbox_live_app_config::sandbox() const
{
    return m_sandbox;
}

void xbox_live_app_config::set_sandbox(_In_ const string_t& sandbox)
{
    m_sandbox = sandbox;
}

const web::uri&
xbox_live_app_config::_Proxy() const
{
    return m_proxy;
}

void
xbox_live_app_config::set_title_telemetry_device_id(
    _In_ const string_t& deviceId
    )
{
    m_titleTelemetryDeviceId = deviceId;
}

const string_t&
xbox_live_app_config::title_telemetry_device_id() const
{
    return m_titleTelemetryDeviceId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
