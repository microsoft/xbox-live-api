// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"
#include "xsapi-c/multiplayer_manager_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_peer_to_host_requirements::multiplayer_peer_to_host_requirements(
    _In_ const XblMultiplayerPeerToHostRequirements& requirements
) 
    : m_requirements(requirements)
{
}

std::chrono::milliseconds multiplayer_peer_to_host_requirements::latency_maximum() const
{
    return std::chrono::milliseconds(m_requirements.LatencyMaximum);
}

uint64_t multiplayer_peer_to_host_requirements::bandwidth_down_minimum_in_kilobits_per_second() const
{
    return m_requirements.BandwidthDownMinimumInKbps;
}

uint64_t multiplayer_peer_to_host_requirements::bandwidth_up_minimum_in_kilobits_per_second() const
{
    return m_requirements.BandwidthUpMinimumInKbps;
}

multiplay_metrics multiplayer_peer_to_host_requirements::host_selection_metric() const
{
    return static_cast<multiplay_metrics>(m_requirements.HostSelectionMetric);
}

multiplayer_peer_to_peer_requirements::multiplayer_peer_to_peer_requirements(
    _In_ const XblMultiplayerPeerToPeerRequirements& requirements
) 
    : m_requirements(requirements)
{
}

uint64_t multiplayer_peer_to_peer_requirements::bandwidth_minimum_in_kilobits_per_second() const
{
    return m_requirements.BandwidthMinimumInKbps;
}

std::chrono::milliseconds multiplayer_peer_to_peer_requirements::latency_maximum() const
{
    return std::chrono::milliseconds(m_requirements.LatencyMaximum);
}

multiplayer_member_initialization::multiplayer_member_initialization(
    _In_opt_ const XblMultiplayerMemberInitialization* memberInitialization
)
    : m_initializationSet(memberInitialization != nullptr)
{
    if (m_initializationSet)
    {
        m_memberInitialization = *memberInitialization;
    }
}

bool multiplayer_member_initialization::member_initialization_set() const
{
    return m_initializationSet;
}

std::chrono::milliseconds multiplayer_member_initialization::join_timeout() const
{
    return std::chrono::milliseconds(m_memberInitialization.JoinTimeout);
}

std::chrono::milliseconds multiplayer_member_initialization::measurement_timeout() const
{
    return std::chrono::milliseconds(m_memberInitialization.MeasurementTimeout);
}

std::chrono::milliseconds multiplayer_member_initialization::evaluation_timeout() const
{
    return std::chrono::milliseconds(m_memberInitialization.EvaluationTimeout);
}

bool multiplayer_member_initialization::external_evaluation() const
{
    return m_memberInitialization.ExternalEvaluation;
}

uint32_t multiplayer_member_initialization::members_need_to_start() const
{
    return m_memberInitialization.MembersNeededToStart;
}

multiplayer_session_capabilities::multiplayer_session_capabilities()
{
    m_capabilities = XblMultiplayerSessionCapabilities{};
}

bool multiplayer_session_capabilities::connectivity() const
{
    return m_capabilities.Connectivity;
}

void multiplayer_session_capabilities::set_connectivity(_In_ bool connectivity)
{
    m_capabilities.Connectivity = connectivity;
}

bool multiplayer_session_capabilities::suppress_presence_activity_check() const
{
    return m_capabilities.SuppressPresenceActivityCheck;
}

void multiplayer_session_capabilities::set_suppress_presence_activity_check(_In_ bool suppressPresenceActivityCheck)
{
    m_capabilities.SuppressPresenceActivityCheck = suppressPresenceActivityCheck;
}

bool multiplayer_session_capabilities::gameplay() const
{
    return m_capabilities.Gameplay;
}

void multiplayer_session_capabilities::set_gameplay(_In_ bool gameplay)
{
    m_capabilities.Gameplay = gameplay;
}

bool multiplayer_session_capabilities::large() const
{
    return m_capabilities.Large;
}

void multiplayer_session_capabilities::set_large(_In_ bool large)
{
    m_capabilities.Large = large;
}

bool multiplayer_session_capabilities::connection_required_for_active_members() const
{
    return m_capabilities.ConnectionRequiredForActiveMembers;
}

void multiplayer_session_capabilities::set_connection_required_for_active_members(_In_ bool connectionRequired)
{
    m_capabilities.ConnectionRequiredForActiveMembers = connectionRequired;
}

bool multiplayer_session_capabilities::user_authorization_style() const
{
    return m_capabilities.UserAuthorizationStyle;
}

void multiplayer_session_capabilities::set_user_authorization_style(_In_ bool userAuthorizationStyle)
{
    m_capabilities.UserAuthorizationStyle = userAuthorizationStyle;
}

bool multiplayer_session_capabilities::crossplay() const
{
    return m_capabilities.Crossplay;
}

void multiplayer_session_capabilities::set_crossplay(_In_ bool crossplay)
{
    m_capabilities.Crossplay = crossplay;
}

bool multiplayer_session_capabilities::searchable() const
{
    return m_capabilities.Searchable;
}

void multiplayer_session_capabilities::set_searchable(_In_ bool searchable)
{
    m_capabilities.Searchable = searchable;
}

bool multiplayer_session_capabilities::has_owners() const
{
    return m_capabilities.HasOwners;
}

void multiplayer_session_capabilities::set_has_owners(_In_ bool hasOwners)
{
    m_capabilities.HasOwners = hasOwners;
}

multiplayer_quality_of_service_measurements::multiplayer_quality_of_service_measurements(
    _In_ const string_t& memberDeviceToken,
    _In_ std::chrono::milliseconds latency,
    _In_ uint64_t bandwidthDownInKilobitsPerSecond,
    _In_ uint64_t bandwidthUpInKilobitsPerSecond,
    _In_ const string_t& customJson
)
    : m_memberDeviceToken(memberDeviceToken),
    m_latency(latency),
    m_bandwidthDown(bandwidthDownInKilobitsPerSecond),
    m_bandwidthUp(bandwidthUpInKilobitsPerSecond)
{
    try
    {
        m_customJson = web::json::value::parse(customJson);
        m_measurementsJson[_T("latency")] = static_cast<int64_t>(m_latency.count());
        m_measurementsJson[_T("bandwidthDown")] = m_bandwidthDown;
        m_measurementsJson[_T("bandwidthUp")] = m_bandwidthUp;
        m_measurementsJson[_T("custom")] = m_customJson;
    }
    catch (web::json::json_exception) {}
}

multiplayer_quality_of_service_measurements::multiplayer_quality_of_service_measurements(
    _In_ const string_t& memberDeviceToken,
    _In_ const web::json::value& measurementsJson
)
    : m_memberDeviceToken(memberDeviceToken)
    , m_measurementsJson(measurementsJson)

{
    m_latency = std::chrono::milliseconds(Utils::ExtractJsonUint64(m_measurementsJson, _T("latency")));
    m_bandwidthDown = Utils::ExtractJsonUint64(m_measurementsJson, _T("bandwidthDown"));
    m_bandwidthUp = Utils::ExtractJsonUint64(m_measurementsJson, _T("bandwidthUp"));
    m_customJson = Utils::ExtractJsonField(m_measurementsJson, _T("custom"), false);
}

const string_t& multiplayer_quality_of_service_measurements::member_device_token() const
{
    return m_memberDeviceToken;
}

const std::chrono::milliseconds& multiplayer_quality_of_service_measurements::latency() const
{
    return m_latency;
}

uint64_t multiplayer_quality_of_service_measurements::bandwidth_down_in_kilobits_per_second() const
{
    return m_bandwidthDown;
}

uint64_t multiplayer_quality_of_service_measurements::bandwidth_up_in_kilobits_per_second() const
{
    return m_bandwidthUp;
}

const web::json::value& multiplayer_quality_of_service_measurements::custom_json() const
{
    return m_customJson;
}

multiplayer_session_constants::multiplayer_session_constants(_In_ XblMultiplayerSessionHandle sessionHandle)
{
    XblMultiplayerSessionDuplicateHandle(sessionHandle, &m_sessionHandle);
    m_constants = XblMultiplayerSessionSessionConstants(m_sessionHandle);
}

multiplayer_session_constants::multiplayer_session_constants(bool isLobbySession) : m_sessionHandle(nullptr)
{
    if (isLobbySession)
    {
        m_constants = XblMultiplayerManagerLobbySessionConstants();
    }
    else // MPM game session
    {
        m_constants = XblMultiplayerManagerGameSessionConstants();
    }
}

multiplayer_session_constants::~multiplayer_session_constants()
{
    if (m_sessionHandle != nullptr)
    {
        XblMultiplayerSessionCloseHandle(m_sessionHandle);
    }
}

uint32_t multiplayer_session_constants::max_members_in_session() const
{
    return m_constants->MaxMembersInSession;
}

void multiplayer_session_constants::set_max_members_in_session(_In_ uint32_t maxMembersInSession)
{
    if (m_sessionHandle != nullptr)
    {
        XblMultiplayerSessionConstantsSetMaxMembersInSession(m_sessionHandle, maxMembersInSession);
    }
}

multiplayer_session_visibility multiplayer_session_constants::visibility() const
{
    return static_cast<multiplayer_session_visibility>(m_constants->Visibility);
}

void multiplayer_session_constants::set_visibility(_In_ multiplayer_session_visibility visibility)
{
    if (m_sessionHandle != nullptr)
    {
        XblMultiplayerSessionConstantsSetVisibility(m_sessionHandle, static_cast<XblMultiplayerSessionVisibility>(visibility));
    }
}

std::vector<string_t> multiplayer_session_constants::initiator_xbox_user_ids() const
{
    return Utils::XuidStringVectorFromXuidArray(m_constants->InitiatorXuids, m_constants->InitiatorXuidsCount);
}

web::json::value multiplayer_session_constants::session_custom_constants_json() const
{
    return Utils::ParseJson(m_constants->CustomJson);
}

web::json::value multiplayer_session_constants::session_cloud_compute_package_constants_json() const
{
    return Utils::ParseJson(m_constants->SessionCloudComputePackageConstantsJson);
}

std::chrono::milliseconds multiplayer_session_constants::member_reserved_time_out() const
{
    return std::chrono::milliseconds(m_constants->MemberReservedTimeout);
}

std::chrono::milliseconds multiplayer_session_constants::member_inactive_timeout() const
{
    return std::chrono::milliseconds(m_constants->MemberInactiveTimeout);
}

std::chrono::milliseconds multiplayer_session_constants::member_ready_timeout() const
{
    return std::chrono::milliseconds(m_constants->MemberReadyTimeout);
}

std::chrono::milliseconds multiplayer_session_constants::session_empty_timeout() const
{
    return std::chrono::milliseconds(m_constants->SessionEmptyTimeout);
}

bool multiplayer_session_constants::enable_metrics_latency() const
{
    return m_constants->EnableMetricsLatency;
}

bool multiplayer_session_constants::enable_metrics_bandwidth_down() const
{
    return m_constants->EnableMetricsBandwidthDown;
}

bool multiplayer_session_constants::enable_metrics_bandwidth_up() const
{
    return m_constants->EnableMetricsBandwidthUp;
}

