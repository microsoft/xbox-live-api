// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once
#include "pch.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM == HC_PLATFORM_GDK
#include <DbgHelp.h>
#endif

struct StackInfo
{
    int stackSize;
    bool isInXsapi[64];
    DWORD lineNumber[64];
    char szFileNames[64][1024];
    char szStack[64][1024];
};

#define APIRUNNER_MAX_UNHOOKED_MEM_TRACKED 1024 
class ApiRunerMemHook
{
public:
    ApiRunerMemHook();

    HRESULT StartMemTracking();
    HRESULT StopMemTracking();

    bool IsMemTrackingStarted();
    void ResetStats();
    void LogStats(const std::string& name);
    void LogLeaks();
    void AssertOnAllocOfId(uint64_t id);

    _Ret_maybenull_ _Post_writable_byte_size_(size) void* AllocMem(_In_ size_t size);
    void DeleteMem(_In_ _Post_invalid_ void* pointer);
    std::vector<std::string> GetStackLogLine();

    bool IsStackInXSAPI(StackInfo& stackInfo);
    bool IsMemHooked(void* p);
    void GetStackTrace(StackInfo &stackInfo);

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM == HC_PLATFORM_GDK
    bool IsStackFramesInsideTestCode(StackInfo* pStackInfo);
    bool IsStackFramesInsideCallback(StackInfo* pStackInfo);
#endif

    bool IsFilePathInXSAPI(const char* filePath);
    bool IsStackInsideCallback(const char* stackSymbolName, const char* filePath);
    void LogUnhookedStats();
    uint64_t m_rawAllocMemory{ 0 };
    uint64_t m_rawAllocMemorySpots{ 0 };
    char m_allocTrace[APIRUNNER_MAX_UNHOOKED_MEM_TRACKED][1024] { 0 };
    unsigned long m_allocTraceLine[APIRUNNER_MAX_UNHOOKED_MEM_TRACKED] { 0 };

private:
#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM == HC_PLATFORM_GDK
    HANDLE m_process{ nullptr };
#endif
    bool m_startMemTracking{ false };
    uint64_t m_allocated{ 0 };
    uint64_t m_allocDeleted{ 0 };
    uint64_t m_unhookedAllocated{ 0 };
    uint64_t m_unhookedDeleted{ 0 };
    uint64_t m_allocId{ 0 };
    uint64_t m_assertId{ 0 };
    std::recursive_mutex m_lock;
    std::map< void*, uint64_t > m_allocSizeMap;
    std::map< void*, uint64_t > m_allocIdMap;
    std::map< void*, std::vector<std::string> > m_mapStackLog;
    std::atomic<int> m_refCountInsideAlloc;
};

ApiRunerMemHook* GetApiRunnerMemHook();
