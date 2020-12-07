// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
    #error C++11 required
#endif

#pragma once

#if HC_PLATFORM != HC_PLATFORM_ANDROID
#pragma warning(disable: 4062)
#endif

#include "httpClient/pal.h"

#if HC_PLATFORM == HC_PLATFORM_GDK
    #include <grdk.h>
    #if !(_GRDK_YY == 19 && _GRDK_MM_NUM == 5) // GPEA GDK doesn't have XGameEventWrite API
        #define XSAPI_BUILD_WITH_1910_GRTS 1
    #endif
#endif

// Events/Notifications services
#if !(HC_PLATFORM == HC_PLATFORM_XDK || XSAPI_UNIT_TESTS)
    #ifndef XSAPI_EVENTS_SERVICE
        #define XSAPI_EVENTS_SERVICE 1
    #endif
#endif

#if HC_PLATFORM == HC_PLATFORM_GDK && XSAPI_BUILD_WITH_1910_GRTS
    #ifndef XSAPI_GRTS_EVENTS_SERVICE
        #define XSAPI_GRTS_EVENTS_SERVICE 1
    #endif
#endif

#if HC_PLATFORM_IS_MICROSOFT && HC_PLATFORM != HC_PLATFORM_WIN32 && !XSAPI_BUILD_WITH_1910_GRTS
    #ifndef XSAPI_WRL_EVENTS_SERVICE
        #define XSAPI_WRL_EVENTS_SERVICE 1
    #endif
#endif

#if !XSAPI_GRTS_EVENTS_SERVICE && !XSAPI_WRL_EVENTS_SERVICE
    #ifndef XSAPI_INTERNAL_EVENTS_SERVICE
        #define XSAPI_INTERNAL_EVENTS_SERVICE 1
    #endif
#endif

#if !(HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_GDK || XSAPI_UNIT_TESTS)
    #ifndef XSAPI_NOTIFICATION_SERVICE
        #define XSAPI_NOTIFICATION_SERVICE 1
    #endif
#endif

// WinRT APIs
#if XSAPI_UNIT_TESTS
    #define XSAPI_WINRT 1
#endif

extern "C"
{

#if HC_PLATFORM_IS_MICROSOFT
    #ifndef _WIN32_WINNT_WIN10
    #define _WIN32_WINNT_WIN10 0x0A00
    #endif
#endif

#if !HC_PLATFORM_IS_MICROSOFT
    #ifdef _In_
    #undef _In_
    #endif
    #define _In_

    #ifdef _Ret_maybenull_
    #undef _Ret_maybenull_
    #endif
    #define _Ret_maybenull_

    #ifdef _Post_writable_byte_size_
    #undef _Post_writable_byte_size_
    #endif
    #define _Post_writable_byte_size_(X)

    #ifdef _Outptr_result_maybenull_
    #undef _Outptr_result_maybenull_
    #endif
    #define _Outptr_result_maybenull_

    #ifndef ANYSIZE_ARRAY
    #define ANYSIZE_ARRAY 1
    #endif

    typedef long LONG;
    typedef long LONG_PTR;

    #ifndef FIELD_OFFSET
    #define FIELD_OFFSET(type, field)    ((long)(long)&(((type *)0)->field))
    #endif

    #ifndef UNREFERENCED_PARAMETER
    #define UNREFERENCED_PARAMETER(P)   (P)
    #endif
#endif

#if HC_PLATFORM_IS_MICROSOFT
    #if _MSC_VER >= 1900
        #define XBL_DEPRECATED __declspec(deprecated)
    #else
        #define XBL_DEPRECATED
    #endif
    #define STDAPI_XBL_DEPRECATED           EXTERN_C XBL_DEPRECATED HRESULT STDAPICALLTYPE
    #define STDAPI_XBL_DEPRECATED_(type)    EXTERN_C XBL_DEPRECATED type STDAPICALLTYPE

    #define XBL_WARNING_PUSH __pragma(warning(push))
    #define XBL_WARNING_DISABLE_DEPRECATED __pragma(warning(disable:4996))
    #define XBL_WARNING_POP __pragma(warning(pop))
#else
    #define XBL_DEPRECATED
    #define STDAPI_XBL_DEPRECATED           EXTERN_C XBL_DEPRECATED HRESULT STDAPIVCALLTYPE
    #define STDAPI_XBL_DEPRECATED_(type)    EXTERN_C XBL_DEPRECATED type STDAPIVCALLTYPE

    #define XBL_WARNING_PUSH
    #define XBL_WARNING_DISABLE_DEPRECATED
    #define XBL_WARNING_POP
#endif

#ifndef _T
    #if HC_PLATFORM_IS_MICROSOFT
        #define _T(x) L ## x
    #else
        #define _T(x) x
    #endif
#endif

#ifndef XBL_CALLING_CONV
    #define XBL_CALLING_CONV __cdecl
#endif

#ifdef __cplusplus
#define XBL_NOEXCEPT noexcept
#else
#define XBL_NOEXCEPT
#endif

} // end extern "C"