bool multiplayer_session_constants::enable_metrics_custom() const
{
    return m_constants->EnableMetricsCustom;
}

multiplayer_member_initialization multiplayer_session_constants::member_initialization() const
{
    return multiplayer_member_initialization(m_constants->MemberInitialization);
}

multiplayer_peer_to_peer_requirements multiplayer_session_constants::peer_to_peer_requirements() const
{
    return multiplayer_peer_to_peer_requirements(m_constants->PeerToPeerRequirements);
}

multiplayer_peer_to_host_requirements multiplayer_session_constants::peer_to_host_requirements() const
{
    return multiplayer_peer_to_host_requirements(m_constants->PeerToHostRequirements);
}

web::json::value multiplayer_session_constants::measurement_server_addresses_json() const
{
    return Utils::ParseJson(m_constants->MeasurementServerAddressesJson);
}

bool multiplayer_session_constants::client_matchmaking_capable() const
{
    return m_constants->ClientMatchmakingCapable;
}

bool multiplayer_session_constants::capabilities_connectivity() const
{
    return m_constants->SessionCapabilities.Connectivity;
}

bool multiplayer_session_constants::capabilities_suppress_presence_activity_check() const
{
    return m_constants->SessionCapabilities.SuppressPresenceActivityCheck;
}

bool multiplayer_session_constants::capabilities_gameplay() const
{
    return m_constants->SessionCapabilities.Gameplay;
}

bool multiplayer_session_constants::capabilities_large() const
{
    return m_constants->SessionCapabilities.Large;
}

bool multiplayer_session_constants::capabilities_connection_required_for_active_member() const
{
    return m_constants->SessionCapabilities.ConnectionRequiredForActiveMembers;
}

bool multiplayer_session_constants::capabilities_crossplay() const
{
    return m_constants->SessionCapabilities.Crossplay;
}

bool multiplayer_session_constants::capabilities_user_authorization_style() const
{
    return m_constants->SessionCapabilities.UserAuthorizationStyle;
}

bool multiplayer_session_constants::capabilities_searchable() const
{
    return m_constants->SessionCapabilities.Searchable;
}

multiplayer_session_reference::multiplayer_session_reference() : m_reference{} 
{
}

multiplayer_session_reference::multiplayer_session_reference(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sessionTemplateName,
    _In_ const string_t& sessionName
)
{
    Utils::Utf8FromCharT(serviceConfigurationId.data(), m_reference.Scid, sizeof(m_reference.Scid));
    Utils::Utf8FromCharT(sessionTemplateName.data(), m_reference.SessionTemplateName, sizeof(m_reference.SessionTemplateName));
    Utils::Utf8FromCharT(sessionName.data(), m_reference.SessionName, sizeof(m_reference.SessionName));
}

multiplayer_session_reference::multiplayer_session_reference(
    _In_ const XblMultiplayerSessionReference& reference
)
    : m_reference(reference)
{
}

string_t multiplayer_session_reference::service_configuration_id() const
{
    return Utils::StringTFromUtf8(m_reference.Scid);
}

string_t multiplayer_session_reference::session_template_name() const
{
    return Utils::StringTFromUtf8(m_reference.SessionTemplateName);
}

string_t multiplayer_session_reference::session_name() const
{
    return Utils::StringTFromUtf8(m_reference.SessionName);
}

bool multiplayer_session_reference::is_null() const
{
    return m_reference.Scid[0] == 0 ||
        m_reference.SessionName[0] == 0 ||
        m_reference.SessionTemplateName[0] == 0;
}

string_t multiplayer_session_reference::to_uri_path() const
{
    XblMultiplayerSessionReferenceUri uri{};
    XblMultiplayerSessionReferenceToUriPath(&m_reference, &uri);

    return Utils::StringTFromUtf8(uri.value);
}

multiplayer_session_reference multiplayer_session_reference::parse_from_uri_path(_In_ const string_t& path)
{
    XblMultiplayerSessionReference reference;
    XblMultiplayerSessionReferenceParseFromUriPath(Utils::StringFromStringT(path).data(), &reference);
    return multiplayer_session_reference(reference);
}

multiplayer_session_matchmaking_server::multiplayer_session_matchmaking_server(
    _In_ XblMultiplayerSessionHandle sessionHandle
)
{
    XblMultiplayerSessionDuplicateHandle(sessionHandle, &m_sessionHandle);
    m_server = XblMultiplayerSessionMatchmakingServer(m_sessionHandle);
}

multiplayer_session_matchmaking_server::multiplayer_session_matchmaking_server(
    const multiplayer_session_matchmaking_server& other
)
    : m_server(other.m_server)
{
    XblMultiplayerSessionDuplicateHandle(other.m_sessionHandle, &m_sessionHandle);
    m_server = other.m_server;
}

multiplayer_session_matchmaking_server& multiplayer_session_matchmaking_server::operator=(
    multiplayer_session_matchmaking_server other
)
{
    std::swap(m_sessionHandle, other.m_sessionHandle);
    m_server = other.m_server;
    return *this;
}

multiplayer_session_matchmaking_server::~multiplayer_session_matchmaking_server()
{
    XblMultiplayerSessionCloseHandle(m_sessionHandle);
}

matchmaking_status multiplayer_session_matchmaking_server::status() const
{
    return m_server != nullptr ? static_cast<matchmaking_status>(m_server->Status) : matchmaking_status::unknown;
}

string_t multiplayer_session_matchmaking_server::status_details() const
{
    return m_server != nullptr ? Utils::StringTFromUtf8(m_server->StatusDetails) : string_t();
}

std::chrono::seconds multiplayer_session_matchmaking_server::typical_wait() const
{
    return std::chrono::seconds(m_server != nullptr ? m_server->TypicalWaitInSeconds : 0);
}

multiplayer_session_reference multiplayer_session_matchmaking_server::target_session_ref() const
{
    return m_server != nullptr ? multiplayer_session_reference(m_server->TargetSessionRef) : multiplayer_session_reference();
}

bool multiplayer_session_matchmaking_server::is_null() const
{
    return m_server == nullptr;
}

multiplayer_role_info::multiplayer_role_info()
{
}

multiplayer_role_info::multiplayer_role_info(
    const XblMultiplayerRole* roleInfo
)
    : m_maxMembersCount(roleInfo->MaxMemberCount),
    m_membersCount(roleInfo->MemberCount),
    m_targetCount(roleInfo->TargetCount)
{
    m_memberXuids = Utils::XuidStringVectorFromXuidArray(roleInfo->MemberXuids, roleInfo->MemberCount);
}

const std::vector<string_t>& multiplayer_role_info::member_xbox_user_ids() const
{
    return m_memberXuids;
}

uint32_t multiplayer_role_info::members_count() const
{
    return m_membersCount;
}

uint32_t multiplayer_role_info::target_count() const
{
    return m_targetCount;
}

uint32_t multiplayer_role_info::max_members_count() const
{
    return m_maxMembersCount;
}

void multiplayer_role_info::set_max_members_count(_In_ uint32_t maxCount)
{
    m_maxMembersCount = maxCount;
}

void multiplayer_role_info::set_target_count(_In_ uint32_t targetCount)
{
    m_targetCount = targetCount;
}

multiplayer_role_type::multiplayer_role_type()
{
}

multiplayer_role_type::multiplayer_role_type(
    const XblMultiplayerRoleType* roleType
)
    : m_ownerManaged(roleType->OwnerManaged)
{
    if ((roleType->MutableRoleSettings & XblMutableRoleSettings::Max) == XblMutableRoleSettings::Max)
    {
        m_mutableRoleSettings.push_back(mutable_role_setting::max);
    }
    if ((roleType->MutableRoleSettings & XblMutableRoleSettings::Target) == XblMutableRoleSettings::Target)
    {
        m_mutableRoleSettings.push_back(mutable_role_setting::target);
    }

    for (uint32_t i = 0; i < roleType->RoleCount; ++i)
    {
        m_roles.insert(std::make_pair(Utils::StringTFromUtf8(roleType->Roles[i].Name), multiplayer_role_info(roleType->Roles + i)));
    }
}

bool multiplayer_role_type::owner_managed() const
{
    return m_ownerManaged;
}

const std::vector<mutable_role_setting>& multiplayer_role_type::mutable_role_settings() const
{
    return m_mutableRoleSettings;
}

const std::unordered_map<string_t, multiplayer_role_info>& multiplayer_role_type::roles() const
{
    return m_roles;
}

void multiplayer_role_type::set_roles(_In_ const std::unordered_map<string_t, multiplayer_role_info>& roles)
{
    m_roles = roles;
}

multiplayer_session_role_types::multiplayer_session_role_types(
    const XblMultiplayerRoleType* roleTypes, 
    size_t roleTypesCount
)
{
    for (size_t i = 0; i < roleTypesCount; ++i)
    {
        m_roleTypes.insert(std::make_pair(Utils::StringTFromUtf8(roleTypes[i].Name), multiplayer_role_type(roleTypes + i)));
    }
}

const std::unordered_map<string_t, multiplayer_role_type>& multiplayer_session_role_types::role_types() const
{
    return m_roleTypes;
}

multiplayer_activity_details::multiplayer_activity_details(const XblMultiplayerActivityDetails& activityDetails)
    : m_activityDetails(activityDetails)
{
    if (activityDetails.CustomSessionPropertiesJson != nullptr)
    {
        auto len = strlen(activityDetails.CustomSessionPropertiesJson) + 1;
        auto copy = new char[len]{};
        Utils::CopyUtf8(copy, len, activityDetails.CustomSessionPropertiesJson);
        m_activityDetails.CustomSessionPropertiesJson = copy;
    }
}

multiplayer_activity_details::multiplayer_activity_details(const multiplayer_activity_details& other)
    : multiplayer_activity_details(other.m_activityDetails)
{
}

multiplayer_activity_details& multiplayer_activity_details::operator=(multiplayer_activity_details other)
{
    m_activityDetails = other.m_activityDetails;
    if (m_activityDetails.CustomSessionPropertiesJson != nullptr)
    {
        auto len = strlen(m_activityDetails.CustomSessionPropertiesJson) + 1;
        auto copy = new char[len] {};
        Utils::CopyUtf8(copy, len, m_activityDetails.CustomSessionPropertiesJson);
        m_activityDetails.CustomSessionPropertiesJson = copy;
    }
    return *this;
}

multiplayer_activity_details::~multiplayer_activity_details()
{
    if (m_activityDetails.CustomSessionPropertiesJson != nullptr)
    {
        delete[] m_activityDetails.CustomSessionPropertiesJson;
    }
}

multiplayer_session_reference multiplayer_activity_details::session_reference() const
{
    return multiplayer_session_reference(m_activityDetails.SessionReference);
}

string_t multiplayer_activity_details::handle_id() const
{
    return Utils::StringTFromUtf8(m_activityDetails.HandleId);
}

uint32_t multiplayer_activity_details::title_id() const
{
    return m_activityDetails.TitleId;
}

multiplayer_session_visibility multiplayer_activity_details::visibility() const
{
    return static_cast<multiplayer_session_visibility>(m_activityDetails.Visibility);
}

