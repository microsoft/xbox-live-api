// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"
#include "multiplayer_internal.h"

using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

std::shared_ptr<multiplayer_session_member> multiplayer_session_member::_Create_deep_copy()
{
    auto copy = std::make_shared<multiplayer_session_member>();
    copy->deep_copy_from(*this);
    return copy;
}

void multiplayer_session_member::deep_copy_from(
    _In_ const multiplayer_session_member& other
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdMemberLock);
    m_memberId = other.m_memberId;
    m_customConstantsJson = other.m_customConstantsJson;
    m_customPropertiesJson = other.m_customPropertiesJson;
    m_gamertag = other.m_gamertag;
    m_xboxUserId = other.m_xboxUserId;
    m_isCurrentUser = other.m_isCurrentUser;
    m_isTurnAvailable = other.m_isTurnAvailable;
    m_isReserved = other.m_isReserved;
    m_isActive = other.m_isActive;
    m_isReady = other.m_isReady;
    m_roles = other.m_roles;
    m_secureDeviceAddressBase64 = other.m_secureDeviceAddressBase64;
    m_groups = other.m_groups;
    m_encounters = other.m_encounters;
    m_subscribedChangeTypes = other.m_subscribedChangeTypes;
    m_teamId = other.m_teamId;
    m_arbitrationStatus = other.m_arbitrationStatus;
    m_results = other.m_results;
    m_registrationReason = other.m_registrationReason;
    m_registrationState = other.m_registrationState;
    m_tournamentTeamSessionRef = other.m_tournamentTeamSessionRef;

    // QoS
    m_deviceToken = other.m_deviceToken;
    m_nat = other.m_nat;
    m_activeTitleId = other.m_activeTitleId;
    m_initializationEpisode = other.m_initializationEpisode;
    m_joinTime = other.m_joinTime;
    m_initializationFailure = other.m_initializationFailure;
    m_initialize = other.m_initialize;
    m_matchmakingResultServerMeasurementsJson = other.m_matchmakingResultServerMeasurementsJson;
    m_memberServerMeasurementsJson = other.m_memberServerMeasurementsJson;
    m_membersInGroup = other.m_membersInGroup;
    m_memberMeasurementsJson = other.m_memberMeasurementsJson;
    m_memberMeasurements = other.m_memberMeasurements;
}

multiplayer_session_member::multiplayer_session_member() :
    m_memberId(0),
    m_isCurrentUser(false),
    m_isTurnAvailable(false),
    m_isReserved(false),
    m_isActive(false),
    m_isReady(false),
    m_nat(network_address_translation_setting::unknown),
    m_activeTitleId(0),
    m_initializationEpisode(0),
    m_initializationFailure(multiplayer_measurement_failure::none),
    m_initialize(false),
    m_subscribedChangeTypes(multiplayer_session_change_types::none)
{
    m_memberServerMeasurementsJson = web::json::value::object();
    m_matchmakingResultServerMeasurementsJson = web::json::value::object();
    m_customConstantsJson = web::json::value::object();
    m_customPropertiesJson = web::json::value::object();
    m_memberMeasurements = std::make_shared<std::vector<multiplayer_quality_of_service_measurements>>();
}

multiplayer_session_member::multiplayer_session_member(
    _In_ bool isCurrentUser,
    _In_ uint32_t memberId,
    _In_ string_t xboxUserId,
    _In_ web::json::value customConstants
    ) :
    m_isCurrentUser(isCurrentUser),
    m_memberId(memberId),
    m_xboxUserId(std::move(xboxUserId)),
    m_customConstantsJson(std::move(customConstants)),
    m_isTurnAvailable(false),
    m_isReserved(false),
    m_isActive(false),
    m_isReady(false),
    m_nat(network_address_translation_setting::unknown),
    m_activeTitleId(0),
    m_initializationEpisode(0),
    m_initializationFailure(multiplayer_measurement_failure::none),
    m_initialize(false),
    m_subscribedChangeTypes(multiplayer_session_change_types::none)
{
    m_memberServerMeasurementsJson = web::json::value::object();
    m_matchmakingResultServerMeasurementsJson = web::json::value::object();
    m_customConstantsJson = web::json::value::object();
    m_customPropertiesJson = web::json::value::object();
    m_memberMeasurements = std::make_shared<std::vector<multiplayer_quality_of_service_measurements>>();
}

