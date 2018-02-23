// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "local_config.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockLocalConfig : public local_config
{
public:
    xbox_live_result<void> read() override;

    xsapi_internal_string get_value_from_config(_In_ const xsapi_internal_string& name, _In_ bool required, _In_ const xsapi_internal_string& defaultValue) override;
    uint64_t get_uint64_from_config(_In_ const xsapi_internal_string& name, _In_ bool required, _In_ uint64_t defaultValue) override;
    bool get_bool_from_config(_In_ const xsapi_internal_string& name, _In_ bool required, _In_ bool defaultValue) override;
    xsapi_internal_string get_value_from_local_storage(_In_ const xsapi_internal_string& name) override;
    xbox_live_result<void> write_value_to_local_storage(_In_ const xsapi_internal_string& name, _In_ const xsapi_internal_string& value) override;
    xbox_live_result<void> delete_value_from_local_storage(_In_ const xsapi_internal_string& name) override;

    uint32_t title_id() override;
    uint32_t override_title_id() override;
    xsapi_internal_string scid() override;
    xsapi_internal_string override_scid() override;
    xsapi_internal_string environment_prefix() override;
    xsapi_internal_string environment() override;
    xsapi_internal_string sandbox() override;
    xsapi_internal_string client_secret() override;
    bool use_first_party_token() override;

    MockLocalConfig();

    void reinit();

    xsapi_internal_string ResultValue;
    uint64_t ResultValue64;
    bool ResultValueBool;
    uint32_t MockTitleId;
    uint32_t MockOverrideTitleId;
    xsapi_internal_string MockScid;
    xsapi_internal_string MockOverrideScid;
    xsapi_internal_string MockEnvironment;
    xsapi_internal_string MockEnvironmentPrefix;
    xsapi_internal_string MockSandbox;
    xsapi_internal_string MockClientSecret;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

