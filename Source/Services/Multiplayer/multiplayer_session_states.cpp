// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_states::multiplayer_session_states() :
    m_accepted(0),
    m_joinRestriction(multiplayer_session_restriction::unknown),
    m_status(multiplayer_session_status::unknown),
    m_visibility(multiplayer_session_visibility::unknown),
    m_myTurn(false)
{
}

const utility::datetime& 
multiplayer_session_states::start_time() const
{
    return m_startTime;
}

const multiplayer_session_reference&
multiplayer_session_states::session_reference() const
{
    return m_multiplayerSessionReference;
}

multiplayer_session_status
multiplayer_session_states::status() const
{
    return m_status;
}

multiplayer_session_visibility 
multiplayer_session_states::visibility() const
{
    return m_visibility;
}

bool 
multiplayer_session_states::is_my_turn() const
{
    return m_myTurn;
}

const string_t& 
multiplayer_session_states::xbox_user_id() const
{
    return m_xboxUserId;
}

uint32_t
multiplayer_session_states::accepted_member_count() const
{
    return m_accepted;
}

multiplayer_session_restriction
multiplayer_session_states::join_restriction() const
{
    return m_joinRestriction;
}

const std::vector<string_t>& 
multiplayer_session_states::keywords() const
{
    return m_keywords;
}

multiplayer_session_restriction
multiplayer_session_states::_Convert_string_to_multiplayer_session_restriction(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("none")) == 0)
    {
        return multiplayer_session_restriction::none;
    }
    else if (utils::str_icmp(value, _T("local")) == 0)
    {
        return multiplayer_session_restriction::local;
    }
    else if (utils::str_icmp(value, _T("followed")) == 0)
    {
        return multiplayer_session_restriction::followed;
    }

    return multiplayer_session_restriction::unknown;
}

const xbox_live_result<string_t>
multiplayer_session_states::_Convert_multiplayer_session_restriction_to_string(
    _In_ multiplayer_session_restriction joinRestriction
    )
{
    switch (joinRestriction)
    {
        case multiplayer_session_restriction::unknown: return xbox_live_result<string_t>(_T("unknown"));

        case multiplayer_session_restriction::none: return xbox_live_result<string_t>(_T("none"));

        case multiplayer_session_restriction::local: return xbox_live_result<string_t>(_T("local"));

        case multiplayer_session_restriction::followed: return xbox_live_result<string_t>(_T("followed"));

        default: return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Enum out of range");
    }
}

multiplayer_session_status
multiplayer_session_states::_Convert_string_to_session_status(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("active")) == 0)
    {
        return multiplayer_session_status::active;
    }
    else if (utils::str_icmp(value, _T("inactive")) == 0)
    {
        return multiplayer_session_status::inactive;
    }
    else if (utils::str_icmp(value, _T("reserved")) == 0)
    {
        return multiplayer_session_status::reserved;
    }

    return multiplayer_session_status::unknown;
}

multiplayer_session_visibility
multiplayer_session_states::_Convert_string_to_session_visibility(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("private")) == 0)
    {
        return multiplayer_session_visibility::private_session;
    }
    else if (utils::str_icmp(value, _T("visible")) == 0)
    {
        return multiplayer_session_visibility::visible;
    }
    else if (utils::str_icmp(value, _T("full")) == 0)
    {
        return multiplayer_session_visibility::full;
    }
    else if (utils::str_icmp(value, _T("open")) == 0)
    {
        return multiplayer_session_visibility::open;
    }

    return multiplayer_session_visibility::unknown;
}

const xbox_live_result<string_t>
multiplayer_session_states::_Convert_multiplayer_session_visibility_to_string(_In_ multiplayer_session_visibility sessionVisibility)
{
    switch (sessionVisibility)
    {
    case multiplayer_session_visibility::unknown: return xbox_live_result<string_t>(_T("unknown"));
        case multiplayer_session_visibility::any: return xbox_live_result<string_t>(_T("any"));
        case multiplayer_session_visibility::private_session: return xbox_live_result<string_t>(_T("private"));
        case multiplayer_session_visibility::visible: return xbox_live_result<string_t>(_T("visible"));
        case multiplayer_session_visibility::full: return xbox_live_result<string_t>(_T("full"));
        case multiplayer_session_visibility::open: return xbox_live_result<string_t>(_T("open"));
        default: return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Enum out of range");
    }
}

xbox_live_result<multiplayer_session_states>
multiplayer_session_states::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_session_states returnResult;
    if (json.is_null()) return xbox_live_result<multiplayer_session_states>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value sessionRefJson = utils::extract_json_field(json, _T("sessionRef"), errc, true);
    
    returnResult.m_multiplayerSessionReference = multiplayer_session_reference(
        utils::extract_json_string(sessionRefJson, _T("scid"), errc),
        utils::extract_json_string(sessionRefJson, _T("templateName"), errc),
        utils::extract_json_string(sessionRefJson, _T("name"), errc)
        );

    returnResult.m_myTurn = utils::extract_json_bool(json, _T("myTurn"), errc);
    string_t sessionStatus = utils::extract_json_string(json, _T("status"), errc);
    if (!sessionStatus.empty())
    {
        returnResult.m_status = _Convert_string_to_session_status(std::move(sessionStatus));
    }

    string_t sessionVisibility = utils::extract_json_string(json, _T("visibility"), errc);
    if (!sessionVisibility.empty())
    {
        returnResult.m_visibility = _Convert_string_to_session_visibility(sessionVisibility);
    }

    string_t joinRestriction = utils::extract_json_string(json, _T("joinRestriction"), errc);
    if (!joinRestriction.empty())
    {
        returnResult.m_joinRestriction = _Convert_string_to_multiplayer_session_restriction(joinRestriction);
    }

    returnResult.m_startTime = utils::extract_json_time(json, _T("startTime"), errc);
    returnResult.m_xboxUserId = utils::extract_json_string(json, _T("xuid"), errc);
    returnResult.m_accepted = utils::extract_json_int(json, _T("accepted"), errc);
    returnResult.m_keywords = utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("keywords"), errc, false);

    return returnResult;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END