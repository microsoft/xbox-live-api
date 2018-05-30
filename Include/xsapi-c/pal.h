// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#pragma warning(disable: 4062)

#include <stdint.h>

#ifdef _WIN32
    #ifndef _WIN32_WINNT_WIN10
    #define _WIN32_WINNT_WIN10 0x0A00
    #endif

    #ifndef XDK_API
    #define XDK_API (WINAPI_FAMILY == WINAPI_FAMILY_TV_APP || WINAPI_FAMILY == WINAPI_FAMILY_TV_TITLE) 
    #endif

    #ifndef UWP_API
    #define UWP_API (WINAPI_FAMILY == WINAPI_FAMILY_APP && _WIN32_WINNT >= _WIN32_WINNT_WIN10)
    #endif
#endif //#ifdef _WIN32

#ifndef _WIN32
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

    #ifndef ANYSIZE_ARRAY
    #define ANYSIZE_ARRAY 1
    #endif

    #ifndef FIELD_OFFSET
    #define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))
    #endif
#endif

#if defined _WIN32
  #ifdef XBL_API_NONE
    #define XBL_API
    #if _MSC_VER >= 1900
        #define XBL_API_DEPRECATED __declspec(deprecated)
    #else
        #define XBL_API_DEPRECATED
    #endif
  #else
    #ifdef XBL_API_EXPORT
      #define XBL_API __declspec(dllexport)
      #define XBL_API_DEPRECATED __declspec(dllexport, deprecated)
    #else
      #define XBL_API __declspec(dllimport)
      #define XBL_API_DEPRECATED __declspec(dllimport, deprecated)
    #endif
  #endif
#else
  #if defined _NOXBL_API || __GNUC__ < 4
     #define XBL_API
     #define XBL_API_DEPRECATED __attribute__ ((deprecated))
  #else
    #define XBL_API __attribute__ ((visibility ("default")))
    #define XBL_API_DEPRECATED __attribute__ ((visibility ("default"), deprecated))
  #endif
#endif

#ifndef _T
    #ifdef _WIN32
        #define _T(x) L ## x
    #else
        #define _T(x) x
    #endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define XBL_CALLING_CONV __cdecl

#ifdef __cplusplus
#define XBL_NOEXCEPT noexcept
#else
#define XBL_NOEXCEPT
#endif

#include "httpClient/pal.h"

// FACILITY_XBOX + 0x5200 + offset
#define E_XBL_RUNTIME_ERROR                     _HRESULT_TYPEDEF_(0x89235200)
#define E_XBL_RTA_GENERIC_ERROR                 _HRESULT_TYPEDEF_(0x89235201)
#define E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED    _HRESULT_TYPEDEF_(0x89235202)
#define E_XBL_RTA_ACCESS_DENIED                 _HRESULT_TYPEDEF_(0x89235203)
#define E_XBL_AUTH_UNKNOWN_ERROR                _HRESULT_TYPEDEF_(0x89235204)
#define E_XBL_AUTH_RUNTIME_ERROR                _HRESULT_TYPEDEF_(0x89235205)
#define E_XBL_AUTH_NO_TOKEN                     _HRESULT_TYPEDEF_(0x89235206)
#define E_XBL_ALREADY_INITIALISED               _HRESULT_TYPEDEF_(0x89235207)

#define XBL_COLOR_CHAR_SIZE                     (7 * 3)
#define XBL_DISPLAY_NAME_CHAR_SIZE              (30 * 3)
#define XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE       (225 * 3)
#define XBL_GAMERSCORE_CHAR_SIZE                (16 * 3)
#define XBL_GAMERTAG_CHAR_SIZE                  (16 * 3)
#define XBL_NUM_PRESENCE_RECORDS                (6 * 3)
#define XBL_REAL_NAME_CHAR_SIZE                 (255 * 3)
#define XBL_RICH_PRESENCE_CHAR_SIZE             (100 * 3)
#define XBL_XBOX_USER_ID_CHAR_SIZE              (21 * 3)

typedef int32_t function_context;
typedef struct xbl_xbox_live_context* xbl_context_handle;

#if XDK_API
typedef struct IInspectable* xbl_user_handle;
#else
typedef struct xbl_xbox_live_user* xbl_user_handle;
#endif

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)