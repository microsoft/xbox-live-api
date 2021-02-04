// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "local_config.h"
#include "xsapi_utils.h"

using namespace std;
#if __cplusplus_winrt
using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Foundation;
#endif
using namespace xbox::services;
using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if !(HC_PLATFORM == HC_PLATFORM_XDK)
xbl_result<void> local_config::read()
{
#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM == HC_PLATFORM_UWP
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    if (!m_jsonConfig.IsNull())
    {
        return xbl_result<void>();
    }

#if HC_PLATFORM == HC_PLATFORM_UWP
    Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
    Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;
    xsapi_internal_string_t configPath = xsapi_internal_string_t(installedLocation->Path->Data()) + L"\\xboxservices.config";
#elif HC_PLATFORM == HC_PLATFORM_WIN32
    // Don't rely on the current directory to be set correctly.
    // Instead assume xboxservices.config is next to the exe
    WCHAR processPath[MAX_PATH + 1];
    GetModuleFileName(NULL, processPath, MAX_PATH + 1);
    WCHAR* lastSlash = wcsrchr(processPath, L'\\');
    if (lastSlash)
    {
        *lastSlash = L'\0';
    }

    xsapi_internal_stringstream_t path;
    path << processPath << _T("\\xboxservices.config");
    xsapi_internal_string_t configPath = path.str();
#endif

    xsapi_internal_string_t fileData = utils::read_file_to_string(configPath);
    if( !fileData.empty() )
    {
        m_jsonConfig.Parse(xbox::services::convert::to_utf8string(fileData).c_str());
        if (!m_jsonConfig.HasParseError())
        {
            return xbl_result<void>();
        }
        else
        {
            return xbl_result<void>(
                make_error_code(xbl_error_code::invalid_config),
                "Invalid config file"
                );
        }
    }
    else
    {
        return xbl_result<void>(
            make_error_code(xbl_error_code::invalid_config),
            "ERROR: Could not find xboxservices.config"
            );
    }
#else 
    return xbl_result<void>();
#endif
}

xsapi_internal_string local_config::get_value_from_local_storage(
    _In_ const xsapi_internal_string& name
    )
{
#if HC_PLATFORM == HC_PLATFORM_UWP
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;

        auto values = localSettings->Values;
        Platform::String^ key = ref new Platform::String(utils::string_t_from_internal_string(name).data());
        Platform::String^ value = nullptr;
        
        if (values->HasKey(key))
        {
            value = safe_cast<Platform::String^>(key);
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
    catch (Platform::Exception^ ex)
    {
        return "";
    }
#else 
    UNREFERENCED_PARAMETER(name);
    return "";
#endif
}

xbl_result<void> local_config::write_value_to_local_storage(_In_ const xsapi_internal_string& name, _In_ const xsapi_internal_string& value)
{
#if HC_PLATFORM == HC_PLATFORM_UWP
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;

        auto values = localSettings->Values;
        values->Insert(
            ref new Platform::String(utils::string_t_from_internal_string(name).c_str()),
            dynamic_cast<PropertyValue^>(PropertyValue::CreateString(ref new Platform::String(utils::string_t_from_internal_string(value).c_str())))
            );
        return xbl_result<void>();
    }
    catch (Platform::Exception^ ex)
    {
        xbl_error_code err = utils::convert_exception_to_xbox_live_error_code();
        return xbl_result<void>(err, "write_value_to_local_storage exception");
    }
#else 
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(value);
    return xbl_result<void>();
#endif
}

xbl_result<void> local_config::delete_value_from_local_storage(
    _In_ const xsapi_internal_string& name
    )
{
#if HC_PLATFORM == HC_PLATFORM_UWP
    try
    {
        ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
        localSettings->Values->Remove(ref new Platform::String(utils::string_t_from_internal_string(name).c_str()));
        return xbl_result<void>();
    }
    catch (Platform::Exception^ ex)
    {
        xbl_error_code err = utils::convert_exception_to_xbox_live_error_code();
        return xbl_result<void>(err, "delete_value_from_local_storage exception");
    }
#else
    UNREFERENCED_PARAMETER(name);
    return xbl_result<void>();
#endif
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

