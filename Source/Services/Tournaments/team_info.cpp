// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"
#include "utils.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

team_info::team_info() :
    m_ranking(0),
    m_state(team_state::unknown),
    m_completedReason(team_completed_reason::unknown)
{
}

const string_t& 
team_info::id() const
{
    return m_teamId;
}

const string_t&
team_info::name() const
{
    return m_teamName;
}

const std::vector<string_t>&
team_info::member_xbox_user_ids() const
{
    return m_memberXuids;
}

const utility::datetime& 
team_info::registration_date() const
{
    return m_registrationDate;
}

const string_t&
team_info::standing() const
{
    return m_standing;
}

team_state 
team_info::state() const
{
    return m_state;
}

team_completed_reason
team_info::completed_reason() const
{
    return m_completedReason;
}

uint64_t 
team_info::ranking() const
{
    return m_ranking;
}

const string_t&
team_info::continuation_uri() const
{
    return m_continuationUri;
}

const xbox::services::tournaments::current_match_metadata&
team_info::current_match_metadata() const
{
    return m_currentMatchMetadata;
}

const xbox::services::tournaments::previous_match_metadata&
team_info::previous_match_metadata() const
{
    return m_previousMatchMetadata;
}

xbox::services::xbox_live_result<team_info> 
team_info::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<team_info>();

    team_info result;
    std::error_code errCode = xbox_live_error_code::no_error;

    result.m_teamId = utils::extract_json_string(json, _T("id"), errCode);
    result.m_teamName = utils::extract_json_string(json, _T("name"), errCode);
    result.m_standing = utils::extract_json_string(json, _T("standing"), errCode);
    result.m_registrationDate = utils::extract_json_time(json, _T("registrationDate"), errCode);
    result.m_ranking = utils::extract_json_int(json, _T("ranking"), errCode);
    result.m_state = tournament_service::_Convert_string_to_team_state(utils::extract_json_string(json, _T("state"), errCode));
    result.m_completedReason = _Convert_string_to_completed_reason(utils::extract_json_string(json, _T("completedReason"), errCode));

    web::json::value members = utils::extract_json_field(json, _T("members"), errCode, false);
    web::json::array membersArray = members.as_array();
    for (const auto& memberJson : membersArray)
    {
        auto xuid = utils::extract_json_string(memberJson, _T("id"), errCode);
        result.m_memberXuids.push_back(xuid);
    }

    web::json::value continuationUriJson = utils::extract_json_field(json, _T("continuationUri"), errCode, false);
    if (!continuationUriJson.is_null())
    {
        web::json::value continuationUriValueJson;
#if TV_API
        continuationUriValueJson = utils::extract_json_field(continuationUriJson, _T("uwp_xboxone"), errCode, false);
#else
        continuationUriValueJson = utils::extract_json_field(continuationUriJson, _T("uwp_desktop"), errCode, false);
#endif
        result.m_continuationUri = utils::extract_json_string(continuationUriValueJson, _T("continuationUri"), errCode);;
    }

    auto currentMatchData = xbox::services::tournaments::current_match_metadata::_Deserialize(utils::extract_json_field(json, _T("currentMatch"), errCode, false));
    if (currentMatchData.err())
    {
        errCode = currentMatchData.err();
    }
    result.m_currentMatchMetadata = currentMatchData.payload();

    auto previoustMatchData = xbox::services::tournaments::previous_match_metadata::_Deserialize(utils::extract_json_field(json, _T("previousMatch"), errCode, false));
    if (previoustMatchData.err())
    {
        errCode = previoustMatchData.err();
    }
    result.m_previousMatchMetadata = previoustMatchData.payload();

    return xbox_live_result<team_info>(result, errCode);
}

team_completed_reason
team_info::_Convert_string_to_completed_reason(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("rejected")) == 0)
    {
        return team_completed_reason::rejected;
    }
    else if (utils::str_icmp(value, _T("eliminated")) == 0)
    {
        return team_completed_reason::eliminated;
    }
    else if (utils::str_icmp(value, _T("evicted")) == 0)
    {
        return team_completed_reason::evicted;
    }
    else if (utils::str_icmp(value, _T("completed")) == 0)
    {
        return team_completed_reason::completed;
    }

    return team_completed_reason::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END