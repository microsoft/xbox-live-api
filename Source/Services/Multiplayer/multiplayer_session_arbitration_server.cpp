// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"

using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_arbitration_server::multiplayer_session_arbitration_server() :
    m_resultState(tournament_arbitration_state::no_results),
    m_resultSource(tournament_game_result_source::none),
    m_resultConfidenceLevel(0),
    m_isNull(true)
{
}

utility::datetime
multiplayer_session_arbitration_server::arbitration_start_time() const
{
    return m_arbitrationStartTime;
}

tournament_arbitration_state multiplayer_session_arbitration_server::result_state() const
{
    return m_resultState;
}

tournament_game_result_source multiplayer_session_arbitration_server::result_source() const
{
    return m_resultSource;
}

uint32_t multiplayer_session_arbitration_server::result_confidence_level() const
{
    return m_resultConfidenceLevel;
}

const std::unordered_map<string_t, tournament_team_result>& multiplayer_session_arbitration_server::results() const
{
    return m_results;
}

bool
multiplayer_session_arbitration_server::_Is_null() const
{
    return m_isNull;
}

bool 
multiplayer_session_arbitration_server::_Is_same(const multiplayer_session_arbitration_server& other) const
{
    std::unordered_map<string_t, tournament_team_result> otherResults = other.results();
    if (m_resultConfidenceLevel != other.result_confidence_level() ||
        m_resultState != other.result_state() ||
        m_resultSource != other.result_source() ||
        m_results.size() != otherResults.size())
    {
        return false;
    }
    
    for (const auto& result : m_results)
    {
        const auto& otherResult = otherResults.find(result.first);
        if (otherResult != otherResults.end() ||
            otherResult->second.state() != result.second.state() ||
            otherResult->second.ranking() != result.second.ranking())
        {
            return false;
        }
    }

    return true;
}

tournament_arbitration_state 
multiplayer_session_arbitration_server::_Convert_string_to_arbitration_state(
    _In_ const string_t& value
    )
{
    if (value == _T("completed"))
    {
        return tournament_arbitration_state::completed;
    }
    else if (value == _T("canceled"))
    {
        return tournament_arbitration_state::canceled;
    }
    else if (value == _T("partialresults"))
    {
        return tournament_arbitration_state::partial_results;
    }

    return tournament_arbitration_state::no_results;
}

xbox_live_result<multiplayer_session_arbitration_server>
multiplayer_session_arbitration_server::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_session_arbitration_server returnObject;
    if (json.is_null()) return xbox_live_result<multiplayer_session_arbitration_server>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;

    auto constantsJson = utils::extract_json_field(json, _T("constants"), errc, true);
    auto systemConstantsJson = utils::extract_json_field(constantsJson, _T("system"), errc, true);
    returnObject.m_arbitrationStartTime = utils::extract_json_time(systemConstantsJson, _T("startTime"), errc);

    auto propertiesJson = utils::extract_json_field(json, _T("properties"), errc, true);
    auto systemPropertiesJson = utils::extract_json_field(propertiesJson, _T("system"), errc, true);

    returnObject.m_resultState = _Convert_string_to_arbitration_state(utils::extract_json_string(systemPropertiesJson, _T("resultState"), errc, false));
    returnObject.m_resultSource = multiplayer_service::_Convert_string_to_game_result_source(utils::extract_json_string(systemPropertiesJson, _T("resultSource"), errc, false));
    returnObject.m_resultConfidenceLevel = static_cast<uint32_t>(utils::extract_json_uint52(systemPropertiesJson, "resultConfidenceLevel", errc, false));

    auto resultsJson = utils::extract_json_field(systemPropertiesJson, _T("results"), errc, false);
    if (!resultsJson.is_null())
    {
        web::json::object resultsObj = resultsJson.as_object();
        for (const auto& result : resultsObj)
        {
            const string_t& team = result.first;
            auto tournamentTemResult = tournament_team_result::_Deserialize(result.second);
            if (tournamentTemResult.err())
            {
                errc = tournamentTemResult.err();
            }
            returnObject.m_results[team] = tournamentTemResult.payload();
        }
    }

    returnObject.m_isNull = false;
    return xbox_live_result<multiplayer_session_arbitration_server>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END