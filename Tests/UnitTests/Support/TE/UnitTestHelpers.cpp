// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include <sstream>
#include <iomanip>
#include "iso8601.h"

TEST_MODULE_INITIALIZE(ModuleInitialize)
{
    Logger::WriteMessage("Started unit test");
}

TEST_MODULE_CLEANUP(ModuleCleanup)
{
    Logger::WriteMessage("Stopped unit test");
}

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

Microsoft::Xbox::Services::System::XboxLiveUser^ SignInUserWithMocks_WinRT(const string_t& id)
{
    auto user = ref new Microsoft::Xbox::Services::System::XboxLiveUser();
    auto asyncOp = user->SignInAsync(nullptr);

    pplx::create_task(asyncOp).wait();
    if (!id.empty())
    {
        user->_User_impl()->_Set_xbox_user_id(xbox::services::utils::internal_string_from_string_t(id));
    }

    return user;
}

Microsoft::Xbox::Services::XboxLiveContext^ GetMockXboxLiveContext_WinRT(const string_t& id)
{
    auto user = SignInUserWithMocks_WinRT(id);
    xbox::services::user_context userContext(user);
    Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(user);
    return xboxLiveContext;
}

std::shared_ptr<xbox::services::xbox_live_context> GetMockXboxLiveContext_Cpp(const string_t& id)
{
    auto user = SignInUserWithMocks_WinRT(id);
    xbox::services::user_context userContext(user);
    Microsoft::Xbox::Services::XboxLiveContext^ xboxLiveContext = ref new Microsoft::Xbox::Services::XboxLiveContext(user);
    return xboxLiveContext->GetCppObj();
}


std::wstring
TimeSpanToString(
    _In_ Windows::Foundation::TimeSpan timeSpan
    )
{
    auto totalSeconds = timeSpan.Duration / TICKS_PER_SECOND;

    int hour = (int)totalSeconds / 3600;
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


