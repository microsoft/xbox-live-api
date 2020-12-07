//--------------------------------------------------------------------------------------
// StringUtil.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#pragma once
#include <string>

namespace DX
{
    size_t GetWideLength(const char* utf8String, size_t utf8Length);
    size_t GetUtf8Length(const wchar_t* wideString, size_t wideLength);

    std::wstring Utf8ToWide(const char* utf8String, size_t utf8Length);
    std::string WideToUtf8(const wchar_t* wideString, size_t wideLength);

    std::wstring Utf8ToWide(const std::string& utf8String);
    std::string WideToUtf8(const std::wstring& wideString);

    std::string ToLower(const std::string& utf8String);
    void ToLowerInPlace(std::string& utf8String);
    std::wstring ToLower(const std::wstring& wideString);
    void ToLowerInPlace(std::wstring& wideString);

    std::string ToUpper(const std::string& utf8String);
    void ToUpperInPlace(std::string& utf8String);
    std::wstring ToUpper(const std::wstring& wideString);
    void ToUpperInPlace(std::wstring& wideString);
}
