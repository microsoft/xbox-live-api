// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestBase.h"
#include "UnitTestBase_winrt.h"
#include "iso8601.h"
#include <sstream>
#include <iomanip>
#include <xsapi\xbox_live_context.h>
#include <user_context.h>


using namespace Platform;
using namespace WEX::Logging;
using namespace Windows::Data::Json;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;


XboxLiveUser^ SignInUserWithMocks_WinRT(const string_t& id)
{
    auto user = ref new XboxLiveUser();
    auto asyncOp = user->SignInAsync(nullptr);

    VERIFY_NO_THROW(pplx::create_task(asyncOp).wait());
    if (!id.empty())
    {
        user->_User_impl()->_Set_xbox_user_id(xbox::services::utils::internal_string_from_string_t(id));
    }

    return user;
}

Microsoft::Xbox::Services::XboxLiveContext^ GetMockXboxLiveContext_WinRT(const string_t& id)
{
    auto user = SignInUserWithMocks_WinRT(id);
    Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(user);
    return xboxLiveContext;
}

std::shared_ptr<xbox::services::xbox_live_context> GetMockXboxLiveContext_Cpp(const string_t& id)
{
    auto user = SignInUserWithMocks_WinRT(id);
    Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(user);
    return xboxLiveContext->GetCppObj();
}

/// <summary>
/// Calls Log::Comment. If the [strMsg] size is greater than 2048, will chunk the message into multiple comments
/// to get around a logging limitation in TAEF where it will not show the message if too large.
/// </summary>
/// <param name="strMsg">The message to log</param>
void LogComment(Platform::String^ strMsg)
{
    const int c_ChunkSize = 3072;

    if (strMsg->Length() <= c_ChunkSize)
    {
        Log::Comment(strMsg->Data());
        return;
    }

    std::wstringstream stringStream(strMsg->Data());
    std::wstring strBuffer(strMsg->Data());
    strBuffer.resize(c_ChunkSize);
    bool addContinueMessage = false;

    while (stringStream.peek() != WEOF)
    {
        std::streamsize readSize = stringStream.readsome(&strBuffer[0], c_ChunkSize);
        strBuffer[static_cast<UINT>(readSize)] = 0;

        // Add continue message if more than one Log::Comment for the same string
        if (addContinueMessage)
        {
            Log::Comment(FormatString(L"continued...\n%s", strBuffer.data()).c_str());
        }
        else
        {
            Log::Comment(strBuffer.data());
            addContinueMessage = true;
        }
    }
}

/// <summary>
/// Calls LogComment with message and error formatted into one string.
/// </summary>
/// <param name="message">The message to log</param>
/// <param name="hr">Error to log</param>
void LogCommentWithError(
    _In_ Platform::String^ message, 
    _In_ HRESULT hr
    )
{
    LogComment(ref new Platform::String(FormatString(L"%s: %0.8x", message->Data(), hr).c_str()));
}

/// <summary>
/// Verifies 2 strings are equal
/// </summary>
/// <param name="expectedStr">Platform::String to verify</param>
/// <param name="actualStr">Platform::String to verify</param>
void VERIFY_ARE_EQUAL_PLATFORM_STRING( 
    Platform::String^ expectedStr,
    Platform::String^ actualStr
    )
{
    VERIFY_IS_EQUAL_STR(
        expectedStr->Data(),
        actualStr->Data()
        );
}

/// <summary>
/// Calls IsEmpty() on Platform::String to verify not null
/// </summary>
/// <param name="strToVerify">Platform::String to verify</param>
void VERIFY_IS_NOT_NULL_OR_EMPTY_STR(
    Platform::String^ strToVerify )
{
    VERIFY_IS_FALSE(strToVerify->IsEmpty());
}

/// <summary>
/// Calls IsEmpty() on Platform::String to verify not null
/// </summary>
/// <param name="strName">Name of string used in the log message</param>
/// <param name="strToVerify">Platform::String to verify</param>
void VERIFY_IS_NOT_NULL_OR_EMPTY_STR_AND_LOG(
    Platform::String^ strName, 
    Platform::String^ strToVerify )
{
    VERIFY_IS_NOT_NULL_OR_EMPTY_STR(strToVerify);
    Log::Comment(FormatString(L"Name: %s     Value: %s", strName->Data(), strToVerify->Data()).c_str());
}

