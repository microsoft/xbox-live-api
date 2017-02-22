// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/system.h"
#include "local_config.h"
#include "xbox_system_factory.h"
#include "Utils.h"

using namespace std;
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Foundation;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if !TV_API
xbox_live_result<void> local_config::read()
{
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
}

string_t local_config::get_value_from_local_storage(
    _In_ const string_t& name
    )
{
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;

        auto values = localSettings->Values;
        String^ key = ref new String(name.c_str());
        String^ value = nullptr;
        
        if (values->HasKey(key))
        {
            value = safe_cast<String^>(key);
        }
         
        if (!value)
        {
            return L"";
        }
        else
        {
            return value->Data();
        }
    }
    catch (Exception^ ex)
    {
        return L"";
    }
}

xbox_live_result<void> local_config::write_value_to_local_storage(_In_ const string_t& name, _In_ const string_t& value)
{
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;

        auto values = localSettings->Values;
        values->Insert(
            ref new Platform::String(name.c_str()),
            dynamic_cast<PropertyValue^>(PropertyValue::CreateString(ref new Platform::String(value.c_str())))
            );
        return xbox_live_result<void>();
    }
    catch (Exception^ ex)
    {
        xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
        return xbox_live_result<void>(err, "write_value_to_local_storage exception");
    }
}

xbox_live_result<void> local_config::delete_value_from_local_storage(
    _In_ const string_t& name
    )
{
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
        localSettings->Values->Remove(ref new Platform::String(name.c_str()));
        return xbox_live_result<void>();
    }
    catch (Exception^ ex)
    {
        xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
        return xbox_live_result<void>(err, "delete_value_from_local_storage exception");
    }
}


#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

