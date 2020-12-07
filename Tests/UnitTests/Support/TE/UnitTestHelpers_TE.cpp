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