multiplayer_session_restriction multiplayer_activity_details::join_restriction() const
{
    return static_cast<multiplayer_session_restriction>(m_activityDetails.JoinRestriction);
}

bool multiplayer_activity_details::closed() const
{
    return m_activityDetails.Closed;
}

string_t multiplayer_activity_details::owner_xbox_user_id() const
{
    return Utils::StringTFromUint64(m_activityDetails.OwnerXuid);
}

uint32_t multiplayer_activity_details::max_members_count() const
{
    return m_activityDetails.MaxMembersCount;
}

uint32_t multiplayer_activity_details::members_count() const
{
    return m_activityDetails.MembersCount;
}

web::json::value multiplayer_activity_details::custom_session_properties_json() const
{
    return Utils::ParseJson(m_activityDetails.CustomSessionPropertiesJson);
}

multiplayer_session_reference multiplayer_search_handle_details::session_reference() const
{
    XblMultiplayerSessionReference sessionRef{};
    XblMultiplayerSearchHandleGetSessionReference(m_handle, &sessionRef);
    return multiplayer_session_reference(sessionRef);
}

string_t multiplayer_search_handle_details::handle_id() const
{
    const char* id{ nullptr };
    XblMultiplayerSearchHandleGetId(m_handle, &id);
    return Utils::StringTFromUtf8(id);
}

std::vector<string_t> multiplayer_search_handle_details::session_owner_xbox_user_ids() const
{
    const uint64_t* xuids{ nullptr };
    size_t xuidsCount{ 0 };
    XblMultiplayerSearchHandleGetSessionOwnerXuids(m_handle, &xuids, &xuidsCount);

    return Utils::XuidStringVectorFromXuidArray(xuids, xuidsCount);
}

std::vector<string_t> multiplayer_search_handle_details::tags() const
{
    const XblMultiplayerSessionTag* tags{ nullptr };
    size_t tagsCount{ 0 };
    XblMultiplayerSearchHandleGetTags(m_handle, &tags, &tagsCount);

    return Utils::Transform<string_t>(tags, tagsCount, [](XblMultiplayerSessionTag tag)
    {
        return Utils::StringTFromUtf8(tag.value);
    });
}

std::unordered_map<string_t, double> multiplayer_search_handle_details::numbers_metadata() const
{
    const XblMultiplayerSessionNumberAttribute* attributes{ nullptr };
    size_t attributesCount{ 0 };
    XblMultiplayerSearchHandleGetNumberAttributes(m_handle, &attributes, &attributesCount);

    std::unordered_map<string_t, double> out;
    for (auto i = 0u; i < attributesCount; ++i)
    {
        out[Utils::StringTFromUtf8(attributes[i].name)] = attributes[i].value;
    }

    return out;
}

std::unordered_map<string_t, string_t> multiplayer_search_handle_details::strings_metadata() const
{
    const XblMultiplayerSessionStringAttribute* attributes{ nullptr };
    size_t attributesCount{ 0 };
    XblMultiplayerSearchHandleGetStringAttributes(m_handle, &attributes, &attributesCount);

    std::unordered_map<string_t, string_t> out;
    for (auto i = 0u; i < attributesCount; ++i)
    {
        out[Utils::StringTFromUtf8(attributes[i].name)] = Utils::StringTFromUtf8(attributes[i].value);
    }

    return out;
}

std::unordered_map<string_t, multiplayer_role_type> multiplayer_search_handle_details::role_types() const
{
    return std::unordered_map<string_t, multiplayer_role_type>();
}

multiplayer_session_visibility multiplayer_search_handle_details::visibility() const
{
    XblMultiplayerSessionVisibility visibility{ XblMultiplayerSessionVisibility::Unknown };
    XblMultiplayerSearchHandleGetVisibility(m_handle, &visibility);
    return static_cast<multiplayer_session_visibility>(visibility);
}

multiplayer_session_restriction multiplayer_search_handle_details::join_restriction() const
{
    XblMultiplayerSessionRestriction joinRestriction{ XblMultiplayerSessionRestriction::Unknown };
    XblMultiplayerSearchHandleGetJoinRestriction(m_handle, &joinRestriction);
    return static_cast<multiplayer_session_restriction>(joinRestriction);
}

bool multiplayer_search_handle_details::closed() const
{
    bool closed{ false };
    XblMultiplayerSearchHandleGetSessionClosed(m_handle, &closed);
    return closed;
}

uint32_t multiplayer_search_handle_details::max_members_count() const
{
    size_t maxCount{ 0 };
    size_t currentCount{ 0 };
    XblMultiplayerSearchHandleGetMemberCounts(m_handle, &maxCount, &currentCount);
    return static_cast<uint32_t>(maxCount);
}

uint32_t multiplayer_search_handle_details::members_count() const
{
    size_t maxCount{ 0 };
    size_t currentCount{ 0 };
    XblMultiplayerSearchHandleGetMemberCounts(m_handle, &maxCount, &currentCount);
    return static_cast<uint32_t>(currentCount);
}

web::json::value multiplayer_search_handle_details::custom_session_properties_json() const
{
    const char* customPropertiesJson{ nullptr };
    XblMultiplayerSearchHandleGetCustomSessionPropertiesJson(m_handle, &customPropertiesJson);
    return Utils::ParseJson(customPropertiesJson);
}

utility::datetime multiplayer_search_handle_details::handle_creation_time() const
{
    time_t creationTime{ 0 };
    XblMultiplayerSearchHandleGetCreationTime(m_handle, &creationTime);
    return Utils::DatetimeFromTimeT(creationTime);
}

multiplayer_search_handle_details::multiplayer_search_handle_details(
    XblMultiplayerSearchHandle handle
)
{
    XblMultiplayerSearchHandleDuplicateHandle(handle, &m_handle);
}

multiplayer_search_handle_details::multiplayer_search_handle_details(
    const multiplayer_search_handle_details& other
)
{
    XblMultiplayerSearchHandleDuplicateHandle(other.m_handle, &m_handle);
}

multiplayer_search_handle_details& multiplayer_search_handle_details::operator=(
    multiplayer_search_handle_details other
)
{
    std::swap(m_handle, other.m_handle);
    return *this;
}

multiplayer_search_handle_details::~multiplayer_search_handle_details()
{
    XblMultiplayerSearchHandleCloseHandle(m_handle);
}

multiplayer_session_states::multiplayer_session_states(
    _In_ const XblMultiplayerSessionQueryResult& state
) 
    : m_state(state)
{
}

utility::datetime multiplayer_session_states::start_time() const
{
    return Utils::DatetimeFromTimeT(m_state.StartTime);
}

multiplayer_session_reference multiplayer_session_states::session_reference() const
{
    return multiplayer_session_reference(m_state.SessionReference);
}

multiplayer_session_status multiplayer_session_states::status() const
{
    return static_cast<multiplayer_session_status>(m_state.Status);
}

multiplayer_session_visibility multiplayer_session_states::visibility() const
{
    return static_cast<multiplayer_session_visibility>(m_state.Visibility);
}

bool multiplayer_session_states::is_my_turn() const
{
    return m_state.IsMyTurn;
}

string_t multiplayer_session_states::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_state.Xuid);
}

uint32_t multiplayer_session_states::accepted_member_count() const
{
    return m_state.AcceptedMemberCount;
}

multiplayer_session_restriction multiplayer_session_states::join_restriction() const
{
    return static_cast<multiplayer_session_restriction>(m_state.JoinRestriction);
}

std::vector<string_t> multiplayer_session_states::keywords() const
{
    return std::vector<string_t>();
}

multiplayer_session_member::multiplayer_session_member(
    _In_ XblMultiplayerSessionHandle session,
    _In_ const XblMultiplayerSessionMember* member
) : m_member(member)
{
    XblMultiplayerSessionDuplicateHandle(session, &m_session);
}

multiplayer_session_member::~multiplayer_session_member()
{
    XblMultiplayerSessionCloseHandle(m_session);
}

uint32_t multiplayer_session_member::member_id() const
{
    return m_member->MemberId;
}

string_t multiplayer_session_member::initial_team() const
{
    return Utils::StringTFromUtf8(m_member->InitialTeam);
}

string_t multiplayer_session_member::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_member->Xuid);
}

web::json::value multiplayer_session_member::member_custom_constants_json() const
{
    return Utils::ParseJson(m_member->CustomConstantsJson);
}

string_t multiplayer_session_member::secure_device_base_address64() const
{
    return Utils::StringTFromUtf8(m_member->SecureDeviceBaseAddress64);
}

std::unordered_map<string_t, string_t> multiplayer_session_member::roles() const
{
    std::unordered_map<string_t, string_t> out;
    for (uint32_t i = 0; i < m_member->RolesCount; ++i)
    {
        out[Utils::StringTFromUtf8(m_member->Roles[i].roleTypeName)] = Utils::StringTFromUtf8(m_member->Roles[i].roleName);
    }
    return out;
}

web::json::value multiplayer_session_member::member_custom_properties_json() const
{
    return Utils::ParseJson(m_member->CustomPropertiesJson);
}

string_t multiplayer_session_member::gamertag() const
{
    return Utils::StringTFromUtf8(m_member->Gamertag);
}

multiplayer_session_member_status multiplayer_session_member::status() const
{
    return static_cast<multiplayer_session_member_status>(m_member->Status);
}

bool multiplayer_session_member::is_turn_available() const
{
    return m_member->IsTurnAvailable;
}

bool multiplayer_session_member::is_current_user() const
{
    return m_member->IsCurrentUser;
}

bool multiplayer_session_member::initialize_requested() const
{
    return m_member->InitializeRequested;
}

web::json::value multiplayer_session_member::matchmaking_result_server_measurements_json() const
{
    return Utils::ParseJson(m_member->MatchmakingResultServerMeasurementsJson);
}

web::json::value multiplayer_session_member::member_server_measurements_json() const
{
    return Utils::ParseJson(m_member->ServerMeasurementsJson);
}

std::vector<std::shared_ptr<multiplayer_session_member>> multiplayer_session_member::members_in_group() const
{
    return Utils::Transform<std::shared_ptr<multiplayer_session_member>>(m_member->MembersInGroupIds, m_member->MembersInGroupIds + m_member->MembersInGroupCount,
        [this](uint32_t id)
    {
        auto member = XblMultiplayerSessionGetMember(m_session, id);
        if (member != nullptr)
        {
            return std::make_shared<multiplayer_session_member>(m_session, member);
        }
        return std::shared_ptr<multiplayer_session_member>();
    });
}

std::error_code multiplayer_session_member::set_members_list(_In_ const std::vector<std::shared_ptr<multiplayer_session_member>>& members)
{
    UNREFERENCED_PARAMETER(members);
    auto ids = Utils::Transform<uint32_t>(members, [](std::shared_ptr<multiplayer_session_member> member)
    {
        return member->member_id();
    });

    return Utils::ConvertHr(XblMultiplayerSessionCurrentUserSetMembersInGroup(m_session, ids.data(), static_cast<uint32_t>(ids.size())));
}

