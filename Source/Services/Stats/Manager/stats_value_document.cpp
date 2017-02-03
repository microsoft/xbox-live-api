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
#include "xsapi/stats_manager.h"
#include "stats_manager_internal.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN


stats_value_document::stats_value_document() :
    m_isDirty(false),
    m_version(0),
    m_clientVersion(0),
    m_serverVersion(0)
{
}

xbox_live_result<std::shared_ptr<stat_value>>
stats_value_document::get_stat(
    _In_ const char_t* statName
    ) const
{
    auto statLocIter = m_statisticDocument.find(statName);
    if (statLocIter == m_statisticDocument.end())
    {
        return xbox_live_result<std::shared_ptr<stat_value>>(xbox_live_error_code::invalid_argument, "Stat not found in document");
    }

    return statLocIter->second;
}

xbox_live_result<void>
stats_value_document::set_stat(
    _In_ const char_t* statName,
    _In_ double statValue
    )
{
    m_isDirty = true;
    stat_pending_state statPendingState;
    utils::char_t_copy(statPendingState.statPendingName, ARRAYSIZE(statPendingState.statPendingName), statName);
    statPendingState.statDataType = stat_data_type::number;
    statPendingState.statPendingData.numberType = statValue;

    m_svdEventList.push_back(svd_event(statPendingState));
    return xbox_live_result<void>();
}

xbox_live_result<void>
stats_value_document::set_stat(
    _In_ const char_t* statName,
    _In_ const char_t* statValue
    )
{
    m_isDirty = true;
    stat_pending_state statPendingState;
    utils::char_t_copy(statPendingState.statPendingName, ARRAYSIZE(statPendingState.statPendingName), statName);
    statPendingState.statDataType = stat_data_type::string;
    utils::char_t_copy(statPendingState.statPendingData.stringType, ARRAYSIZE(statPendingState.statPendingData.stringType), statValue);
    m_svdEventList.push_back(svd_event(statPendingState));
    return xbox_live_result<void>();
}

void
stats_value_document::get_stat_names(
    _Inout_ std::vector<string_t>& statNameList
    ) const
{
    statNameList.clear();
    for (auto& stat : m_statisticDocument)
    {
        statNameList.push_back(stat.first.c_str());
    }
}

uint32_t
stats_value_document::client_version() const
{
    return m_clientVersion;
}

uint32_t
stats_value_document::server_version() const
{
    return m_serverVersion;
}

const xsapi_internal_string&
stats_value_document::client_id() const
{
    return m_clientId;
}

uint32_t
stats_value_document::version() const
{
    return m_version;
}

void
stats_value_document::increment_client_version_number()
{
    ++m_clientVersion;
}

bool stats_value_document::is_dirty() const
{
    return m_isDirty;
}

void stats_value_document::clear_dirty_state()
{
    m_isDirty = false;
}

void
stats_value_document::do_work()
{
    for (auto& svdEvent : m_svdEventList)
    {
        switch (svdEvent.event_type())
        {
            case svd_event_type::stat_change:
            {
                auto& pendingStat = svdEvent.stat_info();
                auto statIter = m_statisticDocument.find(pendingStat.statPendingName);

                if (statIter == m_statisticDocument.end())
                {
                    m_statisticDocument[pendingStat.statPendingName] = std::make_shared<stat_value>();  // this will need changed to be more like social manager
                }

                switch (pendingStat.statDataType)
                {
                    case stat_data_type::number:
                        m_statisticDocument[pendingStat.statPendingName]->set_stat(
                            pendingStat.statPendingData.numberType
                            );
                        break;

                    case stat_data_type::string:
                        m_statisticDocument[pendingStat.statPendingName]->set_stat(pendingStat.statPendingData.stringType);
                        break;
                }
                break;
            }
        }
    }

    m_svdEventList.clear();
}

void
stats_value_document::set_flush_function(
    _In_ const std::function<void()> flushFunction
    )
{
    m_fRequestFlush = flushFunction;
}

web::json::value
stats_value_document::serialize() const
{
    web::json::value requestJSON;
    requestJSON[_T("ver")] = web::json::value::number(m_version);
#if TV_API
    requestJSON[_T("timestamp")] = web::json::value(utils::datetime_to_string(utility::datetime::utc_now()));
#else
    requestJSON[_T("timestamp")] = web::json::value(utility::datetime::utc_now().to_string(utility::datetime::ISO_8601));
#endif
    auto& envelopeField = requestJSON[_T("envelope")];
    envelopeField[_T("serverVersion")] = web::json::value::number(m_serverVersion);
    envelopeField[_T("clientVersion")] = web::json::value::number(m_clientVersion);
    envelopeField[_T("clientId")] = web::json::value::string(m_clientId.c_str());
    auto& statsField = requestJSON[_T("stats")];

    auto& contextualKeyField = statsField[_T("tags")];
    contextualKeyField = web::json::value::object();

    auto& titleField = statsField[_T("title")];
    titleField = web::json::value::object();
    for (auto& stat : m_statisticDocument)
    {
        titleField[stat.first.c_str()] = stat.second->serialize();
    }

    return requestJSON;
}

xbox_live_result<stats_value_document>
stats_value_document::_Deserialize(
    _In_ const web::json::value& data
    )
{
    stats_value_document returnObject;
    if (data.is_null()) { return returnObject; }

    std::error_code errc;

    returnObject.m_version = utils::extract_json_int(data, _T("ver"), errc, false);

    auto envelopeField = utils::extract_json_field(data, _T("envelope"), errc, false);
    returnObject.m_clientVersion = utils::extract_json_int(envelopeField, _T("clientVersion"), errc);
    ++returnObject.m_clientVersion; // increment the version so first write is always new version
    returnObject.m_serverVersion = utils::extract_json_int(envelopeField, _T("serverVersion"), errc);
    returnObject.m_clientId = utils::extract_json_string(envelopeField, _T("clientId"), errc).c_str();

    auto statsField = utils::extract_json_field(data, _T("stats"), errc, false);
    auto titleField = utils::extract_json_field(statsField, _T("title"), errc, false);
    auto statsArray = titleField.as_object();
    for (auto& stat : statsArray)
    {
        returnObject.m_statisticDocument[stat.first.c_str()] = std::make_shared<stat_value>(stat_value::_Deserialize(stat.second).payload());
    }

    return returnObject;
}

svd_event::svd_event(
    _In_ stat_pending_state statPendingState
    ) :
    m_statPendingState(std::move(statPendingState)),
    m_svdEventType(svd_event_type::stat_change)
{
}

svd_event::svd_event(
    _In_ xsapi_internal_vector(stat_context) statContextList
    ) :
    m_statContextList(std::move(statContextList))
{
}

svd_event_type
svd_event::event_type() const
{
    return m_svdEventType;
}

const stat_pending_state&
svd_event::stat_info() const
{
    return m_statPendingState;
}

const xsapi_internal_vector(stat_context)&
svd_event::context_list() const
{
    return m_statContextList;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END