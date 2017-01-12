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

#include "service_call_logger.h"
#include "service_call_logger_data.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

static XBOX_LIVE_NAMESPACE::system::xbox_live_mutex g_serviceLoggerSingletonLock;
static std::shared_ptr<service_call_logger> g_serviceLoggerSingleton;

std::shared_ptr<service_call_logger> service_call_logger::get_singleton_instance()
{
    std::lock_guard<std::mutex> guard(g_serviceLoggerSingletonLock.get());
    if (g_serviceLoggerSingleton == nullptr)
    {
        g_serviceLoggerSingleton = std::shared_ptr<service_call_logger>(new service_call_logger());
    }
    return g_serviceLoggerSingleton;
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
        m_fileLocation = _T("");
        m_fileStream.close();
    }
}

bool service_call_logger::is_enabled()
{
    return m_isEnabled;
}

void service_call_logger::log(_In_ const string_t& item)
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

string_t service_call_logger::file_location()
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
    const string_t fileDir = currentAppData->TemporaryFolder->Path->Data();

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

    m_fileLocation = fileLocation;

    m_fileStream.open(m_fileLocation, std::ios_base::app | std::ios_base::out);

    if (!m_fileStream.is_open())
    {
        LOGS_ERROR <<"WriteFile failed. Path: " << m_fileLocation;
        return;
    }
}
#endif
void service_call_logger::add_data_to_file(_In_ const string_t& data)
{
    // Json string is all ansi, so store in a more compact format
    std::string jsonAnsi(data.begin(), data.end());
    
    if (m_fileStream.is_open())
    {
        m_fileStream << jsonAnsi.c_str();
        m_fileStream.flush();
    }
    else
    {
        LOGS_ERROR << "WriteFile failed.Path '" << m_fileLocation << "'; Contents: '" << jsonAnsi << "'";
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
