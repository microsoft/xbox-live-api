//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "shared_macros.h"
#include "token_result.h"
#include "utils.h"
#include "cpprest/json.h"

#include <string>
#include <stdint.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


token_result::token_result() :
    m_expiration(0),
    m_httpStatusCode(0)
{
}

token_result::token_result(
    _In_ string_t token, 
    _In_ int64_t expiration,
    _In_ string_t userHash,
    _In_ string_t gamertag,
    _In_ string_t xuid,
    _In_ string_t titleId,
    _In_ string_t ageGroup,
    _In_ string_t privileges) :
    m_token(std::move(token)),
    m_expiration(expiration),
    m_userHash(std::move(userHash)),
    m_userGamertag(std::move(gamertag)),
    m_userXuid(std::move(xuid)),
    m_titleId(std::move(titleId)),
    m_ageGroup(std::move(ageGroup)),
    m_privileges(std::move(privileges))
{
}

token_result::token_result(
    _In_ token_error error) :
    m_expiration(0),
    m_tokenError(std::move(error))
{
}

#ifndef DEFAULT_MOVE_ENABLED
token_result::token_result(token_result&& other)
{
    *this = std::move(other);
}

token_result& token_result::operator=(token_result&& other)
{
    if (this != &other)
    {
        m_token = std::move(other.m_token);
        m_expiration = other.m_expiration;
        m_userHash = std::move(other.m_userHash);
        m_userGamertag = std::move(other.m_userGamertag);
        m_userXuid = std::move(other.m_userXuid);
        m_tokenError = std::move(other.m_tokenError);
        m_titleId = std::move(other.m_titleId);
        m_ageGroup = std::move(other.m_ageGroup);
        m_privileges = std::move(other.m_privileges);
    }

    return *this;
}
#endif

uint32_t token_result::xerr() const
{
    return m_tokenError.xerr();
}

const token_error& token_result::error() const
{
    return m_tokenError;
}

const string_t& token_result::token() const
{
    return m_token;
}

uint64_t token_result::expiration() const
{
    return m_expiration;
}

const string_t& token_result::user_hash() const
{
    return m_userHash;
}

const string_t& token_result::user_gamertag() const
{
    return m_userGamertag;
}

const string_t& token_result::user_xuid() const
{
    return m_userXuid;
}

const string_t& token_result::title_id() const
{
    return m_titleId;
}

const string_t& token_result::age_group() const
{
    return m_ageGroup;
}

const string_t& token_result::privileges() const
{
    return m_privileges;
}

void token_result::set_title_id(
    _In_ const string_t& titleId
    )
{
    m_titleId = titleId;
}

uint32_t
token_result::http_status_code() const
{
    return m_httpStatusCode;
}

void
token_result::set_http_status_code(
    _In_ uint32_t httpStatusCode
    )
{
    m_httpStatusCode = httpStatusCode;
}

token_result token_result::deserialize(
    _In_ const web::json::value& json
    )
{
    std::error_code errc;
    string_t token(utils::extract_json_string(json, _T("Token"), errc, true));
    string_t expiration(utils::extract_json_string(json, _T("NotAfter"), errc, true));

    // The date is in the form: "2014-04-16T08:44:10.0810553Z"
    utility::datetime d = utility::datetime::from_string(
        expiration, 
        utility::datetime::date_format::ISO_8601);

    string_t userHash;
    string_t userGamertag;
    string_t userXuid;
    string_t titleId;
    string_t ageGroup;
    string_t privileges;

    const web::json::value displayClaims(utils::extract_json_field(json, _T("DisplayClaims"), errc, false));
    if (!displayClaims.is_null())
    {
        const web::json::value xui(utils::extract_json_field(displayClaims, _T("xui"), errc, false));
        if (xui.is_array() && xui.size() > 0)
        {
            const web::json::value& xui0(xui.as_array().at(0));
            userHash = utils::extract_json_string(xui0, _T("uhs"), errc);
            userGamertag = utils::extract_json_string(xui0, _T("gtg"), errc);
            userXuid = utils::extract_json_string(xui0, _T("xid"), errc);
            ageGroup = utils::extract_json_string(xui0, _T("agg"), errc);
            privileges = utils::extract_json_string(xui0, _T("prv"), errc);
        }

        const web::json::value xti(utils::extract_json_field(displayClaims, _T("xti"), errc, false));
        if (xti.is_object() && xti.size() > 0)
        {
            titleId = utils::extract_json_string(xti, _T("tid"), errc);
        }
    }

    return token_result(token, d.to_interval(), userHash, userGamertag, userXuid, titleId, ageGroup, privileges);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

