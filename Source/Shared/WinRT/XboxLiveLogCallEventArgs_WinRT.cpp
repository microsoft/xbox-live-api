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
#include "XboxLiveLogCallEventArgs_WinRT.h"
#include "utils.h"

using namespace Concurrency;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

XboxLiveLogCallEventArgs::XboxLiveLogCallEventArgs(
    _In_ xbox_services_diagnostics_trace_level level,
    _In_ const std::string& category,
    _In_ const std::string& message
    )
{
    m_diagnosticLevel = static_cast<XboxServicesDiagnosticsTraceLevel>(level);
    m_category = ref new Platform::String(std::wstring(category.begin(), category.end()).c_str());
    m_message = ref new Platform::String(std::wstring(message.begin(), message.end()).c_str());

}

XboxServicesDiagnosticsTraceLevel XboxLiveLogCallEventArgs::Level::get()
{
    return m_diagnosticLevel;
}

Platform::String^ XboxLiveLogCallEventArgs::Category::get()
{
    return m_category;
}

Platform::String^ XboxLiveLogCallEventArgs::Message::get()
{
    return m_message;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
