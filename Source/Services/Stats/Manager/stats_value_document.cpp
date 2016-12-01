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
    m_statPendingState.push_back(statPendingState);
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
    m_statPendingState.push_back(statPendingState);
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
    m_currentStatContexts.clear();
    m_currentStatContexts.insert(m_currentStatContexts.end(), statContextList.begin(), statContextList.end());
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

void
stats_value_document::do_work()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
    for (auto& pendingStat : m_statPendingState)
    {
        switch (pendingStat.statDataType)
        {
            case stat_data_type::number:
                m_statisticDocument[pendingStat.statPendingName]->set_stat(pendingStat.statPendingData.numberType);
                break;
            case stat_data_type::string:
                m_statisticDocument[pendingStat.statPendingName]->set_stat(pendingStat.statPendingData.stringType);
                break;
        }
    }
}

web::json::value
stats_value_document::serialize()
{
    std::lock_guard<std::mutex> lock(m_svdMutex.get());
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

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END