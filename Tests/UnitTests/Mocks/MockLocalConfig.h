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
#include "pch.h"
#include "local_config.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockLocalConfig : public local_config
{
public:
    xbox_live_result<void> read() override;

    string_t get_value_from_config(_In_ const string_t& name, _In_ bool required, _In_ const string_t& defaultValue) override;
    uint64_t get_uint64_from_config(_In_ const string_t& name, _In_ bool required, _In_ uint64_t defaultValue) override;
    bool get_bool_from_config(_In_ const string_t& name, _In_ bool required, _In_ bool defaultValue) override;
    string_t get_value_from_local_storage(_In_ const string_t& name) override;
    xbox_live_result<void> write_value_to_local_storage(_In_ const string_t& name, _In_ const string_t& value) override;
    xbox_live_result<void> delete_value_from_local_storage(_In_ const string_t& name) override;

    uint32_t title_id() override;
    uint32_t override_title_id() override;
    string_t scid() override;
    string_t override_scid() override;
    string_t environment_prefix() override;
    string_t environment() override;
    string_t sandbox() override;
    string_t client_secret() override;
    bool use_first_party_token() override;

    MockLocalConfig();

    void reinit();

    string_t ResultValue;
    uint64_t ResultValue64;
    bool ResultValueBool;
    uint32_t MockTitleId;
    uint32_t MockOverrideTitleId;
    string_t MockScid;
    string_t MockOverrideScid;
    string_t MockEnvironment;
    string_t MockEnvironmentPrefix;
    string_t MockSandbox;
    string_t MockClientSecret;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

