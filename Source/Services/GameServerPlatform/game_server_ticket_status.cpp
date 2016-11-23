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

game_server_ticket_status::game_server_ticket_status() : 
    m_titleId(0),
    m_status(game_server_host_status::unknown)
{
}

game_server_ticket_status::game_server_ticket_status(
    _In_ string_t ticketId,
    _In_ string_t clusterId,
    _In_ uint32_t titleId,
    _In_ string_t hostName,
    _In_ game_server_host_status status,
    _In_ string_t description,
    _In_ string_t secureContext,
    _In_ std::vector< game_server_port_mapping > portMappings,
    _In_ string_t gameHostId,
    _In_ string_t region
    ) :
    m_ticketId(std::move(ticketId)),
    m_clusterId(std::move(clusterId)),
    m_titleId(titleId),
    m_hostName(std::move(hostName)),
    m_status(status),
    m_description(std::move(description)),
    m_secureContext(std::move(secureContext)),
    m_portMappings(std::move(portMappings)),
    m_gameHostId(std::move(gameHostId)),
    m_region(std::move(region))
{
}

const string_t& 
game_server_ticket_status::ticket_id() const
{
    return m_ticketId;
}

const string_t& 
game_server_ticket_status::cluster_id() const
{
    return m_clusterId;
}

uint32_t 
game_server_ticket_status::title_id() const
{
    return m_titleId;
}

const string_t& 
game_server_ticket_status::host_name() const
{
    return m_hostName;
}

game_server_host_status 
game_server_ticket_status::status() const
{
    return m_status;
}

const string_t& 
game_server_ticket_status::description() const
{
    return m_description;
}

const string_t& 
game_server_ticket_status::secure_context() const
{
    return m_secureContext;
}

std::vector< game_server_port_mapping > 
game_server_ticket_status::port_mappings() const
{
    return m_portMappings;
}

const string_t& 
game_server_ticket_status::game_host_id() const
{
    return m_gameHostId;
}

const string_t& 
game_server_ticket_status::region() const
{
    return m_region;
}

xbox_live_result<game_server_ticket_status>
game_server_ticket_status::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<game_server_ticket_status>();

    std::error_code errc;
    auto portMappings = utils::extract_json_vector<game_server_port_mapping>(game_server_port_mapping::_Deserialize, json, _T("portMappings"), errc, false);

    auto result = game_server_ticket_status(
        utils::extract_json_string(json, _T("ticketId"), errc),
        utils::extract_json_string(json, _T("clusterId"), errc),
        utils::extract_json_int(json, _T("titleId"), errc),
        utils::extract_json_string(json, _T("hostName"), errc),
        convert_string_to_host_status(utils::extract_json_string(json, _T("status"), errc)),
        utils::extract_json_string(json, _T("description"), errc),
        utils::extract_json_string(json, _T("secureContext"), errc),
        std::move(portMappings),
        utils::extract_json_string(json, _T("gameHostId"), errc),
        utils::extract_json_string(json, _T("region"), errc)
        );

    return xbox_live_result<game_server_ticket_status>(result, errc);
}

game_server_host_status
game_server_ticket_status::convert_string_to_host_status(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("active")) == 0)
    {
        return game_server_host_status::active;
    }
    else if (utils::str_icmp(value, _T("queued")) == 0)
    {
        return game_server_host_status::queued;
    }
    else if (utils::str_icmp(value, _T("aborted")) == 0)
    {
        return game_server_host_status::aborted;
    }
    else if (utils::str_icmp(value, _T("error")) == 0)
    {
        return game_server_host_status::error;
    }

    return game_server_host_status::unknown;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
