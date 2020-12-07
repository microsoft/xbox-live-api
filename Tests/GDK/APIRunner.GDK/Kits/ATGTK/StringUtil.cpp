//--------------------------------------------------------------------------------------
// StringUtil.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "StringUtil.h"
#include <cctype>
#include <string>

namespace
{
    constexpr DWORD MBConversionFlags = MB_ERR_INVALID_CHARS;
    constexpr DWORD WCConversionFlags = WC_ERR_INVALID_CHARS;
}

// Get the wchar length of a utf8 string
size_t DX::GetWideLength(const char* utf8String, size_t utf8Length)
{
    const int newLen = ::MultiByteToWideChar(
        CP_UTF8,
        MBConversionFlags,
        utf8String,
        static_cast<int>(utf8Length),
        nullptr,
        0
    );

    return static_cast<size_t>(newLen);
}

// Get the utf8 length of a wchar string
size_t DX::GetUtf8Length(const wchar_t* wideString, size_t wideLength)
{
    const int newLen = ::WideCharToMultiByte(
        CP_UTF8,
        WCConversionFlags,
        wideString,
        static_cast<int>(wideLength),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    return static_cast<size_t>(newLen);
}

std::wstring DX::Utf8ToWide(const char* utf8String, size_t utf8Length)
{
    std::wstring dest;
    const size_t wideLength = GetWideLength(utf8String, utf8Length);
    dest.resize(wideLength);
    ::MultiByteToWideChar(
        CP_UTF8,
        MBConversionFlags,
        utf8String,
        static_cast<int>(utf8Length),
        &dest[0],
        static_cast<int>(wideLength)
    );

    return dest;
}

std::string DX::WideToUtf8(const wchar_t* wideString, size_t wideLength)
{
    std::string dest;
    const size_t utf8Length = GetUtf8Length(wideString, wideLength);
    dest.resize(utf8Length);

    ::WideCharToMultiByte(
        CP_UTF8,
        WCConversionFlags,
        wideString,
        static_cast<int>(wideLength),
        &dest[0],
        static_cast<int>(utf8Length),
        nullptr,
        nullptr
    );

    return dest;
}

std::wstring DX::Utf8ToWide(const std::string& utf8String)
{
    return Utf8ToWide(utf8String.c_str(), utf8String.length());
}

std::string DX::WideToUtf8(const std::wstring& wideString)
{
    return WideToUtf8(wideString.c_str(), wideString.length());
}

std::string DX::ToLower(const std::string & utf8String)
{
    std::string lower = utf8String;
    ToLowerInPlace(lower);
    return lower;
}

void DX::ToLowerInPlace(std::string& utf8String)
{
    std::transform(
        utf8String.begin(),
        utf8String.end(),
        utf8String.begin(),
        [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
    );
}

std::wstring DX::ToLower(const std::wstring & wideString)
{
    std::wstring lower = wideString;
    ToLowerInPlace(lower);
    return lower;
}

void DX::ToLowerInPlace(std::wstring & wideString)
{
    std::transform(
        wideString.begin(),
        wideString.end(),
        wideString.begin(),
        [](wchar_t c) { return static_cast<wchar_t>(std::tolower(static_cast<wchar_t>(c))); }
    );
}

std::string DX::ToUpper(const std::string & utf8String)
{
    std::string lower = utf8String;
    ToUpperInPlace(lower);
    return lower;
}

void DX::ToUpperInPlace(std::string& utf8String)
{
    std::transform(
        utf8String.begin(),
        utf8String.end(),
        utf8String.begin(),
        [](char c) { return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); }
    );
}

std::wstring DX::ToUpper(const std::wstring & wideString)
{
    std::wstring lower = wideString;
    ToUpperInPlace(lower);
    return lower;
}

void DX::ToUpperInPlace(std::wstring & wideString)
{
    std::transform(
        wideString.begin(),
        wideString.end(),
        wideString.begin(),
        [](wchar_t c) { return static_cast<wchar_t>(std::toupper(static_cast<wchar_t>(c))); }
    );
}
