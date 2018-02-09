// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "service_call_logger.h"
#include "service_call_logger_data.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<service_call_logger> service_call_logger::get_singleton_instance()
{
    auto xsapiSingleton = xbox::services::get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
    if (xsapiSingleton->m_serviceLoggerSingleton == nullptr)
    {
        xsapiSingleton->m_serviceLoggerSingleton = std::shared_ptr<service_call_logger>(new service_call_logger());
    }
    return xsapiSingleton->m_serviceLoggerSingleton;
}

service_call_logger::service_call_logger() :
    m_isEnabled(false),
    m_firstWrite(false)
{
}

service_call_logger::~service_call_logger()
{
    disable();
}

void service_call_logger::enable()
{
    if (m_isEnabled)
    {
        return;
    }
    create_log_file();
    m_isEnabled = true;
    m_firstWrite = true;
}

void service_call_logger::disable()
{
    std::lock_guard<std::mutex> lock(m_writeLock.get());
    if (!m_isEnabled)
    {
        return;
    }
    m_isEnabled = false;
    if(m_fileStream.is_open())
    {
        m_fileLocation = "";
        m_fileStream.close();
    }
}

bool service_call_logger::is_enabled()
{
    return m_isEnabled;
}

void service_call_logger::log(_In_ const xsapi_internal_string& item)
{
    if (m_isEnabled)
    {
        std::lock_guard<std::mutex> lock(m_writeLock.get());
        if (m_firstWrite)
        {
            add_data_to_file(service_call_logger_data::get_csv_header());
            m_firstWrite = false;
        }
        add_data_to_file(item);
    }
}

xsapi_internal_string service_call_logger::file_location()
{
    return m_fileLocation;
}
#if TV_API || defined(_WIN32)       
void service_call_logger::create_log_file()
{
    //
    // Write to title scratch folder

    SYSTEMTIME stLocalTime;
    GetLocalTime(&stLocalTime);
    wchar_t fileLocation[MAX_PATH];

#if TV_API
    swprintf_s(fileLocation, _T("d:\\%s-%04d%02d%02d-%02d%02d%02d.csv"),
        _T("callHistoryJson"),
        stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
        stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
#elif UWP_API
    Windows::Storage::ApplicationData^ currentAppData = Windows::Storage::ApplicationData::Current;
    const xsapi_internal_wstring fileDir = currentAppData->TemporaryFolder->Path->Data();

    swprintf_s(fileLocation, _T("%s\\%s-%04d%02d%02d-%02d%02d%02d.csv"),
        fileDir.c_str(),
        _T("callHistoryJson"),
        stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
        stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
#else
    swprintf_s(fileLocation, _T("%s-%04d%02d%02d-%02d%02d%02d.csv"),
        _T("callHistoryJson"),
        stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
        stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
#endif

    m_fileLocation = utils::internal_string_from_string_t(fileLocation);

    m_fileStream.open(m_fileLocation.data(), std::ios_base::app | std::ios_base::out);

    if (!m_fileStream.is_open())
    {
        LOGS_ERROR <<"WriteFile failed. Path: " << m_fileLocation;
        return;
    }
}
#endif
void service_call_logger::add_data_to_file(_In_ const xsapi_internal_string& data)
{
    if (m_fileStream.is_open())
    {
        m_fileStream << data.c_str();
        m_fileStream.flush();
    }
    else
    {
        LOGS_ERROR << "WriteFile failed.Path '" << m_fileLocation << "'; Contents: '" << data << "'";
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
