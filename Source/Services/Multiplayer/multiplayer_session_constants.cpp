// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

const uint32_t c_multiplayerSessionVersion = 1;

multiplayer_session_constants::multiplayer_session_constants() :
    m_maxMembersInSession(0),
    m_visibility(multiplayer_session_visibility::unknown),
    m_writeTimeouts(false),
    m_writeArbitrationTimeouts(false),
    m_writeQualityOfServiceConnectivityMetrics(false),
    m_enableMetricsLatency(false),
    m_enableMetricsBandwidthDown(false),
    m_enableMetricsBandwidthUp(false),
    m_enableMetricsCustom(false),
    m_shouldSerialize(false),
    m_writeMemberInitialization(false),
    m_writePeerToPeerRequirements(false),
    m_writePeerToHostRequirements(false),
    m_writeMeasurementServerAddresses(false)
{
    m_sessionCustomConstants = web::json::value::object();
    m_measurementServerAddressesJson = web::json::value::object();
}

multiplayer_session_constants::multiplayer_session_constants(
    _In_ uint32_t maxMembersInSession,
    _In_ multiplayer_session_visibility visibility,
    _In_ std::vector<string_t> initiatorXboxIds,
    _In_ web::json::value sessionCustomConstants
    ) : 
    m_maxMembersInSession(maxMembersInSession),
    m_visibility(visibility),
    m_initiatorXboxUserIds(std::move(initiatorXboxIds)),
    m_sessionCustomConstants(std::move(sessionCustomConstants)),
    m_shouldSerialize(true),
    m_writeTimeouts(false),
    m_writeArbitrationTimeouts(false),
    m_writeQualityOfServiceConnectivityMetrics(false),
    m_enableMetricsLatency(false),
    m_enableMetricsBandwidthDown(false),
    m_enableMetricsBandwidthUp(false),
    m_enableMetricsCustom(false),
    m_writeMemberInitialization(false),
    m_writePeerToPeerRequirements(false),
    m_writePeerToHostRequirements(false),
    m_writeMeasurementServerAddresses(false)
{
    XSAPI_ASSERT(
        visibility >= multiplayer_session_visibility::any &&
        visibility <= multiplayer_session_visibility::open
        );

    m_measurementServerAddressesJson = web::json::value::object();
}

multiplayer_session_constants::multiplayer_session_constants(
    _In_ std::vector<string_t> initiatorXboxIds
    ) :
    m_maxMembersInSession(0),
    m_visibility(multiplayer_session_visibility::unknown),
    m_initiatorXboxUserIds(std::move(initiatorXboxIds)),
    m_shouldSerialize(true),
    m_writeTimeouts(false),
    m_writeArbitrationTimeouts(false),
    m_writeQualityOfServiceConnectivityMetrics(false),
    m_enableMetricsLatency(false),
    m_enableMetricsBandwidthDown(false),
    m_enableMetricsBandwidthUp(false),
    m_enableMetricsCustom(false),
    m_writeMemberInitialization(false),
    m_writePeerToPeerRequirements(false),
    m_writePeerToHostRequirements(false),
    m_writeMeasurementServerAddresses(false)
{
    m_sessionCustomConstants = web::json::value::object();
    m_measurementServerAddressesJson = web::json::value::object();
}

uint32_t 
multiplayer_session_constants::max_members_in_session() const
{
    return m_maxMembersInSession;
}

void 
multiplayer_session_constants::set_max_members_in_session(
    _In_ uint32_t maxMembersInSession
    )
{
    m_maxMembersInSession = maxMembersInSession;
    m_shouldSerialize = true;
}

multiplayer_session_visibility
multiplayer_session_constants::visibility() const
{
    return m_visibility;
}

void
multiplayer_session_constants::set_visibility(
    _In_ multiplayer_session_visibility visibility
    )
{
    m_visibility = visibility;
    m_shouldSerialize = true;
}

const std::vector<string_t>&
multiplayer_session_constants::initiator_xbox_user_ids() const
{
    return m_initiatorXboxUserIds;
}

const web::json::value&
multiplayer_session_constants::session_custom_constants_json() const
{
    return m_sessionCustomConstants;
}

