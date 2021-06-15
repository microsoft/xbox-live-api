// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#include "xsapi-c/xbox_live_global_c.h"
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// The memhooks are intentionally global and independent of GlobalState. GlobalState depends
// on them being configured before it is constructed.
extern XblMemAllocFunction g_pMemAllocHook;
extern XblMemFreeFunction g_pMemFreeHook;

// Raw allocation/deallocation functions, using memhooks if they are provided
_Ret_maybenull_ _Post_writable_byte_size_(size) void* DefaultAlloc(
    size_t size,
    HCMemoryType memoryType
) noexcept;

void DefaultFree(
    _In_ _Post_invalid_ void* pointer,
    HCMemoryType memoryType
) noexcept;

_Post_writable_byte_size_(size) void* STDAPIVCALLTYPE Alloc(
    size_t size,
    HCMemoryType memoryType = 0
) noexcept;

void STDAPIVCALLTYPE Free(
    _Post_invalid_ void* pointer,
    HCMemoryType memoryType = 0
) noexcept;

// Memhooked STL types
template<typename T>
struct Allocator
{
public:
    typedef T value_type;

    Allocator() = default;
    template<class U> Allocator(Allocator<U> const&) {}

    T* allocate(size_t n)
    {
        T* p = static_cast<T*>(Alloc(n * sizeof(T)));
        if (p == nullptr)
        {
            throw std::bad_alloc();
        }
        return p;
    }

    void deallocate(_In_opt_ void* p, size_t)
    {
        Free(p);
    }
};

template<class T>
bool operator==(Allocator<T> const&, Allocator<T> const&)
{
    return true;
}

template<class T>
bool operator!=(Allocator<T> const&, Allocator<T> const&)
{
    return false;
}

template<class T>
struct Deleter
{
public:
    Deleter() = default;

    void operator()(typename std::allocator_traits<Allocator<T>>::pointer p) const
    {
        Allocator<T> alloc{};
        std::allocator_traits<Allocator<T>>::destroy(alloc, std::addressof(*p));
        std::allocator_traits<Allocator<T>>::deallocate(alloc, p, 1);
    }
};

template<class T>
using UniquePtr = std::unique_ptr<T, Deleter<T>>;

template<class T>
using Vector = std::vector<T, Allocator<T>>;

template<class K, class V, class LESS = std::less<K>>
using Map = std::map<K, V, LESS, Allocator<std::pair<K const, V>>>;

template<class K, class LESS = std::less<K>>
using Set = std::set<K, LESS, Allocator<K>>;

template<class K, class V, class HASH = std::hash<K>, class EQUAL = std::equal_to<K>>
using UnorderedMap = std::unordered_map<K, V, HASH, EQUAL, Allocator<std::pair<K const, V>>>;

template<class K, class HASH = std::hash<K>, class EQUAL = std::equal_to<K>>
using UnorderedSet = std::unordered_set<K, HASH, EQUAL, Allocator<K>>;

template<class C, class TRAITS = std::char_traits<C>>
using BasicString = std::basic_string<C, TRAITS, Allocator<C>>;
using String = BasicString<char>;
using WString = BasicString<wchar_t>;

template<class C, class TRAITS = std::char_traits<C>>
using BasicStringsteam = std::basic_stringstream<C, TRAITS, Allocator<C>>;
using Stringstream = BasicStringsteam<char>;
using WStringstream = BasicStringsteam<wchar_t>;

template<class C, class TRAITS = std::char_traits<C>>
using BasicIStringsteam = std::basic_istringstream<C, TRAITS, Allocator<C>>;
using IStringstream = BasicIStringsteam<char>;
using WIStringstream = BasicIStringsteam<wchar_t>;

template<class C, class TRAITS = std::char_traits<C>>
using BasicOStringsteam = std::basic_ostringstream<C, TRAITS, Allocator<C>>;
using OStringstream = BasicOStringsteam<char>;
using WOStringstream = BasicOStringsteam<wchar_t>;

template<class T>
using Deque = std::deque<T, Allocator<T>>;

template<class T>
using Queue = std::queue<T, Deque<T>>;

template<class T>
using List = std::list<T, Allocator<T>>;

// Memhooked allocation/deallocation helpers
template<typename T, class... TArgs>
inline std::shared_ptr<T> MakeShared(TArgs&&... args)
{
#if !HC_PLATFORM_IS_MICROSOFT || _MSC_VER >= 1910
    return std::allocate_shared<T, Allocator<T>>(Allocator<T>(), std::forward<TArgs>(args)...);
#else
    return std::allocate_shared<T, std::allocator<T>>(std::allocator<T>(), std::forward<TArgs>(args)...);
#endif
}

template<typename T, typename... TArgs>
UniquePtr<T> MakeUnique(TArgs&& ... args)
{
    Allocator<T> alloc{};
    auto mem = alloc.allocate(1);
    auto obj = new (mem) T(std::forward<TArgs>(args)...);
    return UniquePtr<T>{ obj };
}

