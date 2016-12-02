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
#include "xsapi/simple_stats.h"
#include "simplified_stats_internal.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

xbox_live_result<std::shared_ptr<stat_value>>
stats_value_document::get_stat(
    _In_ const char_t* statName
    )
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
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
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
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
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
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
    )
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    statNameList.clear();
    for (auto& stat : m_statisticDocument)
    {
        statNameList.push_back(stat.first.c_str());
    }
}

void
stats_value_document::get_stat_contexts(
    _Inout_ std::vector<stat_context>& statisticContextList
    )
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    statisticContextList.clear();
    statisticContextList.insert(statisticContextList.end(), m_currentStatContexts.begin(), m_currentStatContexts.end());
}

void
stats_value_document::set_stat_contexts(
    _In_ const std::vector<stat_context>& statContextList
    )
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    m_svdEventList.push_back(svd_event(utils::std_vector_to_xsapi_vector<stat_context>(statContextList)));
}

void
stats_value_document::clear_stat_contexts()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    m_currentStatContexts.clear();
}

uint32_t
stats_value_document::client_version()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    return m_clientVersion;
}

uint32_t
stats_value_document::server_version()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    return m_serverVersion;
}

const xsapi_internal_string&
stats_value_document::client_id()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    return m_clientId;
}

uint32_t
stats_value_document::version()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    return m_version;
}

void
stats_value_document::increment_client_version_number()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    ++m_clientVersion;
}

bool stats_value_document::is_dirty()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    return m_isDirty;
}

void stats_value_document::clear_dirty_state()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    m_isDirty = false;
}

void
stats_value_document::do_work()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    for (auto& svdEvent : m_svdEventList)
    {
        switch (svdEvent.event_type())
        {
            case svd_event_type::stat_change:
            {
                auto& pendingStat = svdEvent.stat_info();
                switch (pendingStat.statDataType)
                {
                    case stat_data_type::number:
                        m_statisticDocument[pendingStat.statPendingName]->set_stat(pendingStat.statPendingData.numberType);
                        break;
                    case stat_data_type::string:
                        m_statisticDocument[pendingStat.statPendingName]->set_stat(pendingStat.statPendingData.stringType);
                        break;
                }
                break;
            }
            case svd_event_type::stat_context_change:
            {
                auto& contextList = svdEvent.context_list();
                m_currentStatContexts.clear();
                if (!contextList.empty())
                {
                    m_currentStatContexts.insert(m_currentStatContexts.end(), contextList.begin(), contextList.end());
                }

                if (m_fRequestFlush != nullptr)
                {
                    m_fRequestFlush();
                }
                break;
            }
        }
    }
}

void
stats_value_document::set_flush_function(
    _In_ const std::function<void()> flushFunction
    )
{
    m_fRequestFlush = flushFunction;
}

web::json::value
stats_value_document::serialize(
    _In_ bool shouldOverrideLock
    )
{
    if (!shouldOverrideLock)
    {
        std::lock_guard<std::mutex> lock(m_svdMutex.get());
        return serialize_impl();
    }

    return serialize_impl();
}

web::json::value
stats_value_document::serialize_impl() const
{
    web::json::value requestJSON;
    requestJSON[_T("ver")] = web::json::value::number(m_version);
    requestJSON[_T("timestamp")] = web::json::value(utility::datetime::utc_now().to_string(utility::datetime::date_format::ISO_8601));
    auto& envelopeField = requestJSON[_T("envelope")];
    envelopeField[_T("serverVersion")] = web::json::value::number(m_serverVersion);
    envelopeField[_T("clientVersion")] = web::json::value::number(m_clientVersion);
    envelopeField[_T("clientId")] = web::json::value::string(m_clientId.c_str());
    auto& statsField = requestJSON[_T("stats")];

    auto& contextualKeyField = statsField[_T("contextualKeys")];
    contextualKeyField = web::json::value::object();
    for (auto& context : m_currentStatContexts)
    {
        contextualKeyField[context.name()] = web::json::value::string(context.value());
    }

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
    returnObject.m_serverVersion = utils::extract_json_int(envelopeField, _T("serverVersion"), errc);
    returnObject.m_clientId = utils::extract_json_string(envelopeField, _T("clientId"), errc).c_str();

    auto statsField = utils::extract_json_field(data, _T("stats"), errc, false);
    auto currentContextArray = utils::extract_json_field(statsField, _T("contextualKeys"), errc, false).as_object();
    for (auto& context : currentContextArray)
    {
        stat_context statContext(context.first.c_str(), context.second.as_string().c_str());
        returnObject.m_currentStatContexts.push_back(statContext);
    }

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
    m_statContextList(std::move(statContextList)),
    m_svdEventType(svd_event_type::stat_context_change)
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