const web::json::value&
multiplayer_session_constants::session_cloud_compute_package_constants_json() const
{
    return m_sessionCloudComputePackageJson;
}

const std::chrono::milliseconds&
multiplayer_session_constants::member_reserved_time_out() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_memberReservedTimeout;
}

const std::chrono::milliseconds&
multiplayer_session_constants::member_inactive_timeout() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_memberInactiveTimeout;
}

const std::chrono::milliseconds&
multiplayer_session_constants::member_ready_timeout() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_memberReadyTimeout;
}

const std::chrono::milliseconds&
multiplayer_session_constants::session_empty_timeout() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionEmptyTimeout;
}

const std::chrono::milliseconds&
multiplayer_session_constants::arbitration_timeout() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_arbitrationTimeout;
}

const std::chrono::milliseconds&
multiplayer_session_constants::forfeit_timeout() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_forfeitTimeout;
}

bool
multiplayer_session_constants::enable_metrics_latency() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_enableMetricsLatency;
}

bool
multiplayer_session_constants::enable_metrics_bandwidth_down() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_enableMetricsBandwidthDown;
}

bool
multiplayer_session_constants::enable_metrics_bandwidth_up() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_enableMetricsBandwidthUp;
}

bool
multiplayer_session_constants::enable_metrics_custom() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_enableMetricsCustom;
}

const multiplayer_managed_initialization&
multiplayer_session_constants::managed_initialization() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_managedInitialization;
}

const multiplayer_member_initialization&
multiplayer_session_constants::member_initialization() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_memberInitialization;
}


const multiplayer_peer_to_peer_requirements&
multiplayer_session_constants::peer_to_peer_requirements() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_peerToPeerRequirements;
}

const multiplayer_peer_to_host_requirements&
multiplayer_session_constants::peer_to_host_requirements() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_peerToHostRequirements;
}

const web::json::value&
multiplayer_session_constants::measurement_server_addresses_json() const
{
    return m_measurementServerAddressesJson;
}

bool
multiplayer_session_constants::capabilities_connectivity() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.connectivity();
}

bool
multiplayer_session_constants::capabilities_suppress_presence_activity_check() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.suppress_presence_activity_check();
}

bool
multiplayer_session_constants::capabilities_gameplay() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.gameplay();
}

bool
multiplayer_session_constants::capabilities_large() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.large();
}

bool
multiplayer_session_constants::capabilities_connection_required_for_active_member() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.connection_required_for_active_members();
}

bool
multiplayer_session_constants::capabilities_crossplay() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.crossplay();
}

bool
multiplayer_session_constants::capabilities_user_authorization_style() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.user_authorization_style();
}

bool multiplayer_session_constants::capabilities_team() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.team();
}

bool multiplayer_session_constants::capabilities_searchable() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.searchable();
}


bool multiplayer_session_constants::capabilities_arbitration() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_sessionCapabilities.arbitration();
}

bool
multiplayer_session_constants::_Should_serialize() const
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    return m_shouldSerialize;
}

void 
multiplayer_session_constants::_Set_timeouts(
    _In_ std::chrono::milliseconds memberReservedTimeout,
    _In_ std::chrono::milliseconds memberInactiveTimeout,
    _In_ std::chrono::milliseconds memberReadyTimeout,
    _In_ std::chrono::milliseconds sessionEmptyTimeout
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_writeTimeouts = true;
    m_memberReservedTimeout = std::move(memberReservedTimeout);
    m_memberInactiveTimeout = std::move(memberInactiveTimeout);
    m_memberReadyTimeout = std::move(memberReadyTimeout);
    m_sessionEmptyTimeout = std::move(sessionEmptyTimeout);
    
    m_shouldSerialize = true;
}

void 
multiplayer_session_constants::_Set_arbitration_timeouts(
    _In_ std::chrono::milliseconds arbitrationTimeout,
    _In_ std::chrono::milliseconds forfeitTimeout
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_writeArbitrationTimeouts = true;
    m_arbitrationTimeout = std::move(arbitrationTimeout);
    m_forfeitTimeout = std::move(forfeitTimeout);
    m_shouldSerialize = true;
}

