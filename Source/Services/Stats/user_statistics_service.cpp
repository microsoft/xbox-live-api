// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <cpprest/json.h>
#include "xsapi/user_statistics.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_statistics_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

user_statistics_service::user_statistics_service()
{
}

user_statistics_service::user_statistics_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_userStatisticsServiceImpl(std::make_shared<user_statistics_service_impl>(rtaService))
{
}

pplx::task<xbox_live_result<user_statistics_result>>
user_statistics_service::get_single_user_statistics(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& statisticName
    )
{
    std::vector<string_t> statistics;
    statistics.push_back(statisticName);
    return get_single_user_statistics(
        xboxUserId,
        serviceConfigurationId,
        statistics
        );
}

pplx::task<xbox_live_result<user_statistics_result>>
user_statistics_service::get_single_user_statistics(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const std::vector<string_t>& statisticNames
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(xboxUserId, user_statistics_result, "xboxUserId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, user_statistics_result, "serviceConfigurationId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(xboxUserId, user_statistics_result, "xboxUserId is empty");
    auto subPath = user_stats_sub_path(
        xboxUserId,
        serviceConfigurationId,
        statisticNames
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("userstats"), m_appConfig),
        subPath,
        xbox_live_api::get_single_user_statistics
        );
    httpCall->set_xbox_contract_version_header_value(_T("1"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([serviceConfigurationId](std::shared_ptr<http_call_response> response)
    {
        auto result = user_statistics_result::_Deserialize(response->response_body_json());
        auto& userStatisticsResult = result.payload();
        userStatisticsResult._Set_input_service_configuration_id(serviceConfigurationId);
        return utils::generate_xbox_live_result<user_statistics_result>(
            result, 
            response
            );
    });

    return utils::create_exception_free_task<user_statistics_result>(
        task
        );
}

pplx::task<xbox_live_result<std::vector<user_statistics_result>>>
user_statistics_service::get_multiple_user_statistics(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const string_t& serviceConfigurationId,
    _In_ std::vector<string_t>& statisticNames
    )
{
    std::vector<requested_statistics> statistics;
    statistics.push_back(requested_statistics(serviceConfigurationId, statisticNames));
    return get_multiple_user_statistics_for_multiple_service_configurations(
        xboxUserIds,
        statistics
        );
}

pplx::task<xbox_live_result<std::vector<user_statistics_result>>>
user_statistics_service::get_multiple_user_statistics_for_multiple_service_configurations(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<requested_statistics>& requestedServiceConfigurationStatisticsCollection
    )
{
    // Set request body to something like:
    //{    
    //    "requestedusers": 
    //    [
    //        "1234567890123460",
    //        "1234567890123234"
    //    ],
    //    "requestedscids": 
    //    [
    //        {
    //            "scid": "c402ff50-3e76-11e2-a25f-0800200c1212",
    //            "requestedstats": 
    //            [
    //                "Game4FirefightKills",
    //                "Game4FirefightHeadshots"
    //            ]
    //        },
    //        {
    //            "scid": "c402ff50-3e76-11e2-a25f-0800200c0343",
    //            "requestedstats": 
    //            [
    //                "OverallGameKills",
    //                "GameHeadshots"
    //            ]
    //        }
    //    ] 
    //}

    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserIds.empty(), std::vector<user_statistics_result>, "xboxUserIds are empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(requestedServiceConfigurationStatisticsCollection.empty(), std::vector<user_statistics_result>, "requestedServiceConfigurationStatisticsCollection are empty");

    web::json::value rootJson;
    rootJson[_T("requestedusers")] = utils::serialize_vector<string_t>(utils::json_string_serializer, xboxUserIds);

    //requestedscids
    web::json::value requestedscidsJson = web::json::value::array();
    uint32_t i = 0;
    for (const auto& request : requestedServiceConfigurationStatisticsCollection)
    {
        web::json::value requestedJson;
        requestedJson[_T("scid")] = web::json::value::string(request.service_configuration_id());

        web::json::value requestedstatsJson = web::json::value::array();
        uint32_t j = 0;
        for (const auto& stat : request.statistics())
        {
            requestedstatsJson[j++] = web::json::value::string(stat);
        }
        requestedJson[_T("requestedstats")] = requestedstatsJson;

        requestedscidsJson[i++] = requestedJson;
    }

    rootJson[_T("requestedscids")] = requestedscidsJson;

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("userstats"), m_appConfig),
        _T("/batch?operation=read"),
        xbox_live_api::get_multiple_user_statistics_for_multiple_service_configurations
        );
    httpCall->set_xbox_contract_version_header_value(_T("1"));
    httpCall->set_request_body(rootJson.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto userStatistics = utils::extract_xbox_live_result_json_vector<user_statistics_result>(
            user_statistics_result::_Deserialize,
            response->response_body_json(),
            _T("users"),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<user_statistics_result>>(
            userStatistics,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<user_statistics_result>>(
        task
        );
}

string_t 
user_statistics_service::user_stats_sub_path(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ std::vector<string_t> statNames
    )
{
    stringstream_t ss;
    ss << _T("/users/xuid(") << xboxUserId << _T(")/scids/") << serviceConfigurationId  << _T("/stats/");

    auto& last = statNames.back();
    for (const string_t& statName : statNames)
    {
        ss << statName;
        if (&statName != &last)
        {
            ss << _T(",");
        }
    }

    return ss.str();
}

function_context
user_statistics_service::add_statistic_changed_handler(
    _In_ std::function<void(statistic_change_event_args)> handler
    )
{
    return m_userStatisticsServiceImpl->add_statistic_changed_handler(
        std::move(handler)
        );
}

void
user_statistics_service::remove_statistic_changed_handler(
    _In_ function_context context
    )
{
    return m_userStatisticsServiceImpl->remove_statistic_changed_handler(
        context
        );
}

xbox_live_result<std::shared_ptr<statistic_change_subscription>>
user_statistics_service::subscribe_to_statistic_change(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& statisticName
    )
{
    return m_userStatisticsServiceImpl->subscribe_to_statistic_change(
        xboxUserId,
        serviceConfigurationId,
        statisticName
        );
}

xbox_live_result<void>
user_statistics_service::unsubscribe_from_statistic_change(
    _In_ std::shared_ptr<statistic_change_subscription> subscription
    )
{
    return m_userStatisticsServiceImpl->unsubscribe_from_statistic_change(subscription);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END