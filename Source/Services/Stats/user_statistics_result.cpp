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
#include "xsapi/user_statistics.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

user_statistics_result::user_statistics_result()
{
}

// class user_statistics_result
user_statistics_result::user_statistics_result(
    _In_ string_t xboxUserId,
    _In_ std::vector<service_configuration_statistic> serviceConfigStatistics
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_serviceConfigStatistics(std::move(serviceConfigStatistics))
{}

const string_t&
user_statistics_result::xbox_user_id() const
{
    return m_xboxUserId;
}

const std::vector<service_configuration_statistic>&
user_statistics_result::service_configuration_statistics() const
{
    return m_serviceConfigStatistics;
}

void
user_statistics_result::_Set_input_service_configuration_id(string_t serviceConfigId)
{
    for (auto& stat : m_serviceConfigStatistics)
    {
        stat._Set_input_service_configuration_id(serviceConfigId);
    }
}

xbox_live_result<user_statistics_result>
user_statistics_result::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<user_statistics_result>(xbox_live_error_code::json_error, "Json is null or empty");

    std::error_code errc = xbox_live_error_code::no_error;
    std::vector<service_configuration_statistic> serviceConfigStatisticses =
        utils::extract_json_vector<service_configuration_statistic>(service_configuration_statistic::_Deserialize, json, _T("scids"), errc, false);
    if (serviceConfigStatisticses.size() == 0)
    {
        std::vector<statistic> statistics = utils::extract_json_vector<statistic>(statistic::_Deserialize, json, _T("stats"), errc, false);
        service_configuration_statistic serviceConfigStatistics(string_t(), statistics);
        serviceConfigStatisticses.push_back(serviceConfigStatistics);
    }

    auto result = user_statistics_result(
        utils::extract_json_string(json, _T("xuid"), errc, true),
        serviceConfigStatisticses
        );
    
    return xbox_live_result<user_statistics_result>(
        result, 
        errc,
        ""
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END