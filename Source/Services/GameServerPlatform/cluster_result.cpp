// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/game_server_platform.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN

cluster_result::cluster_result() :
    m_fulfillmentState(game_server_fulfillment_state::unknown)
{
}

cluster_result::cluster_result(
    _In_ std::chrono::milliseconds pollInterval,
    _In_ game_server_fulfillment_state fulfillmentState,
    _In_ string_t hostName,
    _In_ string_t region,
    _In_ std::vector< game_server_port_mapping > portMappings,
    _In_ string_t secureDeviceAddress
    ) :
    m_pollInterval(std::move(pollInterval)),
    m_fulfillmentState(fulfillmentState),
    m_hostName(std::move(hostName)),
    m_region(std::move(region)),
    m_portMappings(std::move(portMappings)),
    m_secureDeviceAddress(std::move(secureDeviceAddress))
{
}

const std::chrono::milliseconds&
cluster_result::poll_interval() const
{
    return m_pollInterval;
}

game_server_fulfillment_state 
cluster_result::fulfillment_state() const
{
    return m_fulfillmentState;
}

const string_t& 
cluster_result::host_name() const
{
    return m_hostName;
}

const string_t& 
cluster_result::region() const
{
    return m_region;
}

const std::vector< game_server_port_mapping >& 
cluster_result::port_mappings() const
{
    return m_portMappings;
}

/// <summary>
/// The secure context which is the secure device address of the cluster
/// </summary>
const string_t& 
cluster_result::secure_device_address() const
{
    return m_secureDeviceAddress;
}

xbox_live_result<cluster_result>
cluster_result::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<cluster_result>();

    std::error_code errc = xbox_live_error_code::no_error;

    string_t pollIntervalMillisecondsStr = utils::extract_json_string(json, _T("pollIntervalMilliseconds"), errc, false, _T("1000"));

    uint32_t pollIntervalMilliseconds = utils::string_t_to_uint32(pollIntervalMillisecondsStr);

    auto clusterResult = cluster_result(
        std::chrono::milliseconds(pollIntervalMilliseconds),
        convert_string_to_fulfillment_state(utils::extract_json_string(json, _T("fulfillmentState"), errc)),
        utils::extract_json_string(json, _T("hostName"), errc),
        utils::extract_json_string(json, _T("region"), errc),
        utils::extract_json_vector<game_server_port_mapping>(game_server_port_mapping::_Deserialize, json, _T("portMappings"), errc, false),
        utils::extract_json_string(json, _T("secureContext"), errc, false, _T(""))
        );

    return xbox_live_result<cluster_result>(clusterResult, errc);
}

game_server_fulfillment_state
cluster_result::convert_string_to_fulfillment_state(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("fulfilled")) == 0)
    {
        return game_server_fulfillment_state::fulfilled;
    }
    else if (utils::str_icmp(value, _T("queued")) == 0)
    {
        return game_server_fulfillment_state::queued;
    }
    else if (utils::str_icmp(value, _T("aborted")) == 0)
    {
        return game_server_fulfillment_state::aborted;
    }

    return game_server_fulfillment_state::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
