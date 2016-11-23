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
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_search_broadcast::contextual_search_broadcast() :
    m_viewers(0)
{
}

contextual_search_broadcast::contextual_search_broadcast(
    _In_ string_t xboxUserId,
    _In_ string_t provider,
    _In_ string_t broadcasterIdFromProvider,
    _In_ uint64_t viewers,
    _In_ utility::datetime startedDate,
    _In_ std::unordered_map<string_t, string_t> currentStats
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_provider(std::move(provider)),
    m_broadcasterIdFromProvider(std::move(broadcasterIdFromProvider)),
    m_viewers(viewers),
    m_startedDate(std::move(startedDate)),
    m_currentStats(std::move(currentStats))
{
}

const string_t& contextual_search_broadcast::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t& contextual_search_broadcast::provider() const
{
    return m_provider;
}

const string_t& contextual_search_broadcast::broadcaster_id_from_provider() const
{
    return m_broadcasterIdFromProvider;
}

uint64_t contextual_search_broadcast::viewers() const
{
    return m_viewers;
}

const utility::datetime& contextual_search_broadcast::started_date() const
{
    return m_startedDate;
}

const std::unordered_map<string_t, string_t>& contextual_search_broadcast::current_stats() const
{
    return m_currentStats;
}

xbox_live_result<contextual_search_broadcast> 
contextual_search_broadcast::_Deserialize(_In_ const web::json::value& inputJson)
{
    if (inputJson.is_null()) return xbox_live_result<contextual_search_broadcast>();

    std::error_code errc = xbox_live_error_code::no_error;

    // Pull out all the key/value pairs in the "stats" field and add them to currentStats object
    web::json::object statsField(utils::extract_json_field(inputJson, _T("stats"), errc, false).as_object());
    std::unordered_map<string_t, string_t> currentStats;
    for (const auto& stat : statsField)
    {
        if(!stat.first.empty() && stat.second.is_string() && !stat.second.as_string().empty())
        {
            currentStats[stat.first] = stat.second.as_string();
        }
    }

    contextual_search_broadcast broadcastsResult(
        utils::extract_json_string(inputJson, _T("xuid"), errc, false),
        utils::extract_json_string(inputJson, _T("provider"), errc, false),
        utils::extract_json_string(inputJson, _T("id"), errc, false),
        utils::extract_json_int(inputJson, _T("viewers"), errc, false),
        utils::extract_json_time(inputJson, _T("started"), errc, false),
        currentStats
        );

    return xbox_live_result<contextual_search_broadcast>(broadcastsResult, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END