uint32_t 
multiplayer_session_member::member_id() const
{
    return m_memberId;
}

const string_t& 
multiplayer_session_member::team_id() const
{
    return m_teamId;
}

tournament_arbitration_status 
multiplayer_session_member::arbitration_status() const
{
    return m_arbitrationStatus;
}

void
multiplayer_session_member::_Set_member_id(
    _In_ uint32_t memberId
    )
{
    m_memberId = memberId;
}

const string_t&
multiplayer_session_member::xbox_user_id() const
{
    return m_xboxUserId;
}

const web::json::value&
multiplayer_session_member::member_custom_constants_json() const
{
    return m_customConstantsJson;
}

const string_t&
multiplayer_session_member::secure_device_base_address64() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdMemberLock);

    return m_secureDeviceAddressBase64;
}

void 
multiplayer_session_member::_Set_secure_device_base_address64(
    _In_ const string_t& deviceBaseAddress
    )
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdMemberLock);

    m_secureDeviceAddressBase64 = std::move(deviceBaseAddress);
    m_memberRequest->set_secure_device_address_base64(m_secureDeviceAddressBase64);
    m_memberRequest->set_write_secure_device_address_base64(true);
}

const std::unordered_map<string_t, string_t>&
multiplayer_session_member::roles() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdMemberLock);
    return m_roles;
}

void
multiplayer_session_member::_Set_role_info(
    _In_ const std::unordered_map<string_t, string_t>& roleInfo
    )
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdMemberLock);

    m_roles = std::move(roleInfo);
    m_memberRequest->set_role_info(m_roles);
}

const web::json::value&
multiplayer_session_member::member_custom_properties_json() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdMemberLock);

    return m_customPropertiesJson;
}

const string_t&
multiplayer_session_member::gamertag() const
{
    return m_gamertag;
}

multiplayer_session_member_status
multiplayer_session_member::status() const
{
    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdMemberLock);

    if (m_isActive)
    {
        return multiplayer_session_member_status::active;
    }
    else if (m_isReady)
    {
        return multiplayer_session_member_status::ready;
    }
    else if (m_isReserved)
    {
        return multiplayer_session_member_status::reserved;
    }
    else
    {
        return multiplayer_session_member_status::inactive;
    }
}

const std::unordered_map<string_t, tournament_team_result>&
multiplayer_session_member::results() const
{
    return m_results;
}

bool
multiplayer_session_member::is_turn_available() const
{
    return m_isTurnAvailable;
}

bool
multiplayer_session_member::is_current_user() const
{
    return m_isCurrentUser;
}

void
multiplayer_session_member::_Set_is_current_user(
    _In_ bool isCurrentUser
    )
{
    m_isCurrentUser = isCurrentUser;
}

bool
multiplayer_session_member::initialize_requested() const
{
    return m_initialize;
}

const web::json::value&
multiplayer_session_member::matchmaking_result_server_measurements_json() const
{
    return m_matchmakingResultServerMeasurementsJson;
}

const web::json::value&
multiplayer_session_member::member_server_measurements_json() const
{
    return m_memberServerMeasurementsJson;
}

const std::vector<std::shared_ptr<multiplayer_session_member>>&
multiplayer_session_member::members_in_group() const
{
    return m_membersInGroup;
}


std::error_code
multiplayer_session_member::set_members_list(
    _In_ std::vector<std::shared_ptr<multiplayer_session_member>> members
    )
{
    std::vector<std::shared_ptr<multiplayer_session_member>> membersInGroup;
    for (const uint32_t memberIndex : m_membersInGroupIndices)
    {
        for (const auto& member : members)
        {
            if (member->member_id() == memberIndex)
            {
                membersInGroup.push_back(member);
                break;
            }
        }
    }

    m_membersInGroup = membersInGroup;

    return convert_measure_json_to_vector();
}

std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>>
multiplayer_session_member::member_measurements() const
{
    return m_memberMeasurements;
}

