// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/xbox_live_app_config.h"
#include "local_config.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<xbox_live_app_config> 
xbox_live_app_config::get_app_config_singleton()
{
    auto xsapiSingleton = xbox::services::get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_appConfigLock);
    if (xsapiSingleton->m_appConfigSingleton == nullptr)
    {
        xsapiSingleton->m_appConfigSingleton = std::shared_ptr<xbox_live_app_config>(new xbox_live_app_config());
    }

    return xsapiSingleton->m_appConfigSingleton;
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
    XBOX_LIVE_NAMESPACE::system::xbox_system_factory::get_factory();

#if TV_API
    m_sandbox = Windows::Xbox::Services::XboxLiveConfiguration::SandboxId->Data();
    m_scid = Windows::Xbox::Services::XboxLiveConfiguration::PrimaryServiceConfigId->Data();
    m_titleId = std::stoi(Windows::Xbox::Services::XboxLiveConfiguration::TitleId->Data());
#else
    std::shared_ptr<XBOX_LIVE_NAMESPACE::local_config> localConfig = XBOX_LIVE_NAMESPACE::system::xbox_system_factory::get_factory()->create_local_config();
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
            std::make_error_code(XBOX_LIVE_NAMESPACE::xbox_live_error_code::invalid_config),
            "ERROR: Could not read \"titleId\" in xboxservices.config.  Must be a JSON number"
            );
    }

    if(m_scid.empty())
    {
        return xbox_live_result<void>(
            std::make_error_code(XBOX_LIVE_NAMESPACE::xbox_live_error_code::invalid_config),
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