// Make specializations to allocate UTF-8 strings
char* Make(const char* str);
char* Make(const String& str);
char* Make(const string_t& strt);

template<typename T, class... TArgs>
inline T* Make(TArgs&&... args)
{
    auto mem = Alloc(sizeof(T));
    return new (mem) T(std::forward<TArgs>(args)...);
}

template<typename T>
inline void Delete(T* ptr)
{
    if (ptr != nullptr)
    {
        ptr->~T();
        Free((void*)ptr);
    }
}

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 6386 ) // VS2019 code analysis incorrectly warns about 'elementCount*sizeof(T)' bytes 
#endif
template<typename T>
inline T* MakeArray(size_t elementCount)
{
    auto mem = Alloc(elementCount * sizeof(T));
    if (mem == nullptr)
    {
        return nullptr;
    }

    T* arrayPtr = reinterpret_cast<T*>(mem);
    for (size_t i = 0; i < elementCount; ++i)
    {
        arrayPtr[i] = T{};
    }
    return arrayPtr;
}

template<typename T>
inline T* MakeArray(T* elements, size_t elementCount)
{
    auto mem = Alloc(elementCount * sizeof(T));
    if (mem == nullptr)
    {
        return nullptr;
    }

    T* arrayPtr = reinterpret_cast<T*>(mem);
    for (size_t i = 0; i < elementCount; ++i)
    {
        arrayPtr[i] = elements[i];
    }
    return arrayPtr;
}

template<typename T>
inline T* MakeArray(const Vector<T>& vector)
{
    auto mem = Alloc(vector.size() * sizeof(T));
    if (mem == nullptr)
    {
        return nullptr;
    }

    T* arrayPtr = reinterpret_cast<T*>(mem);
    for (size_t i = 0; i < vector.size(); ++i)
    {
        arrayPtr[i] = vector[i];
    }
    return arrayPtr;
}

template<typename T>
inline void DeleteArray(T* arrayPtr, size_t elementCount)
{
    for (size_t i = 0; i < elementCount; ++i)
    {
        arrayPtr[i].~T();
    }
    Free(arrayPtr, 0);
}

inline char** MakeArray(const Vector<String>& vector)
{
    char** arrayPtr = static_cast<char**>(Alloc(vector.size() * sizeof(char*)));
    if (arrayPtr == nullptr)
    {
        return nullptr;
    }

    for (size_t i = 0; i < vector.size(); ++i)
    {
        arrayPtr[i] = Make(vector[i]);
    }
    return arrayPtr;
}

template<>
inline void DeleteArray(const char** arrayPtr, size_t elementCount)
{
    for (size_t i = 0; i < elementCount; ++i)
    {
        Delete(arrayPtr[i]);
    }
    Free(arrayPtr);
}
#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

// Keep old global namespace typedefs for now as well. These should be removed from xsapi lib and just kept
// legacy header eventually.

template<class T>
using xsapi_internal_vector = xbox::services::Vector<T>;

template<class K, class V, class LESS = std::less<K>>
using xsapi_internal_map = xbox::services::Map<K, V, LESS>;

template<class K, class LESS = std::less<K>>
using xsapi_internal_set = xbox::services::Set<K, LESS>;

template<class K, class V, class HASH = std::hash<K>, class EQUAL = std::equal_to<K>>
using xsapi_internal_unordered_map = xbox::services::UnorderedMap<K, V, HASH, EQUAL>;

template<class K, class HASH = std::hash<K>, class EQUAL = std::equal_to<K>>
using xsapi_internal_unordered_set = xbox::services::UnorderedSet<K, HASH, EQUAL>;

using xsapi_internal_string = xbox::services::String;
using xsapi_internal_wstring = xbox::services::WString;

using xsapi_internal_stringstream = xbox::services::Stringstream;
using xsapi_internal_wstringstream = xbox::services::WStringstream;

using xsapi_internal_istringstream = xbox::services::IStringstream;
using xsapi_internal_wistringstream = xbox::services::WIStringstream;

using xsapi_internal_ostringstream = xbox::services::OStringstream;
using xsapi_internal_wostringstream = xbox::services::WOStringstream;

#ifdef _WIN32
using xsapi_internal_string_t = xbox::services::WString;
using xsapi_internal_stringstream_t = xbox::services::WStringstream;
using xsapi_internal_istringstream_t = xbox::services::WIStringstream;
using xsapi_internal_ostringstream_t = xbox::services::WOStringstream;
#else
using xsapi_internal_string_t = xbox::services::String;
using xsapi_internal_stringstream_t = xbox::services::Stringstream;
using xsapi_internal_istringstream_t = xbox::services::IStringstream;
using xsapi_internal_ostringstream_t = xbox::services::OStringstream;
#endif


template<class T>
using xsapi_internal_dequeue = xbox::services::Deque<T>;

template<class T>
using xsapi_internal_queue = xbox::services::Queue<T>;

template<class T>
using xsapi_internal_list = xbox::services::List<T>;