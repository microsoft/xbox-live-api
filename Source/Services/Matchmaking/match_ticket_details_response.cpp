// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"
#include "xsapi/matchmaking.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN

match_ticket_details_response::match_ticket_details_response() :
    m_matchStatus(ticket_status::unknown),
    m_preserveSession(preserve_session_mode::unknown)
{
}

match_ticket_details_response::match_ticket_details_response(
    _In_ ticket_status matchStatus,
    _In_ std::chrono::seconds estimatedWaitTime,
    _In_ preserve_session_mode preserveSession,
    _In_ multiplayer_session_reference ticketSession,
    _In_ multiplayer_session_reference targetSession,
    _In_ web::json::value ticketAttributes
    ) :
    m_matchStatus(matchStatus),
    m_estimatedWaitTime(std::move(estimatedWaitTime)),
    m_preserveSession(preserveSession),
    m_ticketSession(std::move(ticketSession)),
    m_targetSession(std::move(targetSession)),
    m_ticketAttributes(std::move(ticketAttributes))
{
}

ticket_status
match_ticket_details_response::match_status() const
{
    return m_matchStatus;
}

const std::chrono::seconds&
match_ticket_details_response::estimated_wait_time() const
{
    return m_estimatedWaitTime;
}

preserve_session_mode
match_ticket_details_response::preserve_session() const
{
    return m_preserveSession;
}

const multiplayer_session_reference&
match_ticket_details_response::ticket_session() const
{
    return m_ticketSession;
}

const multiplayer_session_reference&
match_ticket_details_response::target_session() const
{
    return m_targetSession;
}

const web::json::value&
match_ticket_details_response::ticket_attributes() const
{
    return m_ticketAttributes;
}

xbox_live_result<match_ticket_details_response>
match_ticket_details_response::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<match_ticket_details_response>();

    std::error_code errc = xbox_live_error_code::no_error;

    auto ticketSessionRef = multiplayer_session_reference::_Deserialize(utils::extract_json_field(json, _T("ticketSessionRef"), errc, false));
    auto targetSessionRef = multiplayer_session_reference::_Deserialize(utils::extract_json_field(json, _T("targetSessionRef"), errc, false));

    auto result = match_ticket_details_response(
        convert_string_to_ticket_status(utils::extract_json_string(json, _T("ticketStatus"), errc, true)),
        std::chrono::seconds(utils::extract_json_int(json, _T("waitTime"), errc)),
        convert_string_to_preserve_session_mode(utils::extract_json_string(json, _T("preserveSession"), errc, false)),
        ticketSessionRef.payload(),
        targetSessionRef.payload(),
        utils::extract_json_field(json, _T("ticketAttributes"), errc, false)
        );

    return xbox_live_result<match_ticket_details_response>(result, errc);
}

ticket_status match_ticket_details_response::convert_string_to_ticket_status(
    _In_ const string_t& value
    )
{
    ticket_status ticketStatus = ticket_status::unknown;
    if (!value.empty())
    {
        if (utils::str_icmp(value, _T("expired")) == 0)
        {
            ticketStatus = ticket_status::expired;
        }
        else if (utils::str_icmp(value, _T("searching")) == 0)
        {
            ticketStatus = ticket_status::searching;
        }
        else if (utils::str_icmp(value, _T("found")) == 0)
        {
            ticketStatus = ticket_status::found;
        }
        else if (utils::str_icmp(value, _T("canceled")) == 0)
        {
            ticketStatus = ticket_status::canceled;
        }
    }

    return ticketStatus;
}

preserve_session_mode match_ticket_details_response::convert_string_to_preserve_session_mode(
    _In_ const string_t& value
    )
{
    preserve_session_mode preserve_session_mode = preserve_session_mode::unknown;
    if (!value.empty())
    {
        if (utils::str_icmp(value, _T("always")) == 0)
        {
            preserve_session_mode = preserve_session_mode::always;
        }
        else if (utils::str_icmp(value, _T("never")) == 0)
        {
            preserve_session_mode = preserve_session_mode::never;
        }
    }
    return preserve_session_mode;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END