const string_t&
multiplayer_session_member::device_token() const
{
    return m_deviceToken;
}

network_address_translation_setting
multiplayer_session_member::nat() const
{
    return m_nat;
}

uint32_t
multiplayer_session_member::active_title_id() const
{
    return m_activeTitleId;
}

uint32_t
multiplayer_session_member::initialization_episode() const
{
    return m_initializationEpisode;
}

const utility::datetime&
multiplayer_session_member::join_time() const
{
    return m_joinTime;
}

multiplayer_measurement_failure
multiplayer_session_member::initialization_failure_cause() const
{
    return m_initializationFailure;
}

const std::vector<string_t>&
multiplayer_session_member::groups() const
{
    return m_groups;
}

void
multiplayer_session_member::set_groups(
    _In_ std::vector<string_t> groups
    )
{
    m_groups = std::move(groups);
    m_memberRequest->set_groups(m_groups);
}


const std::vector<string_t>&
multiplayer_session_member::encounters() const
{
    return m_encounters;
}

void 
multiplayer_session_member::set_encounters(
    _In_ std::vector<string_t> encounters
    )
{
    m_encounters = std::move(encounters);
    m_memberRequest->set_encounters(m_encounters);
}


std::shared_ptr<multiplayer_session_request>
multiplayer_session_member::_Session_request() const
{
    return m_sessionRequest;
}

void 
multiplayer_session_member::_Set_session_request(
    _In_ std::shared_ptr<multiplayer_session_request> sessionRequest
    )
{
    m_sessionRequest = std::move(sessionRequest);
}


std::shared_ptr<multiplayer_session_member_request>
multiplayer_session_member::_Member_request() const
{
    return m_memberRequest;
}

void
multiplayer_session_member::_Set_member_request(
    _In_ std::shared_ptr<multiplayer_session_member_request> multiplayerSessionMemberRequest
    )
{
    m_memberRequest = multiplayerSessionMemberRequest;
}

std::error_code
multiplayer_session_member::_Set_current_user_status(
    _In_ multiplayer_session_member_status status
    )
{
    XSAPI_ASSERT(m_isCurrentUser);
    if (status != multiplayer_session_member_status::active && status != multiplayer_session_member_status::inactive)
    {
        return xbox_live_error_code::invalid_argument;
    }

    m_isActive = (status == multiplayer_session_member_status::active);
    m_memberRequest->set_is_active(m_isActive);
    m_memberRequest->set_write_is_active(true);

    return xbox_live_error_code::no_error;
}

void 
multiplayer_session_member::_Set_current_user_members_in_group(
    _In_ std::vector<std::shared_ptr<multiplayer_session_member>> membersInGroup
    )
{
    m_membersInGroupIndices.clear();
    for (const auto& member : membersInGroup)
    {
        m_membersInGroupIndices.push_back(member->member_id());
    }
    m_membersInGroup = std::move(membersInGroup);
    m_memberRequest->set_write_members_in_group(true);
    m_memberRequest->set_members_in_group(m_membersInGroupIndices);
}

std::error_code
multiplayer_session_member::_Set_member_custom_property_json(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson
    )
{
    if (name.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }

    if (!m_isCurrentUser)
    {
        return xbox_live_error_code::logic_error;
    }

    std::lock_guard<std::mutex> lock(get_xsapi_singleton()->m_mpsdMemberLock);

    web::json::value customProperty = web::json::value::null();
    if (!valueJson.is_null())
    {
        customProperty = std::move(valueJson);
    }

    if (m_memberRequest->custom_properties().is_null())
    {
        m_memberRequest->set_custom_properties(web::json::value::object());
    }

    m_memberRequest->set_custom_properties_property(name, customProperty);

    return xbox_live_error_code::no_error;
}

void 
multiplayer_session_member::_Delete_custom_property_json(
    _In_ const string_t& name
    )
{
    _Set_member_custom_property_json(name, web::json::value::null());
}

