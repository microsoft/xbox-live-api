// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "UnitTestBase.h"
#include <Objbase.h>
#if USING_TAEF
#include "WexTestClass.h"
#include <TraceLoggingProvider.h>
#else 
#include <CppUnitTestLogger.h>
#endif
#include "user.h"
#include "xbox_live_context_internal.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;

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

    if (!IsDebuggerPresent())
    {
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool ModuleCleanup()
{
    CoUninitialize();
    return true;
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
    const char* expected,
    const char* actual,
    std::wstring actualName,
    const WEX::TestExecution::ErrorInfo& errorInfo
)
{
    VERIFY_IS_EQUAL_STR_HELPER(
        xbox::services::Utils::StringTFromUtf8(expected),
        xbox::services::Utils::StringTFromUtf8(actual),
        actualName.c_str(),
        errorInfo
    );
}

void VerifyEqualStr(
    const std::string& expected,
    const std::string& actual,
    std::wstring actualName,
    const WEX::TestExecution::ErrorInfo& errorInfo
)
{
    VerifyEqualStr(expected.data(), actual.data(), std::move(actualName), errorInfo);
}

void VerifyEqualStr(
    std::string expected,
    std::wstring actual,
    std::wstring actualName,
    const WEX::TestExecution::ErrorInfo& errorInfo
    )
{
    VERIFY_IS_EQUAL_STR_HELPER(
        xbox::services::utils::string_t_from_utf8(expected.c_str()),
        actual, actualName.c_str(), errorInfo
        );
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

void VerifyEqualStr(
    xsapi_internal_string expected,
    xsapi_internal_string actual,
    std::wstring actualName,
    const WEX::TestExecution::ErrorInfo& errorInfo
    )
{
    VerifyEqualStr(expected.data(), actual.data(), std::move(actualName), errorInfo);
}

#endif

