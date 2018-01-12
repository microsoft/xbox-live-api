// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "user_taskargs.h"

using namespace xbox::services;

sign_in_taskargs::sign_in_taskargs(
    _In_ XSAPI_XBOX_LIVE_USER* _pUser,
    _In_ Platform::Object^ _coreDispatcher,
    _In_opt_ bool _signInSilently
    )
    : pUser(_pUser),
    coreDispatcher(_coreDispatcher),
    signInSilently(_signInSilently)
{
}

get_token_and_signature_taskargs::get_token_and_signature_taskargs(
    _In_ XSAPI_XBOX_LIVE_USER* _pUser,
    _In_ PCSTR _httpMethod,
    _In_ PCSTR _url,
    _In_ PCSTR _headers,
    _In_ PCSTR _requestBodyString
    )
    : pUser(_pUser)
{
    httpMethod = utils::utf16_from_utf8(_httpMethod);
    url = utils::utf16_from_utf8(_url);
    headers = utils::utf16_from_utf8(_headers);
    if (_requestBodyString != nullptr)
    {
        requestBodyString = utils::utf16_from_utf8(_requestBodyString);
    }
}