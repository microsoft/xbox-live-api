// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "utils.h"
#include "multiplayer_internal.h"

using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_member_request::multiplayer_session_member_request():
    m_writeRequest(false),
    m_writeConstants(false),
    m_writeIsActive(false),
    m_isActive(false),
    m_writeSecureDeviceAddressBase64(false),
    m_addInitializePropertyToRequest(false),
    m_initializeRequested(false),
    m_writeMemberMeasurementsJson(false),
    m_writeMemberServerMeasurementsJson(false),
    m_writeMembersInGroup(false),
    m_writeSubscribedChangeType(false),
    m_changeTypes(multiplayer_session_change_types::none)
{
}

multiplayer_session_member_request::multiplayer_session_member_request(
    _In_ bool writeRequest,
    _In_ string_t memberId,
    _In_ string_t xboxUserId,
    _In_ web::json::value memberConstants,
    _In_ bool addInitializePropertyToRequest,
    _In_ bool initializeRequested
    ) :
    m_writeRequest(writeRequest),
    m_memberId(std::move(memberId)),
    m_xboxUserId(std::move(xboxUserId)),
    m_customConstants(std::move(memberConstants)),
    m_addInitializePropertyToRequest(addInitializePropertyToRequest),
    m_initializeRequested(initializeRequested),
    m_writeConstants(false),
    m_writeIsActive(false),
    m_isActive(false),
    m_writeSecureDeviceAddressBase64(false),
    m_writeMemberMeasurementsJson(false),
    m_writeMemberServerMeasurementsJson(false),
    m_writeMembersInGroup(false),
    m_writeSubscribedChangeType(false),
    m_changeTypes(multiplayer_session_change_types::none)
{
    m_writeConstants = !m_customConstants.is_null();
}

bool 
multiplayer_session_member_request::write_request() const
{
    return m_writeRequest;
}

bool 
multiplayer_session_member_request::write_constants() const
{
    return m_writeConstants;
}

const string_t& 
multiplayer_session_member_request::member_id() const
{
    return m_memberId;
}

const web::json::value&
multiplayer_session_member_request::custom_constants() const
{
    return m_customConstants;
}

const web::json::value&
multiplayer_session_member_request::custom_properties() const
{
    return m_customProperties;
}

void
multiplayer_session_member_request::set_custom_properties(
    _In_ web::json::value customProperties
    )
{
    m_customProperties = std::move(customProperties);
}

std::error_code
multiplayer_session_member_request::set_custom_properties_property(
    _In_ string_t name,
    _In_ web::json::value customProperty
    )
{
    if (name.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }

    m_customProperties[name.c_str()] = customProperty;

    return xbox_live_error_code::no_error;
}

const string_t& 
multiplayer_session_member_request::xbox_user_id() const
{
    return m_xboxUserId;
}

bool
multiplayer_session_member_request::is_active() const
{
    return m_isActive;
}

void
multiplayer_session_member_request::set_is_active(
    _In_ bool isActive
    )
{
    m_isActive = isActive;
}

bool
multiplayer_session_member_request::write_is_active() const
{
    return m_writeIsActive;
}

void 
multiplayer_session_member_request::set_write_is_active(
    _In_ bool writeIsActive
    )
{
    m_writeIsActive = writeIsActive;
}

const string_t&
multiplayer_session_member_request::service_device_address_base64() const
{
    return m_serviceDeviceAddressBase64;
}

void
multiplayer_session_member_request::set_secure_device_address_base64(
    _In_ string_t deviceAddress
    )
{
    m_serviceDeviceAddressBase64 = std::move(deviceAddress);
}

bool 
multiplayer_session_member_request::write_secure_device_address_base64() const
{
    return m_writeSecureDeviceAddressBase64;
}

void
multiplayer_session_member_request::set_write_secure_device_address_base64(
    _In_ bool shouldWrite
    )
{
    m_writeSecureDeviceAddressBase64 = shouldWrite;
}

void
multiplayer_session_member_request::set_role_info(
    _In_ const std::unordered_map<string_t, string_t>& roles
    )
{
    m_roles = roles;
}

bool 
multiplayer_session_member_request::does_initialized_requested_exist() const
{
    return m_addInitializePropertyToRequest;
}

bool 
multiplayer_session_member_request::initialize_requested() const
{
    return m_initializeRequested;
}

bool 
multiplayer_session_member_request::write_member_measurements_json() const
{
    return m_writeMemberMeasurementsJson;
}

void
multiplayer_session_member_request::set_write_member_measurements_json(
    _In_ bool shouldWrite
    )
{
    m_writeMemberMeasurementsJson = shouldWrite;
}

void
multiplayer_session_member_request::set_member_measurements_json(
    _In_ web::json::value memberMeasurements
    )
{
    m_memberMeasurementsJson = std::move(memberMeasurements);
}

const web::json::value&
multiplayer_session_member_request::member_measurements_json() const
{
    return m_memberMeasurementsJson;
}

