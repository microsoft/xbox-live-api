// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if !TV_API && defined(_WIN32)
#include <Pathcch.h>
#endif
#include "xsapi/system.h"
#include "local_config.h"
#include "xbox_system_factory.h"
#include "Utils.h"

using namespace std;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if !TV_API && defined(_WIN32)

xbox_live_result<void> local_config::read()
{
    std::lock_guard<std::mutex> guard(m_jsonConfigLock);
    if (m_jsonConfig.size() > 0)
    {
        return xbox_live_result<void>();
    }

    WCHAR configPath[MAX_PATH] = { 0 };
    if (0 != GetModuleFileName(0, configPath, MAX_PATH))
    {
        WCHAR* lastSlash = wcsrchr(configPath, L'\\');
        if (lastSlash != nullptr)
        {
            *lastSlash = 0;
        }
    }
    PathCchAppend(configPath, MAX_PATH, L"xboxservices.config");

    string_t fileData = utils::read_file_to_string(configPath);
    if(!fileData.empty())
    {
        std::error_code err;
        m_jsonConfig = web::json::value::parse(fileData, err);
        if( !err )
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

string_t local_config::get_registry_path()
{
    return L"Software\\XSAPI\\" + get_value_from_config(L"ClientId", true, _T(""));
}

string_t local_config::get_value_from_local_storage(
    _In_ const string_t& name
    )
{
    string_t value;
    HKEY hKey = nullptr;
    string_t regPath = get_registry_path();

    // if we need to go deeper, we need to parse the path and rewrite from 
    // path a\\b\\c + subkey d\\e to path a\\b\\c\\d + subkey e.
    size_t additionalSubkeysLoc = name.find_last_of(L"\\");

    string_t finalPath;
    string_t finalKey;

    if (additionalSubkeysLoc != string_t::npos)
    {
        finalPath = regPath.append(L"\\" + name.substr(0, additionalSubkeysLoc));
        finalKey = name.substr(additionalSubkeysLoc + 1);
    }
    else
    {
        finalPath = regPath;
        finalKey = name;
    }

    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, finalPath.c_str(), 0, KEY_QUERY_VALUE, &hKey);
    if (result == ERROR_SUCCESS)
    {
        value.resize(1024);
        LONG keyResult = ERROR_MORE_DATA; // begin "needs more data"
        while (keyResult == ERROR_MORE_DATA)
        {            
            DWORD dwLength = static_cast<DWORD>(value.size() * sizeof(WCHAR));
            DWORD dwType = 0;
            keyResult = RegQueryValueEx(hKey, finalKey.c_str(), NULL, &dwType, reinterpret_cast<LPBYTE>(&value[0]), &dwLength);
            
            if (ERROR_MORE_DATA == keyResult)
            {
                value.resize(value.length() * 2);
                continue;
            }
            else if (ERROR_SUCCESS == keyResult)
            {
                value.resize((dwLength / sizeof(WCHAR)) - 1);
            }
            else
            {
                value.resize(0);
            }

            break;
        }

        RegCloseKey(hKey);
    }

    return value;
}

xbox_live_result<void> local_config::write_value_to_local_storage(_In_ const string_t& name, _In_ const string_t& value)
{
    HKEY hKey = nullptr;
    string_t regPath = get_registry_path();
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, regPath.c_str(), 0, KEY_WRITE, &hKey);
    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueExW(hKey, name.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), static_cast<DWORD>(value.length() * sizeof(WORD)));
        RegCloseKey(hKey);
    }

    if (result == ERROR_SUCCESS)
    {
        return xbox_live_result<void>();
    }
    else
    {
        xbox_live_error_code err = static_cast<xbox_live_error_code>(result);
        return xbox_live_result<void>(err, "write_value_to_local_storage failed");
    }
}

xbox_live_result<void> local_config::delete_value_from_local_storage(
    _In_ const string_t& name
    )
{
    string_t regPath = get_registry_path();
    LONG result = RegDeleteKeyValue(HKEY_CURRENT_USER, regPath.c_str(), name.c_str());

    if (result == ERROR_SUCCESS)
    {
        return xbox_live_result<void>();
    }
    else
    {
        xbox_live_error_code err = static_cast<xbox_live_error_code>(result);
        return xbox_live_result<void>(err, "delete_value_from_local_storage failed");
    }
}

#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

