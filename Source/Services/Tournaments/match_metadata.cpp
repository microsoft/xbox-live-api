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
match_metadata::label() const
{
    return m_label;
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
    
    web::json::value labelJson = utils::extract_json_field(json, _T("label"), false);
    if (!labelJson.is_null())
    {
        string_t formatMoniker = utils::extract_json_string(labelJson, _T("formatMoniker"), errCode);
        auto params = utils::extract_json_vector<string_t>(utils::json_string_extractor, labelJson, _T("params"), errCode, false);
        int index = 0;
        for (const auto& param : params)
        {
            stringstream_t ss;
            ss << _T("{");
            ss << index;
            ss << _T("}");
            formatMoniker = utils::replace_sub_string(formatMoniker, ss.str(), param);
            index++;
        }

        result.m_label = formatMoniker;
    }

    result.m_isBye = utils::extract_json_bool(json, _T("bye"), errCode);
    result.m_startTime = utils::extract_json_time(json, _T("startTime"), errCode);
    result.m_opposingTeamIds = utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("opposingTeamIds"), errCode, true);

    return result;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END