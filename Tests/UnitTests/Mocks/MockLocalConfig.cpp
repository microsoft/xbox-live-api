// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MockLocalConfig.h"
#include "StockMocks.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockLocalConfig::MockLocalConfig()
{
    reinit();
}

void MockLocalConfig::reinit()
{
    ResultValue = xsapi_internal_string();
    ResultValue64 = 0;
    ResultValueBool = false;
    MockTitleId = 1234;
    MockOverrideTitleId = 12345;
    MockScid = "MockScid";
    MockOverrideScid = "";
    MockEnvironment = ".mockenv";
    MockEnvironmentPrefix = "";
    MockSandbox = "MockSandbox";
    MockClientSecret = "";
}

xbox_live_result<void> MockLocalConfig::read()
{
    return xbox_live_result<void>();
}

xsapi_internal_string MockLocalConfig::get_value_from_config(_In_ const xsapi_internal_string& name, _In_ bool required, _In_ const xsapi_internal_string& defaultValue)
{
    UNREFERENCED_PARAMETER(defaultValue);
    UNREFERENCED_PARAMETER(required);
    UNREFERENCED_PARAMETER(name);
    xsapi_internal_string val = ResultValue;
    return val;
}

uint64_t MockLocalConfig::get_uint64_from_config(
    _In_ const xsapi_internal_string& name,
    _In_ bool required,
    _In_ uint64_t defaultValue)
{
    UNREFERENCED_PARAMETER(defaultValue);
    UNREFERENCED_PARAMETER(required);
    UNREFERENCED_PARAMETER(name);
    uint64_t val = ResultValue64;
    return val;
}

bool MockLocalConfig::get_bool_from_config(_In_ const xsapi_internal_string& name, _In_ bool required, _In_ bool defaultValue)
{
    UNREFERENCED_PARAMETER(defaultValue);
    UNREFERENCED_PARAMETER(required);
    UNREFERENCED_PARAMETER(name);
    bool val = ResultValueBool;
    return val;
}

xsapi_internal_string MockLocalConfig::get_value_from_local_storage(_In_ const xsapi_internal_string& name)
{
    UNREFERENCED_PARAMETER(name);
    xsapi_internal_string val = ResultValue;
    return val;
}

xbox_live_result<void> MockLocalConfig::write_value_to_local_storage(_In_ const xsapi_internal_string& name, _In_ const xsapi_internal_string& value)
{
    UNREFERENCED_PARAMETER(value);
    UNREFERENCED_PARAMETER(name);
    return xbox_live_result<void>();
}

xbox_live_result<void> MockLocalConfig::delete_value_from_local_storage(_In_ const xsapi_internal_string& name)
{
    UNREFERENCED_PARAMETER(name);
    return xbox_live_result<void>();
}

uint32_t MockLocalConfig::title_id()
{
    return MockTitleId;
}

uint32_t MockLocalConfig::override_title_id()
{
    return MockOverrideTitleId;
}

xsapi_internal_string MockLocalConfig::scid()
{
    return MockScid;
}

xsapi_internal_string MockLocalConfig::override_scid()
{
    return MockOverrideScid;
}

xsapi_internal_string MockLocalConfig::environment_prefix()
{
    return MockEnvironmentPrefix;
}

xsapi_internal_string MockLocalConfig::environment()
{
    return MockEnvironment;
}

xsapi_internal_string MockLocalConfig::sandbox()
{
    return MockSandbox;
}

xsapi_internal_string MockLocalConfig::client_secret()
{
    return MockClientSecret;
}

bool MockLocalConfig::use_first_party_token()
{
    return false;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