void 
multiplayer_session_constants::_Set_quality_of_service_connectivity_metrics(
    _In_ bool enableLatencyMetric,
    _In_ bool enableBandwidthDownMetric,
    _In_ bool enableBandwidthUpMetric,
    _In_ bool enableCustomMetric
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_writeQualityOfServiceConnectivityMetrics = true;
    m_enableMetricsLatency = enableLatencyMetric;
    m_enableMetricsBandwidthDown = enableBandwidthDownMetric;
    m_enableMetricsBandwidthUp = enableBandwidthUpMetric;
    m_enableMetricsCustom = enableCustomMetric;

    m_shouldSerialize = true;
}

void 
multiplayer_session_constants::_Set_managed_initialization(
    _In_ std::chrono::milliseconds joinTimeout,
    _In_ std::chrono::milliseconds measurementTimeout,
    _In_ std::chrono::milliseconds evaluationTimeout,
    _In_ bool autoEvalute,
    _In_ uint32_t membersNeededToStart
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_writeMemberInitialization = true;
    m_managedInitialization = multiplayer_managed_initialization(
        std::move(joinTimeout),
        std::move(measurementTimeout),
        std::move(evaluationTimeout),
        autoEvalute,
        membersNeededToStart
        );

    m_memberInitialization = multiplayer_member_initialization(
        std::move(joinTimeout),
        std::move(measurementTimeout),
        std::move(evaluationTimeout),
        !autoEvalute,
        membersNeededToStart
        );

    m_shouldSerialize = true;
}

void
multiplayer_session_constants::_Set_member_initialization(
    _In_ std::chrono::milliseconds joinTimeout,
    _In_ std::chrono::milliseconds measurementTimeout,
    _In_ std::chrono::milliseconds evaluationTimeout,
    _In_ bool externalEvaluation,
    _In_ uint32_t membersNeededToStart
)
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_writeMemberInitialization = true;
    m_managedInitialization = multiplayer_managed_initialization(
        std::move(joinTimeout),
        std::move(measurementTimeout),
        std::move(evaluationTimeout),
        !externalEvaluation,
        membersNeededToStart
        );

    m_memberInitialization = multiplayer_member_initialization(
        std::move(joinTimeout),
        std::move(measurementTimeout),
        std::move(evaluationTimeout),
        externalEvaluation,
        membersNeededToStart
        );

    m_shouldSerialize = true;
}

void 
multiplayer_session_constants::_Set_peer_to_peer_requirements(
    _In_ std::chrono::milliseconds latencyMaximum,
    _In_ uint32_t bandwidthMinimumInKilobitsPerSecond
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_writePeerToPeerRequirements = true;
    m_peerToPeerRequirements = multiplayer_peer_to_peer_requirements(
        std::move(latencyMaximum),
        bandwidthMinimumInKilobitsPerSecond
        );

    m_shouldSerialize = true;
}

void 
multiplayer_session_constants::_Set_peer_to_host_requirements(
    _In_ std::chrono::milliseconds latencyMaximum,
    _In_ uint32_t bandwidthDownMinimumInKilobitsPerSecond,
    _In_ uint32_t bandwidthUpMinimumInKilobitsPerSecond,
    _In_ multiplay_metrics hostSelectionMetric
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_writePeerToHostRequirements = true;
    m_peerToHostRequirements = multiplayer_peer_to_host_requirements(
        latencyMaximum,
        bandwidthDownMinimumInKilobitsPerSecond,
        bandwidthUpMinimumInKilobitsPerSecond,
        hostSelectionMetric
    );

    m_shouldSerialize = true;
}

void
multiplayer_session_constants::_Set_session_capabilities(
    _In_ const multiplayer_session_capabilities& capabilities
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_sessionCapabilities = capabilities;
    m_shouldSerialize = true;
}

void 
multiplayer_session_constants::_Set_measurement_server_addresses(
    _In_ const std::vector<xbox::services::game_server_platform::quality_of_service_server>& serverAddresses
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    m_writeMeasurementServerAddresses = true;

    for (const auto& address : serverAddresses)
    {
        web::json::value qosServerJson;
        qosServerJson[_T("secureDeviceAddress")] = web::json::value::string(address.secure_device_address_base64());
        auto targetLocationKey = address.target_location();
        std::transform(targetLocationKey.begin(), targetLocationKey.end(), targetLocationKey.begin(), [](utility::char_t c) {
            return (utility::char_t)tolower(c);
        });
        m_measurementServerAddressesJson[targetLocationKey] = qosServerJson;
    }
    
    m_shouldSerialize = true;
}