/// <summary>
/// Calls VERIFY_FAIL which stops the execution of the test case and logs exception
/// </summary>
/// <param name="context">Message to prepend to exception data... will not append if empty or null</param>
/// <param name="exception">The exception to log</param>
void LOG_EXCEPTION_AND_FAIL(
    Platform::String^ context,
    Platform::Exception^ exception)
{
    std::wstring errorMessage = L"Exception: ";
    int hr = exception->HResult;

    // example of an error that is produced when having a title ingestion / sandbox issue
    //Error: Verify: Exception: GetLeaderboardAsync
    //  hr: 87dd000e
    //  message:  [File: e:\wgf1\root\xbox\live\services\testsrc\microsoft.xbox.services.unit.test\base.cpp, Function: Microsoft::Xbox::System::LOG_EXCEPTION_AND_FAIL, Line: 481]
    //EndGroup: Microsoft::Xbox::System::LeaderboardsScenarioTest::TestGetLeaderboardAsync [Failed]
    if(hr == 0x87dd000e)
    {
        errorMessage += FormatString(L"\n  hr: %0.8x", exception->HResult);
        errorMessage += L"\n  issue: This HR is usually produced when there is a problem getting an X token for a unauthenticated ";
        errorMessage += L"\n         title.  It usually means theres a title ingestion or sandbox issue.";
        errorMessage += L"\n         TitleID / SCID assignment website: http://xboxwiki/index.php?title=SCID_Assignments";
    }
    else
    {
        if (!context->IsEmpty())
        {
            errorMessage += context->Data();
        }

        errorMessage += FormatString(L"\n  hr: %0.8x", exception->HResult);
        errorMessage += L"\n  message: ";
        errorMessage += exception->Message->Data();
    }

    VERIFY_FAIL(errorMessage.c_str());
}

JsonArray^ MakeStringArray(Platform::String^ str1, Platform::String^ str2)
{
    JsonArray^ jsonArray = ref new JsonArray();
    if( !str1->IsEmpty() ) jsonArray->Append(JsonValue::CreateStringValue(str1));
    if( !str2->IsEmpty() ) jsonArray->Append(JsonValue::CreateStringValue(str2));
    return jsonArray;
}

JsonArray^ MakeNumberArray(int num1, int num2)
{
    JsonArray^ jsonArray = ref new JsonArray();
    if( num1 != 0 ) jsonArray->Append(JsonValue::CreateNumberValue(num1));
    if( num2 != 0 ) jsonArray->Append(JsonValue::CreateNumberValue(num2));
    return jsonArray;
}

Platform::String^
StringToLowerCase(
    _In_ Platform::String^ source
    )
{
    std::wstring strLowerCase = source->ToString()->Data();
    
    for(unsigned int i = 0; i < strLowerCase.length(); i++)
    {
        strLowerCase[i] = (char)tolower(strLowerCase[i]);
    }

    return ref new Platform::String(strLowerCase.c_str());
}

std::wstring
#pragma warning(suppress: 28301)
TimeSpanToString(
    _In_ Windows::Foundation::TimeSpan timeSpan
    )
{
    auto totalSeconds = timeSpan.Duration / TICKS_PER_SECOND;

    int hour = (int)totalSeconds/3600;
    int min = (int)(totalSeconds % 3600) / 60;
    int sec = (int)(totalSeconds % 3600) % 60;

    stringstream_t ss;
    ss << std::setw(2) << std::setfill(L'0') << hour;
    ss << ":";
    ss << std::setw(2) << std::setfill(L'0') << min;
    ss << ":";
    ss << std::setw(2) << std::setfill(L'0') << sec;

    return ss.str();
}

std::wstring
#pragma warning(suppress: 28301)
DateTimeToString(
    _In_ Windows::Foundation::DateTime dateTime
    )
{
    FILETIME fileTime;
    fileTime.dwLowDateTime = (dateTime.UniversalTime & (DWORD)-1);
    fileTime.dwHighDateTime = (dateTime.UniversalTime >> 32 & (DWORD)-1);

    WCHAR dateString[ISO8601_MAX_CCH];
    THROW_IF_HR_FAILED(FILETIMEToISO8601W(&fileTime, false, dateString, ARRAYSIZE(dateString), FALSE));
    
    return dateString;
}

