// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/game_server_platform.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN

game_server_port_mapping::game_server_port_mapping() :
    m_internalPortNumber(0),
    m_externalPortNumber(0)
{
}

game_server_port_mapping::game_server_port_mapping(
    _In_ string_t portName,
    _In_ uint32_t internalPortNumber,
    _In_ uint32_t externalPortNumber
    ) :
    m_portName(std::move(portName)),
    m_internalPortNumber(internalPortNumber),
    m_externalPortNumber(externalPortNumber)
{
}

const string_t&
game_server_port_mapping::port_name() const
{
    return m_portName;
}

uint32_t
game_server_port_mapping::internal_port_number() const
{
    return m_internalPortNumber;
}

uint32_t
game_server_port_mapping::external_port_number() const
{
    return m_externalPortNumber;
}

xbox_live_result<game_server_port_mapping>
game_server_port_mapping::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<game_server_port_mapping>();

    uint32_t externalPort = 0;
    uint32_t internalPort = 0;

    web::json::value relatedInfoObject = utils::extract_json_field(json, _T("Value"), false);
    if (!relatedInfoObject.is_null())
    {
        externalPort = utils::extract_json_int(relatedInfoObject, _T("external"), false, 0);
        internalPort = utils::extract_json_int(relatedInfoObject, _T("internal"), false, 0);
    }

    return game_server_port_mapping(
        utils::extract_json_string(json, _T("Key"), true),
        internalPort,
        externalPort
        );
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
