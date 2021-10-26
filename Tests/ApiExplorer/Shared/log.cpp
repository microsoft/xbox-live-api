#include "pch.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 
#include <shlobj.h>
#endif
#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include "pal.h"
#endif

#include <stdio.h>

std::string g_logFilePath;

void LogInit()
{
#if HC_PLATFORM == HC_PLATFORM_WIN32 
    char path[MAX_PATH + 1] = {0};
    SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path);
    g_logFilePath = path;
    g_logFilePath += "\\";
#endif
#if HC_PLATFORM == HC_PLATFORM_UWP
    Platform::String^ localfolder = Windows::Storage::ApplicationData::Current->LocalFolder->Path;	//for local saving for future

    //convert folder name from wchar to ascii
    std::wstring folderNameW(localfolder->Begin());
    std::string folderNameA(folderNameW.begin(), folderNameW.end());
    g_logFilePath = folderNameA;
    g_logFilePath += "\\";
#endif

    g_logFilePath += "apirunner-log.txt";
    if (!g_logFilePath.empty())
    {
        std::remove(g_logFilePath.c_str()); // erase existing file
    }
}

void LogToFile(_Printf_format_string_ char const* format, ...)
{
    char message[4096] = {};
    va_list varArgs{};
    va_start(varArgs, format);
    pal::vsprintf(message, 4096, format, varArgs);
    va_end(varArgs);

    if (!g_logFilePath.empty())
    {
        std::ofstream file{ g_logFilePath.c_str(), std::ios::app };
        if (file.is_open())
        {
            file << message << std::endl;
            file.close();
        }
    }

#if HC_PLATFORM_IS_MICROSOFT
    OutputDebugStringA(message);
    OutputDebugStringA("\r\n");
#endif
}

#if HC_PLATFORM != HC_PLATFORM_UWP && HC_PLATFORM != HC_PLATFORM_WIN32 && HC_PLATFORM != HC_PLATFORM_GDK && HC_PLATFORM != HC_PLATFORM_XDK && HC_PLATFORM != HC_PLATFORM_IOS && HC_PLATFORM != HC_PLATFORM_ANDROID

void LogToScreen(_Printf_format_string_ char const* format, ...)
{
    // Stock impl that just logs to file
    // Hook up UI logs for each platform based on platform specific UI display calls

    char message[8000] = {};

    va_list varArgs{};
    va_start(varArgs, format);
    pal::vsprintf(message, 4096, format, varArgs);
    va_end(varArgs);

    LogToFile(message);
}
#endif
