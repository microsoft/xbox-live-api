// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    bool needToReadConfig = false;
    {
        std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
        if (xsapiSingleton->m_localConfigSingleton == nullptr)
        {
            needToReadConfig = true; 
            xsapiSingleton->m_localConfigSingleton = std::make_shared<local_config>();
        }
    }

    if (needToReadConfig)
    {
#if !TV_API
        xbox_live_result<void> configResult = xsapiSingleton->m_localConfigSingleton->read();
        if (configResult.err())
        {
            LOGS_ERROR << "Loading local config file error: " << configResult.err() << ", msg:" << configResult.err_message();
            XSAPI_ASSERT(!configResult.err());
        }
#endif
    }
    return xsapiSingleton->m_localConfigSingleton;
}

local_config::local_config()
{
}

#if !TV_API
uint64_t local_config::get_uint64_from_config(
    _In_ const xsapi_internal_string& name,
    _In_ bool required,
    _In_ uint64_t defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    return utils::extract_json_uint52(m_jsonConfig, name.c_str(), required, defaultValue);
}

xsapi_internal_string local_config::get_value_from_config(
    _In_ const xsapi_internal_string& name,
    _In_ bool required,
    _In_ const xsapi_internal_string& defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    return utils::extract_json_string(m_jsonConfig, name, required, defaultValue);
}

bool local_config::get_bool_from_config(
    _In_ const xsapi_internal_string& name,
    _In_ bool required,
    _In_ bool defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);

    std::error_code err;
    auto value = utils::extract_json_field(m_jsonConfig, name, err, required);

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
    uint64_t titleId = get_uint64_from_config("TitleId", false, 0);

    // Also support title id in string dec form or string hex form 
    if (titleId == 0)
    {
        xsapi_internal_string titleIdString = get_value_from_config("TitleId", false, "");
        if (!titleIdString.empty())
        {
            titleId = utils::internal_string_to_uint64(titleIdString);
            if (titleId == 0)
            {
                xsapi_internal_stringstream ss;
                ss << std::hex << titleIdString;
                ss >> titleId;
            }
        }
    }

    return static_cast<uint32_t>(titleId);
}

xsapi_internal_string local_config::scid()
{
    return get_value_from_config("PrimaryServiceConfigId", false, "");
}

uint32_t local_config::override_title_id()
{
    uint64_t titleId = get_uint64_from_config("OverrideTitleId", false, 0);
    return static_cast<uint32_t>(titleId);
}

xsapi_internal_string local_config::override_scid()
{
    return get_value_from_config("OverrideServiceConfigId", false, "");
}

xsapi_internal_string local_config::environment_prefix()
{
    return get_value_from_config("EnvironmentPrefix", false, "");
}

xsapi_internal_string local_config::environment()
{
    xsapi_internal_string environment = get_value_from_config("Environment", false, "");
    if (!environment.empty())
    {
        if (environment[0] != '.')
        {
            environment = "." + environment;
        }
    }

    return environment;
}

xsapi_internal_string local_config::sandbox()
{
    return get_value_from_config("Sandbox", false, "RETAIL");
}

xsapi_internal_string local_config::client_secret()
{
    return get_value_from_config("ClientSecret", false, "");
}

bool local_config::use_first_party_token()
{
    return get_bool_from_config("FirstParty", false, false);
}

bool local_config::is_creators_title()
{
    return get_bool_from_config("XboxLiveCreatorsTitle", false, false);
}

xsapi_internal_string local_config::msa_sub_target()
{
    return get_value_from_config("MsaSubTarget", false, "");
}

xsapi_internal_string local_config::scope()
{
    xsapi_internal_string defaultScope = is_creators_title() ? "xbl.signin xbl.friends" : "xboxlive.signin";
    return get_value_from_config("Scope", false, defaultScope);
}

#if XSAPI_I
xsapi_internal_string local_config::apns_environment()
{
    return get_value_from_config("ApnsEnvironment", false, "apnsProduction");
}
#endif

#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

