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

    virtual string_t get_value_from_config(
        _In_ const string_t& name,
        _In_ bool required,
        _In_ const string_t& defaultValue);

    virtual uint64_t get_uint64_from_config(
        _In_ const string_t& name,
        _In_ bool required,
        _In_ uint64_t defaultValue);
    
    virtual bool get_bool_from_config(
        _In_ const string_t& name,
        _In_ bool required,
        _In_ bool defaultValue);

    virtual uint32_t title_id();
    virtual uint32_t override_title_id();
    virtual string_t scid();
    virtual string_t override_scid();
    virtual string_t environment_prefix();
    virtual string_t environment();
    virtual string_t sandbox();
    virtual string_t client_secret();
    virtual bool use_first_party_token();
    virtual bool is_creators_title();

    virtual string_t get_value_from_local_storage(_In_ const string_t& name);
    virtual xbox_live_result<void> write_value_to_local_storage(_In_ const string_t& name, _In_ const string_t& value);
    virtual xbox_live_result<void> delete_value_from_local_storage(_In_ const string_t& name);

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
