// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <stdint.h>
#include "pal.h"
#include "Xal/xal_types.h"

extern "C"
{

// Xbox live error codes.
// FACILITY_XBOX + 0x5200 + offset
#define E_XBL_RUNTIME_ERROR                     _HRESULT_TYPEDEF_(0x89235200)
#define E_XBL_RTA_GENERIC_ERROR                 _HRESULT_TYPEDEF_(0x89235201)
#define E_XBL_RTA_SUBSCRIPTION_LIMIT_REACHED    _HRESULT_TYPEDEF_(0x89235202)
#define E_XBL_RTA_ACCESS_DENIED                 _HRESULT_TYPEDEF_(0x89235203)
#define E_XBL_AUTH_UNKNOWN_ERROR                _HRESULT_TYPEDEF_(0x89235204)
#define E_XBL_AUTH_RUNTIME_ERROR                _HRESULT_TYPEDEF_(0x89235205)
#define E_XBL_AUTH_NO_TOKEN                     _HRESULT_TYPEDEF_(0x89235206)
#define E_XBL_ALREADY_INITIALIZED               _HRESULT_TYPEDEF_(0x89235207)
#define E_XBL_NOT_INITIALIZED                   _HRESULT_TYPEDEF_(0x89235208)
#define E_XBL_RTA_NOT_ACTIVATED                 _HRESULT_TYPEDEF_(0x89235209)

// Xbox live size constants
#define XBL_COLOR_CHAR_SIZE                     (7 * 3)
#define XBL_DISPLAY_NAME_CHAR_SIZE              (30 * 3)
#define XBL_DISPLAY_PIC_URL_RAW_CHAR_SIZE       (225 * 3)
#define XBL_GAMERSCORE_CHAR_SIZE                (16 * 3)
#define XBL_GAMERTAG_CHAR_SIZE                  (16 * 3)
#define XBL_MODERN_GAMERTAG_CHAR_SIZE           (((12 + 12) * 4) + 1) // 12 characters + 12 diacritic, 4 bytes each, plus 1 byte null terminator
#define XBL_MODERN_GAMERTAG_SUFFIX_CHAR_SIZE    (14 + 1) // 14 alphanumeric characters + null terminator
#define XBL_UNIQUE_MODERN_GAMERTAG_CHAR_SIZE    (XBL_MODERN_GAMERTAG_CHAR_SIZE + 1 + 3 ) // modern gamertag + '#' + max suffix size for cases when MGT display length is 12. Null terminator already accoutned for in MGT
#define XBL_REAL_NAME_CHAR_SIZE                 (255 * 3)
#define XBL_RICH_PRESENCE_CHAR_SIZE             (100 * 3)
#define XBL_TITLE_NAME_CHAR_SIZE                (100 * 3)
#define XBL_XBOX_USER_ID_CHAR_SIZE              (21 * 3)
#define XBL_LAST_TIME_PLAYED_CHAR_SIZE          (25 * 3)

#define XBL_GUID_LENGTH                         40
#define XBL_SCID_LENGTH                         XBL_GUID_LENGTH

/// <summary>
/// Handle to the underlying user used to create an Xbox Live context.
/// </summary>
#if HC_PLATFORM == HC_PLATFORM_GDK
typedef XUserHandle XblUserHandle;
#else
typedef XalUserHandle XblUserHandle;
#endif

/// <summary>
/// A context token returned when registering a callback/handler to identify the registration.  
/// This context token is later used to unregister the callback/handler.
/// A value of 0 indicates and invalid handler token.
/// </summary>
typedef int32_t XblFunctionContext;

/// <summary>
/// Handle to an Xbox live context.  
/// Needed to interact with Xbox live services.
/// </summary>
typedef struct XblContext* XblContextHandle;

/// <summary>
/// Generic Guid struct.  
/// Used by various Xbox live services.
/// </summary>
typedef struct XblGuid
{
    /// <summary>
    /// The stored Guid.
    /// </summary>
    _Null_terminated_ char value[XBL_GUID_LENGTH];
} XblGuid;

}
