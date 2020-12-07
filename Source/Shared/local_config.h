// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class local_config
{
public:
    local_config();
    NO_COPY_AND_ASSIGN(local_config);

#if !(HC_PLATFORM == HC_PLATFORM_XDK)

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
    virtual xbl_result<void> write_value_to_local_storage(_In_ const xsapi_internal_string& name, _In_ const xsapi_internal_string& value);
    virtual xbl_result<void> delete_value_from_local_storage(_In_ const xsapi_internal_string& name);

    virtual xbl_result<void> read();

#if !HC_PLATFORM_IS_MICROSOFT
    /// <summary>
    /// Reads a value from local storage that is part of a collection.
    /// </summary>
    virtual xsapi_internal_string get_value_from_local_storage_collection(_In_ const xsapi_internal_string& collectionName, _In_ const xsapi_internal_string& name);
    
    /// <summary>
    /// Returns the list of keys that are in local storage in a given collection.
    /// </summary>
    virtual xsapi_internal_vector<xsapi_internal_string> get_keys_from_local_storage_collection(_In_ const xsapi_internal_string& collectionName);
    
    /// <summary>
    /// Writes a value to local storage as part of a collection.
    /// </summary>
    virtual xbox_live_result<void> write_value_to_local_storage_collection(_In_ const xsapi_internal_string& collectionName, _In_ const xsapi_internal_string& name, _In_ const xsapi_internal_string& value);
    
    /// <summary>
    /// Deletes a value from local storage that is part of a collection.
    /// </summary>
    virtual xbox_live_result<void> delete_value_from_local_storage_collection(_In_ const xsapi_internal_string& collectionName, _In_ const xsapi_internal_string& name);
    
    /// <summary>
    /// Deletes an entire collection from local storage.
    /// </summary>
    virtual xbox_live_result<void> delete_local_storage_collection(_In_ const xsapi_internal_string& collectionName);
#endif
    
#if HC_PLATFORM == HC_PLATFORM_IOS
    virtual xsapi_internal_string APNSEnvironment();
#elif HC_PLATFORM == HC_PLATFORM_ANDROID
    bool use_brokered_authorization();
    bool is_android_native_activity();
#endif
    
#endif

protected:
#if !(HC_PLATFORM == HC_PLATFORM_XDK)
    JsonDocument read_config_file();
    string_t get_registry_path();

    JsonDocument m_jsonConfig;
    std::mutex m_jsonConfigLock;
#endif
    
#if !HC_PLATFORM_IS_MICROSOFT
    virtual void read_local_storage();
    virtual string_t get_local_storage_folder();
    
    JsonDocument m_jsonLocalStorage;
    std::mutex m_jsonLocalStorageLock;
#endif
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