void 
multiplayer_session_member::_Set_current_user_quality_of_service_measurements(
    _In_ std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>> qualityOfServiceMeasurements
    )
{
    for (const auto& measurement : *qualityOfServiceMeasurements)
    {
        web::json::value jsonMeasurement;
        jsonMeasurement[_T("latency")] = measurement.latency().count();
        jsonMeasurement[_T("bandwidthDown")] = measurement.bandwidth_down_in_kilobits_per_second();
        jsonMeasurement[_T("bandwidthUp")] = measurement.bandwidth_up_in_kilobits_per_second();
        jsonMeasurement[_T("custom")] = measurement.custom_json();

        m_memberMeasurementsJson[measurement.member_device_token().c_str()] = jsonMeasurement;
    }

    m_memberMeasurements = qualityOfServiceMeasurements;
    m_memberRequest->set_write_member_measurements_json(true);
    m_memberRequest->set_member_measurements_json(m_memberMeasurementsJson);
}

void 
multiplayer_session_member::_Set_current_user_quality_of_service_server_measurements_json(
    _In_ web::json::value serverMeasurementsJson
    )
{
    m_memberServerMeasurementsJson = serverMeasurementsJson;
    m_memberRequest->set_write_member_server_measurements_json(true);
    m_memberRequest->set_member_server_measurements_json(m_memberServerMeasurementsJson);
}

multiplayer_session_change_types 
multiplayer_session_member::_Subscribed_change_types() const
{
    return m_subscribedChangeTypes;
}

void 
multiplayer_session_member::_Set_session_change_subscription(
    _In_ multiplayer_session_change_types changeTypes, 
    _In_ const string_t& subscriptionId
    )
{
    m_subscribedChangeTypes = changeTypes;
    m_memberRequest->set_subscribed_change_types(changeTypes);
    m_memberRequest->set_subscription_id(subscriptionId);
    m_memberRequest->set_write_subscribed_change_types(true);
}

void 
multiplayer_session_member::_Set_arbitration_results(
    _In_ const std::unordered_map<string_t, tournament_team_result>& results
    )
{
    m_results = results;
    m_memberRequest->set_results(results);
}

void
multiplayer_session_member::_Set_rta_connection_id(
    _In_ const string_t& rtaConnectionId
    )
{
    m_memberRequest->set_rta_connection_id(std::move(rtaConnectionId));
}

const multiplayer_session_reference&
multiplayer_session_member::tournament_team_session_reference() const
{
    return m_tournamentTeamSessionRef;
}

network_address_translation_setting
multiplayer_session_member::_Convert_string_to_multiplayer_nat_setting(
    _In_ const string_t& value
    )
{
    if (value.empty())
    {
        return network_address_translation_setting::unknown;
    }
    else if (utils::str_icmp(value, _T("strict")) == 0)
    {
        return network_address_translation_setting::strict;
    }
    else if (utils::str_icmp(value, _T("moderate")) == 0)
    {
        return network_address_translation_setting::moderate;
    }
    else if (utils::str_icmp(value, _T("open")) == 0)
    {
        return network_address_translation_setting::open;
    }

    return network_address_translation_setting::unknown;
}

multiplayer_measurement_failure
multiplayer_session_member::_Convert_string_to_multiplayer_metric_stage(
    _In_ const string_t& value
    )
{
    if (value.empty())
    {
        return multiplayer_measurement_failure::none;
    }
    else if (utils::str_icmp(value, _T("bandwidthUp")) == 0)
    {
        return multiplayer_measurement_failure::bandwidth_up;
    }
    else if (utils::str_icmp(value, _T("bandwidthDown")) == 0)
    {
        return multiplayer_measurement_failure::bandwidth_down;
    }
    else if (utils::str_icmp(value, _T("latency")) == 0)
    {
        return multiplayer_measurement_failure::latency;
    }
    else if (utils::str_icmp(value, _T("timeout")) == 0)
    {
        return multiplayer_measurement_failure::timeout;
    }
    else if (utils::str_icmp(value, _T("group")) == 0)
    {
        return multiplayer_measurement_failure::group;
    }
    else if (utils::str_icmp(value, _T("network")) == 0)
    {
        return multiplayer_measurement_failure::network;
    }
    else if (utils::str_icmp(value, _T("episode")) == 0)
    {
        return multiplayer_measurement_failure::episode;
    }

    return multiplayer_measurement_failure::unknown;
}

