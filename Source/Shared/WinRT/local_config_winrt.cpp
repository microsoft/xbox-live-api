// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/system.h"
#include "local_config.h"
#include "xbox_system_factory.h"
#include "Utils.h"

using namespace std;
#if UWP_API || TV_API || UNIT_TEST_SERVICES
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Foundation;
#endif
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if !TV_API
xbox_live_result<void> local_config::read()
{
#if UWP_API
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    if (m_jsonConfig.size() > 0)
    {
        return xbox_live_result<void>();
    }

    Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
    Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;
    string_t configPath = string_t(installedLocation->Path->Data()) + _T("\\xboxservices.config");

    string_t fileData = utils::read_file_to_string(configPath);
    if( !fileData.empty() )
    {
        std::error_code err;
        m_jsonConfig = web::json::value::parse(fileData, err);
        if (!err)
        {
            return xbox_live_result<void>();
        }
        else
        {
            return xbox_live_result<void>(
                std::make_error_code(xbox::services::xbox_live_error_code::invalid_config),
                "Invalid config file"
                );
        }
    }
    else
    {
        return xbox_live_result<void>(
            std::make_error_code(xbox::services::xbox_live_error_code::invalid_config),
            "ERROR: Could not find xboxservices.config"
            );
    }
#else 
    return xbox_live_result<void>();
#endif
}

xsapi_internal_string local_config::get_value_from_local_storage(
    _In_ const xsapi_internal_string& name
    )
{
#if UWP_API
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;

        auto values = localSettings->Values;
        String^ key = ref new String(utils::string_t_from_internal_string(name).data());
        String^ value = nullptr;
        
        if (values->HasKey(key))
        {
            value = safe_cast<String^>(key);
        }
         
        if (!value)
        {
            return "";
        }
        else
        {
            return xsapi_internal_string(utils::internal_string_from_utf16(value->Data()));
        }
    }
    catch (Exception^ ex)
    {
        return "";
    }
#else 
    UNREFERENCED_PARAMETER(name);
    return "";
#endif
}

xbox_live_result<void> local_config::write_value_to_local_storage(_In_ const xsapi_internal_string& name, _In_ const xsapi_internal_string& value)
{
#if UWP_API
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;

        auto values = localSettings->Values;
        values->Insert(
            ref new Platform::String(utils::string_t_from_internal_string(name).c_str()),
            dynamic_cast<PropertyValue^>(PropertyValue::CreateString(ref new Platform::String(utils::string_t_from_internal_string(value).c_str())))
            );
        return xbox_live_result<void>();
    }
    catch (Exception^ ex)
    {
        xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
        return xbox_live_result<void>(err, "write_value_to_local_storage exception");
    }
#else 
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(value);
    return xbox_live_result<void>();
#endif
}

xbox_live_result<void> local_config::delete_value_from_local_storage(
    _In_ const xsapi_internal_string& name
    )
{
#if UWP_API
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
        localSettings->Values->Remove(ref new Platform::String(utils::string_t_from_internal_string(name).c_str()));
        return xbox_live_result<void>();
    }
    catch (Exception^ ex)
    {
        xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
        return xbox_live_result<void>(err, "delete_value_from_local_storage exception");
    }
#else
    UNREFERENCED_PARAMETER(name);
    return xbox_live_result<void>();
#endif
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

