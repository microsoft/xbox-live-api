// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "mem_hook.h"

#define TRACK_UNHOOKED_ALLOCS 1

#if HC_PLATFORM == HC_PLATFORM_WIN32 || (HC_PLATFORM == HC_PLATFORM_GDK && !_GAMING_XBOX)
#include <process.h>
#include <iostream>
#include <Windows.h>
#include <DbgHelp.h>

#define TRACE_MAX_STACK_FRAMES 1024
#define TRACE_MAX_FUNCTION_NAME_LENGTH 1024
#endif

#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include "runner.h"
#endif

ApiRunerMemHook* g_mem = new ApiRunerMemHook();
bool g_rawMemHookInitTracking{ false };

ApiRunerMemHook* GetApiRunnerMemHook()
{
    return g_mem;
}

_Ret_maybenull_ _Post_writable_byte_size_(size) void* STDAPIVCALLTYPE ApiRunnerMemManagerMemAlloc(_In_ size_t size, _In_ HCMemoryType)
{
    auto mem = GetApiRunnerMemHook();
    return mem->AllocMem(size);
}

void STDAPIVCALLTYPE ApiRunnerMemManagerMemFree(_In_ _Post_invalid_ void* pointer, _In_ HCMemoryType)
{
    auto mem = GetApiRunnerMemHook();
    mem->DeleteMem(pointer);
}

_Ret_maybenull_ _Post_writable_byte_size_(size) void* ApiRunnerMemManagerXalMemAlloc(size_t size, uint32_t)
{
    auto mem = GetApiRunnerMemHook();
    return mem->AllocMem(size);
}

void ApiRunnerMemManagerXalMemFree(_In_ _Post_invalid_ void* pointer, uint32_t)
{
    auto mem = GetApiRunnerMemHook();
    mem->DeleteMem(pointer);
}

bool ApiRunerMemHook::IsMemTrackingStarted()
{
    return m_startMemTracking && m_refCountInsideAlloc == 0;
}

ApiRunerMemHook::ApiRunerMemHook()
{
    m_refCountInsideAlloc = 0;

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM == HC_PLATFORM_GDK
    m_process = GetCurrentProcess();
    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEBUG | SYMOPT_DEFERRED_LOADS | SYMOPT_CASE_INSENSITIVE);
    SymInitialize(m_process, NULL, TRUE);
#endif
}

HRESULT ApiRunerMemHook::StartMemTracking()
{
    if (m_startMemTracking)
        return S_OK;

    // libHttpClient hooks does not need to be called if XBL or XAL is hooked but for extra insurance during tests
    HRESULT hr = HCMemSetFunctions(&ApiRunnerMemManagerXalMemAlloc, &ApiRunnerMemManagerXalMemFree);
    assert(SUCCEEDED(hr));
    if (FAILED(hr))
        return hr;

    hr = XblMemSetFunctions(&ApiRunnerMemManagerMemAlloc, &ApiRunnerMemManagerMemFree);
    assert(SUCCEEDED(hr));
    if (FAILED(hr))
        return hr;

    ResetStats();

    m_startMemTracking = true;
    g_rawMemHookInitTracking = true;

    return S_OK;
}


HRESULT ApiRunerMemHook::StopMemTracking()
{
    if (!m_startMemTracking)
        return S_OK;

    HRESULT hr = HCMemSetFunctions(nullptr, nullptr);
    assert(SUCCEEDED(hr));
    if (FAILED(hr))
        return hr;

    hr = XblMemSetFunctions(nullptr, nullptr);
    assert(SUCCEEDED(hr));
    if (FAILED(hr))
        return hr;

    m_startMemTracking = false;
    g_rawMemHookInitTracking = false;

    return S_OK;
}


std::vector<std::string> ApiRunerMemHook::GetStackLogLine()
{
#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM == HC_PLATFORM_GDK
    StackInfo stackInfo{ 0 };
    GetStackTrace(stackInfo);

    std::vector<std::string> logs;
    logs.reserve(static_cast<size_t>(stackInfo.stackSize));
    for (int i = 0; i < stackInfo.stackSize; i++)
    {
        char sz[1024];
        if (stackInfo.szStack[i][0] != 0)
        {
            sprintf_s(sz, "%.32s in %s: line: %lu", stackInfo.szStack[i], stackInfo.szFileNames[i], stackInfo.lineNumber[i]);
        }

        logs.push_back(sz);
    }
    return logs;
#else
    return std::vector<std::string>();
#endif
}

void ApiRunerMemHook::AssertOnAllocOfId(uint64_t id)
{
    m_assertId = id;
}