std::error_code
multiplayer_session_member::convert_measure_json_to_vector()
{
    m_memberMeasurements->clear();
    if (!m_memberMeasurementsJson.is_null())
    {
        for (const auto& memberPair : m_memberMeasurementsJson.as_object())
        {
            auto multiplayerQOSMeasurements = multiplayer_quality_of_service_measurements::_Deserialize(memberPair.second);
            if (multiplayerQOSMeasurements.err()) return multiplayerQOSMeasurements.err();

            multiplayer_quality_of_service_measurements measurements = multiplayerQOSMeasurements.payload();
            measurements._Set_member_device_token(memberPair.first);
            m_memberMeasurements->push_back(measurements);
        }
    }

    return xbox_live_error_code::no_error;
}

multiplayer_session_change_types 
multiplayer_session_member::_Convert_string_vector_to_change_types(
    std::vector<string_t> changeTypeList
    )
{
    uint32_t resultingChangeTypes = static_cast<uint32_t>(multiplayer_session_change_types::none);

    for(auto& current : changeTypeList)
    {
        if (utils::str_icmp(current, _T("everything")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::everything;
        }
        else if (utils::str_icmp(current, _T("host")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::host_device_token_change;
        }
        else if (utils::str_icmp(current, _T("initialization")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::initialization_state_change;
        }
        else if (utils::str_icmp(current, _T("matchmakingStatus")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::matchmaking_status_change;
        }
        else if (utils::str_icmp(current, _T("membersList")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::member_list_change;
        }
        else if (utils::str_icmp(current, _T("membersStatus")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::member_status_change;
        }
        else if (utils::str_icmp(current, _T("joinability")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::session_joinability_change;
        }
        else if (utils::str_icmp(current, _T("customProperty")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::custom_property_change;
        }
        else if (utils::str_icmp(current, _T("membersCustomProperty")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::member_custom_property_change;
        }
        else if (utils::str_icmp(current, _T("tournaments")) == 0)
        {
            resultingChangeTypes |= multiplayer_session_change_types::tournament_property_change;
        }
    }

    return static_cast<multiplayer_session_change_types>(resultingChangeTypes);

}

xbox_live_result<multiplayer_session_member>
multiplayer_session_member::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_session_member returnResult;
    if (json.is_null()) return xbox_live_result<multiplayer_session_member>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;

    web::json::value constantsJson = utils::extract_json_field(json, _T("constants"), errc, true);
    web::json::value constantsSystemJson = utils::extract_json_field(constantsJson, _T("system"), errc, true);
    web::json::value constantsSystemMatchmakingResultJson = utils::extract_json_field(constantsSystemJson, _T("matchmakingResult"), errc, false);

    web::json::value propertiesJson = utils::extract_json_field(json, _T("properties"), errc, true);
    web::json::value propertiesSystemJson = utils::extract_json_field(propertiesJson, _T("system"), errc, true);
    web::json::value propertiesSystemSubscriptionJson = utils::extract_json_field(propertiesSystemJson, _T("subscription"), errc, false);
    
    returnResult.m_isReserved = utils::extract_json_bool(json, _T("reserved"), errc);
    returnResult.m_xboxUserId = utils::extract_json_string(constantsSystemJson, _T("xuid"), errc);
    returnResult.m_initialize = utils::extract_json_bool(constantsSystemJson, _T("initialize"), errc);
    returnResult.m_customPropertiesJson = utils::extract_json_field(propertiesJson, _T("custom"), errc, false);
    returnResult.m_customConstantsJson = utils::extract_json_field(constantsJson, _T("custom"), errc, false);
    returnResult.m_teamId = utils::extract_json_string(constantsSystemJson, _T("team"), errc);
    returnResult.m_arbitrationStatus = multiplayer_service::_Convert_string_to_arbitration_status(utils::extract_json_string(json, _T("arbitrationStatus"), errc));
    returnResult.m_gamertag = utils::extract_json_string(json, _T("gamertag"), errc);
    returnResult.m_deviceToken = utils::extract_json_string(json, _T("deviceToken"), errc);
    returnResult.m_nat = _Convert_string_to_multiplayer_nat_setting(utils::extract_json_string(json, _T("nat"), errc));
    returnResult.m_isTurnAvailable = utils::extract_json_bool(json, _T("turn"), errc);
    returnResult.m_isActive = utils::extract_json_bool(propertiesSystemJson, _T("active"), errc);
    returnResult.m_isReady = utils::extract_json_bool(propertiesSystemJson, _T("ready"), errc);
    returnResult.m_secureDeviceAddressBase64 = utils::extract_json_string(propertiesSystemJson, _T("secureDeviceAddress"), errc);
    returnResult.m_subscribedChangeTypes = _Convert_string_vector_to_change_types(
        utils::extract_json_vector<string_t>(utils::json_string_extractor, propertiesSystemSubscriptionJson, _T("changeTypes"), errc, false)
        );

    returnResult.m_memberServerMeasurementsJson = utils::extract_json_field(propertiesSystemJson, _T("serverMeasurements"), errc, false);
    returnResult.m_memberMeasurementsJson = utils::extract_json_field(propertiesSystemJson, _T("measurements"), errc, false);
    returnResult.m_membersInGroupIndices = utils::extract_json_vector<uint32_t>(utils::json_int_extractor, propertiesSystemJson, _T("initializationGroup"), errc, false);

    web::json::value rolesJson = utils::extract_json_field(json, _T("roles"), errc, false);
    if (!rolesJson.is_null() && rolesJson.is_object())
    {
        web::json::object rolesObj = rolesJson.as_object();
        for (const auto& role : rolesObj)
        {
            returnResult.m_roles[role.first] = role.second.as_string();
        }
    }

    web::json::value registrationJson = utils::extract_json_field(propertiesSystemJson, _T("registration"), errc, false);
    if (!registrationJson.is_null())
    {
        returnResult.m_registrationState = multiplayer_session_tournaments_server::_Convert_string_to_registration_result(
            utils::extract_json_string(registrationJson, _T("state"), errc)
            );
        returnResult.m_registrationReason = multiplayer_session_tournaments_server::_Convert_string_to_registration_reason(
            utils::extract_json_string(registrationJson, _T("reason"), errc)
            );
    }

    web::json::value arbitrationJson = utils::extract_json_field(propertiesSystemJson, _T("arbitration"), errc, false);
    if (!arbitrationJson.is_null())
    {
        web::json::value resultsJson = utils::extract_json_field(arbitrationJson, _T("results"), errc, false);
        if (!resultsJson.is_null() && resultsJson.is_object())
        {
            web::json::object resultsObj = resultsJson.as_object();
            for (const auto& result : resultsObj)
            {
                const auto& team = result.first;
                auto tournamentTeamResult = tournament_team_result::_Deserialize(result.second);
                if (tournamentTeamResult.err())
                {
                    errc = tournamentTeamResult.err();
                }
                returnResult.m_results[team] = tournamentTeamResult.payload();
            }
        }
    }

    auto teamJson = utils::extract_json_field(constantsSystemJson, _T("teamSessionRef"), errc, false);
    auto teamSessionResult = multiplayer_session_reference::_Deserialize(teamJson);
    if (teamSessionResult.err())
    {
        errc = teamSessionResult.err();
    }
    returnResult.m_tournamentTeamSessionRef = teamSessionResult.payload();

    auto titleIdString = utils::extract_json_string(json, _T("activeTitleId"), errc);
    if (!titleIdString.empty())
    {
        returnResult.m_activeTitleId = utils::string_t_to_uint32(titleIdString);
    }

    returnResult.m_joinTime = utils::extract_json_time(json, _T("joinTime"), errc);
    returnResult.m_initializationFailure = _Convert_string_to_multiplayer_metric_stage(utils::extract_json_string(json, _T("initializationFailure"), errc));
    returnResult.m_initializationEpisode = utils::extract_json_int(json, _T("initializationEpisode"), errc);
    returnResult.m_matchmakingResultServerMeasurementsJson = utils::extract_json_field(constantsSystemMatchmakingResultJson, _T("serverMeasurements"), errc, false);

    return returnResult;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END