const web::json::value&
multiplayer_session_member_request::member_server_measurements_json() const
{
    return m_memberServerMeasurementsJson;
}

void 
multiplayer_session_member_request::set_member_server_measurements_json(
    _In_ web::json::value json
    )
{
    m_memberServerMeasurementsJson = std::move(json);
}

bool
multiplayer_session_member_request::write_member_server_measurements_json() const
{
    return m_writeMemberServerMeasurementsJson;
}

void 
multiplayer_session_member_request::set_write_member_server_measurements_json(
    _In_ bool shouldWrite
    )
{
    m_writeMemberServerMeasurementsJson = shouldWrite;
}

std::vector<uint32_t>
multiplayer_session_member_request::members_in_group() const
{
    return m_membersInGroup;
}

void
multiplayer_session_member_request::set_members_in_group(
    _In_ std::vector<uint32_t> membersInGroup
    )
{
    m_membersInGroup = std::move(membersInGroup);
}

bool
multiplayer_session_member_request::write_members_in_group() const
{
    return m_writeMembersInGroup;
}

void 
multiplayer_session_member_request::set_write_members_in_group(
    _In_ bool writeMembersInGroup
    )
{
    m_writeMembersInGroup = writeMembersInGroup;
}

const std::vector<string_t>&
multiplayer_session_member_request::groups() const
{
    return m_groups;
}

void 
multiplayer_session_member_request::set_groups(
    _In_ std::vector<string_t> groups
    )
{
    m_groups = std::move(groups);
}

const std::vector<string_t>&
multiplayer_session_member_request::encounters() const
{
    return m_encounters;
}

void 
multiplayer_session_member_request::set_encounters(
    _In_ std::vector<string_t> encounters
    )
{
    m_encounters = std::move(encounters);
}

bool 
multiplayer_session_member_request::write_subscribed_change_types() const
{
    return m_writeSubscribedChangeType;
}

void 
multiplayer_session_member_request::set_write_subscribed_change_types(
    _In_ bool shouldWrite
    )
{
    m_writeSubscribedChangeType = shouldWrite;
}

multiplayer_session_change_types 
multiplayer_session_member_request::subscribed_change_types() const
{
    return m_changeTypes;
}

void 
multiplayer_session_member_request::set_subscribed_change_types(
    _In_ multiplayer_session_change_types changeTypes
    )
{
    m_changeTypes = changeTypes;
}

const string_t& 
multiplayer_session_member_request::subscription_id() const
{
    return m_subscriptionId;
}

void
multiplayer_session_member_request::set_subscription_id(
    _In_ string_t subscriptionId
    )
{
    m_subscriptionId = std::move(subscriptionId);
}

const string_t& 
multiplayer_session_member_request::rta_connection_id() const
{
    return m_rtaConnectionId;
}

void
multiplayer_session_member_request::set_rta_connection_id(
    _In_ string_t connectionId
    )
{
    m_rtaConnectionId = connectionId;
}

const std::unordered_map<string_t, tournament_team_result>&
multiplayer_session_member_request::results() const
{
    return m_results;
}

void 
multiplayer_session_member_request::set_result(
    _In_ const string_t& team, 
    _In_ const tournament_team_result& result
    )
{
    if (!m_results.insert(std::make_pair(team, result)).second)
    {
        m_results[team] = result;
    }
}

void
multiplayer_session_member_request::set_results(
    _In_ const std::unordered_map<string_t, tournament_team_result>& results
    )
{
    m_results = results;
}

std::vector<string_t> 
multiplayer_session_member_request::get_vector_view_for_change_types(
    _In_ multiplayer_session_change_types changeTypes
    )
{
    std::vector<string_t> resultVector;
    if ((changeTypes & static_cast<uint32_t>(multiplayer_session_change_types::everything)) == multiplayer_session_change_types::everything)
    {
        resultVector.push_back(_T("everything"));
    }
    if ((changeTypes & multiplayer_session_change_types::host_device_token_change) == multiplayer_session_change_types::host_device_token_change)
    {
        resultVector.push_back(_T("host"));
    }
    if ((changeTypes & multiplayer_session_change_types::initialization_state_change) == multiplayer_session_change_types::initialization_state_change)
    {
        resultVector.push_back(_T("initialization"));
    }
    if ((changeTypes & multiplayer_session_change_types::matchmaking_status_change) == multiplayer_session_change_types::matchmaking_status_change)
    {
        resultVector.push_back(_T("matchmakingStatus"));
    }
    if ((changeTypes & multiplayer_session_change_types::tournament_property_change) == multiplayer_session_change_types::tournament_property_change)
    {
        resultVector.push_back(_T("tournaments"));
    }
    if ((changeTypes & multiplayer_session_change_types::member_list_change) == multiplayer_session_change_types::member_list_change)
    {
        resultVector.push_back(_T("membersList"));
    }
    if ((changeTypes & multiplayer_session_change_types::member_status_change) == multiplayer_session_change_types::member_status_change)
    {
        resultVector.push_back(_T("membersStatus"));
    }
    if ((changeTypes & multiplayer_session_change_types::session_joinability_change) == multiplayer_session_change_types::session_joinability_change)
    {
        resultVector.push_back(_T("joinability"));
    }
    if ((changeTypes & multiplayer_session_change_types::custom_property_change) == multiplayer_session_change_types::custom_property_change)
    {
        resultVector.push_back(_T("customProperty"));
    }
    if ((changeTypes & multiplayer_session_change_types::member_custom_property_change) == multiplayer_session_change_types::member_custom_property_change)
    {
        resultVector.push_back(_T("membersCustomProperty"));
    }

    return resultVector;
}