std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>> multiplayer_session_member::member_measurements() const
{
    auto out = std::make_shared<std::vector<multiplayer_quality_of_service_measurements>>();
    try
    {
        if (m_member->QosMeasurementsJson != nullptr)
        {
            auto measurementsJson = Utils::ParseJson(m_member->QosMeasurementsJson);
            for (const auto& pair : measurementsJson.as_object())
            {
                out->push_back(multiplayer_quality_of_service_measurements(pair.first, pair.second));
            }
        }
    }
    catch (web::json::json_exception) {}
    return out;
}

string_t multiplayer_session_member::device_token() const
{
    return Utils::StringTFromUtf8(m_member->DeviceToken.Value);
}

network_address_translation_setting multiplayer_session_member::nat() const
{
    return static_cast<network_address_translation_setting>(m_member->Nat);
}

uint32_t multiplayer_session_member::active_title_id() const
{
    return m_member->ActiveTitleId;
}

uint32_t multiplayer_session_member::initialization_episode() const
{
    return m_member->InitializationEpisode;
}

utility::datetime multiplayer_session_member::join_time() const
{
    return Utils::DatetimeFromTimeT(m_member->JoinTime);
}

multiplayer_measurement_failure multiplayer_session_member::initialization_failure_cause() const
{
    return static_cast<multiplayer_measurement_failure>(m_member->InitializationFailureCause);
}

std::vector<string_t> multiplayer_session_member::groups() const
{
    return Utils::StringTVectorFromCStringArray(m_member->Groups, m_member->GroupsCount);
}

void multiplayer_session_member::set_groups(_In_ const std::vector<string_t>& groups)
{
    UTF8StringArrayRef groupsUtf8{ groups };
    XblMultiplayerSessionCurrentUserSetGroups(m_session, groupsUtf8.Data(), static_cast<uint32_t>(groupsUtf8.Size()));
}

std::vector<string_t> multiplayer_session_member::encounters() const
{
    return Utils::StringTVectorFromCStringArray(m_member->Encounters, m_member->EncountersCount);
}

void multiplayer_session_member::set_encounters(_In_ const std::vector<string_t>& encounters)
{
    UTF8StringArrayRef encountersUtf8{ encounters };
    XblMultiplayerSessionCurrentUserSetEncounters(m_session, encountersUtf8.Data(), static_cast<uint32_t>(encountersUtf8.Size()));
}

multiplayer_session_properties::multiplayer_session_properties(_In_ XblMultiplayerSessionHandle session)
{
    XblMultiplayerSessionDuplicateHandle(session, &m_session);
    m_properties = XblMultiplayerSessionSessionProperties(m_session);
}

multiplayer_session_properties::~multiplayer_session_properties()
{
    XblMultiplayerSessionCloseHandle(m_session);
}

std::vector<string_t> multiplayer_session_properties::keywords() const
{
    return Utils::StringTVectorFromCStringArray(m_properties->Keywords, m_properties->KeywordCount);
}

void multiplayer_session_properties::set_keywords(_In_ const std::vector<string_t>& keywords)
{
    UTF8StringArrayRef utf8Keywords{ keywords };
    XblMultiplayerSessionPropertiesSetKeywords(m_session, utf8Keywords.Data(), utf8Keywords.Size());
}

multiplayer_session_restriction multiplayer_session_properties::join_restriction() const
{
    return static_cast<multiplayer_session_restriction>(m_properties->JoinRestriction);
}

std::error_code multiplayer_session_properties::set_join_restriction(_In_ multiplayer_session_restriction joinRestriction)
{
    XblMultiplayerSessionPropertiesSetJoinRestriction(m_session, static_cast<XblMultiplayerSessionRestriction>(joinRestriction));
    return std::error_code();
}

multiplayer_session_restriction multiplayer_session_properties::read_restriction() const
{
    return static_cast<multiplayer_session_restriction>(m_properties->ReadRestriction);
}

std::error_code multiplayer_session_properties::set_read_restriction(_In_ multiplayer_session_restriction readRestriction)
{
    XblMultiplayerSessionPropertiesSetReadRestriction(m_session, static_cast<XblMultiplayerSessionRestriction>(readRestriction));
    return std::error_code();
}

std::vector<std::shared_ptr<multiplayer_session_member>> multiplayer_session_properties::turn_collection() const
{
    std::vector<std::shared_ptr<multiplayer_session_member>> out;
    for (size_t i = 0; i < m_properties->TurnCollectionCount; ++i)
    {
        out.push_back(std::make_shared<multiplayer_session_member>(
            m_session,
            XblMultiplayerSessionGetMember(m_session, m_properties->TurnCollection[i])
            ));
    }
    return out;
}

std::error_code multiplayer_session_properties::set_turn_collection(
    _In_ const std::vector<std::shared_ptr<multiplayer_session_member>>& turnCollection
)
{
    std::vector<uint32_t> memberIds;
    for (auto& member : turnCollection)
    {
        memberIds.push_back(member->member_id());
    }
    auto hr = XblMultiplayerSessionPropertiesSetTurnCollection(m_session, memberIds.data(), static_cast<uint32_t>(memberIds.size()));

    return std::make_error_code(xbox::services::xbox_live_error_code(hr));
}

web::json::value multiplayer_session_properties::matchmaking_target_session_constants_json() const
{
    return Utils::ParseJson(m_properties->MatchmakingTargetSessionConstantsJson);
}

web::json::value multiplayer_session_properties::session_custom_properties_json() const
{
    return Utils::ParseJson(m_properties->SessionCustomPropertiesJson);
}

string_t multiplayer_session_properties::matchmaking_server_connection_string() const
{
    return Utils::StringTFromUtf8(m_properties->MatchmakingServerConnectionString);
}

std::vector<string_t> multiplayer_session_properties::server_connection_string_candidates() const
{
    return Utils::StringTVectorFromCStringArray(m_properties->ServerConnectionStringCandidates, m_properties->ServerConnectionStringCandidatesCount);
}

std::vector<uint32_t> multiplayer_session_properties::session_owner_indices() const
{
    return std::vector<uint32_t>(m_properties->SessionOwnerMemberIds, m_properties->SessionOwnerMemberIds + m_properties->SessionOwnerMemberIdsCount);
}

string_t multiplayer_session_properties::host_device_token() const
{
    return Utils::StringTFromUtf8(m_properties->HostDeviceToken.Value);
}

bool multiplayer_session_properties::closed() const
{
    return m_properties->Closed;
}

bool multiplayer_session_properties::locked() const
{
    return m_properties->Locked;
}

bool multiplayer_session_properties::allocate_cloud_compute() const
{
    return m_properties->AllocateCloudCompute;
}

multiplayer_session::multiplayer_session(
    _In_ const string_t& xboxUserId
)
{
    m_handle = XblMultiplayerSessionCreateHandle(Utils::Uint64FromStringT(xboxUserId), nullptr, nullptr);
    m_sessionInfo = XblMultiplayerSessionGetInfo(m_handle);
}

multiplayer_session::multiplayer_session(
    _In_ const string_t& xboxUserId,
    _In_ multiplayer_session_reference sessionReference
)
{
    m_handle = XblMultiplayerSessionCreateHandle(
        Utils::Uint64FromStringT(xboxUserId),
        &sessionReference.m_reference,
        nullptr
    );
    m_sessionInfo = XblMultiplayerSessionGetInfo(m_handle);
}

multiplayer_session::multiplayer_session(
    _In_ const string_t& xboxUserId,
    _In_ multiplayer_session_reference multiplayerSessionReference,
    _In_ uint32_t maxMembersInSession,
    _In_ multiplayer_session_visibility multiplayerSessionVisibility,
    _In_ std::vector<string_t> initiatorXboxUserIds,
    _In_ const web::json::value& sessionCustomConstantsJson
)
{
    auto initiatorXuids = Utils::Transform<uint64_t>(initiatorXboxUserIds.begin(), initiatorXboxUserIds.end(), Utils::Uint64FromStringT);
    std::string sessionCustomConstantsString = Utils::StringFromStringT(sessionCustomConstantsJson.serialize());

    XblMultiplayerSessionInitArgs args{};
    args.MaxMembersInSession = maxMembersInSession;
    args.Visibility = static_cast<XblMultiplayerSessionVisibility>(multiplayerSessionVisibility);
    if (!initiatorXuids.empty())
    {
        args.InitiatorXuidsCount = static_cast<uint32_t>(initiatorXuids.size());
        args.InitiatorXuids = initiatorXuids.data();
    }
    if (!sessionCustomConstantsJson.is_null())
    {
        args.CustomJson = sessionCustomConstantsString.data();
    }

    m_handle = XblMultiplayerSessionCreateHandle(
        Utils::Uint64FromStringT(xboxUserId),
        &multiplayerSessionReference.m_reference,
        &args
    );
    m_sessionInfo = XblMultiplayerSessionGetInfo(m_handle);
}

multiplayer_session::multiplayer_session(XblMultiplayerSessionHandle handle)
{
    XblMultiplayerSessionDuplicateHandle(handle, &m_handle);
    m_sessionInfo = XblMultiplayerSessionGetInfo(m_handle);
}

multiplayer_session::~multiplayer_session()
{
    XblMultiplayerSessionCloseHandle(m_handle);
}

string_t multiplayer_session::multiplayer_correlation_id() const
{
    return Utils::StringTFromUtf8(m_sessionInfo->CorrelationId);
}

string_t multiplayer_session::search_handle_id() const
{
    return Utils::StringTFromUtf8(m_sessionInfo->SearchHandleId);
}

utility::datetime multiplayer_session::start_time() const
{
    return Utils::DatetimeFromTimeT(m_sessionInfo->StartTime);
}

utility::datetime multiplayer_session::date_of_next_timer() const
{
    return Utils::DatetimeFromTimeT(m_sessionInfo->NextTimer);
}

utility::datetime multiplayer_session::date_of_session() const
{
    return Utils::DatetimeFromTimeT(XblMultiplayerSessionTimeOfSession(m_handle));
}

multiplayer_initialization_stage multiplayer_session::initialization_stage() const
{
    auto initInfo = XblMultiplayerSessionGetInitializationInfo(m_handle);
    if (initInfo != nullptr)
    {
        return static_cast<multiplayer_initialization_stage>(initInfo->Stage);
    }
    return multiplayer_initialization_stage::none;
}

utility::datetime multiplayer_session::initializing_stage_start_time() const
{
    auto initInfo = XblMultiplayerSessionGetInitializationInfo(m_handle);
    if (initInfo != nullptr)
    {
        return Utils::DatetimeFromTimeT(initInfo->StageStartTime);
    }
    return utility::datetime();
}

uint32_t multiplayer_session::intializing_episode() const
{
    auto initInfo = XblMultiplayerSessionGetInitializationInfo(m_handle);
    if (initInfo != nullptr)
    {
        return initInfo->Episode;
    }
    return 0;
}

multiplayer_session_change_types multiplayer_session::subscribed_change_types() const
{
    return static_cast<multiplayer_session_change_types>(XblMultiplayerSessionSubscribedChangeTypes(m_handle));
}

