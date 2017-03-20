// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"

using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

match_metadata::match_metadata() :
    m_isBye(false)
{
}

const string_t&
match_metadata::description() const
{
    return m_description;
}

const utility::datetime& 
match_metadata::start_time() const
{
    return m_startTime;
}

bool 
match_metadata::is_bye() const
{
    return m_isBye;
}

const std::vector<string_t>&
match_metadata::opposing_team_ids() const
{
    return m_opposingTeamIds;
}

xbox_live_result<match_metadata>
match_metadata::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<match_metadata>();

    match_metadata result;
    std::error_code errCode = xbox_live_error_code::no_error;

    result.m_description = utils::extract_json_string(json, _T("label"), errCode, false);
    result.m_isBye = utils::extract_json_bool(json, _T("bye"), errCode);
    result.m_startTime = utils::extract_json_time(json, _T("startTime"), errCode);
    result.m_opposingTeamIds = utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("opposingTeamIds"), errCode, true);

    return result;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END