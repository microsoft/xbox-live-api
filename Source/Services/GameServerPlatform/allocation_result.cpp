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
#include "xsapi/game_server_platform.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN

allocation_result::allocation_result()
{
}

allocation_result::allocation_result(
    _In_ game_server_fulfillment_state fulfillmentState,
    _In_ string_t hostName,
    _In_ string_t sessionHostId,
    _In_ string_t region,
    _In_ std::vector< game_server_port_mapping > portMappings,
    _In_ string_t secureDeviceAddress
    ) :
    m_fulfillmentState(std::move(fulfillmentState)),
    m_hostName(std::move(hostName)),
    m_sessionHostId(std::move(sessionHostId)),
    m_region(std::move(region)),
    m_portMappings(std::move(portMappings)),
    m_secureDeviceAddress(std::move(secureDeviceAddress))
{
}

game_server_fulfillment_state
allocation_result::fulfillment_state() const
{
    return m_fulfillmentState;
}

const string_t&
allocation_result::host_name() const
{
    return m_hostName;
}

const string_t& 
allocation_result::session_host_id() const
{
    return m_sessionHostId;
}

const string_t& 
allocation_result::region() const
{
    return m_region;
}

const std::vector< game_server_port_mapping >& 
allocation_result::port_mappings() const
{
    return m_portMappings;
}

/// <summary>
/// The secure context which is the secure device address of the cluster
/// </summary>
const string_t& 
allocation_result::secure_device_address() const
{
    return m_secureDeviceAddress;
}

xbox_live_result<allocation_result>
allocation_result::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<allocation_result>();

    std::error_code errc = xbox_live_error_code::no_error;

    //{ 
    //    "hostName": "1234", 
    //    "portMappings": 
    //    [ 
    //        { 
    //            "Key": "1234", 
    //            "Value": 
    //            { 
    //                "internal": 10100, 
    //                "external": 10103 
    //            } 
    //        }
    //    ], 
    //    "region": "WestUS", 
    //    "secureContext": "1234" 
    //    "sessionHostId": "345789", 
    //    "status": "2345"
    //} 

    auto clusterResult = allocation_result(
        convert_string_to_fulfillment_state(utils::extract_json_string(json, _T("fulfillmentState"), errc)),
        utils::extract_json_string(json, _T("hostName"), errc),
        utils::extract_json_string(json, _T("sessionHostId"), errc),
        utils::extract_json_string(json, _T("region"), errc),
        utils::extract_json_vector<game_server_port_mapping>(game_server_port_mapping::_Deserialize, json, _T("portMappings"), errc, false),
        utils::extract_json_string(json, _T("secureContext"), errc)
        );

    return xbox_live_result<allocation_result>(clusterResult, errc);
}

game_server_fulfillment_state
allocation_result::convert_string_to_fulfillment_state(
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
    else if (value.empty())
    {
        return game_server_fulfillment_state::fulfilled;
    }

    return game_server_fulfillment_state::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
