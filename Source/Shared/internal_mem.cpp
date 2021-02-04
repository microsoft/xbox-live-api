// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

XblMemAllocFunction g_pMemAllocHook{ DefaultAlloc };
XblMemFreeFunction g_pMemFreeHook{ DefaultFree };

_Ret_maybenull_ _Post_writable_byte_size_(size) void* DefaultAlloc(
    size_t size,
    HCMemoryType memoryType
) noexcept
{
    UNREFERENCED_PARAMETER(memoryType);
    if (size > 0)
    {
        return malloc(size);
    }
    return static_cast<void*>(nullptr);
}

void DefaultFree(
    _In_ _Post_invalid_ void* pointer,
    HCMemoryType memoryType
) noexcept
{
    UNREFERENCED_PARAMETER(memoryType);
    if (pointer)
    {
        free(pointer);
    }
}

_Post_writable_byte_size_(size) void* STDAPIVCALLTYPE Alloc(
    size_t size,
    HCMemoryType memoryType
) noexcept
{
    assert(g_pMemAllocHook);
    try
    {
        return g_pMemAllocHook(size, memoryType);
    }
    catch (...)
    {
        LOGS_ERROR << "Caught exception in MemAlloc hook!";
        return nullptr;
    }
}

void STDAPIVCALLTYPE Free(
    _Post_invalid_ void* pointer,
    HCMemoryType memoryType
) noexcept
{
    assert(g_pMemFreeHook);
    try
    {
        DISABLE_WARNING_PUSH;
        SUPPRESS_WARNING_UNINITIALIZED_MEMORY;
        g_pMemFreeHook(pointer, memoryType);
        DISABLE_WARNING_POP;
    }
    catch (...)
    {
        LOGS_ERROR << "Caught exception in MemFree hook!";
    }
}

char* Make(const char* str)
{
    auto length = strlen(str) + 1;
    char* copy = static_cast<char*>(Alloc(length));
    if (copy != nullptr)
    {
        utils::strcpy(copy, length, str);
    }
    return copy;
}

char* Make(const String& str)
{
    char* cstr = static_cast<char*>(Alloc(str.length() + 1));
    if (cstr != nullptr)
    {
        utils::strcpy(cstr, str.length() + 1, str.data());
    }
    return cstr;
}

char* Make(const string_t& strt)
{
    auto cchCString = utils::utf8_from_char_t(strt.data(), nullptr, 0);
    char* cstr = static_cast<char*>(Alloc(cchCString));
    if (cstr != nullptr)
    {
        utils::utf8_from_char_t(strt.data(), cstr, cchCString);
    }
    return cstr;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
