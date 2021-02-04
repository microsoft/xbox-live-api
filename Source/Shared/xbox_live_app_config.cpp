// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xbox_live_app_config_internal.h"
#if HC_PLATFORM == HC_PLATFORM_UWP
#include "local_config.h"
#endif

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<AppConfig> AppConfig::Instance()
{
    auto state = GlobalState::Get();
    if (state)
    {
        return state->AppConfig();
    }
    return nullptr;
}

#if HC_PLATFORM == HC_PLATFORM_UWP 
HRESULT AppConfig::Initialize()
{
    m_localConfig = MakeShared<local_config>();
    auto readResult = m_localConfig->read();
    if (readResult.err())
    {
        LOGS_ERROR << "Loading local config file error : " << readResult.err() << ", msg : " << readResult.err_message();
        return utils::convert_xbox_live_error_code_to_hresult(readResult.err());
    }

    m_titleId = m_localConfig->title_id();
    m_scid = m_localConfig->scid();
    m_overrideScid = m_localConfig->override_scid();
    m_overrideTitleId = m_localConfig->override_title_id();

    if (m_titleId == 0)
    {
        LOGS_ERROR << "ERROR: Could not read \"titleId\" in xboxservices.config.  Must be a JSON number";
        return E_FAIL;
    }

    if (m_scid.empty())
    {
        LOGS_ERROR << "ERROR: Could not read \"PrimaryServiceConfigId\" in xboxservices.config.  Must be a JSON string";
        return E_FAIL;
    }
    return S_OK;
}

#elif HC_PLATFORM == HC_PLATFORM_XDK
HRESULT AppConfig::Initialize()
{
    m_sandbox = utils::internal_string_from_utf16(Windows::Xbox::Services::XboxLiveConfiguration::SandboxId->Data());
    m_titleId = std::stoi(Windows::Xbox::Services::XboxLiveConfiguration::TitleId->Data());
    m_scid = utils::internal_string_from_utf16(Windows::Xbox::Services::XboxLiveConfiguration::PrimaryServiceConfigId->Data());

    return S_OK;
}
#else

HRESULT AppConfig::Initialize(
    xsapi_internal_string scid
)
{
    HRESULT hr = XalGetTitleId(&m_titleId);

    size_t sandboxSize = XalGetSandboxSize();
    char* sandbox = MakeArray<char>(sandboxSize);

    hr = XalGetSandbox(sandboxSize, sandbox, nullptr);
    if (SUCCEEDED(hr))
    {
        m_sandbox = sandbox;
    }
    DeleteArray(sandbox, sandboxSize);

    m_scid = std::move(scid);

    return hr;
}
#endif

uint32_t AppConfig::TitleId()
{
    return m_titleId;
}

uint32_t AppConfig::OverrideTitleId() const
{
    if (m_overrideTitleId == 0)
    {
        return m_titleId;
    }
    return m_overrideTitleId;
}

void AppConfig::SetOverrideTitleId(uint32_t overrideTitleId)
{
    m_overrideTitleId = overrideTitleId;
}

const xsapi_internal_string& AppConfig::Scid() const
{
    return m_scid;
}

const xsapi_internal_string& AppConfig::OverrideScid() const
{
    if (m_overrideScid.empty())
    {
        return m_scid;
    }
    return m_overrideScid;
}

void AppConfig::SetOverrideScid(const xsapi_internal_string& overrideScid)
{
    m_overrideScid = overrideScid;
}

const xsapi_internal_string& AppConfig::Sandbox() const
{
    return m_sandbox;
}

#if HC_PLATFORM == HC_PLATFORM_UWP
void AppConfig::SetSandbox(const xsapi_internal_string& sandbox)
{
    m_sandbox = sandbox;
}
#endif

const xsapi_internal_string& AppConfig::EndpointId() const
{
    return m_endpointId;
}

void AppConfig::SetEndpointId(const xsapi_internal_string& endpointId)
{
    m_endpointId = endpointId;
}

void AppConfig::DisableAssertsForXboxLiveThrottlingInDevSandboxes()
{
    m_disableAssertsForXboxLiveThrottlingInDevSandboxes = true;
}

bool AppConfig::IsDisableAssertsForXboxLiveThrottlingInDevSandboxes() const
{
    return m_disableAssertsForXboxLiveThrottlingInDevSandboxes;
}

#if HC_PLATFORM == HC_PLATFORM_IOS
const xsapi_internal_string& AppConfig::APNSEnvironment() const
{
    return m_apnsEnvironment;
}

void AppConfig::SetAPNSEnvironment(const xsapi_internal_string& apnsEnvironment)
{
    m_apnsEnvironment = apnsEnvironment;
}
#endif

#if HC_PLATFORM_IS_EXTERNAL
xsapi_internal_string const& AppConfig::AppId() const
{
    return m_telemetryAppId;
}

xsapi_internal_string const& AppConfig::AppVer() const
{
    return m_telemetryAppVer;
}

xsapi_internal_string const& AppConfig::OsName() const
{
    return m_telemetryOsName;
}

xsapi_internal_string const& AppConfig::OsLocale() const
{
    return m_telemetryOsLocale;
}

xsapi_internal_string const& AppConfig::OsVersion() const
{
    return m_telemetryOsVersion;
}

xsapi_internal_string const& AppConfig::DeviceClass() const
{
    return m_telemetryDeviceClass;
}

xsapi_internal_string const& AppConfig::DeviceId() const
{
    return m_telemetryDeviceId;
}

void AppConfig::SetAppId(xsapi_internal_string&& v)
{
    m_telemetryAppId = std::move(v);
}

void AppConfig::SetAppVer(xsapi_internal_string&& v)
{
    m_telemetryAppVer = std::move(v);
}

void AppConfig::SetOsName(xsapi_internal_string&& v)
{
    m_telemetryOsName = std::move(v);
}

void AppConfig::SetOsLocale(xsapi_internal_string&& v)
{
    m_telemetryOsLocale = std::move(v);
}

void AppConfig::SetOsVersion(xsapi_internal_string&& v)
{
    m_telemetryOsVersion = std::move(v);
}

void AppConfig::SetDeviceClass(xsapi_internal_string&& v)
{
    m_telemetryDeviceClass = std::move(v);
}

void AppConfig::SetDeviceId(xsapi_internal_string&& v)
{
    m_telemetryDeviceId = std::move(v);
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
