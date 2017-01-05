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
#include "token_error.h"

#include <string>
#include <stdint.h>

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

class token_result
{
public:
    token_result();

    token_result(
        _In_ string_t token, 
        _In_ int64_t expiration,
        _In_ string_t userHash,
        _In_ string_t gamertag,
        _In_ string_t xuid,
        _In_ string_t titleId,
        _In_ string_t ageGroup,
        _In_ string_t privileges
        );

    token_result(
        _In_ token_error err
        );

#ifndef DEFAULT_MOVE_ENABLED
    token_result(token_result&& other);

    token_result& operator=(token_result&& other);
#endif

    /// <summary>
    /// Gets an XErr. Zero on success. Non-zero XErr indicates an error
    /// and the value indicate the cause of the error.
    /// </summary>
    uint32_t xerr() const;

    /// <summary>
    /// Gets the error details. Only interesting when xerr() is non-zero.
    /// </summary>
    const token_error& error() const;

    /// <summary>
    /// Gets the token.
    /// Throws std::logic_error if xerr() is non-zero.
    /// </summary>
    const string_t& token() const;

    /// <summary>
    /// Gets the token expiration as a windows filetime timestamp.
    /// Throws std::logic_error if xerr() is non-zero.
    /// </summary>
    uint64_t expiration() const;

    /// <summary>
    /// Gets the user hash from the display claims.
    /// Throws std::logic_error if xerr() is non-zero.
    /// </summary>
    const string_t& user_hash() const;

    /// <summary>
    /// Gets the user's gamertag.
    /// Throws std::logic_error if xerr() is non-zero.
    /// </summary>
    const string_t& user_gamertag() const;

    /// <summary>
    /// Gets the user's XUID.
    /// Throws std::logic_error if xerr() is non-zero.
    /// </summary>
    const string_t& user_xuid() const;

    /// <summary>
    /// Gets the user's age group.
    /// Throws std::logic_error if xerr() is non-zero.
    /// </summary>
    const string_t& age_group() const;

    /// <summary>
    /// Gets the user's privileges.
    /// Throws std::logic_error if xerr() is non-zero.
    /// </summary>
    const string_t& privileges() const;

    const string_t& title_id() const;
    void set_title_id(_In_ const string_t& titleId);

    /// <summary>
    /// HTTP Status code for token acquisition
    /// </summary>s
    uint32_t http_status_code() const;
    void set_http_status_code(_In_ uint32_t httpStatusCode);

    /// <summary>
    /// Deserializes the JSON into a TokenResult.
    /// </summary>
    static token_result deserialize(
        _In_ const web::json::value& json
        );

private:

    string_t m_token;
    uint64_t m_expiration;

    token_error m_tokenError;

    string_t m_userHash;
    string_t m_userGamertag;
    string_t m_userXuid;
    string_t m_titleId;
    string_t m_ageGroup;
    string_t m_privileges;
    uint32_t m_httpStatusCode;
};


#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif
