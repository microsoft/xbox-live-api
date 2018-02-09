// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/stats_manager.h"
#include "stats_manager_internal.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

stats_value_document::stats_value_document() :
    m_isDirty(false),
    m_revision(0),
    m_previousRevision(0),
    m_state(svd_state::not_loaded)
{
    m_svdEventList.reserve(100);
    m_statisticDocument.reserve(20);
}

xbox_live_result<stat_value>
stats_value_document::get_stat(
    _In_ const char_t* statName
    ) const
{
    auto statLocIter = m_statisticDocument.find(statName);
    if (statLocIter == m_statisticDocument.end())
    {
        return xbox_live_result<stat_value>(xbox_live_error_code::invalid_argument, "Stat not found in document");
    }

    return statLocIter->second;
}

xbox_live_result<void>
stats_value_document::set_stat(
    _In_ const char_t* statName,
    _In_ double statValue
    )
{
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

xbox_live_result<void>
stats_value_document::delete_stat(
    _In_ const char_t* statName
    )
{
    stat_pending_state statPendingState;
    utils::char_t_copy(statPendingState.statPendingName, ARRAYSIZE(statPendingState.statPendingName), statName);
    m_svdEventList.push_back(svd_event(statPendingState, svd_event_type::stat_delete));
    return xbox_live_result<void>();
}

void
stats_value_document::set_revision_from_clock()
{
    uint64_t dateTime = utility::datetime::utc_now().to_interval(); // eg. 131472330440000000
    const uint64_t dateTimeFromJan1st2015 = 130645440000000000;
    if (dateTime < dateTimeFromJan1st2015)
    {
        m_revision = 1; // Clock is wrong and is not yet sync'd with internet time, so just setting the revision to 1
    }
    else
    {
        uint64_t dateTimeSince2015 = dateTime - dateTimeFromJan1st2015; // eg. 826888900000000
        uint64_t dateTimeTrimmed = dateTimeSince2015 >> 16; // divide by 2^16 to get it to sub second range.  eg. 12617323303
        m_revision = dateTimeTrimmed;
    }
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
    if (m_state != svd_state::not_loaded)
    {
        for (auto& svdEvent : m_svdEventList)
        {
            auto& pendingStat = svdEvent.stat_info();
            switch (svdEvent.event_type())
            {
                case svd_event_type::stat_change:
                {
                    auto statIter = m_statisticDocument.find(pendingStat.statPendingName);

                    switch (pendingStat.statDataType)
                    {
                        case stat_data_type::number:
                            m_statisticDocument[pendingStat.statPendingName].set_stat(
                                pendingStat.statPendingData.numberType
                                );
                            break;

                        case stat_data_type::string:
                            m_statisticDocument[pendingStat.statPendingName].set_stat(pendingStat.statPendingData.stringType);
                            break;
                    }

                    m_isDirty = true;
                    break;
                }

                case svd_event_type::stat_delete:
                {
                    m_statisticDocument.erase(pendingStat.statPendingName);
                    break;
                }
            }
        }
    }

    m_svdEventList.clear();
}

void
stats_value_document::set_svd_state(
    _In_ svd_state svdState
    )
{
    m_state = svdState;
}

svd_state
stats_value_document::get_svd_state() const
{
    return m_state;
}

void
stats_value_document::merge_stat_value_documents(
    _In_ const stats_value_document& sdvFromService
    )
{
    if( m_state == svd_state::not_loaded )
    {
        // Local values are override service values
        // Only merge in any undefined stats into the stats doc
        for (auto& statFromService : sdvFromService.m_statisticDocument)
        {
            auto statIter = m_statisticDocument.find(statFromService.first);
            if (statIter == m_statisticDocument.end())
            {
                m_statisticDocument[statFromService.first] = statFromService.second;
            }
        }
        m_previousRevision = sdvFromService.m_previousRevision;
    }

    m_state = svd_state::loaded;
}

web::json::value
stats_value_document::serialize()
{
    web::json::value requestJSON;
    requestJSON[_T("$schema")] = web::json::value::string(_T("http://stats.xboxlive.com/2017-1/schema#"));
    requestJSON[_T("revision")] = web::json::value::number(m_revision);
    requestJSON[_T("previousRevision")] = web::json::value::number(m_previousRevision);
#if TV_API
    requestJSON[_T("timestamp")] = web::json::value(utils::datetime_to_string(utility::datetime::utc_now()));
#else
    requestJSON[_T("timestamp")] = web::json::value(utility::datetime::utc_now().to_string(utility::datetime::ISO_8601));
#endif
    auto& statsField = requestJSON[_T("stats")];

    auto& titleField = statsField[_T("title")];
    titleField = web::json::value::object();
    for (auto& stat : m_statisticDocument)
    {
        titleField[stat.first.c_str()] = stat.second.serialize();
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

    returnObject.m_state = svd_state::loaded;
    returnObject.m_previousRevision = utils::extract_json_uint52(data, "revision", errc, false);

    auto statsField = utils::extract_json_field(data, _T("stats"), errc, false);
    auto titleField = utils::extract_json_field(statsField, _T("title"), errc, false);
    auto statsArray = titleField.as_object();
    for (auto& stat : statsArray)
    {
        returnObject.m_statisticDocument[stat.first.c_str()] = stat_value::_Deserialize(stat.second).payload();
        returnObject.m_statisticDocument[stat.first.c_str()].set_name(stat.first);
    }

    return returnObject;
}

svd_event::svd_event(
    _In_ stat_pending_state statPendingState,
    _In_ svd_event_type eventType
    ) :
    m_statPendingState(std::move(statPendingState)),
    m_svdEventType(eventType)
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

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END