web::json::value
multiplayer_session_member_request::serialize()
{
    web::json::value serializedObject;
    if (m_writeRequest || m_writeConstants)
    {
        web::json::value systemConstantsJson;
        systemConstantsJson[_T("xuid")] = web::json::value::string(m_xboxUserId);

        if (m_addInitializePropertyToRequest)
        {
            systemConstantsJson[_T("initialize")] = web::json::value(m_initializeRequested);
        }

        web::json::value constantsJson;
        constantsJson[_T("system")] = systemConstantsJson;
        if (m_writeConstants && !m_customConstants.is_null())
        {
            constantsJson[_T("custom")] = m_customConstants;
        }
        serializedObject[_T("constants")] = std::move(constantsJson);
    }

    web::json::value propertiesJson;
    web::json::value systemPropertiesJson;

    if (m_writeIsActive)
    {
        systemPropertiesJson[_T("active")] = web::json::value(m_isActive);
        if (!m_isActive)
        {
            systemPropertiesJson[_T("ready")] = web::json::value(m_isActive);
        }
        else
        {
            if (!m_rtaConnectionId.empty())
            {
                systemPropertiesJson[_T("connection")] = web::json::value::string(m_rtaConnectionId);
            }
        }
    }

    if (m_roles.size() > 0)
    {
        web::json::value rolesJson;
        for (const auto& role : m_roles)
        {
            rolesJson[role.first] = web::json::value::string(role.second);
        }
        serializedObject[_T("roles")] = rolesJson;
    }

    if (m_writeSubscribedChangeType)
    {
        web::json::value subscriptionJson = web::json::value::null();

        if (m_changeTypes != multiplayer_session_change_types::none)
        {
            subscriptionJson[_T("id")] = web::json::value::string(m_subscriptionId);
            subscriptionJson[_T("changeTypes")] = utils::serialize_vector<string_t>(
                utils::json_string_serializer, 
                get_vector_view_for_change_types(m_changeTypes)
                );
        }

        systemPropertiesJson[_T("subscription")] = subscriptionJson;
    }

    if (m_results.size() > 0)
    {
        web::json::value arbitrationResultsJson;
        for (const auto& result : m_results)
        {
            web::json::value teamResultJson;
            teamResultJson[_T("outcome")] = web::json::value::string(multiplayer_service::_Convert_game_result_state_to_string(result.second.state()));
            if (result.second.state() == tournament_game_result_state::rank)
            {
                teamResultJson[_T("ranking")] = web::json::value::number(result.second.ranking());
            }
            arbitrationResultsJson[result.first] = teamResultJson;
        }
        systemPropertiesJson[_T("arbitration")][_T("results")] = arbitrationResultsJson;
    }

    if (m_writeSecureDeviceAddressBase64)
    {
        systemPropertiesJson[_T("secureDeviceAddress")] = web::json::value::string(m_serviceDeviceAddressBase64);
    }

    if (m_writeMembersInGroup)
    {
        systemPropertiesJson[_T("initializationGroup")] = utils::serialize_vector<uint32_t>(utils::json_int_serializer, m_membersInGroup);
    }

    if (!m_groups.empty())
    {
        systemPropertiesJson[_T("groups")] = utils::serialize_vector<string_t>(utils::json_string_serializer, m_groups);
    }

    if (!m_encounters.empty())
    {
        systemPropertiesJson[_T("encounters")] = utils::serialize_vector<string_t>(utils::json_string_serializer, m_encounters);
    }

    if (m_writeMemberMeasurementsJson)
    {
        systemPropertiesJson[_T("measurements")] = m_memberMeasurementsJson;
    }

    if (m_writeMemberServerMeasurementsJson)
    {
        systemPropertiesJson[_T("serverMeasurements")] = m_memberServerMeasurementsJson;
    }

    if (!systemPropertiesJson.is_null())
    {
        propertiesJson[_T("system")] = std::move(systemPropertiesJson);
    }

    if (!m_customProperties.is_null())
    {
        propertiesJson[_T("custom")] = m_customProperties;
    }

    if (!propertiesJson.is_null())
    {
        serializedObject[_T("properties")] = std::move(propertiesJson);
    }

    return serializedObject;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END