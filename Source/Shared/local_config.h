// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class local_config
{
public:
    static std::shared_ptr<local_config> get_local_config_singleton();

    local_config();
    NO_COPY_AND_ASSIGN(local_config);

#if !TV_API

    virtual xsapi_internal_string get_value_from_config(
        _In_ const xsapi_internal_string& name,
        _In_ bool required,
        _In_ const xsapi_internal_string& defaultValue);

    virtual uint64_t get_uint64_from_config(
        _In_ const xsapi_internal_string& name,
        _In_ bool required,
        _In_ uint64_t defaultValue);
    
    virtual bool get_bool_from_config(
        _In_ const xsapi_internal_string& name,
        _In_ bool required,
        _In_ bool defaultValue);

    virtual uint32_t title_id();
    virtual uint32_t override_title_id();
    virtual xsapi_internal_string scid();
    virtual xsapi_internal_string override_scid();
    virtual xsapi_internal_string environment_prefix();
    virtual xsapi_internal_string environment();
    virtual xsapi_internal_string sandbox();
    virtual xsapi_internal_string client_secret();
    virtual bool use_first_party_token();
    virtual bool is_creators_title();
    virtual xsapi_internal_string msa_sub_target();
    virtual xsapi_internal_string scope();

    virtual xsapi_internal_string get_value_from_local_storage(_In_ const xsapi_internal_string& name);
    virtual xbox_live_result<void> write_value_to_local_storage(_In_ const xsapi_internal_string& name, _In_ const xsapi_internal_string& value);
    virtual xbox_live_result<void> delete_value_from_local_storage(_In_ const xsapi_internal_string& name);

#if XSAPI_I
    virtual string_t apns_environment();
#elif XSAPI_A
    bool use_brokered_authorization();
    bool is_android_native_activity();
#endif
    
#endif

protected:
#if !TV_API
    web::json::value& read_config_file();
    string_t get_registry_path();

#endif

#if XSAPI_I
    string_t get_local_storage_folder();
#endif

#if !TV_API
    virtual xbox_live_result<void> read();

    web::json::value m_jsonConfig;
    std::mutex m_jsonConfigLock;
#if XSAPI_U
    web::json::value m_jsonLocalStorage;
    std::mutex m_jsonLocalStorageLock;
#endif
#endif
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
