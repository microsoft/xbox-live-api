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
#include "utils.h"
#include "xsapi/multiplayer.h"
#include "xsapi/matchmaking.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN

create_match_ticket_response::create_match_ticket_response()
{
}

create_match_ticket_response::create_match_ticket_response(
    _In_ string_t matchTicketId,
    _In_ std::chrono::seconds estimatedWaitTime
    ) :
    m_matchTicketId(std::move(matchTicketId)),
    m_estimatedWaitTime(std::move(estimatedWaitTime))
{
}

const string_t&
create_match_ticket_response::match_ticket_id() const
{
    return m_matchTicketId;
}

const std::chrono::seconds&
create_match_ticket_response::estimated_wait_time() const
{
    return m_estimatedWaitTime;
}

xbox_live_result<create_match_ticket_response>
create_match_ticket_response::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<create_match_ticket_response>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto response = create_match_ticket_response(
        utils::extract_json_string(json, _T("ticketId"), errc, true),
        std::chrono::seconds(utils::extract_json_int(json, _T("waitTime"), errc))
        );

    return xbox_live_result<create_match_ticket_response>(response, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END