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

service_configuration_statistic::service_configuration_statistic()
{
}

service_configuration_statistic::service_configuration_statistic(
    _In_ string_t serviceConfigurationId,
    _In_ std::vector<statistic> stats
    ) :
    m_serviceConfigurationId(serviceConfigurationId),
    m_stats(stats)
{
}

const string_t&
service_configuration_statistic::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

void
service_configuration_statistic::_Set_input_service_configuration_id(string_t serviceConfigId)
{
    m_serviceConfigurationId = std::move(serviceConfigId);
}

const std::vector<statistic>&
service_configuration_statistic::statistics() const
{
    return m_stats;
}

xbox_live_result<service_configuration_statistic>
service_configuration_statistic::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<service_configuration_statistic>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto result = service_configuration_statistic(
        utils::extract_json_string(json, _T("scid"), errc, true),
        utils::extract_json_vector<statistic>(statistic::_Deserialize, json, _T("stats"), errc, true)
        );

    return xbox_live_result<service_configuration_statistic>(
        result,
        errc,
        ""
        );
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END