void* ApiRunerMemHook::AllocMem(_In_ size_t size)
{
#if TRACK_UNHOOKED_ALLOCS == 1
    m_refCountInsideAlloc++;
    std::lock_guard<std::recursive_mutex> guard(m_lock);
#endif

    void* ptr = malloc(size);

#if TRACK_UNHOOKED_ALLOCS == 1
    m_allocSizeMap[ptr] = size;
    m_allocated += size;
    m_allocId++;
    m_allocIdMap[ptr] = m_allocId;

    if (m_allocId == m_assertId)
    {
        LogToFile("Breakpoint here");
    }

    m_mapStackLog[ptr] = GetStackLogLine();
    m_refCountInsideAlloc--;
#endif

    return ptr;
}


bool ApiRunerMemHook::IsMemHooked(void* p)
{
#if TRACK_UNHOOKED_ALLOCS == 1
    std::lock_guard<std::recursive_mutex> guard(m_lock);
    auto it = m_allocSizeMap.find(p);
    return (it != m_allocSizeMap.end());
#else
    return false;
#endif
}

void ApiRunerMemHook::DeleteMem(_In_ _Post_invalid_ void* pointer)
{
#if TRACK_UNHOOKED_ALLOCS == 1
    std::lock_guard<std::recursive_mutex> guard(m_lock);
    uint64_t size = 0;
    auto it = m_allocSizeMap.find(pointer);
    if (it != m_allocSizeMap.end())
    {
        size = it->second;
        m_allocSizeMap.erase(it);
    }
    else
    {
       // assert(false);
        // Add the following ifdef above line 53 in internal_mem.h 
        // if this assertion is reached. This will help track down what process
        // is being freed incorrectly.
#if 0
        static int s_id = 0;
        std::wstringstream msg;
        msg << L"s_id:" << s_id << L" p: 0x" << std::hex << p << "\n";
        OutputDebugString(msg.str().c_str());
        s_id++;
#endif
    }
    m_allocDeleted += size;
#endif

    free(pointer);
}

void ApiRunerMemHook::ResetStats()
{
    std::lock_guard<std::recursive_mutex> guard(m_lock);
    m_allocated = 0;
    m_allocDeleted = 0;
    m_allocId = 0;
    m_allocSizeMap.clear();
    m_mapStackLog.clear();
}

void MemHookLog(_Printf_format_string_ char const* format, ...)
{
    // Stock impl that just logs to file
    // Hook up UI logs for each platform based on platform specific UI display calls

#if HC_PLATFORM_IS_MICROSOFT
    char message[8000] = {};

    va_list varArgs{};
    va_start(varArgs, format);
    pal::vsprintf(message, 4096, format, varArgs);
    va_end(varArgs);

    OutputDebugStringA(message);
    OutputDebugStringA("\n");
#else
    UNREFERENCED_PARAMETER(format);
#endif
}

void ApiRunerMemHook::LogLeaks()
{
    std::lock_guard<std::recursive_mutex> guard(m_lock);
    LogToScreen("Leaks: %d mem leaks found", m_allocSizeMap.size());

    // Using MemHookLog since long LogToScreen lines can causes mem allocations and thus doesn't work well in this module
    MemHookLog("Leaks: -- START --");
    for (auto& it : m_allocSizeMap)
    {
        void* ptr = it.first;
        uint64_t size = it.second;
        auto& stackLog = m_mapStackLog[ptr];
        auto& id = m_allocIdMap[ptr];
        if (stackLog.size() >= 4)
            MemHookLog("[%d] %0.8x: %d from %s", id, ptr, size, stackLog[4].c_str());
        else
            MemHookLog("[%d] %0.8x: %d", id, ptr, size);
    }
    MemHookLog("Leaks: -- END --");

    MemHookLog("== Leak CSV Start ==");
    for (auto& it : m_allocSizeMap)
    {
        void* ptr = it.first;
        uint64_t size = it.second;
        auto& stackLog = m_mapStackLog[ptr];
        auto& id = m_allocIdMap[ptr];
        int stackLineId{ 0 };
        for (auto& stackLine : stackLog)
        {
            stackLineId++;
            MemHookLog("%d,%0.8x,%d,%d,%s", id, ptr, size, stackLineId, stackLine.c_str());
        }
    }
    MemHookLog("== Leak CSV End ==");

}

void ApiRunerMemHook::LogStats(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> guard(m_lock);
    MemHookLog("%s mem: %u outstanding alloc, (%u total / %u deleted)", name.c_str(), m_allocated - m_allocDeleted, m_allocated, m_allocDeleted);
}

