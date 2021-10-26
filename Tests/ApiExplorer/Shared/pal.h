// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

typedef unsigned char byte;
#if !HC_PLATFORM_IS_MICROSOFT
typedef char CHAR;
typedef unsigned long DWORD;

constexpr auto sprintf_s = sprintf;

#define MAX_PATH 260
#define WINAPI
#define LPVOID void*

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x)
#endif

#else // Windows Platforms

#endif


#if HC_PLATFORM == HC_PLATFORM_ANDROID
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
#endif


namespace pal
{
    void strcpy(char * dst, size_t size, const char* src);
    int stricmp(const char* left, const char* right);
    int vsprintf(char* message, size_t size,char const* format, va_list varArgs);
    char* strtok(char* str, char const* delimiter, char** context);

    // Due to java not supporting unsigned numeric values, we have to do regular long on Android
#if HC_PLATFORM != HC_PLATFORM_ANDROID
    void Sleep(unsigned long duration);
#else
    void Sleep(long duration);
#endif

    bool FileExists(std::string fileName); // TODO might only be used on win32
    std::vector<std::string> EnumFilesInFolder(std::string folder, std::string spec);
    std::string FindFile(std::string fileName);
    std::string GetLuaPath();
#if HC_PLATFORM == HC_PLATFORM_ANDROID
    std::string OpenFile(std::string filePath);
#endif
}
