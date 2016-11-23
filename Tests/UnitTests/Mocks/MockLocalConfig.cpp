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
#include "MockLocalConfig.h"
#include "StockMocks.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockLocalConfig::MockLocalConfig()
{
    reinit();
}

void MockLocalConfig::reinit()
{
    ResultValue = string_t();
    ResultValue64 = 0;
    ResultValueBool = false;
    MockTitleId = 1234;
    MockOverrideTitleId = 12345;
    MockScid = _T("MockScid");
    MockOverrideScid = _T("");
    MockEnvironment = _T(".mockenv");
    MockEnvironmentPrefix = _T("");
    MockSandbox = _T("MockSandbox");
    MockClientSecret = _T("");
}

xbox_live_result<void> MockLocalConfig::read()
{
    return xbox_live_result<void>();
}

string_t MockLocalConfig::get_value_from_config(_In_ const string_t& name, _In_ bool required, _In_ const string_t& defaultValue)
{
    string_t val = ResultValue;
    return val;
}

uint64_t MockLocalConfig::get_uint64_from_config(
    _In_ const string_t& name,
    _In_ bool required,
    _In_ uint64_t defaultValue)
{
    uint64_t val = ResultValue64;
    return val;
}

bool MockLocalConfig::get_bool_from_config(_In_ const string_t& name, _In_ bool required, _In_ bool defaultValue)
{
    bool val = ResultValueBool;
    return val;
}

string_t MockLocalConfig::get_value_from_local_storage(_In_ const string_t& name)
{
    string_t val = ResultValue;
    return val;
}

xbox_live_result<void> MockLocalConfig::write_value_to_local_storage(_In_ const string_t& name, _In_ const string_t& value)
{
    return xbox_live_result<void>();
}

xbox_live_result<void> MockLocalConfig::delete_value_from_local_storage(_In_ const string_t& name)
{
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

string_t MockLocalConfig::scid()
{
    return MockScid;
}

string_t MockLocalConfig::override_scid()
{
    return MockOverrideScid;
}

string_t MockLocalConfig::environment_prefix()
{
    return MockEnvironmentPrefix;
}

string_t MockLocalConfig::environment()
{
    return MockEnvironment;
}

string_t MockLocalConfig::sandbox() 
{
    return MockSandbox;
}

string_t MockLocalConfig::client_secret()
{
    return MockClientSecret;
}

bool MockLocalConfig::use_first_party_token()
{
    return false;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

