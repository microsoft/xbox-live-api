// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"

// TODO: Add XML doc comments to this file and elsewhere
// TODO: Add SAL to structs in this file and elsewhere
// TODO: Choose consistent naming XSAPI_ or XboxLive_ or Xbl_ here and elsewhere
// TODO: run and clean code analysis errors
#if defined(__cplusplus)
extern "C" {
#endif

struct XSAPI_XBOX_LIVE_USER;
struct XSAPI_XBOX_LIVE_APP_CONFIG;
struct XSAPI_XBOX_LIVE_CONTEXT_IMPL;

typedef struct XBL_XBOX_LIVE_CONTEXT
{
    PCSTR xboxUserId;
#if XDK_API | XBOX_UWP
    Windows::Xbox::System::User^ user;
#else
    XSAPI_XBOX_LIVE_USER* pUser;
#endif
    CONST XSAPI_XBOX_LIVE_APP_CONFIG* pAppConfig;
    XSAPI_XBOX_LIVE_CONTEXT_IMPL* pImpl;
} XBL_XBOX_LIVE_CONTEXT;

#if !(XDK_API | XBOX_UWP)

// TODO: Change to *CreateHandle
XBL_API XBL_RESULT XBL_CALLING_CONV
XboxLiveContextCreate(
    _In_ CONST XSAPI_XBOX_LIVE_USER* pUser,
    _Out_ XBL_XBOX_LIVE_CONTEXT** ppContext
    );

#endif

// TODO: Change to *CloseHandle, and *DuplicateHandle
XBL_API void XBL_CALLING_CONV
XboxLiveContextDelete(
    XBL_XBOX_LIVE_CONTEXT* pContext
    );

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)