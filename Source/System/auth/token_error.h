//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "shared_macros.h"
#include "cpprest/json.h"

#include <string>
#include <stdint.h>

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif


class token_error
{
public:
    token_error();

    token_error(
        _In_ string_t identity,
        _In_ uint32_t xerr,
        _In_ string_t redirect
        );

#ifndef DEFAULT_MOVE_ENABLED
    token_error(token_error&& other);

    token_error& operator=(token_error&& other);
#endif

    /// <summary>
    /// Indicates which identity (user, device, or title) the error
    /// is for.
    /// </summary>
    const string_t& identity() const;

    /// <summary>
    /// The XErr code. Zero on success. Non-zero otherwise.
    /// </summary>
    uint32_t xerr() const;

    /// <summary>
    /// A URL to a web blend for resolving the error.
    /// </summary>
    const string_t& redirect() const;

    static token_error deserialize(
        _In_ const web::json::value& json
        );

private:

    string_t m_identity;
    uint32_t m_xerr;
    string_t m_redirect;
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif


