// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <new>
#include <set>
#include <unordered_set>
#include <stddef.h>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>
#include <list>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class xsapi_memory
{
public:
    static inline _Ret_maybenull_ _Post_writable_byte_size_(dwSize) void* mem_alloc(
        _In_ size_t dwSize
        );

    static inline void mem_free(
        _In_opt_ void* pAddress
        );

private:
    xsapi_memory() = delete;
};

class xsapi_memory_buffer
{
public:
    xsapi_memory_buffer(_In_ size_t dwSize)
    {
        m_pBuffer = xsapi_memory::mem_alloc(dwSize);
    }

    ~xsapi_memory_buffer()
    {
        xsapi_memory::mem_free(m_pBuffer);
        m_pBuffer = nullptr;
    }

    void* get()
    {
        return m_pBuffer;
    }

private:
    void* m_pBuffer;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#include "impl/mem.hpp"
