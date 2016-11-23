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
#if !TV_API && defined(_WIN32)
#include <Pathcch.h>
#endif
#include "xsapi/system.h"
#include "local_config.h"
#include "xbox_system_factory.h"
#include "Utils.h"
#if XSAPI_A
#include "a/java_interop.h"
#endif
#include <fstream>

using namespace std;

const string_t c_localStorageFileName = _T("XBLStoage.json");

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void write_local_storage_helper(const string_t& filePath, const string_t& data)
{
    ofstream file(filePath, ofstream::trunc);
    if (file.is_open())
    {
        file << data;
    }
    else
    {
        LOG_ERROR("error on storage write");
    }
}

string_t file_stream_to_string(const ifstream& fileStream)
{
    stringstream_t ss;
    ss << fileStream.rdbuf();
    return ss.str();
}

string_t read_local_storage_helper(const string_t& filePath)
{
    try
    {
        ifstream file(filePath, ifstream::in);
        if (file) 
        {
            return file_stream_to_string(file);
        }
    }
    catch (std::ifstream::failure e)
    {
        LOG_ERROR("read_local_storage_helper: ifstream failure");
        return string_t();
    }

    return string_t();
}

string_t local_config::get_value_from_local_storage(_In_ const string_t& name)
{
    std::lock_guard<std::mutex> guard(m_jsonLocalStorageLock);
#if XSAPI_A
    string_t filePath = java_interop::get_java_interop_singleton()->get_local_storage_path() + c_localStorageFileName;
#else
    string_t filePath = get_local_storage_folder() + c_localStorageFileName;
#endif

    if (!filePath.empty())
    {
        std::error_code errc;
        m_jsonLocalStorage = web::json::value::parse(read_local_storage_helper(filePath), errc);
        if(errc)
        {
            return string_t();
        }
    }

    return utils::extract_json_string(m_jsonLocalStorage, name);
}

xbox_live_result<void> local_config::write_value_to_local_storage(_In_ const string_t& name, _In_ const string_t& value)
{
    std::lock_guard<std::mutex> guard(m_jsonLocalStorageLock);
#if XSAPI_A
    string_t filePath = java_interop::get_java_interop_singleton()->get_local_storage_path() + c_localStorageFileName;
#else
    string_t filePath = get_local_storage_folder() + c_localStorageFileName;
#endif

    if (!filePath.empty())
    {
        // TODO:  Take a look at this.  Seeing a crash when the filePath!="" but parse throws exception
        std::error_code errc;
        m_jsonLocalStorage = web::json::value::parse(read_local_storage_helper(filePath), errc);
        if(errc)
        {
            LOG_INFO("Local config read faild while write_value_to_local_storage!");
            m_jsonLocalStorage = web::json::value();
        }
        m_jsonLocalStorage[name] = web::json::value(value);
        write_local_storage_helper(filePath, m_jsonLocalStorage.serialize());
    }
    return xbox_live_result<void>();
}

xbox_live_result<void> local_config::delete_value_from_local_storage(_In_ const string_t& name)
{
    std::lock_guard<std::mutex> guard(m_jsonLocalStorageLock);
#if XSAPI_A
    string_t filePath = java_interop::get_java_interop_singleton()->get_local_storage_path() + c_localStorageFileName;
#else
    string_t filePath = get_local_storage_folder() + c_localStorageFileName;
#endif

    if (!filePath.empty())
    {
        std::error_code errc;
        m_jsonLocalStorage = web::json::value::parse(read_local_storage_helper(filePath), errc);
        if(errc)
        {
            m_jsonLocalStorage = web::json::value();
            return xbox_live_result<void>(xbox_live_error_code::runtime_error, _T("Error parsing from local storage."));
        }
        try
        {
            m_jsonLocalStorage.erase(name);
        }
        catch (const web::json::json_exception& ex)
        {
            LOG_INFO(ex.what());
        }
        write_local_storage_helper(filePath, m_jsonLocalStorage.serialize());
    }
    return xbox_live_result<void>();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