// IsStackInsideCallback() needs non-allocating case insenstive string compare so using manual impl
char* stristr(const char* str1, const char* str2)
{
    const char* p1 = str1;
    const char* p2 = str2;
    const char* r = *p2 == 0 ? str1 : 0;

    while (*p1 != 0 && *p2 != 0)
    {
        if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
        {
            if (r == 0)
            {
                r = p1;
            }

            p2++;
        }
        else
        {
            p2 = str2;
            if (r != 0)
            {
                p1 = r + 1;
            }

            if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
            {
                r = p1;
                p2++;
            }
            else
            {
                r = 0;
            }
        }

        p1++;
    }

    return *p2 == 0 ? (char*)r : 0;
}

bool ApiRunerMemHook::IsStackInsideCallback(const char* stackSymbolName, const char* filePath)
{
    const char* found;

    found = stristr(stackSymbolName, "ExampleWebsocketMessageReceived");
    if (found != nullptr)
        return true;
    found = stristr(stackSymbolName, "ExampleWebsocketClosed");
    if (found != nullptr)
        return true;
    found = stristr(stackSymbolName, "WaitForTestResult");
    if (found != nullptr)
        return true;
    found = stristr(filePath, "\\Include\\xsapi-cpp\\");
    if (found != nullptr)
        return true;
    found = stristr(stackSymbolName, "<lambda");
    if (found != nullptr)
    {
        found = stristr(filePath, "\\apiexplorer\\");
        if (found != nullptr)
            return true;
        found = stristr(filePath, "apis_xblc_social_manager.cpp");
        if (found != nullptr)
            return true;
    }

    return false;
}

bool ApiRunerMemHook::IsFilePathInXSAPI(const char* filePath)
{
    const char* found = stristr(filePath, "ource\\");
    if (found != nullptr)
    {
        found = stristr(filePath, "\\Source\\Services\\");
        if (found != nullptr)
            return true;
        found = stristr(filePath, "\\Source\\Shared\\");
        if (found != nullptr)
            return true;
        found = stristr(filePath, "\\Source\\System\\");
        if (found != nullptr)
            return true;
        found = stristr(filePath, "\\libhttpclient\\source\\");
        if (found != nullptr)
        {
            found = stristr(filePath, "\\libhttpclient\\source\\global\\mem.cpp");
            if (found == nullptr)
            {
                return true;
            }
        }
    }

    return false;
}

void ApiRunerMemHook::LogUnhookedStats()
{
    g_rawMemHookInitTracking = false;
    LogToScreen("%d memory unhooked in XSAPI and libHttpClient across %d alloc calls", m_rawAllocMemory, m_rawAllocMemorySpots);
    std::vector<std::string> list;
    for (int i = 0; i < m_rawAllocMemorySpots; i++)
    {
        std::stringstream msg;
        msg << m_allocTrace[i] << "(" << m_allocTraceLine[i] << ")";
        list.push_back(msg.str());
    }

    std::sort(list.begin(), list.end());
    list.erase(std::unique(list.begin(), list.end()), list.end());

    for(std::string s : list)
    {
        LogToScreen(s.c_str());
    }
    g_rawMemHookInitTracking = true;
}

bool ApiRunerMemHook::IsStackInXSAPI(StackInfo& stackInfo)
{
#if HC_PLATFORM == HC_PLATFORM_WIN32 || (HC_PLATFORM == HC_PLATFORM_GDK && !_GAMING_XBOX)
    GetStackTrace(stackInfo);

    bool foundInXsapi = false;
    for (int i = 0; i < stackInfo.stackSize; i++)
    {
        stackInfo.isInXsapi[i] = IsFilePathInXSAPI(stackInfo.szFileNames[i]);
        if (stackInfo.isInXsapi[i])
        {
            foundInXsapi = true;
        }
    }

    if (foundInXsapi)
    {
        bool foundCallback = IsStackFramesInsideCallback(&stackInfo);
        if (foundCallback)
        {
            return false;
        }

        bool foundTestCodeFirst = IsStackFramesInsideTestCode(&stackInfo);
        if (foundTestCodeFirst)
        {
            return false;
        }

        return true;
    }
#else
    UNREFERENCED_PARAMETER(stackInfo);
#endif

    return false;
}

