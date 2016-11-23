////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "UnitTestIncludes.h"
#include "UnitTestBase.h"
#include <Objbase.h>
#if USING_TAEF
#include "WexTestClass.h"
#include "telemetry.h"
#include <TraceLoggingProvider.h>
#else 
#include <CppUnitTestLogger.h>
#endif

using namespace WEX::Logging;

BEGIN_MODULE()
    MODULE_PROPERTY(L"XtpAreaPath", L"XBox Live\\Client") 
END_MODULE()

MODULE_SETUP(ModuleSetup);
MODULE_CLEANUP(ModuleCleanup);

////////////////////////////////////////////////////////////////////////////////
bool ModuleSetup()
{
#pragma warning(suppress: 6031)
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    WEX::Common::String strOpt;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool ModuleCleanup()
{
    CoUninitialize();
    return true;
}

// 
/// <summary>
/// Adds the response logger subscription to help debug issues using the response and request data
/// </summary>
void UnitTestBase::StartResponseLogging()
{
    Log::Comment(L"Starting Response Logger");
} 

/// <summary>
/// Removes the response logger subscription
/// </summary>
void UnitTestBase::RemoveResponseLogging()
{
    Log::Comment(L"Removing Response Logger");
}

UnitTestBaseProperties::UnitTestBaseProperties(LPCWSTR strMsg)
{
    m_strMsg = strMsg;
    LogFormatString(FormatString(L"Start TestMethod: %s", m_strMsg).c_str());
}

UnitTestBaseProperties::~UnitTestBaseProperties()
{
    LogFormatString(FormatString(L"End TestMethod: %s", m_strMsg).c_str());
}

/// <summary>
/// Formats the string with StringCchVPrintfW. Trims string if size is greater than 2048.
/// </summary>
/// <param name="strMsg">The formatter string and/or message to log</param>
/// <param name="args">The params to log</param>
std::wstring FormatString(LPCWSTR strMsg, ...)
{
    WCHAR strBuffer[2048];

    va_list args;
    va_start(args, strMsg);
    StringCchVPrintfW(strBuffer, 2048, strMsg, args);
    strBuffer[2047] = L'\0';

    va_end(args);

    return std::wstring(strBuffer);
}

/// <summary>
/// Calls Log::Comment
/// </summary>
/// <param name="strMsg">The message to log</param>
void LogFormatString(LPCWSTR strMsg, ...)
{
    TraceLoggingWrite(g_hUnitTestTraceLoggingProvider, "UnitTest", TraceLoggingWideString(strMsg, "test_log"));

#ifdef USING_TAEF
#if ENABLE_SCREEN_LOGGING
    WEX::Logging::Log::Comment(strMsg);
#endif
#else 
    Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(strMsg);
#endif
}

#ifdef USING_TAEF

void VerifyEqualStr(
    Platform::String^ expected,
    std::wstring actual,
    std::wstring actualName,
    const WEX::TestExecution::ErrorInfo& errorInfo
    )
{
    VERIFY_IS_EQUAL_STR_HELPER(expected->Data(), actual, actualName.c_str(), errorInfo);
}

void VerifyEqualStr(
    Platform::String^ expected, 
    Platform::String^ actual, 
    std::wstring actualName, 
    const WEX::TestExecution::ErrorInfo& errorInfo
    )
{
    VERIFY_IS_EQUAL_STR_HELPER(expected->Data(), actual->Data(), actualName.c_str(), errorInfo);
}

void VerifyEqualStr(
    std::wstring expected, 
    std::wstring actual, 
    std::wstring actualName, 
    const WEX::TestExecution::ErrorInfo& errorInfo
    )
{
    VERIFY_IS_EQUAL_STR_HELPER(expected, actual, actualName.c_str(), errorInfo);
}

#endif