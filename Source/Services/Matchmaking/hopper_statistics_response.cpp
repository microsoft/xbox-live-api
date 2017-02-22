// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"
#include "xsapi/matchmaking.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN

hopper_statistics_response::hopper_statistics_response():
    m_playersWaitingToMatch(0)
{
}

hopper_statistics_response::hopper_statistics_response(
    _In_ string_t hopperName,
    _In_ std::chrono::seconds estimatedWaitTime,
    _In_ uint32_t playersWaitingToMatch
    ) :
    m_hopperName(std::move(hopperName)),
    m_estimatedWaitTime(std::move(estimatedWaitTime)),
    m_playersWaitingToMatch(playersWaitingToMatch)
{
}

/// <summary>
/// Name of the hopper in which a match was requested.
/// </summary>
const string_t& 
hopper_statistics_response::hopper_name() const
{
    return m_hopperName;
}

/// <summary>
/// Estimated wait time for a match request to be matched with other players.
/// </summary>
const std::chrono::seconds& hopper_statistics_response::estimated_wait_time() const
{
    return m_estimatedWaitTime;
}

/// <summary>
/// The number of players in the hopper waiting to be matched.
/// </summary>
uint32_t hopper_statistics_response::players_waiting_to_match() const
{
    return m_playersWaitingToMatch;
}

xbox_live_result<hopper_statistics_response>
hopper_statistics_response::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<hopper_statistics_response>();

    std::error_code errc = xbox_live_error_code::no_error;
    return hopper_statistics_response(
        utils::extract_json_string(json, _T("name"), errc, true),
        std::chrono::seconds(utils::extract_json_int(json, _T("waitTime"), errc, false)),
        utils::extract_json_int(json, _T("population"), errc, false)
        );
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END