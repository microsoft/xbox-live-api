// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils_uwp.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

Win32Event::Win32Event()
{
    m_event = CreateEvent(NULL, TRUE, FALSE, nullptr);
}

Win32Event::~Win32Event()
{
    CloseHandle(m_event);
}

void Win32Event::Set()
{
    SetEvent(m_event);
}

void Win32Event::WaitForever()
{
    WaitForSingleObject(m_event, INFINITE);
}

xsapi_internal_string utils::internal_string_from_string_t(_In_ const string_t& externalString)
{
    return internal_string_from_utf16(externalString.c_str(), externalString.size());
}

xsapi_internal_string utils::internal_string_from_char_t(_In_ const char_t* char_t)
{
    return internal_string_from_utf16(char_t, wcslen(char_t));
}

xsapi_internal_string utils::internal_string_from_utf16(_In_z_ PCWSTR utf16)
{
    return internal_string_from_utf16(utf16, wcslen(utf16));
}

xsapi_internal_string utils::internal_string_from_utf16(_In_reads_(size) PCWSTR utf16, size_t size)
{
    // early out on empty strings since they are trivially convertible
    if (size == 0)
    {
        return "";
    }

    // query for the buffer size
    auto queryResult = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        utf16, static_cast<int>(size),
        nullptr, 0,
        nullptr, nullptr
    );
    if (queryResult == 0)
    {
        throw std::exception("internal_string_from_utf16 failed");
    }

    // allocate the output buffer, queryResult is the required size
    xsapi_internal_string utf8(static_cast<size_t>(queryResult), L'\0');
    auto conversionResult = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS,
        utf16, static_cast<int>(size),
        &utf8[0], static_cast<int>(utf8.size()),
        nullptr, nullptr
    );
    if (conversionResult == 0)
    {
        throw std::exception("internal_string_from_utf16 failed");
    }

    return utf8;
}

string_t utils::string_t_from_internal_string(_In_ const xsapi_internal_string& internalString)
{
    return string_t_from_utf8(internalString.data(), internalString.size());
}

string_t utils::string_t_from_utf8(_In_z_ PCSTR utf8)
{
    return string_t_from_utf8(utf8, strlen(utf8));
}

string_t utils::string_t_from_utf8(_In_reads_(size) PCSTR utf8, size_t size)
{
    // early out on empty strings since they are trivially convertible
    if (size == 0)
    {
        return L"";
    }

    // query for the buffer size
    auto queryResult = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        utf8, static_cast<int>(size),
        nullptr, 0
    );
    if (queryResult == 0)
    {
        throw std::exception("string_t_from_utf8 failed");
    }

    // allocate the output buffer, queryResult is the required size
    string_t utf16(static_cast<size_t>(queryResult), L'\0');
    auto conversionResult = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        utf8, static_cast<int>(size),
        &utf16[0], static_cast<int>(utf16.size())
    );
    if (conversionResult == 0)
    {
        throw std::exception("string_t_from_utf8 failed");
    }

    return utf16;
}

std::string utils::utf8_from_utf16(std::wstring const& utf16)
{
    return std::string(utils::internal_string_from_string_t(utf16).data());
}

std::wstring utils::utf16_from_utf8(std::string const& utf8)
{
    return utils::string_t_from_utf8(utf8.data());
}

std::string utils::utf8_from_utf16(_In_z_ PCWSTR utf16)
{
    return std::string(utils::internal_string_from_utf16(utf16).data());
}

std::wstring utils::utf16_from_utf8(_In_z_ PCSTR utf8)
{
    return utils::string_t_from_utf8(utf8);
}

std::string utils::utf8_from_utf16(_In_reads_(size) PCWSTR utf16, size_t size)
{
    return std::string(utils::internal_string_from_utf16(utf16, size).data());
}

std::wstring utils::utf16_from_utf8(_In_reads_(size) PCSTR utf8, size_t size)
{
    return utils::string_t_from_utf8(utf8, size);
}

#if XSAPI_C
time_t utils::time_t_from_datetime(const utility::datetime& datetime)
{
    time_t currentUtcTime = time(nullptr);
    auto diffTime = utility::datetime::utc_now() - datetime;
    return currentUtcTime - diffTime;
}

utility::datetime utils::datetime_from_time_t(const time_t* pTime)
{
    utility::datetime datetime = utility::datetime::utc_now();
    auto diffTime = time(nullptr) - *pTime;
    return datetime - utility::datetime::from_seconds((unsigned int)diffTime);
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