void
multiplayer_session_constants::_Set_cloud_compute_package_json(
    _In_ web::json::value sessionCloudComputePackageConstantsJson
    )
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);

    m_sessionCloudComputePackageJson = std::move(sessionCloudComputePackageConstantsJson);
    m_shouldSerialize = true;
}

web::json::value
multiplayer_session_constants::_Serialize()
{
    std::lock_guard<std::mutex> guard(get_xsapi_singleton()->m_mpsdConstantLock);
    
    web::json::value serializedObject = web::json::value::object();
    if (!m_shouldSerialize)
    {
        return web::json::value::null();
    }

    web::json::value systemJson;
    systemJson[_T("version")] = web::json::value(c_multiplayerSessionVersion);
    if (m_maxMembersInSession > 0)
    {
        systemJson[_T("maxMembersCount")] = web::json::value(m_maxMembersInSession);
    }

    web::json::value systemCapabilitiesJson;

    if (m_sessionCapabilities.connectivity())
    {
        systemCapabilitiesJson[_T("connectivity")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.suppress_presence_activity_check())
    {
        systemCapabilitiesJson[_T("suppressPresenceActivityCheck")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.gameplay())
    {
        systemCapabilitiesJson[_T("gameplay")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.large())
    {
        systemCapabilitiesJson[_T("large")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.user_authorization_style())
    {
        systemCapabilitiesJson[_T("userAuthorizationStyle")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.connection_required_for_active_members())
    {
        systemCapabilitiesJson[_T("connectionRequiredForActiveMembers")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.crossplay())
    {
        systemCapabilitiesJson[_T("crossplay")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.team())
    {
        systemCapabilitiesJson[_T("team")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.arbitration())
    {
        systemCapabilitiesJson[_T("arbitration")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.searchable())
    {
        systemCapabilitiesJson[_T("searchable")] = web::json::value::boolean(true);
    }

    if (m_sessionCapabilities.has_owners())
    {
        systemCapabilitiesJson[_T("hasOwners")] = web::json::value::boolean(true);
    }

    if (!systemCapabilitiesJson.is_null())
    {
        systemJson[_T("capabilities")] = systemCapabilitiesJson;
    }

    if (m_visibility != multiplayer_session_visibility::any && m_visibility != multiplayer_session_visibility::unknown)
    {
        auto visibilityToString = multiplayer_session_states::_Convert_multiplayer_session_visibility_to_string(m_visibility);
        systemJson[_T("visibility")] = web::json::value::string(
            visibilityToString.payload()
            );
    }

    if (!m_initiatorXboxUserIds.empty())
    {
        std::sort(m_initiatorXboxUserIds.begin(), m_initiatorXboxUserIds.end());
        systemJson[_T("initiators")] = utils::serialize_vector<string_t>(utils::json_string_serializer, m_initiatorXboxUserIds);
    }

    if (m_writeTimeouts)
    {
        systemJson[_T("reservedRemovalTimeout")] = utils::serialize_uint52_to_json(m_memberReservedTimeout.count());
        systemJson[_T("inactiveRemovalTimeout")] = utils::serialize_uint52_to_json(m_memberInactiveTimeout.count());
        systemJson[_T("readyRemovalTimeout")] = utils::serialize_uint52_to_json(m_memberReadyTimeout.count());
        systemJson[_T("sessionEmptyTimeout")] = utils::serialize_uint52_to_json(m_sessionEmptyTimeout.count());
    }

    if (m_writeArbitrationTimeouts)
    {
        web::json::value systemArbitrationTimeoutsJson;
        systemArbitrationTimeoutsJson[_T("arbitrationTimeout")] = utils::serialize_uint52_to_json(m_arbitrationTimeout.count());
        systemArbitrationTimeoutsJson[_T("forfeitTimeout")] = utils::serialize_uint52_to_json(m_forfeitTimeout.count());
        systemJson[_T("arbitration")] = systemArbitrationTimeoutsJson;
    }

    if (m_writeQualityOfServiceConnectivityMetrics)
    {
        web::json::value systemMetricsJson;
        systemMetricsJson[_T("latency")] = web::json::value(m_enableMetricsLatency);
        systemMetricsJson[_T("bandwidthDown")] = web::json::value(m_enableMetricsBandwidthDown);
        systemMetricsJson[_T("bandwidthUp")] = web::json::value(m_enableMetricsBandwidthUp);
        systemMetricsJson[_T("custom")] = web::json::value(m_enableMetricsCustom);
        systemJson[_T("metrics")] = systemMetricsJson;
    }

    if (m_writeMemberInitialization)
    {
        systemJson[_T("memberInitialization")] = m_memberInitialization._Serialize();
    }

    if (m_writePeerToPeerRequirements)
    {
        systemJson[_T("peerToPeerRequirements")] = m_peerToPeerRequirements._Serialize();
    }

    if (m_writePeerToHostRequirements)
    {
        systemJson[_T("peerToHostRequirements")] = m_peerToHostRequirements._Serialize();
    }

    if (m_writeMeasurementServerAddresses)
    {
        systemJson[_T("measurementServerAddresses")] = m_measurementServerAddressesJson;
    }

    if (!m_sessionCloudComputePackageJson.is_null())
    {
        systemJson[_T("cloudComputePackage")] = m_sessionCloudComputePackageJson;
    }

    serializedObject[_T("system")] = systemJson;

    if (!m_sessionCustomConstants.is_null())
    {
        serializedObject[_T("custom")] = m_sessionCustomConstants;
    }

    return serializedObject;
}

xbox_live_result<multiplayer_session_constants>
multiplayer_session_constants::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_session_constants returnResult;
    if (json.is_null()) return xbox_live_result<multiplayer_session_constants>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value systemJson = utils::extract_json_field(json, _T("system"), errc, true);
    web::json::value systemCapabilitiesJson = utils::extract_json_field(systemJson, _T("capabilities"), errc, false);
    web::json::value systemMetricsJson = utils::extract_json_field(systemJson, _T("metrics"), errc, false);
    web::json::value systemArbitrationTimeoutsJson = utils::extract_json_field(systemJson, _T("arbitration"), errc, false);
    returnResult.m_sessionCloudComputePackageJson = utils::extract_json_field(systemJson, _T("cloudComputePackage"), errc, false);

    returnResult.m_maxMembersInSession = utils::extract_json_int(systemJson, _T("maxMembersCount"), errc);

    returnResult.m_visibility = multiplayer_session_states::_Convert_string_to_session_visibility(utils::extract_json_string(systemJson, _T("visibility"), errc)),
    returnResult.m_initiatorXboxUserIds = utils::extract_json_vector<string_t>(utils::json_string_extractor, systemJson, _T("initiators"), errc, false);
    returnResult.m_sessionCustomConstants = utils::extract_json_field(json, _T("custom"), errc, false);

    bool clientConnectivityCapable = utils::extract_json_bool(systemCapabilitiesJson, _T("connectivity"), errc, false);
    bool suppressPresenceActivityCheck = utils::extract_json_bool(systemCapabilitiesJson, _T("suppressPresenceActivityCheck"), errc, false);
    bool gameplay = utils::extract_json_bool(systemCapabilitiesJson, _T("gameplay"), errc, false);
    bool large = utils::extract_json_bool(systemCapabilitiesJson, _T("large"), errc, false);
    bool connectionRequiredForActiveMembers = utils::extract_json_bool(systemCapabilitiesJson, _T("connectionRequiredForActiveMembers"), errc, false);
    bool userAuthorizationStyle = utils::extract_json_bool(systemCapabilitiesJson, _T("userAuthorizationStyle"), errc, false);
    bool crossplay = utils::extract_json_bool(systemCapabilitiesJson, _T("crossplay"), errc, false);
    bool team = utils::extract_json_bool(systemCapabilitiesJson, _T("team"), errc, false);
    bool arbitration = utils::extract_json_bool(systemCapabilitiesJson, _T("arbitration"), errc, false);
    bool hasOwners = utils::extract_json_bool(systemCapabilitiesJson, _T("hasOwners"), errc, false);
    bool searchable = utils::extract_json_bool(systemCapabilitiesJson, _T("searchable"), errc, false);

    returnResult.m_sessionCapabilities.set_connectivity( clientConnectivityCapable );
    returnResult.m_sessionCapabilities.set_suppress_presence_activity_check( suppressPresenceActivityCheck );
    returnResult.m_sessionCapabilities.set_gameplay( gameplay );
    returnResult.m_sessionCapabilities.set_large( large );
    returnResult.m_sessionCapabilities.set_connection_required_for_active_members(connectionRequiredForActiveMembers);
    returnResult.m_sessionCapabilities.set_user_authorization_style(userAuthorizationStyle);
    returnResult.m_sessionCapabilities.set_crossplay(crossplay);
    returnResult.m_sessionCapabilities.set_team(team);
    returnResult.m_sessionCapabilities.set_arbitration(arbitration);
    returnResult.m_sessionCapabilities.set_has_owners(hasOwners);
    returnResult.m_sessionCapabilities.set_searchable(searchable);

    returnResult.m_memberReservedTimeout = std::chrono::milliseconds(utils::extract_json_uint52(systemJson, "reservedRemovalTimeout", errc, false));
    returnResult.m_memberInactiveTimeout = std::chrono::milliseconds(utils::extract_json_uint52(systemJson, "inactiveRemovalTimeout", errc, false));
    returnResult.m_memberReadyTimeout = std::chrono::milliseconds(utils::extract_json_uint52(systemJson, "readyRemovalTimeout", errc, false));
    returnResult.m_sessionEmptyTimeout = std::chrono::milliseconds(utils::extract_json_uint52(systemJson, "sessionEmptyTimeout", errc, false));
    returnResult.m_arbitrationTimeout = std::chrono::milliseconds(utils::extract_json_uint52(systemArbitrationTimeoutsJson, "arbitrationTimeout", errc, false));
    returnResult.m_forfeitTimeout = std::chrono::milliseconds(utils::extract_json_uint52(systemArbitrationTimeoutsJson, "forfeitTimeout", errc, false));

    returnResult.m_enableMetricsLatency = utils::extract_json_bool(systemMetricsJson, _T("latency"), errc, false);
    returnResult.m_enableMetricsBandwidthDown = utils::extract_json_bool(systemMetricsJson, _T("bandwidthDown"), errc, false);
    returnResult.m_enableMetricsBandwidthUp = utils::extract_json_bool(systemMetricsJson, _T("bandwidthUp"), errc, false);
    returnResult.m_enableMetricsCustom = utils::extract_json_bool(systemMetricsJson, _T("custom"), errc, false);

    auto multiplayerManagedInitialization = multiplayer_managed_initialization::_Deserialize(systemJson);
    if (multiplayerManagedInitialization.err())
    {
        errc = multiplayerManagedInitialization.err();
    }
    returnResult.m_managedInitialization = multiplayerManagedInitialization.payload();

    auto multiplayerMemberInitialization = multiplayer_member_initialization::_Deserialize(systemJson);
    if (multiplayerMemberInitialization.err())
    {
        errc = multiplayerMemberInitialization.err();
    }
    returnResult.m_memberInitialization = multiplayerMemberInitialization.payload();

    auto multiplayerPeerToPeerRequirements = multiplayer_peer_to_peer_requirements::_Deserialize(utils::extract_json_field(systemJson, _T("peerToPeerRequirements"), errc, false));
    if (multiplayerPeerToPeerRequirements.err())
    {
        errc = multiplayerPeerToPeerRequirements.err();
    }

    returnResult.m_peerToPeerRequirements = multiplayerPeerToPeerRequirements.payload();

    auto multiplayerPeerToHostRequirements = multiplayer_peer_to_host_requirements::_Deserialize(utils::extract_json_field(systemJson, _T("peerToHostRequirements"), errc, false));
    if (multiplayerPeerToHostRequirements.err())
    {
        errc = multiplayerPeerToHostRequirements.err();
    }
    returnResult.m_peerToHostRequirements = multiplayerPeerToHostRequirements.payload();

    returnResult.m_measurementServerAddressesJson = utils::extract_json_field(systemJson, _T("measurementServerAddresses"), errc, false);

    returnResult.m_shouldSerialize = true;
    return xbox_live_result<multiplayer_session_constants>(returnResult, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END