std::vector<string_t> multiplayer_session::host_candidates() const
{
    const XblDeviceToken* tokens;
    size_t tokensCount;
    XblMultiplayerSessionHostCandidates(m_handle, &tokens, &tokensCount);
    return Utils::Transform<string_t>(tokens, tokens + tokensCount, [](const XblDeviceToken& token) 
    {
        return Utils::StringTFromUtf8(token.Value);
    });
}

multiplayer_session_reference multiplayer_session::session_reference() const
{
    return multiplayer_session_reference(*XblMultiplayerSessionSessionReference(m_handle));
}

std::shared_ptr<multiplayer_session_constants> multiplayer_session::session_constants() const
{
    if (m_sessionConstants == nullptr)
    {
        m_sessionConstants = std::make_shared<multiplayer_session_constants>(m_handle);
    }
    return m_sessionConstants;
}

std::shared_ptr<multiplayer_session_properties> multiplayer_session::session_properties() const
{
    if (m_sessionProperties == nullptr)
    {
        m_sessionProperties = std::make_shared<multiplayer_session_properties>(m_handle);
    }
    return m_sessionProperties;
}

std::shared_ptr<multiplayer_session_role_types> multiplayer_session::session_role_types() const
{
    const XblMultiplayerRoleType* roleTypes;
    size_t roleTypesCount;
    XblMultiplayerSessionRoleTypes(m_handle, &roleTypes, &roleTypesCount);
    return std::shared_ptr<multiplayer_session_role_types>(new multiplayer_session_role_types(roleTypes, roleTypesCount));
}

std::vector<std::shared_ptr<multiplayer_session_member>> multiplayer_session::members() const
{
    const XblMultiplayerSessionMember* members;
    size_t membersCount;
    XblMultiplayerSessionMembers(m_handle, &members, &membersCount);

    return Utils::Transform<std::shared_ptr<multiplayer_session_member>>(members, members + membersCount,
        [this](const XblMultiplayerSessionMember& member)
    {
        return std::make_shared<multiplayer_session_member>(m_handle, &member);
    });
}

multiplayer_session_matchmaking_server multiplayer_session::matchmaking_server() const
{
    return multiplayer_session_matchmaking_server(m_handle);
}

uint32_t multiplayer_session::members_accepted() const
{
    return XblMultiplayerSessionMembersAccepted(m_handle);
}

web::json::value multiplayer_session::servers_json() const
{
    return Utils::ParseJson(XblMultiplayerSessionRawServersJson(m_handle));
}

void multiplayer_session::set_servers_json(_In_ const web::json::value& serversJson)
{
    std::string serversJsonString = Utils::StringFromStringT(serversJson.serialize());
    XblMultiplayerSessionSetRawServersJson(m_handle, serversJsonString.data());
}

string_t multiplayer_session::e_tag() const
{
    return Utils::StringTFromUtf8(XblMultiplayerSessionEtag(m_handle));
}

std::shared_ptr<multiplayer_session_member> multiplayer_session::current_user() const
{
    auto currentUser = XblMultiplayerSessionCurrentUser(m_handle);
    if (currentUser != nullptr)
    {
        return std::make_shared<multiplayer_session_member>(m_handle, currentUser);
    }
    return nullptr;
}

string_t multiplayer_session::branch() const
{
    return Utils::StringTFromUtf8(m_sessionInfo->Branch);
}

uint64_t multiplayer_session::change_number() const
{
    return m_sessionInfo->ChangeNumber;
}

write_session_status multiplayer_session::write_status() const
{
    return static_cast<write_session_status>(XblMultiplayerSessionWriteStatus(m_handle));
}

std::error_code multiplayer_session::add_member_reservation(
    _In_ const string_t& xboxUserId,
    _In_ const web::json::value& memberCustomConstantsJson
)
{
    return add_member_reservation(xboxUserId, memberCustomConstantsJson, false);
}

std::error_code multiplayer_session::add_member_reservation(
    _In_ const string_t& xboxUserId,
    _In_ const web::json::value& memberCustomConstantsJson,
    _In_ bool initializeRequested
)
{
    std::string jsonString = Utils::StringFromStringT(memberCustomConstantsJson.serialize());

    auto hr = XblMultiplayerSessionAddMemberReservation(
        m_handle,
        Utils::Uint64FromStringT(xboxUserId),
        memberCustomConstantsJson.is_null() ? nullptr : jsonString.data(),
        initializeRequested
    );

    return std::make_error_code(xbox::services::xbox_live_error_code(hr));
}

xbox_live_result<std::shared_ptr<multiplayer_session_member>> multiplayer_session::join(
    _In_ const web::json::value& memberCustomConstantsJson,
    _In_ bool initializeRequested,
    _In_ bool joinWithActiveStatus,
    _In_ bool addInitializePropertyToRequest
)
{
    UNREFERENCED_PARAMETER(addInitializePropertyToRequest);
    std::string jsonString = Utils::StringFromStringT(memberCustomConstantsJson.serialize());

    auto hr = XblMultiplayerSessionJoin(
        m_handle,
        memberCustomConstantsJson.is_null() ? nullptr : jsonString.data(),
        initializeRequested,
        joinWithActiveStatus
    );

    if (SUCCEEDED(hr))
    {
        auto currentUser = std::make_shared<multiplayer_session_member>(m_handle, XblMultiplayerSessionCurrentUser(m_handle));
        return xbox_live_result<std::shared_ptr<multiplayer_session_member>>(currentUser);
    }
    return xbox_live_result<std::shared_ptr<multiplayer_session_member>>(
        std::make_error_code(xbox::services::xbox_live_error_code(hr))
        );
}

void multiplayer_session::set_visibility(_In_ multiplayer_session_visibility visibility)
{
    XblMultiplayerSessionConstantsSetVisibility(m_handle, static_cast<XblMultiplayerSessionVisibility>(visibility));
}

void multiplayer_session::set_max_members_in_session(_In_ uint32_t maxMembersInSession)
{
    XblMultiplayerSessionConstantsSetMaxMembersInSession(m_handle, maxMembersInSession);
}

std::error_code multiplayer_session::set_mutable_role_settings(
    _In_ const std::unordered_map<string_t, multiplayer_role_type>& roleTypes
)
{
    HRESULT finalHr = S_OK;
    for (const auto& roleType : roleTypes)
    {
        for (const auto& role : roleType.second.roles())
        {
            auto max = role.second.max_members_count();
            auto target = role.second.target_count();
            auto hr = XblMultiplayerSessionSetMutableRoleSettings(
                m_handle,
                Utils::StringFromStringT(roleType.first).data(),
                Utils::StringFromStringT(role.first).data(),
                &max,
                &target
            );
            if (FAILED(hr))
            {
                finalHr = hr;
            }
        }
    }
    return Utils::ConvertHr(finalHr);
}