void ApiRunerMemHook::GetStackTrace(StackInfo &stackInfo)
{
#if HC_PLATFORM == HC_PLATFORM_WIN32 || (HC_PLATFORM == HC_PLATFORM_GDK && !_GAMING_XBOX)
    std::lock_guard<std::recursive_mutex> guard(m_lock);
    void* stack[TRACE_MAX_STACK_FRAMES] = { 0 };
    WORD numberOfFrames = CaptureStackBackTrace(0, TRACE_MAX_STACK_FRAMES, stack, NULL);
    stackInfo.stackSize = numberOfFrames < 64 ? numberOfFrames : 64;
    for (int i = 0; i < stackInfo.stackSize; i++)
    {
        stackInfo.szFileNames[i][0] = 0;
        stackInfo.szStack[i][0] = 0;
        stackInfo.lineNumber[i] = 0;
        DWORD64 address = (DWORD64)(stack[i]);

        char buffer[sizeof(SYMBOL_INFO) + TRACE_MAX_FUNCTION_NAME_LENGTH * sizeof(CHAR)] = { 0 };
        PSYMBOL_INFO symbolInfo = (PSYMBOL_INFO)buffer;
        symbolInfo->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH - 1;
        symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        SymFromAddr(m_process, address, NULL, symbolInfo);

        DWORD displacement;
        char line64Buffer[sizeof(IMAGEHLP_LINE64)] = { 0 };
        IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64*)line64Buffer;
        line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        if (SymGetLineFromAddr64(m_process, address, &displacement, line))
        {
#if HC_PLATFORM_IS_MICROSOFT
            strcpy_s(stackInfo.szFileNames[i], line->FileName);
            strcpy_s(stackInfo.szStack[i], symbolInfo->Name);
#else
            strcpy(stackInfo.szFileNames[i], line->FileName);
            strcpy(stackInfo.szStack[i], symbolInfo->Name);
#endif
            stackInfo.lineNumber[i] = line->LineNumber;
        }
    }
#else
    memset(&stackInfo, 0, sizeof(StackInfo));
#endif
}

#if HC_PLATFORM == HC_PLATFORM_WIN32 || (HC_PLATFORM == HC_PLATFORM_GDK && !_GAMING_XBOX)
bool ApiRunerMemHook::IsStackFramesInsideTestCode(StackInfo* pStackInfo)
{
    for (int i = 0; i < pStackInfo->stackSize; i++)
    {
        if (pStackInfo->isInXsapi[i])
            return false;

        const char* found = stristr(pStackInfo->szFileNames[i], "\\apiexplorer\\");
        if (found != nullptr)
            return true;
    }
    return false;
}

bool ApiRunerMemHook::IsStackFramesInsideCallback(StackInfo* pStackInfo)
{
    for (int i = 0; i < pStackInfo->stackSize; i++)
    {
        if (IsStackInsideCallback(pStackInfo->szStack[i], pStackInfo->szFileNames[i]))
        {
            return true;
        }
    }
    return false;
}
#endif

#if TRACK_UNHOOKED_ALLOCS == 1
using namespace std;
void* operator new(size_t size)
{
    if (g_rawMemHookInitTracking)
    {
        auto mem = GetApiRunnerMemHook();
        if (mem->IsMemTrackingStarted())
        {
            StackInfo stackInfo{ 0 };
            bool isInXSAPI = mem->IsStackInXSAPI(stackInfo);
            if (isInXSAPI)
            {
                for (int i = 0; i < stackInfo.stackSize; i++)
                {
                    if (stackInfo.isInXsapi[i])
                    {
#if HC_PLATFORM_IS_MICROSOFT
                        strcpy_s(mem->m_allocTrace[mem->m_rawAllocMemorySpots], stackInfo.szFileNames[i]);
#else
                        strcpy(mem->m_allocTrace[mem->m_rawAllocMemorySpots], stackInfo.szFileNames[i]);
#endif
                        mem->m_allocTraceLine[mem->m_rawAllocMemorySpots] = stackInfo.lineNumber[i];
                        break;
                    }
                }

                mem->m_rawAllocMemory += size;
                mem->m_rawAllocMemorySpots++;
                if (mem->m_rawAllocMemorySpots >= APIRUNNER_MAX_UNHOOKED_MEM_TRACKED)
                {
                    mem->m_rawAllocMemorySpots = APIRUNNER_MAX_UNHOOKED_MEM_TRACKED - 1;
                }
            }
        }
    }

    void* p = malloc(size);
    return p;
}

void operator delete(void * p)
{
    if (g_rawMemHookInitTracking)
    {
        auto mem = GetApiRunnerMemHook();
        if (mem->IsMemHooked(p))
        {
            // This shouldn't be mem hooked
            assert(false);
        }
    }

    free(p);
}
#endif

