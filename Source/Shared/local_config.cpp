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
#if !TV_API && defined(_WIN32)
#include <Pathcch.h>
#endif
#include "xsapi/system.h"
#include "local_config.h"
#include "xbox_system_factory.h"
#include "Utils.h"
#if XSAPI_A
#include "a/user_impl_a.h"
#include "a/java_interop.h"
#endif

using namespace std;
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<local_config> local_config::get_local_config_singleton()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->s_singletonLock);
    if (xsapiSingleton->s_localConfigSingleton == nullptr)
    {
        xsapiSingleton->s_localConfigSingleton = std::make_shared<local_config>();

#if !TV_API
        xbox_live_result<void> configResult = xsapiSingleton->s_localConfigSingleton->read();
        if (configResult.err())
        {
            LOGS_ERROR << "Loading local config file error: " << configResult.err() << ", msg:" << configResult.err_message();
            XSAPI_ASSERT(!configResult.err());
        }
#endif
    }
    return xsapiSingleton->s_localConfigSingleton;
}

local_config::local_config()
{
}

#if !TV_API
uint64_t local_config::get_uint64_from_config(
    _In_ const string_t& name,
    _In_ bool required,
    _In_ uint64_t defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    return utils::extract_json_uint52(m_jsonConfig, name.c_str(), required, defaultValue);
}

string_t local_config::get_value_from_config(
    _In_ const string_t& name,
    _In_ bool required,
    _In_ const string_t& defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    return utils::extract_json_string(m_jsonConfig, name.c_str(), required, defaultValue);
}

bool local_config::get_bool_from_config(
    _In_ const string_t& name,
    _In_ bool required,
    _In_ bool defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);

    std::error_code err;
    auto value = utils::extract_json_field(m_jsonConfig, name.c_str(), err, required);

    // the value could be "0", "1", or true false
    if (value.is_boolean())
    {
        return value.as_bool();
    }
    else if (value.is_string())
    {
        return value.as_string() == _T("1");
    }
    else
    {
        return defaultValue;
    }
}

uint32_t local_config::title_id()
{
    uint64_t titleId = get_uint64_from_config(_T("TitleId"), false, 0);

    // Also support title id in string from
    if (titleId == 0)
    {
        string_t titleIdString = get_value_from_config(_T("TitleId"), false, _T(""));
        if (!titleIdString.empty())
        {
            titleId = utils::string_t_to_uint64(titleIdString);
        }
    }

    return static_cast<uint32_t>(titleId);
}

string_t local_config::scid()
{
    return get_value_from_config(_T("PrimaryServiceConfigId"), false, _T(""));
}

uint32_t local_config::override_title_id()
{
    uint64_t titleId = get_uint64_from_config(_T("OverrideTitleId"), false, 0);
    return static_cast<uint32_t>(titleId);
}

string_t local_config::override_scid()
{
    return get_value_from_config(_T("OverrideServiceConfigId"), false, _T(""));
}

string_t local_config::environment_prefix()
{
    return get_value_from_config(_T("EnvironmentPrefix"), false, _T(""));
}

string_t local_config::environment()
{
    string_t environment = get_value_from_config(_T("Environment"), false, _T(""));
    if (!environment.empty())
    {
        if (environment[0] != _T('.'))
        {
            environment = _T(".") + environment;
        }
    }

    return environment;
}

string_t local_config::sandbox()
{
    return get_value_from_config(_T("Sandbox"), false, _T("RETAIL"));
}

string_t local_config::client_secret()
{
    return get_value_from_config(_T("ClientSecret"), false, _T(""));
}

bool local_config::use_first_party_token()
{
    return get_bool_from_config(_T("FirstParty"), false, false);
}

string_t local_config::sidecar_pfn()
{
    return get_value_from_config(_T("SidecarPFN"), false, _T(""));
}

string_t local_config::sidecar_appchannel()
{
    return get_value_from_config(_T("SidecarAppChannel"), false, _T(""));
}

#if XSAPI_I
string_t local_config::apns_environment()
{
    return get_value_from_config(_T("ApnsEnvironment"), false, _T("apnsProduction"));
}
#endif

#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

