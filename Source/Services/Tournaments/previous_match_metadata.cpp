// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"

using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

previous_match_metadata::previous_match_metadata()
    : m_isNull(true)
{
}

xbox::services::tournaments::tournament_team_result
previous_match_metadata::result() const
{
    return m_result;
}

const utility::datetime&
previous_match_metadata::end_time() const
{
    return m_endTime;
}

const match_metadata& 
previous_match_metadata::match_details() const
{
    return m_matchDetails;
}

bool
previous_match_metadata::is_null() const
{
    return m_isNull;
}

xbox_live_result<previous_match_metadata>
previous_match_metadata::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<previous_match_metadata>();

    previous_match_metadata result;
    std::error_code errCode = xbox_live_error_code::no_error;
    
    auto matchDetailsResult = match_metadata::_Deserialize(json);
    if (matchDetailsResult.err())
    {
        errCode = matchDetailsResult.err();
    }
    result.m_matchDetails = matchDetailsResult.payload();
    result.m_endTime = utils::extract_json_time(json, _T("endTime"), errCode);

    auto resultJson = utils::extract_json_field(json, _T("result"), errCode, false);
    if (!resultJson.is_null())
    {
        auto tournamentTemResult = tournament_team_result::_Deserialize(resultJson);
        if (tournamentTemResult.err())
        {
            errCode = tournamentTemResult.err();
        }

        result.m_result = tournamentTemResult.payload();
    }

    result.m_isNull = false;

    return result;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END