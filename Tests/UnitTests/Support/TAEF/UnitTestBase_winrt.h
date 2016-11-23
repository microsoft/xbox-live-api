/////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Macros_WinRT.h"
#include "User_WinRT.h"

#include "XboxLiveContext_WinRT.h"

using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;

// Global help function
Microsoft::Xbox::Services::System::XboxLiveUser^ SignInUserWithMocks_WinRT(const string_t& id = string_t());
Microsoft::Xbox::Services::XboxLiveContext^ GetMockXboxLiveContext_WinRT(const string_t& id = string_t());
std::shared_ptr<xbox::services::xbox_live_context> GetMockXboxLiveContext_Cpp(const string_t& id = string_t());

Windows::Data::Json::JsonArray^ MakeStringArray( Platform::String^ str1, Platform::String^ str2 );
Windows::Data::Json::JsonArray^ MakeNumberArray( int num1, int num2 );
Platform::String^ StringToLowerCase(_In_ Platform::String^ source);

std::wstring TimeSpanToString(
    Windows::Foundation::TimeSpan timeSpan
    );

std::wstring DateTimeToString(
    Windows::Foundation::DateTime dateTime
    );

/// <summary>
/// Calls Log::Comment. If the [strMsg] size is greater than 2048, will chunk the message into multiple comments
/// to get around a logging limitation in TAEF where it will not show the messsage if too large.
/// </summary>
/// <param name="strMsg">The message to log</param>
void LogComment(
    Platform::String^ strMsg
    );

/// <summary>
/// Calls LogComment with message and error formatted into one string.
/// </summary>
/// <param name="message">The message to log</param>
/// <param name="hr">Error to log</param>
void LogCommentWithError(
    _In_ Platform::String^ message, 
    _In_ HRESULT hr
    );

/// <summary>
/// Verifies if a string is null or empty
/// </summary>
/// <param name="strToVerify">The string to verify</param>
void VERIFY_IS_NOT_NULL_OR_EMPTY_STR(
    Platform::String^ strToVerify 
    );

/// <summary>
/// Verifies 2 strings are equal
/// </summary>
/// <param name="expectedStr">Platform::String to verify</param>
/// <param name="actualStr">Platform::String to verify</param>
void VERIFY_ARE_EQUAL_PLATFORM_STRING( 
    Platform::String^ expectedStr,
    Platform::String^ actualStr
    );

/// <summary>
/// Verifies if a string is null or empty. Logs the string value to check
/// </summary>
/// <param name="strName">Describes the data in the string to log</param>
/// <param name="strToVerify">The string to verify and log it's value</param>
void VERIFY_IS_NOT_NULL_OR_EMPTY_STR_AND_LOG(
    Platform::String^ strName, 
    Platform::String^ strToVerify 
    );

/// <summary>
/// Calls VERIFY_FAIL which stops the execution of the test case and logs exception
/// </summary>
/// <param name="context">Message to prepend to exception data... will not append if empty or null</param>
/// <param name="exception">The exception to log</param>
void LOG_EXCEPTION_AND_FAIL(
    Platform::String^ context,
    Platform::Exception^ exception);

#define VERIFY_ARE_EQUAL_TIMESPAN_TO_SECONDS(__timespan, __seconds) VERIFY_ARE_EQUAL(Microsoft::Xbox::Services::System::timeSpanTicks(__timespan.Duration), std::chrono::seconds(__seconds))
#define VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(__timespan, __seconds) VERIFY_ARE_EQUAL(Microsoft::Xbox::Services::System::timeSpanTicks(__timespan.Duration), std::chrono::milliseconds(__seconds))

#define VERIFY_THROWS_CX(__operation, __exception)                                                                                                                          \
{                                                                                                                                                                           \
    bool __exceptionHit = false;                                                                                                                                            \
    try                                                                                                                                                                     \
{                                                                                                                                                                       \
    __operation;                                                                                                                                                        \
}                                                                                                                                                                       \
    catch(__exception^ __e)                                                                                                                                                 \
{                                                                                                                                                                       \
    WEX::Logging::Log::Comment( FormatString( L"Verify: Expected Exception Thrown( %s )", L#__exception ).c_str() );                                                    \
    __exceptionHit = true;                                                                                                                                              \
}                                                                                                                                                                       \
    catch(...)                                                                                                                                                              \
{                                                                                                                                                                       \
}                                                                                                                                                                       \
    if(!__exceptionHit)                                                                                                                                                     \
{                                                                                                                                                                       \
    WEX::Logging::Log::Comment( FormatString( L"Error: Expected Exception Not Thrown ( %s )", L#__exception ).c_str() );                                                \
    (bool)WEX::TestExecution::Private::MacroVerify::Fail(PRIVATE_VERIFY_ERROR_INFO, FormatString( L"Expected Exception Not Thrown ( %s )", L#__exception ).c_str());    \
}                                                                                                                                                                       \
}


#define VERIFY_THROWS_HR_CX(__operation, __hr)                                                                                                                                  \
{                                                                                                                                                                               \
    bool __exceptionHit = false;                                                                                                                                                \
    try                                                                                                                                                                         \
    {                                                                                                                                                                           \
        __operation;                                                                                                                                                            \
    }                                                                                                                                                                           \
    catch(Platform::Exception^ __e)                                                                                                                                             \
    {                                                                                                                                                                           \
        if(__e->HResult == (__hr))                                                                                                                                              \
        {                                                                                                                                                                       \
            WEX::Logging::Log::Comment( FormatString( L"Verify: Expected Exception Thrown ( hr == %s )", L#__hr ).c_str() );                                                      \
            __exceptionHit = true;                                                                                                                                              \
        }                                                                                                                                                                       \
    }                                                                                                                                                                           \
    catch(...)                                                                                                                                                                  \
    {                                                                                                                                                                           \
    }                                                                                                                                                                           \
    if(!__exceptionHit)                                                                                                                                                         \
    {                                                                                                                                                                           \
        WEX::Logging::Log::Comment( FormatString( L"Error: Expected Exception Not Thrown ( hr == %s )", L#__hr ).c_str() );                                                       \
        (bool)WEX::TestExecution::Private::MacroVerify::Fail(PRIVATE_VERIFY_ERROR_INFO, FormatString( L"Expected Exception Not Thrown ( hr == %s )", L#__hr ).c_str());  \
    }                                                                                                                                                                           \
}

#define VERIFY_THROWS_WIN32_CX(__operation, __e)  VERIFY_THROWS_HR_CX( __HRESULT_FROM_WIN32(__e) 