std::error_code multiplayer_session::set_timeouts(
    _In_ std::chrono::milliseconds memberReservedTimeout,
    _In_ std::chrono::milliseconds memberInactiveTimeout,
    _In_ std::chrono::milliseconds memberReadyTimeout,
    _In_ std::chrono::milliseconds sessionEmptyTimeout
)
{
    auto hr = XblMultiplayerSessionConstantsSetTimeouts(
        m_handle,
        memberReservedTimeout.count(),
        memberInactiveTimeout.count(),
        memberReadyTimeout.count(),
        sessionEmptyTimeout.count()
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_quality_of_service_connectivity_metrics(
    _In_ bool enableLatencyMetric,
    _In_ bool enableBandwidthDownMetric,
    _In_ bool enableBandwidthUpMetric,
    _In_ bool enableCustomMetric
)
{
    auto hr = XblMultiplayerSessionConstantsSetQosConnectivityMetrics(
        m_handle,
        enableLatencyMetric,
        enableBandwidthDownMetric,
        enableBandwidthUpMetric,
        enableCustomMetric
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_member_initialization(
    _In_ std::chrono::milliseconds joinTimeout,
    _In_ std::chrono::milliseconds measurementTimeout,
    _In_ std::chrono::milliseconds evaluationTimeout,
    _In_ bool externalEvaluation,
    _In_ uint32_t membersNeededToStart
)
{
    auto hr = XblMultiplayerSessionConstantsSetMemberInitialization(
        m_handle,
        XblMultiplayerMemberInitialization
        {
            static_cast<uint64_t>(joinTimeout.count()),
            static_cast<uint64_t>(measurementTimeout.count()),
            static_cast<uint64_t>(evaluationTimeout.count()),
            externalEvaluation,
            membersNeededToStart
        }
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_peer_to_peer_requirements(
    _In_ std::chrono::milliseconds latencyMaximum,
    _In_ uint32_t bandwidthMinimumInKilobitsPerSecond
)
{
    auto hr = XblMultiplayerSessionConstantsSetPeerToPeerRequirements(
        m_handle,
        XblMultiplayerPeerToPeerRequirements
        {
            static_cast<uint64_t>(latencyMaximum.count()),
            bandwidthMinimumInKilobitsPerSecond
        }
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_peer_to_host_requirements(
    _In_ std::chrono::milliseconds latencyMaximum,
    _In_ uint32_t bandwidthDownMinimumInKilobitsPerSecond,
    _In_ uint32_t bandwidthUpMinimumInKilobitsPerSecond,
    _In_ multiplay_metrics hostSelectionMetric
)
{
    auto hr = XblMultiplayerSessionConstantsSetPeerToHostRequirements(
        m_handle,
        XblMultiplayerPeerToHostRequirements
        {
            static_cast<uint64_t>(latencyMaximum.count()),
            bandwidthDownMinimumInKilobitsPerSecond,
            bandwidthUpMinimumInKilobitsPerSecond,
            static_cast<XblMultiplayerMetrics>(hostSelectionMetric)
        }
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_session_capabilities(
    _In_ const multiplayer_session_capabilities& capabilities
)
{
    auto hr = XblMultiplayerSessionConstantsSetCapabilities(m_handle, capabilities.m_capabilities);
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_cloud_compute_package_json(
    _In_ const web::json::value& sessionCloudComputePackageConstantsJson
)
{
    std::string jsonString = Utils::StringFromStringT(sessionCloudComputePackageConstantsJson.serialize());

    auto hr = XblMultiplayerSessionConstantsSetCloudComputePackageJson(m_handle, jsonString.data());
    return Utils::ConvertHr(hr);
}

void multiplayer_session::set_initialization_status(
    _In_ bool initializationSucceeded
)
{
    XblMultiplayerSessionSetInitializationSucceeded(m_handle, initializationSucceeded);
}

void multiplayer_session::set_host_device_token(
    _In_ const string_t& hostDeviceToken
)
{
    XblDeviceToken token;
    Utils::Utf8FromCharT(hostDeviceToken.data(), token.Value, sizeof(token.Value));
    XblMultiplayerSessionSetHostDeviceToken(m_handle, token);
}

void multiplayer_session::set_matchmaking_server_connection_path(
    _In_ const string_t& serverConnectionPath
)
{
    XblMultiplayerSessionSetMatchmakingServerConnectionPath(
        m_handle,
        Utils::StringFromStringT(serverConnectionPath).data()
    );
}

void multiplayer_session::set_closed(
    _In_ bool closed
)
{
    XblMultiplayerSessionSetClosed(m_handle, closed);
}

void multiplayer_session::set_locked(
    _In_ bool locked
)
{
    XblMultiplayerSessionSetLocked(m_handle, locked);
}

void multiplayer_session::set_allocate_cloud_compute(
    _In_ bool allocateCloudCompute
)
{
    XblMultiplayerSessionSetAllocateCloudCompute(m_handle, allocateCloudCompute);
}

void multiplayer_session::set_matchmaking_resubmit(
    _In_ bool matchResubmit
)
{
    XblMultiplayerSessionSetMatchmakingResubmit(m_handle, matchResubmit);
}

void multiplayer_session::set_server_connection_string_candidates(
    _In_ const std::vector<string_t>& serverConnectionStringCandidates
)
{
    UTF8StringArrayRef candidates{ serverConnectionStringCandidates };
    XblMultiplayerSessionSetServerConnectionStringCandidates(m_handle, candidates.Data(), static_cast<uint32_t>(candidates.Size()));
}

std::error_code multiplayer_session::set_session_change_subscription(
    _In_ multiplayer_session_change_types changeTypes
)
{
    return Utils::ConvertHr(XblMultiplayerSessionSetSessionChangeSubscription(m_handle, static_cast<XblMultiplayerSessionChangeTypes>(changeTypes)));
}

std::error_code multiplayer_session::leave()
{
    return Utils::ConvertHr(XblMultiplayerSessionLeave(m_handle));
}

std::error_code multiplayer_session::set_current_user_status(
    _In_ multiplayer_session_member_status status
)
{
    return Utils::ConvertHr(XblMultiplayerSessionCurrentUserSetStatus(m_handle, static_cast<XblMultiplayerSessionMemberStatus>(status)));
}

std::error_code multiplayer_session::set_current_user_secure_device_address_base64(
    _In_ const string_t& value
)
{
    auto hr = XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64(m_handle, Utils::StringFromStringT(value).data());
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_current_user_role_info(
    _In_ const std::unordered_map<string_t, string_t>& roles
)
{
    auto rolesVector = Utils::Transform<XblMultiplayerSessionMemberRole>(roles.begin(), roles.end(), [](const std::pair<string_t, string_t>& in)
    {
        int cchRoleType = Utils::Utf8FromCharT(in.first.data(), nullptr, 0);
        auto roleTypeName = new char[cchRoleType];
        Utils::Utf8FromCharT(in.first.data(), roleTypeName, cchRoleType);

        int cchRole = Utils::Utf8FromCharT(in.second.data(), nullptr, 0);
        auto roleName = new char[cchRole];
        Utils::Utf8FromCharT(in.second.data(), roleName, cchRole);

        return XblMultiplayerSessionMemberRole{ roleTypeName, roleName };
    });
    auto hr = XblMultiplayerSessionCurrentUserSetRoles(m_handle, rolesVector.data(), static_cast<uint32_t>(rolesVector.size()));

    for (auto& role : rolesVector)
    {
        delete[] role.roleTypeName;
        delete[] role.roleName;
    }

    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_current_user_members_in_group(
    _In_ const std::vector<std::shared_ptr<multiplayer_session_member>>& membersInGroup
)
{
    auto memberIds = Utils::Transform<uint32_t>(membersInGroup.begin(), membersInGroup.end(),
        [](std::shared_ptr<multiplayer_session_member> in)
    {
        return in->member_id();
    });
    auto hr = XblMultiplayerSessionCurrentUserSetMembersInGroup(m_handle, memberIds.data(), static_cast<uint32_t>(memberIds.size()));
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_current_user_quality_of_service_measurements(
    _In_ std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>> measurements
)
{
    web::json::value json;
    for (const auto& measurement : *measurements)
    {
        json[measurement.m_memberDeviceToken] = measurement.m_measurementsJson;
    }
    auto hr = XblMultiplayerSessionCurrentUserSetQosMeasurements(m_handle, Utils::StringFromStringT(json.serialize()).data());
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_current_user_quality_of_service_measurements_json(
    _In_ const web::json::value& serverMeasurementsJson
)
{
    std::string jsonString = Utils::StringFromStringT(serverMeasurementsJson.serialize());

    auto hr = XblMultiplayerSessionCurrentUserSetServerQosMeasurements(m_handle, jsonString.data());
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_current_user_member_custom_property_json(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson
)
{
    auto hr = XblMultiplayerSessionCurrentUserSetCustomPropertyJson(
        m_handle,
        Utils::StringFromStringT(name).data(),
        Utils::StringFromStringT(valueJson.serialize()).data()
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::delete_current_user_member_custom_property_json(
    _In_ const string_t& name
)
{
    auto hr = XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(
        m_handle,
        Utils::StringFromStringT(name).data()
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_matchmaking_target_session_constants_json(
    _In_ const web::json::value& matchmakingTargetSessionConstantsJson
)
{
    auto hr = XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson(
        m_handle,
        Utils::StringFromStringT(matchmakingTargetSessionConstantsJson.serialize()).data()
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::set_session_custom_property_json(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson
)
{
    auto hr = XblMultiplayerSessionSetCustomPropertyJson(
        m_handle,
        Utils::StringFromStringT(name).data(),
        Utils::StringFromStringT(valueJson.serialize()).data()
    );
    return Utils::ConvertHr(hr);
}

std::error_code multiplayer_session::delete_session_custom_property_json(
    _In_ const string_t& name
)
{
    auto hr = XblMultiplayerSessionDeleteCustomPropertyJson(
        m_handle,
        Utils::StringFromStringT(name).data()
    );
    return Utils::ConvertHr(hr);
}

xbox_live_result<multiplayer_session_change_types> multiplayer_session::compare_multiplayer_sessions(
    _In_ std::shared_ptr<multiplayer_session> currentSession,
    _In_ std::shared_ptr<multiplayer_session> oldSession
)
{
    if (currentSession == nullptr || oldSession == nullptr)
    {
        return xbox_live_result<multiplayer_session_change_types>(std::make_error_code(xbox_live_error_code::invalid_argument), "Cannot compare a null session");
    }
    auto changes = XblMultiplayerSessionCompare(currentSession->m_handle, oldSession->m_handle);
    return xbox_live_result<multiplayer_session_change_types>(static_cast<multiplayer_session_change_types>(changes));
}

write_session_status multiplayer_session::convert_http_status_to_write_session_status(
    _In_ int32_t httpStatusCode
)
{
    switch (httpStatusCode)
    {
    case 200: return write_session_status::updated;
    case 201: return write_session_status::created;
    case 204: return write_session_status::session_deleted;
    case 401: return write_session_status::access_denied;
    case 404: return write_session_status::handle_not_found;
    case 409: return write_session_status::conflict;
    case 412: return write_session_status::out_of_sync;
    default: return write_session_status::unknown;
    }
}

multiplayer_session_change_event_args::multiplayer_session_change_event_args(
    const XblMultiplayerSessionChangeEventArgs& args
) 
    : m_args(args) 
{
}

multiplayer_session_reference multiplayer_session_change_event_args::session_reference() const
{
    return multiplayer_session_reference(m_args.SessionReference);
}

string_t multiplayer_session_change_event_args::branch() const
{
    return Utils::StringTFromUtf8(m_args.Branch);
}

uint64_t multiplayer_session_change_event_args::change_number() const
{
    return m_args.ChangeNumber;
}

multiplayer_get_sessions_request::multiplayer_get_sessions_request(
    _In_ string_t serviceConfigurationId,
    _In_ uint32_t maxItems
)
    : m_request{}
{
    Utils::Utf8FromCharT(serviceConfigurationId.data(), m_request.Scid, sizeof(m_request.Scid));
    m_request.MaxItems = maxItems;
}

string_t multiplayer_get_sessions_request::service_configuration_id()
{
    return Utils::StringTFromUtf8(m_request.Scid);
}

uint32_t multiplayer_get_sessions_request::max_items()
{
    return m_request.MaxItems;
}

bool multiplayer_get_sessions_request::include_private_sessions()
{
    return m_request.IncludePrivateSessions;
}

void multiplayer_get_sessions_request::set_include_private_sessions(_In_ bool includePrivateSessions)
{
    m_request.IncludePrivateSessions = includePrivateSessions;
}

bool multiplayer_get_sessions_request::include_reservations()
{
    return m_request.IncludeReservations;
}

void multiplayer_get_sessions_request::set_include_reservations(_In_ bool includeResevations)
{
    m_request.IncludeReservations = includeResevations;
}

bool multiplayer_get_sessions_request::include_inactive_sessions()
{
    return m_request.IncludeInactiveSessions;
}

void multiplayer_get_sessions_request::set_include_inactive_sessions(_In_ bool includeInactiveSessions)
{
    m_request.IncludeInactiveSessions = includeInactiveSessions;
}

string_t multiplayer_get_sessions_request::xbox_user_id_filter()
{
    if (m_request.XuidFiltersCount > 0)
    {
        return Utils::StringTFromUint64(m_request.XuidFilters[0]);
    }
    return string_t();
}

void multiplayer_get_sessions_request::set_xbox_user_id_filter(_In_ const string_t& filter)
{
    m_xuidFilters.clear();
    if (!filter.empty())
    {
        m_xuidFilters.push_back(Utils::Uint64FromStringT(filter));
        m_request.XuidFilters = m_xuidFilters.data();
    }
    m_request.XuidFiltersCount = m_xuidFilters.size();
}

std::vector<string_t> multiplayer_get_sessions_request::xbox_user_ids_filter()
{
    return Utils::Transform<string_t>(m_xuidFilters, Utils::StringTFromUint64);
}

void multiplayer_get_sessions_request::set_xbox_user_ids_filter(_In_ const std::vector<string_t>& filters)
{
    m_xuidFilters = Utils::Transform<uint64_t>(filters, Utils::Uint64FromStringT);
    m_request.XuidFiltersCount = static_cast<uint32_t>(m_xuidFilters.size());
    m_request.XuidFilters = m_xuidFilters.data();
}

string_t multiplayer_get_sessions_request::keyword_filter()
{
    return Utils::StringTFromUtf8(m_keywordFilter.data());
}

void multiplayer_get_sessions_request::set_keyword_filter(_In_ const string_t& filter)
{
    if (!filter.empty())
    {
        m_keywordFilter = Utils::StringFromStringT(filter);
        m_request.KeywordFilter = m_keywordFilter.data();
    }
    else
    {
        m_request.KeywordFilter = nullptr;
    }
}

string_t multiplayer_get_sessions_request::session_template_name_filter()
{
    return Utils::StringTFromUtf8(m_request.SessionTemplateNameFilter);
}

void multiplayer_get_sessions_request::set_session_template_name_filter(_In_ const string_t& filter)
{
    Utils::Utf8FromCharT(filter.data(), m_request.SessionTemplateNameFilter, sizeof(m_request.SessionTemplateNameFilter));
}

multiplayer_session_visibility multiplayer_get_sessions_request::visibility_filter()
{
    return static_cast<multiplayer_session_visibility>(m_request.VisibilityFilter);
}

void multiplayer_get_sessions_request::set_visibility_filter(_In_ multiplayer_session_visibility filter)
{
    m_request.VisibilityFilter = static_cast<XblMultiplayerSessionVisibility>(filter);
}

uint32_t multiplayer_get_sessions_request::contract_version_filter()
{
    return m_request.ContractVersionFilter;
}

void multiplayer_get_sessions_request::set_contract_version_filter(_In_ uint32_t filter)
{
    m_request.ContractVersionFilter = filter;
}

multiplayer_query_search_handle_request::multiplayer_query_search_handle_request(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sessionTemplateName
) :
    m_scid{ serviceConfigurationId },
    m_sessionTemplateName{ sessionTemplateName }
{
}

const string_t& multiplayer_query_search_handle_request::service_configuration_id() const
{
    return m_scid;
}

const string_t& multiplayer_query_search_handle_request::session_template_name() const
{
    return m_sessionTemplateName;
}

const string_t& multiplayer_query_search_handle_request::order_by() const
{
    return m_orderBy;
}

void multiplayer_query_search_handle_request::set_order_by(_In_ const string_t& orderBy)
{
    m_orderBy = orderBy;
}

bool multiplayer_query_search_handle_request::order_ascending() const
{
    return m_orderAscending;
}

void multiplayer_query_search_handle_request::set_order_ascending(_In_ bool orderAscending)
{
    m_orderAscending = orderAscending;
}

const string_t& multiplayer_query_search_handle_request::search_filter() const
{
    return m_searchFilter;
}

void multiplayer_query_search_handle_request::set_search_filter(_In_ const string_t& searchFilter)
{
    m_searchFilter = searchFilter;
}

const string_t& multiplayer_query_search_handle_request::social_group() const
{
    return m_socialGroup;
}

void multiplayer_query_search_handle_request::set_social_group(_In_ const string_t& socialGroup)
{
    m_socialGroup = socialGroup;
}

multiplayer_search_handle_request::multiplayer_search_handle_request(
    _In_ multiplayer_session_reference sessionRef
) :
    m_sessionRef{ sessionRef.m_reference }
{
}

multiplayer_session_reference multiplayer_search_handle_request::session_reference() const
{
    return multiplayer_session_reference(m_sessionRef);
}

std::vector<string_t> multiplayer_search_handle_request::tags() const
{
    return Utils::Transform<string_t>(m_tags, [](XblMultiplayerSessionTag tag)
    {
        return Utils::StringTFromUtf8(tag.value);
    });
}

void multiplayer_search_handle_request::set_tags(_In_ const std::vector<string_t>& value)
{
    m_tags = Utils::Transform<XblMultiplayerSessionTag>(value, [](const string_t& str)
    {
        XblMultiplayerSessionTag tag{};
        Utils::Utf8FromCharT(str.data(), tag.value, sizeof(tag.value));
        return tag;
    });
}

std::unordered_map<string_t, double> multiplayer_search_handle_request::numbers_metadata() const
{
    std::unordered_map<string_t, double> out;
    for (auto& attribute : m_numberAttributes)
    {
        out[Utils::StringTFromUtf8(attribute.name)] = attribute.value;
    }
    return out;
}

void multiplayer_search_handle_request::set_numbers_metadata(
    _In_ const std::unordered_map<string_t, double>& metadata
)
{
    m_numberAttributes = Utils::Transform<XblMultiplayerSessionNumberAttribute>(metadata.begin(), metadata.end(),
        [](const std::unordered_map<string_t, double>::value_type& pair)
    {
        XblMultiplayerSessionNumberAttribute attr{ {}, pair.second };
        Utils::Utf8FromCharT(pair.first.data(), attr.name, sizeof(attr.name));
        return attr;
    });
}

std::unordered_map<string_t, string_t> multiplayer_search_handle_request::strings_metadata() const
{
    std::unordered_map<string_t, string_t> out;
    for (auto& attribute : m_stringAttributes)
    {
        out[Utils::StringTFromUtf8(attribute.name)] = Utils::StringTFromUtf8(attribute.value);
    }
    return out;
}

void multiplayer_search_handle_request::set_strings_metadata(
    _In_ const std::unordered_map<string_t, string_t>& metadata
)
{
    m_stringAttributes = Utils::Transform<XblMultiplayerSessionStringAttribute>(metadata.begin(), metadata.end(),
        [](const std::unordered_map<string_t, string_t>::value_type& pair)
    {
        XblMultiplayerSessionStringAttribute attr{};
        Utils::Utf8FromCharT(pair.first.data(), attr.name, sizeof(attr.name));
        Utils::Utf8FromCharT(pair.second.data(), attr.value, sizeof(attr.value));
        return attr;
    });
}

multiplayer_service::multiplayer_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_handle);
}

multiplayer_service::multiplayer_service(const multiplayer_service& other)
{
    XblContextDuplicateHandle(other.m_handle, &m_handle);
}

multiplayer_service& multiplayer_service::operator=(multiplayer_service other)
{
    std::swap(m_handle, other.m_handle);
    return *this;
}

multiplayer_service::~multiplayer_service()
{
    XblContextCloseHandle(m_handle);
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> multiplayer_service::write_session(
    _In_ std::shared_ptr<multiplayer_session> multiplayerSession,
    _In_ multiplayer_session_write_mode writeMode
)
{
    auto asyncWrapper = new AsyncWrapper<std::shared_ptr<multiplayer_session>>(
        [](XAsyncBlock* async, std::shared_ptr<multiplayer_session>& result)
    {
        XblMultiplayerSessionHandle sessionHandle;
        auto hr = XblMultiplayerWriteSessionResult(async, &sessionHandle);
        if (sessionHandle)
        {
            XblWriteSessionStatus sessionStatus = XblMultiplayerSessionWriteStatus(sessionHandle);
            switch (sessionStatus)
            {
            case XblWriteSessionStatus::Updated: //200 Updated Successfully
            case XblWriteSessionStatus::Created: //201 Created Successfully
            case XblWriteSessionStatus::SessionDeleted: //204 Deleted Successfully
                //Intentional Fallthrough
                hr = S_OK;
                break;
            case XblWriteSessionStatus::AccessDenied: //403 Forbidden
                hr = HTTP_E_STATUS_FORBIDDEN;
                break;
            case XblWriteSessionStatus::HandleNotFound: //404 Not Found
                hr = HTTP_E_STATUS_NOT_FOUND;
                break;
            case XblWriteSessionStatus::Conflict: //409 Conflict
                hr = HTTP_E_STATUS_CONFLICT;
                break;
            case XblWriteSessionStatus::OutOfSync: //412 Not the most recent
                hr = HTTP_E_STATUS_PRECOND_FAILED;
                break;
            case XblWriteSessionStatus::Unknown:
            default:
                break;
            }
            result = std::shared_ptr<multiplayer_session>(new multiplayer_session(sessionHandle));
            XblMultiplayerSessionCloseHandle(sessionHandle);
        }
        else
        {
            result = nullptr;
        }
        return hr;
    });

    auto hr = XblMultiplayerWriteSessionAsync(m_handle, multiplayerSession->m_handle, static_cast<XblMultiplayerSessionWriteMode>(writeMode), &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> multiplayer_service::write_session_by_handle(
    _In_ std::shared_ptr<multiplayer_session> multiplayerSession,
    _In_ multiplayer_session_write_mode writeMode,
    _In_ const string_t& handleId
)
{
    auto asyncWrapper = new AsyncWrapper<std::shared_ptr<multiplayer_session>>(
        [](XAsyncBlock* async, std::shared_ptr<multiplayer_session>& result)
    {
        XblMultiplayerSessionHandle sessionHandle;
        auto hr = XblMultiplayerWriteSessionByHandleResult(async, &sessionHandle);
        if (SUCCEEDED(hr))
        {
            if (sessionHandle)
            {
                result = std::shared_ptr<multiplayer_session>(new multiplayer_session(sessionHandle));
                XblMultiplayerSessionCloseHandle(sessionHandle);
            }
            else
            {
                result = nullptr;
            }
        }
        return hr;
    });

    auto hr = XblMultiplayerWriteSessionByHandleAsync(
        m_handle,
        multiplayerSession->m_handle,
        static_cast<XblMultiplayerSessionWriteMode>(writeMode),
        Utils::StringFromStringT(handleId).data(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> multiplayer_service::get_current_session(
    _In_ multiplayer_session_reference sessionReference
)
{
    auto asyncWrapper = new AsyncWrapper<std::shared_ptr<multiplayer_session>>(
        [](XAsyncBlock* async, std::shared_ptr<multiplayer_session>& result)
    {
        XblMultiplayerSessionHandle sessionHandle;
        auto hr = XblMultiplayerGetSessionResult(async, &sessionHandle);
        if (SUCCEEDED(hr))
        {
            if (sessionHandle)
            {
                result = std::shared_ptr<multiplayer_session>(new multiplayer_session(sessionHandle));
                XblMultiplayerSessionCloseHandle(sessionHandle);
            }
            else
            {
                result = nullptr;
            }
        }
        return hr;
    });

    auto hr = XblMultiplayerGetSessionAsync(m_handle, &sessionReference.m_reference, &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> multiplayer_service::get_current_session_by_handle(
    _In_ const string_t& handleId
)
{
    auto asyncWrapper = new AsyncWrapper<std::shared_ptr<multiplayer_session>>(
        [](XAsyncBlock* async, std::shared_ptr<multiplayer_session>& result)
    {
        XblMultiplayerSessionHandle sessionHandle;
        auto hr = XblMultiplayerGetSessionByHandleResult(async, &sessionHandle);
        if (SUCCEEDED(hr))
        {
            if (sessionHandle)
            {
                result = std::shared_ptr<multiplayer_session>(new multiplayer_session(sessionHandle));
                XblMultiplayerSessionCloseHandle(sessionHandle);
            }
            else
            {
                result = nullptr;
            }
        }
        return hr;
    });

    auto hr = XblMultiplayerGetSessionByHandleAsync(m_handle, Utils::StringFromStringT(handleId).data(), &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<multiplayer_session_states>>> multiplayer_service::get_sessions(
    _In_ multiplayer_get_sessions_request getSessionsRequest
)
{
    auto asyncWrapper = new AsyncWrapper<std::vector<multiplayer_session_states>>(
        [](XAsyncBlock* async, std::vector<multiplayer_session_states>& result)
    {
        size_t resultCount;
        auto hr = XblMultiplayerQuerySessionsResultCount(async, &resultCount);
        if (SUCCEEDED(hr))
        {
            std::vector<XblMultiplayerSessionQueryResult> intermediateResults(resultCount);
            hr = XblMultiplayerQuerySessionsResult(async, resultCount, intermediateResults.data());

            if (SUCCEEDED(hr))
            {
                result = std::vector<multiplayer_session_states>(intermediateResults.begin(), intermediateResults.end());
            }
        }
        return hr;
    });

    auto hr = XblMultiplayerQuerySessionsAsync(m_handle, &getSessionsRequest.m_request, &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}


pplx::task<xbox_live_result<void>> multiplayer_service::set_activity(
    _In_ multiplayer_session_reference sessionReference
)
{
    auto asyncWrapper = new AsyncWrapper<void>();
    auto hr = XblMultiplayerSetActivityAsync(m_handle, &sessionReference.m_reference, &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<string_t>> multiplayer_service::set_transfer_handle(
    _In_ multiplayer_session_reference targetSessionReference,
    _In_ multiplayer_session_reference originSessionReference
)
{
    auto asyncWrapper = new AsyncWrapper<string_t>(
        [](XAsyncBlock* async, string_t& result)
        {
            XblMultiplayerSessionHandleId id{};
            HRESULT hr = XblMultiplayerSetTransferHandleResult(async, &id);
            if (SUCCEEDED(hr))
            {
                result = xbox::services::Utils::StringTFromUtf8(id.value);
            }
            return hr;
        });

    HRESULT hr = XblMultiplayerSetTransferHandleAsync(m_handle, targetSessionReference.m_reference, originSessionReference.m_reference, &asyncWrapper->async);
    return asyncWrapper->Task(hr);

}

pplx::task<xbox_live_result<void>> multiplayer_service::set_search_handle(
    _In_ multiplayer_search_handle_request r
)
{
    auto asyncWrapper = new AsyncWrapper<void>(
        [](XAsyncBlock* async)
    {
        return XblMultiplayerCreateSearchHandleResult(async, nullptr);
    });

    auto hr = XblMultiplayerCreateSearchHandleAsync(
        m_handle,
        &r.m_sessionRef,
        r.m_tags.data(),
        r.m_tags.size(),
        r.m_numberAttributes.data(),
        r.m_numberAttributes.size(),
        r.m_stringAttributes.data(),
        r.m_stringAttributes.size(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<void>> multiplayer_service::multiplayer_service::clear_activity(
    _In_ const string_t& serviceConfigurationId
)
{
    auto asyncWrapper = new AsyncWrapper<void>();
    auto hr = XblMultiplayerClearActivityAsync(m_handle, Utils::StringFromStringT(serviceConfigurationId).data(), &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<void>> multiplayer_service::clear_search_handle(
    _In_ const string_t& handleId
)
{
    auto asyncWrapper = new AsyncWrapper<void>();
    auto hr = XblMultiplayerDeleteSearchHandleAsync(m_handle, Utils::StringFromStringT(handleId).data(), &asyncWrapper->async);
    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<string_t>>> multiplayer_service::send_invites(
    _In_ multiplayer_session_reference sessionReference,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ uint32_t titleId
)
{
    return send_invites(sessionReference, xboxUserIds, titleId, string_t(), string_t());
}

pplx::task<xbox_live_result<std::vector<string_t>>> multiplayer_service::send_invites(
    _In_ multiplayer_session_reference sessionReference,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ uint32_t titleId,
    _In_ const string_t& contextStringId,
    _In_ const string_t& customActivationContext
)
{
    auto xuids = Utils::XuidVectorFromXuidStringVector(xboxUserIds);
    size_t xuidCount = xboxUserIds.size();

    auto asyncWrapper = new AsyncWrapper<std::vector<string_t>>(
        [xuidCount](XAsyncBlock* async, std::vector<string_t>& result)
    {
        std::vector<XblMultiplayerInviteHandle> inviteHandles(xuidCount);
        auto hr = XblMultiplayerSendInvitesResult(async, xuidCount, inviteHandles.data());
        if (SUCCEEDED(hr))
        {
            result = Utils::Transform<string_t>(inviteHandles, [](const XblMultiplayerInviteHandle& handle)
            {
                return Utils::StringTFromUtf8(handle.Data);
            });
        }
        return hr;
    });

    auto hr = XblMultiplayerSendInvitesAsync(
        m_handle,
        &sessionReference.m_reference,
        xuids.data(),
        xuidCount,
        titleId,
        contextStringId.empty() ? nullptr : Utils::StringFromStringT(contextStringId).data(),
        customActivationContext.empty() ? nullptr : Utils::StringFromStringT(customActivationContext).data(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<multiplayer_activity_details>>> multiplayer_service::get_activities_for_social_group(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& socialGroupOwnerXboxUserId,
    _In_ const string_t& socialGroup
)
{
    auto asyncWrapper = new AsyncWrapper<std::vector<multiplayer_activity_details>>(
        [](XAsyncBlock* async, std::vector<multiplayer_activity_details>& result)
    {
        size_t resultSize;
        auto hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResultSize(async, &resultSize);
        if (SUCCEEDED(hr))
        {
            size_t count{ 0 };
            std::vector<char> buffer(resultSize, 0);
            XblMultiplayerActivityDetails* activityDetails{};
            if (resultSize > 0)
            {
                hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResult(async, resultSize, buffer.data(), &activityDetails, &count, nullptr);
                if (SUCCEEDED(hr))
                {
                    result = std::vector<multiplayer_activity_details>(activityDetails, activityDetails + count);
                }
            }
        }
        return hr;
    });

    auto hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync(
        m_handle,
        Utils::StringFromStringT(serviceConfigurationId).data(),
        Utils::Uint64FromStringT(socialGroupOwnerXboxUserId),
        Utils::StringFromStringT(socialGroup).data(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<multiplayer_activity_details>>> multiplayer_service::get_activities_for_users(
    _In_ const string_t& serviceConfigurationId,
    _In_ const std::vector<string_t>& xboxUserIds
)
{
    auto asyncWrapper = new AsyncWrapper<std::vector<multiplayer_activity_details>>(
        [](XAsyncBlock* async, std::vector<multiplayer_activity_details>& result)
    {
        size_t resultSize;
        auto hr = XblMultiplayerGetActivitiesWithPropertiesForUsersResultSize(async, &resultSize);
        if (SUCCEEDED(hr))
        {
            size_t count{ 0 };
            std::vector<char> buffer(resultSize, 0);
            XblMultiplayerActivityDetails* activityDetails{};
            if (resultSize > 0)
            {
                hr = XblMultiplayerGetActivitiesWithPropertiesForUsersResult(async, resultSize, buffer.data(), &activityDetails, &count, nullptr);
                if (SUCCEEDED(hr))
                {
                    result = std::vector<multiplayer_activity_details>(activityDetails, activityDetails + count);
                }
            }
        }
        return hr;
    });

    auto xuids = Utils::XuidVectorFromXuidStringVector(xboxUserIds);

    auto hr = XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(
        m_handle,
        Utils::StringFromStringT(serviceConfigurationId).data(),
        xuids.data(),
        xuids.size(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<multiplayer_search_handle_details>>> multiplayer_service::get_search_handles(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sessionTemplateName,
    _In_ const string_t& orderBy,
    _In_ bool orderAscending,
    _In_ const string_t& searchFilter
)
{
    multiplayer_query_search_handle_request request{ serviceConfigurationId, sessionTemplateName };
    request.set_order_by(orderBy);
    request.set_order_ascending(orderAscending);
    request.set_search_filter(searchFilter);

    return get_search_handles(request);
}

pplx::task<xbox_live_result<std::vector<multiplayer_search_handle_details>>> multiplayer_service::get_search_handles(
    _In_ const multiplayer_query_search_handle_request& r
)
{
    auto asyncWrapper = new AsyncWrapper<std::vector<multiplayer_search_handle_details>>(
        [](XAsyncBlock* async, std::vector<multiplayer_search_handle_details>& result)
    {
        size_t resultCount{ 0 };
        auto hr = XblMultiplayerGetSearchHandlesResultCount(async, &resultCount);
        if (SUCCEEDED(hr))
        {
            std::vector<XblMultiplayerSearchHandle> handles{ resultCount };
            hr = XblMultiplayerGetSearchHandlesResult(async, handles.data(), resultCount);
            if (SUCCEEDED(hr))
            {
                result = std::vector<multiplayer_search_handle_details>{ handles.begin(), handles.end() };
                for (auto& handle : handles)
                {
                    XblMultiplayerSearchHandleCloseHandle(handle);
                }
            }
        }
        return hr;
    });

    auto hr = XblMultiplayerGetSearchHandlesAsync(
        m_handle,
        Utils::StringFromStringT(r.service_configuration_id()).data(),
        Utils::StringFromStringT(r.session_template_name()).data(),
        Utils::StringFromStringT(r.order_by()).data(),
        r.order_ascending(),
        Utils::StringFromStringT(r.search_filter()).data(),
        Utils::StringFromStringT(r.social_group()).data(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

std::error_code multiplayer_service::enable_multiplayer_subscriptions()
{
    return Utils::ConvertHr(XblMultiplayerSetSubscriptionsEnabled(m_handle, true));
}

void multiplayer_service::disable_multiplayer_subscriptions()
{
    XblMultiplayerSetSubscriptionsEnabled(m_handle, false);
}

bool multiplayer_service::subscriptions_enabled()
{
    return XblMultiplayerSubscriptionsEnabled(m_handle);
}

struct multiplayer_service::HandlerContext
{
    XblFunctionContext internalContext;
    std::function<void(const multiplayer_session_change_event_args&)> sessionChangedHandler;
    std::function<void()> subscriptionLostHandler;
    std::function<void()> connectionIdChangedHandler;
};

function_context multiplayer_service::add_multiplayer_session_changed_handler(
    _In_ std::function<void(const multiplayer_session_change_event_args&)> handler
)
{
    auto context = new HandlerContext{};
    context->sessionChangedHandler = std::move(handler);

    context->internalContext = XblMultiplayerAddSessionChangedHandler(m_handle,
        [](_In_ void* context, _In_ XblMultiplayerSessionChangeEventArgs internalArgs)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->sessionChangedHandler(multiplayer_session_change_event_args{ internalArgs });
    }, context);

    return context;
}

void multiplayer_service::remove_multiplayer_session_changed_handler(
    _In_ function_context context
)
{
    auto handlerContext{ static_cast<HandlerContext*>(context) };
    XblMultiplayerRemoveSessionChangedHandler(m_handle, handlerContext->internalContext);
    delete handlerContext;
}

function_context multiplayer_service::add_multiplayer_subscription_lost_handler(
    _In_ std::function<void()> handler
)
{
    auto context = new HandlerContext{};
    context->subscriptionLostHandler = std::move(handler);

    context->internalContext = XblMultiplayerAddSubscriptionLostHandler(m_handle,
        [](void* context)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->subscriptionLostHandler();
    }, context);

    return context;
}

void multiplayer_service::remove_multiplayer_subscription_lost_handler(
    _In_ function_context context
)
{
    auto handlerContext{ static_cast<HandlerContext*>(context) };
    XblMultiplayerRemoveSubscriptionLostHandler(m_handle, handlerContext->internalContext);
    delete handlerContext;
}

function_context multiplayer_service::add_multiplayer_connection_id_changed_handler(
    _In_ std::function<void()> handler
)
{
    auto context = new HandlerContext{};
    context->connectionIdChangedHandler = std::move(handler);

    context->internalContext = XblMultiplayerAddConnectionIdChangedHandler(m_handle,
        [](void* context)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->connectionIdChangedHandler();
    }, context);

    return context;
}

void multiplayer_service::remove_multiplayer_connection_id_changed_handler(
    _In_ function_context context
)
{
    auto handlerContext{ static_cast<HandlerContext*>(context) };
    XblMultiplayerRemoveConnectionIdChangedHandler(m_handle, handlerContext->internalContext);
    delete handlerContext;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END