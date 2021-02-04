// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <Pathcch.h>
#include "local_config.h"
#include "xsapi_utils.h"

using namespace std;
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

local_config::local_config()
{
}

uint64_t local_config::get_uint64_from_config(
    _In_ const xsapi_internal_string& name,
    _In_ bool required,
    _In_ uint64_t defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    uint64_t output = defaultValue;
    JsonUtils::ExtractJsonUInt64(m_jsonConfig, name.c_str(), output, required);
    return  output;
}

xsapi_internal_string local_config::get_value_from_config(
    _In_ const xsapi_internal_string& name,
    _In_ bool required,
    _In_ const xsapi_internal_string& defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    xsapi_internal_string output = defaultValue;
    JsonUtils::ExtractJsonString(m_jsonConfig, name, output, required);
    return output;
}

bool local_config::get_bool_from_config(
    _In_ const xsapi_internal_string& name,
    _In_ bool required,
    _In_ bool defaultValue
    )
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);

    HRESULT err = S_OK;
    if (m_jsonConfig.IsObject() && m_jsonConfig.HasMember(name.c_str()))
    {
        const JsonValue& value = m_jsonConfig[name.c_str()];

        // the value could be "0", "1", or true false
        if (value.IsBool())
        {
            return value.GetBool();
        }
        else if (value.IsString())
        {
            return strncmp(value.GetString(),"1", 1);
        }
    }
    else if (required)
    {
        err = WEB_E_INVALID_JSON_STRING;
    }
    return defaultValue;
}

uint32_t local_config::title_id()
{
    uint32_t titleId = static_cast<uint32_t>(get_uint64_from_config("TitleId", false, 0));

    // Also support title id in string dec form or string hex form 
    if (titleId == 0)
    {
        xsapi_internal_string titleIdString = get_value_from_config("TitleId", false, "");
        if (!titleIdString.empty())
        {
            titleId = utils::internal_string_to_uint32(titleIdString);
            if (titleId == 0)
            {
                xsapi_internal_stringstream ss;
                ss << std::hex << titleIdString;
                ss >> titleId;
            }
        }
    }

    return titleId;
}

xsapi_internal_string local_config::scid()
{
    return get_value_from_config("PrimaryServiceConfigId", false, "");
}

uint32_t local_config::override_title_id()
{
    uint32_t titleId = static_cast<uint32_t>(get_uint64_from_config("OverrideTitleId", false, 0));
    return titleId;
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

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
