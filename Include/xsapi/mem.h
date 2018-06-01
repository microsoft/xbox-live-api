// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <new>
#include <set>
#include <unordered_set>
#include <stddef.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class xsapi_memory
{
public:
    static _Ret_maybenull_ _Post_writable_byte_size_(dwSize) void* mem_alloc(
        _In_ size_t dwSize
        );

    static void mem_free(
        _In_ void* pAddress
        );

private:
    xsapi_memory();
    xsapi_memory(const xsapi_memory&);
    xsapi_memory& operator=(const xsapi_memory&);
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


template<typename T>
class xsapi_stl_allocator
{
public:
    xsapi_stl_allocator() { }

    template<typename Other> xsapi_stl_allocator(const xsapi_stl_allocator<Other> &) { }

    template<typename Other>
    struct rebind
    {
        typedef xsapi_stl_allocator<Other> other;
    };

    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef T           value_type;

    pointer allocate(size_type n, const void * = 0)
    {
        pointer p = reinterpret_cast<pointer>(xbox::services::system::xsapi_memory::mem_alloc(n * sizeof(T)));

        if (p == NULL)
        {
            throw std::bad_alloc();
        }
        return p;
    }

    void deallocate(_In_opt_ void* p, size_type)
    {
        if (p != nullptr)
        {
            xbox::services::system::xsapi_memory::mem_free(p);
        }
    }

    char* _Charalloc(size_type n)
    {
        char* p = reinterpret_cast<char*>(xbox::services::system::xsapi_memory::mem_alloc(n));

        if (p == NULL)
        {
            throw std::bad_alloc();
        }
        return p;
    }

    void construct(_In_ pointer p, const_reference t)
    {
        new ((void*)p) T(t);
    }

    void destroy(_In_ pointer p)
    {
        p; // Needed to avoid unreferenced param on VS2012
        p->~T();
    }

    size_t max_size() const
    {
        size_t n = (size_t)(-1) / sizeof(T);
        return (0 < n ? n : 1);
    }
};

template<typename T>
struct xsapi_alloc_deleter
{
    xsapi_alloc_deleter() {}
    xsapi_alloc_deleter(const xsapi_stl_allocator<T>& alloc) : m_alloc(alloc) { }

    void operator()(typename std::allocator_traits<xsapi_stl_allocator<T>>::pointer p) const
    {
        xsapi_stl_allocator<T> alloc(m_alloc);
        std::allocator_traits<xsapi_stl_allocator<T>>::destroy(alloc, std::addressof(*p));
        std::allocator_traits<xsapi_stl_allocator<T>>::deallocate(alloc, p, 1);
    }

private:
    xsapi_stl_allocator<T> m_alloc;
};

template<typename T, typename... Args>
std::shared_ptr<T> xsapi_allocate_shared(Args&&... args)
{
#if XSAPI_U || _MSC_VER >= 1910
    return std::allocate_shared<T, xsapi_stl_allocator<T>>(xsapi_stl_allocator<T>(), std::forward<Args>(args)...);
#else
    return std::allocate_shared<T, std::allocator<T>>(std::allocator<T>(), std::forward<Args>(args)...);
#endif
}

template<typename T, typename... Args>
std::unique_ptr<T, xsapi_alloc_deleter<T>> xsapi_allocate_unique(Args&&... args)
{
    xsapi_stl_allocator<T> alloc;
    auto p = std::allocator_traits<xsapi_stl_allocator<T>>::allocate(alloc, 1); // malloc memory
    auto o = new(p) T(std::forward<Args>(args)...); // call class ctor using placement new
    return std::unique_ptr<T, xsapi_alloc_deleter<T>>(o, xsapi_alloc_deleter<T>(alloc));
}

template<typename T>
using xsapi_unique_ptr = std::unique_ptr<T, xsapi_alloc_deleter<T>>;

template<typename T1, typename T2>
inline bool operator==(const xsapi_stl_allocator<T1>&, const xsapi_stl_allocator<T2>&)
{
    return true;
}

template<typename T1, typename T2>
bool operator!=(const xsapi_stl_allocator<T1>&, const xsapi_stl_allocator<T2>&)
{
    return false;
}

template<class T>
using xsapi_internal_vector = std::vector<T, xsapi_stl_allocator<T>>;

template<class K, class V, class LESS = std::less<K>>
using xsapi_internal_map = std::map<K, V, LESS, xsapi_stl_allocator<std::pair<K const, V>>>;

template<class K, class LESS = std::less<K>>
using xsapi_internal_set = std::set<K, LESS, xsapi_stl_allocator<K>>;

template<class K, class V, class HASH = std::hash<K>, class EQUAL = std::equal_to<K>>
using xsapi_internal_unordered_map = std::unordered_map<K, V, HASH, EQUAL, xsapi_stl_allocator<std::pair<K const, V>>>;

template<class K, class HASH = std::hash<K>, class EQUAL = std::equal_to<K>>
using xsapi_internal_unordered_set = std::unordered_set<K, HASH, EQUAL, xsapi_stl_allocator<K>>;

template<class C, class TRAITS = std::char_traits<C>>
using xsapi_internal_basic_string = std::basic_string<C, TRAITS, xsapi_stl_allocator<C>>;

using xsapi_internal_string = xsapi_internal_basic_string<char>;
using xsapi_internal_wstring = xsapi_internal_basic_string<wchar_t>;
using xsapi_internal_string_t = xsapi_internal_basic_string<char_t>;

template<class C, class TRAITS = std::char_traits<C>>
using xsapi_internal_basic_stringstream = std::basic_stringstream<C, TRAITS, xsapi_stl_allocator<C>>;

using xsapi_internal_stringstream = xsapi_internal_basic_stringstream<char>;
using xsapi_internal_wstringstream = xsapi_internal_basic_stringstream<wchar_t>;

template<class T>
using xsapi_internal_dequeue = std::deque<T, xsapi_stl_allocator<T>>;

template<class T>
using xsapi_internal_queue = std::queue<T, xsapi_internal_dequeue<T>>;
