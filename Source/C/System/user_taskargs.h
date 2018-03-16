// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#include "taskargs.h"
#include "xsapi-c/system_c.h"

struct sign_in_taskargs : public taskargs_with_payload<XSAPI_SIGN_IN_RESULT>
{
    sign_in_taskargs(
        _In_ XBL_XBOX_LIVE_USER* pUser,
        _In_ Platform::Object^ coreDispatcher,
        _In_opt_ bool signInSilently = false);

    XBL_XBOX_LIVE_USER* pUser;
    bool signInSilently;
    Platform::Object^ coreDispatcher;
};

struct get_token_and_signature_taskargs : public taskargs_with_payload<XSAPI_TOKEN_AND_SIGNATURE_RESULT>
{
    get_token_and_signature_taskargs(
        _In_ XBL_XBOX_LIVE_USER* pUser,
        _In_ PCSTR httpMethod,
        _In_ PCSTR url,
        _In_ PCSTR headers,
        _In_ PCSTR requestBodyString);

    XBL_XBOX_LIVE_USER* pUser;
    string_t httpMethod;
    string_t url;
    string_t headers;
    string_t requestBodyString;

    xsapi_internal_string token;
    xsapi_internal_string signature;
    xsapi_internal_string xboxUserId;
    xsapi_internal_string gamertag;
    xsapi_internal_string xboxUserHash;
    xsapi_internal_string ageGroup;
    xsapi_internal_string privileges;
    xsapi_internal_string webAccountId;
};