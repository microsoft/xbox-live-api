// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/xbox_live_app_config.h"
#include "xbox_live_app_config_internal.h"
#include "local_config.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<xbox_live_app_config> 
xbox_live_app_config::get_app_config_singleton()
{
    auto xsapiSingleton = xbox::services::get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> guard(xsapiSingleton->m_appConfigLock);
    if (xsapiSingleton->m_appConfigSingleton == nullptr)
    {
        auto buffer = xbox::services::system::xsapi_memory::mem_alloc(sizeof(xbox_live_app_config));

        xsapiSingleton->m_appConfigSingleton = std::shared_ptr<xbox_live_app_config>(
            new (buffer) xbox_live_app_config(xbox_live_app_config_internal::get_app_config_singleton()),
            xsapi_alloc_deleter<xbox_live_app_config>()
            );
    }

    return xsapiSingleton->m_appConfigSingleton;
}

xbox_live_app_config::xbox_live_app_config(
    _In_ std::shared_ptr<xbox_live_app_config_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_UINT32(xbox_live_app_config, title_id);
DEFINE_GET_STRING(xbox_live_app_config, scid);
DEFINE_GET_STRING(xbox_live_app_config, environment);
DEFINE_GET_STRING(xbox_live_app_config, sandbox);

std::shared_ptr<xbox_live_app_config_internal>
xbox_live_app_config_internal::get_app_config_singleton()
{
    auto xsapiSingleton = xbox::services::get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> guard(xsapiSingleton->m_appConfigLock);
    if (xsapiSingleton->m_appConfigSingleton == nullptr)
    {
        auto buffer = xbox::services::system::xsapi_memory::mem_alloc(sizeof(xbox_live_app_config_internal));

        xsapiSingleton->m_internalAppConfigSingleton = std::shared_ptr<xbox_live_app_config_internal>(
            new (buffer) xbox_live_app_config_internal(),
            xsapi_alloc_deleter<xbox_live_app_config_internal>()
            );
    }

    return xsapiSingleton->m_internalAppConfigSingleton;
}

xbox_live_app_config_internal::xbox_live_app_config_internal() :
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

xbox_live_result<void> xbox_live_app_config_internal::read()
{
    xbox::services::system::xbox_system_factory::get_factory();

#if TV_API
    m_sandbox = utils::internal_string_from_char_t(Windows::Xbox::Services::XboxLiveConfiguration::SandboxId->Data());
    m_scid = utils::internal_string_from_char_t(Windows::Xbox::Services::XboxLiveConfiguration::PrimaryServiceConfigId->Data());
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

#if XSAPI_U
    m_sandbox = localConfig->sandbox();
#endif

#endif

    return xbox_live_result<void>();
}

uint32_t xbox_live_app_config_internal::title_id()
{
    return m_titleId;
}

uint32_t xbox_live_app_config_internal::override_title_id_for_multiplayer() const
{
    return m_overrideTitleId;
}

const xsapi_internal_string& xbox_live_app_config_internal::scid()
{
    return m_scid;
}

const xsapi_internal_string& xbox_live_app_config_internal::override_scid_for_multiplayer() const
{
    return m_overrideScid;
}

const xsapi_internal_string& xbox_live_app_config_internal::environment() const
{
    return m_environment;
}

void xbox_live_app_config_internal::set_environment(_In_ const xsapi_internal_string& environment)
{
    m_environment = environment;
}

const xsapi_internal_string& xbox_live_app_config_internal::sandbox() const
{
    return m_sandbox;
}

void xbox_live_app_config_internal::set_sandbox(_In_ const xsapi_internal_string& sandbox)
{
    m_sandbox = sandbox;
}

const web::uri&
xbox_live_app_config_internal::proxy() const
{
    return m_proxy;
}

void
xbox_live_app_config_internal::set_title_telemetry_device_id(
    _In_ const xsapi_internal_string& deviceId
    )
{
    m_titleTelemetryDeviceId = deviceId;
}

const xsapi_internal_string&
xbox_live_app_config_internal::title_telemetry_device_id() const
{
    return m_titleTelemetryDeviceId;
}

void
xbox_live_app_config_internal::disable_asserts_for_xbox_live_throttling_in_dev_sandboxes(
    _In_ xbox_live_context_throttle_setting setting
    )
{
    if (setting == xbox_live_context_throttle_setting::this_code_needs_to_be_changed_to_avoid_throttling)
    {
        m_disableAssertsForXboxLiveThrottlingInDevSandboxes = true;
    }
}

bool
xbox_live_app_config_internal::is_disable_asserts_for_xbox_live_throttling_in_dev_sandboxes() const
{
    return m_disableAssertsForXboxLiveThrottlingInDevSandboxes;
}

void
xbox_live_app_config_internal::disable_asserts_for_maximum_number_of_websockets_activated(
    _In_ xbox_live_context_recommended_setting setting
    )
{
    if (setting == xbox_live_context_recommended_setting::this_code_needs_to_be_changed_to_follow_best_practices)
    {
        m_disableAssertsForMaxNumberOfWebsocketsActivated = true;
    }
}

bool
xbox_live_app_config_internal::is_disable_asserts_for_maximum_number_of_websockets_activated() const
{
    return m_disableAssertsForMaxNumberOfWebsocketsActivated;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
