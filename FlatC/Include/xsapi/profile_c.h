// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"
#include "xsapi/errors_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct XSAPI_XBOX_LIVE_CONTEXT;

typedef struct XSAPI_XBOX_USER_PROFILE
{
    PCSTR xboxUserProfile;
    PCSTR appDisplayName;
    PCSTR gameDisplayName;
    PCSTR gamerscore;
    PCSTR gamertag;
    PCSTR xboxUserId;
    /// TODO URIs
} XSAPI_XBOX_USER_PROFILE;

typedef void(*XSAPI_GET_USER_PROFILE_COMPLETION_ROUTINE)(
    _In_ XSAPI_RESULT_INFO result,
    _In_ XSAPI_XBOX_USER_PROFILE profile,
    _In_opt_ void* context
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
XSAPIGetUserProfile(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_ XSAPI_GET_USER_PROFILE_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)