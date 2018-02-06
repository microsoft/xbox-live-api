// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"
#include "multiplayer_internal.h"

using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session::multiplayer_session() :
    m_membersAccepted(0),
    m_joiningSession(false),
    m_newSession(false),
    m_initializationStage(multiplayer_initialization_stage::unknown),
    m_initializationEpisode(0),
    m_hasMatchmakingServer(false),
    m_changeNumber(0)
{
    m_servers = web::json::value::object();
    m_sessionConstants = std::make_shared<multiplayer_session_constants>();
    m_sessionRequest = std::make_shared<multiplayer_session_request>();
    m_multiplayerSessionProperties = std::make_shared<multiplayer_session_properties>();
    m_sessionRoleTypes = std::make_shared<multiplayer_session_role_types>();
}


multiplayer_session::multiplayer_session(
    _In_ string_t xboxUserId
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_membersAccepted(0),
    m_joiningSession(false),
    m_newSession(true),
    m_initializationStage(multiplayer_initialization_stage::unknown),
    m_initializationEpisode(0),
    m_hasMatchmakingServer(false),
    m_changeNumber(0)
{
    XSAPI_ASSERT(!m_xboxUserId.empty());
    _Initialize(m_xboxUserId);
}

multiplayer_session::multiplayer_session(
    _In_ string_t xboxUserId,
    _In_ multiplayer_session_reference sessionReference
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_sessionReference(std::move(sessionReference)),
    m_newSession(true),
    m_membersAccepted(0),
    m_joiningSession(false),
    m_initializationStage(multiplayer_initialization_stage::unknown),
    m_initializationEpisode(0),
    m_hasMatchmakingServer(false),
    m_changeNumber(0)
{
    XSAPI_ASSERT(!m_xboxUserId.empty());

    _Initialize(m_xboxUserId);
    m_sessionRequest->set_session_reference(m_sessionReference);
}


multiplayer_session::multiplayer_session(
    _In_ string_t xboxUserId,
    _In_ multiplayer_session_reference sessionReference,
    _In_ std::vector<string_t> initiatorXboxUserIds
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_sessionReference(std::move(sessionReference)),
    m_newSession(true),
    m_membersAccepted(0),
    m_joiningSession(false),
    m_initializationStage(multiplayer_initialization_stage::unknown),
    m_initializationEpisode(0),
    m_hasMatchmakingServer(false),
    m_changeNumber(0)
{
    XSAPI_ASSERT(!m_xboxUserId.empty());

    _Initialize(m_xboxUserId);

    m_sessionConstants = std::make_shared<multiplayer_session_constants>(initiatorXboxUserIds);
    m_sessionRequest->set_session_constants(m_sessionConstants);
    m_sessionRequest->set_session_reference(m_sessionReference);
}

multiplayer_session::multiplayer_session(
    _In_ string_t xboxUserId,
    _In_ multiplayer_session_reference sessionReference,
    _In_ uint32_t maxMembersInSession,
    _In_ multiplayer_session_visibility multiplayerSessionVisibility,
    _In_ std::vector<string_t> initiatorXboxUserIds,
    _In_ web::json::value sessionCustomConstantsJson
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_sessionReference(std::move(sessionReference)),
    m_newSession(true),
    m_membersAccepted(0),
    m_joiningSession(false),
    m_initializationStage(multiplayer_initialization_stage::unknown),
    m_initializationEpisode(0),
    m_hasMatchmakingServer(false),
    m_changeNumber(0)
{
    XSAPI_ASSERT(!m_xboxUserId.empty());

    XSAPI_ASSERT(
        multiplayerSessionVisibility >= multiplayer_session_visibility::any &&
        multiplayerSessionVisibility <= multiplayer_session_visibility::open
        );

    _Initialize(m_xboxUserId);

    web::json::value sessionCustomConstants;
    if (!sessionCustomConstantsJson.is_null())
    {
        sessionCustomConstants = std::move(sessionCustomConstantsJson);
    }
    m_sessionConstants = std::make_shared<multiplayer_session_constants>(
        maxMembersInSession,
        multiplayerSessionVisibility,
        initiatorXboxUserIds,
        sessionCustomConstants
        );

    m_sessionRequest->set_session_constants(m_sessionConstants);
    m_sessionRequest->set_session_reference(m_sessionReference);
}


std::shared_ptr<multiplayer_session> multiplayer_session::_Create_deep_copy()
{
    auto copy = std::make_shared<multiplayer_session>();
    copy->deep_copy_from(*this);
    return copy;
}

void multiplayer_session::deep_copy_from(
    _In_ const multiplayer_session& other
    )
{
    m_xboxUserId = other.m_xboxUserId;
    m_sessionReference = other.m_sessionReference;
    m_sessionConstants = other.m_sessionConstants;
    m_arbitrationStatus = other.m_arbitrationStatus;
    m_sessionRoleTypes = other.m_sessionRoleTypes;
    m_servers = other.m_servers;
    m_membersAccepted = other.m_membersAccepted;
    m_correlationId = other.m_correlationId;
    m_searchHandleId = other.m_searchHandleId;
    m_eTag = other.m_eTag;
    m_joiningSession = other.m_joiningSession;
    m_newSession = other.m_newSession;
    m_startTime = other.m_startTime;
    m_dateOfSession = other.m_dateOfSession;
    m_matchmakingServer = other.m_matchmakingServer;
    m_hasMatchmakingServer = other.m_hasMatchmakingServer;
    m_sessionSubscriptionGuid = other.m_sessionSubscriptionGuid;
    m_branch = other.m_branch;
    m_changeNumber = other.m_changeNumber;
    m_writeSessionStatus = other.m_writeSessionStatus;
    m_nextTimer = other.m_nextTimer;
    m_initializationStage = other.m_initializationStage;
    m_initializationStageStartTime = other.m_initializationStageStartTime;
    m_initializationEpisode = other.m_initializationEpisode;
    m_hostCandidate = other.m_hostCandidate;
    m_tournamentsServer = other.m_tournamentsServer;
    m_arbitrationServer = other.m_arbitrationServer;

    m_memberCurrentUser = other.m_memberCurrentUser == nullptr ? nullptr : other.m_memberCurrentUser->_Create_deep_copy();
    m_sessionRequest = other.m_sessionRequest == nullptr ? nullptr : other.m_sessionRequest->create_deep_copy();
    m_members = std::vector<std::shared_ptr<multiplayer_session_member>>();

    bool lookForMe = true;
    for (const auto& member : other.m_members)
    {
        std::shared_ptr<multiplayer_session_member> memberCopy = std::make_shared<multiplayer_session_member>();
        memberCopy = member->_Create_deep_copy();
        memberCopy->_Set_session_request(m_sessionRequest);
        if (lookForMe)
        {
            bool isMe =
                utils::str_icmp(memberCopy->xbox_user_id(), m_xboxUserId) == 0;
            if (isMe)
            {
                lookForMe = false;
                std::shared_ptr<multiplayer_session_member_request> memberRequest = m_sessionRequest->add_member_request(
                    false,
                    isMe,
                    m_xboxUserId,
                    web::json::value(),
                    false,
                    false
                    );

                memberCopy->_Set_member_request(memberRequest);
                memberCopy->_Set_is_current_user(true);
                memberCopy->_Set_current_user_status(member->status());
                m_memberCurrentUser = memberCopy;
            }
        }
        m_members.push_back(memberCopy);
    }

    m_multiplayerSessionProperties->_Deep_copy(*(other.m_multiplayerSessionProperties));
    m_multiplayerSessionProperties->_Initialize(
        m_sessionRequest,
        m_members
        );
}

void multiplayer_session::_Initialize(
    _In_ string_t xboxUserId
    )
{
    m_xboxUserId = std::move(xboxUserId);
    m_servers = web::json::value::object();
    m_sessionConstants = std::make_shared<multiplayer_session_constants>();
    m_multiplayerSessionProperties = std::make_shared<multiplayer_session_properties>();
    m_sessionRoleTypes = std::make_shared<multiplayer_session_role_types>();
    m_sessionRequest = std::make_shared<multiplayer_session_request>(m_sessionConstants);

    m_multiplayerSessionProperties->_Initialize(
        m_sessionRequest,
        m_members
        );

    ensure_session_subscription_id_initialized();
}

void 
multiplayer_session::_Initialize_after_deserialize(
    _In_ string_t eTag,
    _In_ string_t responseDate,
    _In_ multiplayer_session_reference sessionReference,
    _In_ string_t xboxUserId
    )
{
    m_eTag = std::move(eTag);
    m_dateOfSession = utility::datetime::from_string(responseDate, utility::datetime::date_format::RFC_1123);
    m_sessionReference = std::move(sessionReference);
    m_xboxUserId = std::move(xboxUserId);
    m_sessionRequest->set_session_reference(m_sessionReference);

    ensure_session_subscription_id_initialized();
    if (!m_members.empty())
    {
        bool lookForMe = true;
        for (auto& member : m_members)
        {
            member->_Set_session_request(m_sessionRequest);
            if (lookForMe)
            {
                bool isMe = 
                    utils::str_icmp(member->xbox_user_id(), m_xboxUserId) == 0;
                if (isMe)
                {
                    lookForMe = false;
                    std::shared_ptr<multiplayer_session_member_request> memberRequest = m_sessionRequest->add_member_request(
                        false,
                        isMe,
                        m_xboxUserId,
                        web::json::value(),
                        false,
                        false
                        );

                    member->_Set_member_request(memberRequest);
                    member->_Set_is_current_user( true );
                    m_memberCurrentUser = member;
                }
            }
        }
    }
}

std::shared_ptr<multiplayer_session_request> 
multiplayer_session::_Session_request() const
{
    return m_sessionRequest;
}

const string_t&
multiplayer_session::multiplayer_correlation_id() const
{
    return m_correlationId;
}

const string_t&
multiplayer_session::search_handle_id() const
{
    return m_searchHandleId;
}

tournament_arbitration_status
multiplayer_session::arbitration_status() const
{
    return m_arbitrationStatus;
}

utility::datetime
multiplayer_session::start_time() const
{
    return m_startTime;
}

utility::datetime
multiplayer_session::date_of_next_timer() const
{
    return m_nextTimer;
}

utility::datetime
multiplayer_session::date_of_session() const
{
    return m_dateOfSession;
}

multiplayer_initialization_stage
multiplayer_session::initialization_stage() const
{
    return m_initializationStage;
}

utility::datetime
multiplayer_session::initializing_stage_start_time() const
{
    return m_initializationStageStartTime;
}

uint32_t 
multiplayer_session::intializing_episode() const
{
    return m_initializationEpisode;
}

multiplayer_session_change_types 
multiplayer_session::subscribed_change_types() const
{
    if (m_memberCurrentUser == nullptr)
    {
        return multiplayer_session_change_types::none;
    }
    return m_memberCurrentUser->_Subscribed_change_types();
}

std::vector<string_t>
multiplayer_session::host_candidates() const
{
    return m_hostCandidate;
}

const multiplayer_session_reference&
multiplayer_session::session_reference() const
{
    return m_sessionReference;
}


std::shared_ptr<multiplayer_session_constants>
multiplayer_session::session_constants() const
{
    return m_sessionConstants;
}

std::shared_ptr<multiplayer_session_properties>
multiplayer_session::session_properties() const
{
    return m_multiplayerSessionProperties;
}

std::shared_ptr<multiplayer_session_role_types>
multiplayer_session::session_role_types() const
{
    return m_sessionRoleTypes;
}

const std::vector<std::shared_ptr<multiplayer_session_member>>&
multiplayer_session::members() const
{
    return m_members;
}

const multiplayer_session_matchmaking_server& 
multiplayer_session::matchmaking_server() const
{
    return m_matchmakingServer;
}

const multiplayer_session_tournaments_server& 
multiplayer_session::tournaments_server() const
{
    return m_tournamentsServer;
}

const multiplayer_session_arbitration_server& 
multiplayer_session::arbitration_server() const
{
    return m_arbitrationServer;
}

uint32_t
multiplayer_session::members_accepted() const
{
    return m_membersAccepted;
}

const web::json::value&
multiplayer_session::servers_json() const
{
    return m_servers; 
}

void
multiplayer_session::set_servers_json(
    _In_ const web::json::value& serversJson
    )
{
    m_servers = serversJson;
    m_sessionRequest->set_servers(m_servers);
}

const string_t&
multiplayer_session::e_tag() const
{
    return m_eTag;
}

std::shared_ptr<multiplayer_session_member>
multiplayer_session::current_user() const
{
    return m_memberCurrentUser;
}

const string_t&
multiplayer_session::branch() const
{
    return m_branch;
}

uint64_t
multiplayer_session::change_number() const
{
    return m_changeNumber;
}

write_session_status
multiplayer_session::write_status() const
{
    return m_writeSessionStatus;
}

void
multiplayer_session::_Set_write_session_status(
    int32_t httpStatusCode
    )
{
    m_writeSessionStatus = convert_http_status_to_write_session_status(httpStatusCode);
}

std::error_code
multiplayer_session::add_member_reservation(
    _In_ const string_t& xboxUserId,
    _In_ const web::json::value& memberCustomConstantsJson
    )
{
    return add_member_reservation_helper(
        xboxUserId,
        memberCustomConstantsJson,
        false,
        false
        );
}

std::error_code
multiplayer_session::add_member_reservation(
    _In_ const string_t& xboxUserId,
    _In_ const web::json::value& memberCustomConstantsJson,
    _In_ bool initializeRequested
    )
{
    return add_member_reservation_helper(
        xboxUserId,
        memberCustomConstantsJson,
        true,
        initializeRequested
        );
}

xbox_live_result<std::shared_ptr<multiplayer_session_member>>
multiplayer_session::join(
    _In_ const web::json::value& memberCustomConstantsJson,
    _In_ bool initializeRequested,
    _In_ bool joinWithActiveStatus,
    _In_ bool addInitializePropertyToRequest
    )
{
    return join_helper(
        memberCustomConstantsJson,
        addInitializePropertyToRequest,
        initializeRequested,
        joinWithActiveStatus
        );
}

void
multiplayer_session::set_visibility(
    _In_ multiplayer_session_visibility visibility
    )
{
    m_sessionConstants->set_visibility(visibility);
}

void
multiplayer_session::set_max_members_in_session(
    _In_ uint32_t maxMembersInSession
    )
{
    m_sessionConstants->set_max_members_in_session(maxMembersInSession);
}


std::error_code
multiplayer_session::set_timeouts(
    _In_ std::chrono::milliseconds memberReservedTimeout,
    _In_ std::chrono::milliseconds memberInactiveTimeout,
    _In_ std::chrono::milliseconds memberReadyTimeout,
    _In_ std::chrono::milliseconds sessionEmptyTimeout
    )
{
    // Call set_timeouts/SetTimeouts before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }
    
    m_sessionConstants->_Set_timeouts(
        memberReservedTimeout,
        memberInactiveTimeout,
        memberReadyTimeout,
        sessionEmptyTimeout
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_arbitration_timeouts(
    _In_ std::chrono::milliseconds arbitrationTimeout,
    _In_ std::chrono::milliseconds forfeitTimeout
    )
{
    // Call set_arbitration_timeouts/SetArbitrationTimeouts before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_arbitration_timeouts(
        arbitrationTimeout,
        forfeitTimeout
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_quality_of_service_connectivity_metrics(
    _In_ bool enableLatencyMetric,
    _In_ bool enableBandwidthDownMetric,
    _In_ bool enableBandwidthUpMetric,
    _In_ bool enableCustomMetric
    )
{
    // Call set_quality_of_service_connectivity_metrics/SetQualityOfServiceConnectivityMetrics 
    // before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_quality_of_service_connectivity_metrics(
        enableLatencyMetric,
        enableBandwidthDownMetric,
        enableBandwidthUpMetric,
        enableCustomMetric
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_managed_initialization(
    _In_ std::chrono::milliseconds joinTimeout,
    _In_ std::chrono::milliseconds measurementTimeout,
    _In_ std::chrono::milliseconds evaluationTimeout,
    _In_ bool autoEvaluate,
    _In_ uint32_t membersNeededToStart
    )
{
    // Call set_managed_initialization/SetManagedInitialization before writing a new session to the service");
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_managed_initialization(
        joinTimeout,
        measurementTimeout,
        evaluationTimeout,
        autoEvaluate,
        membersNeededToStart
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_member_initialization(
    _In_ std::chrono::milliseconds joinTimeout,
    _In_ std::chrono::milliseconds measurementTimeout,
    _In_ std::chrono::milliseconds evaluationTimeout,
    _In_ bool externalEvaluation,
    _In_ uint32_t membersNeededToStart
)
{
    // Call set_member_initialization/SetMemberInitialization before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_member_initialization(
        joinTimeout,
        measurementTimeout,
        evaluationTimeout,
        externalEvaluation,
        membersNeededToStart
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_peer_to_peer_requirements(
    _In_ std::chrono::milliseconds latencyMaximum,
    _In_ uint32_t bandwidthMinimumInKilobitsPerSecond
    )
{
    // Call set_peer_to_peer_requirements/SetPeerToPeerRequirements before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_peer_to_peer_requirements(
        std::move(latencyMaximum),
        bandwidthMinimumInKilobitsPerSecond
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_peer_to_host_requirements(
    _In_ std::chrono::milliseconds latencyMaximum,
    _In_ uint32_t bandwidthDownMinimumInKilobitsPerSecond,
    _In_ uint32_t bandwidthUpMinimumInKilobitsPerSecond,
    _In_ multiplay_metrics hostSelectionMetric
    )
{
    // Call set_peer_to_host_requirements/SetPeerToHostRequirements before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_peer_to_host_requirements(
        std::move(latencyMaximum),
        bandwidthDownMinimumInKilobitsPerSecond,
        bandwidthUpMinimumInKilobitsPerSecond,
        hostSelectionMetric
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_measurement_server_addresses(
    _In_ const std::vector<xbox::services::game_server_platform::quality_of_service_server>& measurementServerAddresses
    )
{
    // Call set_measurement_server_addresses/SetMeasurementServerAddresses
    // before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_measurement_server_addresses(measurementServerAddresses);

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_session_capabilities(
    _In_ const multiplayer_session_capabilities& capabilities
    )
{
    // Call set_session_capabilities/SetSessionCapabilities before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_session_capabilities(
        capabilities
        );

    return xbox_live_error_code::no_error;
}

std::error_code 
multiplayer_session::set_cloud_compute_package_json(
    _In_ web::json::value sessionCloudComputePackageConstantsJson
    )
{
    // Call set_cloud_compute_package_json/SetCloudComputePackageJson before writing a new session to the service
    if (!m_newSession)
    {
        return xbox_live_error_code::logic_error;
    }

    m_sessionConstants->_Set_cloud_compute_package_json(sessionCloudComputePackageConstantsJson);

    return xbox_live_error_code::no_error;
}

void
multiplayer_session::set_initialization_status(
    _In_ bool initializationSucceeded
    )
{
    m_sessionRequest->set_write_initialization_status(true);
    m_sessionRequest->set_initialization_succeeded(initializationSucceeded);
}


void
multiplayer_session::set_host_device_token(
    _In_ const string_t& hostDeviceToken
    )
{
    m_sessionRequest->set_write_host_device_token(true);
    m_sessionRequest->set_host_device_token(hostDeviceToken);
}


void
multiplayer_session::set_matchmaking_server_connection_path(
    _In_ const string_t& serverConnectionPath
    )
{
    m_sessionRequest->set_write_matchmaking_server_connection_path(true);
    m_sessionRequest->set_matchmaking_server_connection_path(serverConnectionPath);
}

void
multiplayer_session::set_matchmaking_resubmit(
    _In_ bool matchResubmit
    )
{
    m_sessionRequest->set_write_matchmaking_resubmit(true);
    m_sessionRequest->set_matchmaking_match_resubmit(matchResubmit);
}

void
multiplayer_session::set_closed(
    _In_ bool closed
    )
{
    m_sessionRequest->set_write_closed(true);
    m_sessionRequest->set_closed(closed);
}

void
multiplayer_session::set_locked(
    _In_ bool locked
    )
{
    m_sessionRequest->set_write_locked(true);
    m_sessionRequest->set_locked(locked);
}

void 
multiplayer_session::set_allocate_cloud_compute(
    _In_ bool allocateCloudCompute
    )
{
    m_sessionRequest->set_write_allocate_cloud_compute(true);
    m_sessionRequest->set_allocate_cloud_compute(allocateCloudCompute);
}

void
multiplayer_session::set_server_connection_string_candidates(
    _In_ const std::vector<string_t>& serverConnectionStringCandidates
    )
{
    m_sessionRequest->set_write_server_connection_string_candidates(true);
    m_sessionRequest->set_server_connection_string_candidates(serverConnectionStringCandidates);
}

std::error_code 
multiplayer_session::set_session_change_subscription(
    _In_ multiplayer_session_change_types changeTypes
    )
{
    if(m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::logic_error;
    }
    
    m_memberCurrentUser->_Set_session_change_subscription(changeTypes, m_sessionSubscriptionGuid);
    
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::leave()
{
    // Failed trying to leave and join the session at the same time
    if (m_joiningSession)
    {
        return xbox_live_error_code::logic_error;
    }

    for (uint32_t i = 0; i < m_members.size(); ++i)
    {
        std::shared_ptr<multiplayer_session_member> member = m_members.at(i);
        if (member->is_current_user())
        {
            m_members.erase((m_members.begin() + i));
            break;
        }
    }

    m_sessionRequest->leave_session();
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_current_user_status(
    _In_ multiplayer_session_member_status status
    )
{
    // Must join the session first before calling SetCurrentUserStatus
    // Can not set member to ready
    // Can not set member to reserved.  Use AddMemberReservation instead
    if (m_memberCurrentUser == nullptr || status == multiplayer_session_member_status::ready || status == multiplayer_session_member_status::reserved)
    {
        return xbox_live_error_code::logic_error;
    }

    m_memberCurrentUser->_Set_current_user_status(status);
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_current_user_secure_device_address_base64(
    _In_ const string_t& value
    )
{
    // Must join the session first before calling SetCurrentUserSecureDeviceAddressBase64
    if (m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::logic_error;
    }

    m_memberCurrentUser->_Set_secure_device_base_address64(value);
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_current_user_role_info(
    _In_ const std::unordered_map<string_t, string_t>& roles
    )
{
    // Must join the session first before calling SetCurrentUserRoleInfo
    if (m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::logic_error;
    }

    m_memberCurrentUser->_Set_role_info(roles);
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_mutable_role_settings(
    _In_ const std::unordered_map<string_t, multiplayer_role_type>& roleTypes
    )
{
    m_sessionRequest->set_mutable_role_settings(roleTypes);
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_current_user_members_in_group(
    _In_ const std::vector<std::shared_ptr<multiplayer_session_member>>& membersInGroup
    )
{
    // Must join the session first before calling SetCurrentUserMembersInGroup
    if (m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::logic_error;
    }

    m_memberCurrentUser->_Set_current_user_members_in_group(membersInGroup);
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_current_user_quality_of_service_measurements(
    _In_ std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>> measurements
    )
{
    // Must join the session first before calling SetCurrentUserQualityOfServiceMeasurements
    if (m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::logic_error;
    }

    m_memberCurrentUser->_Set_current_user_quality_of_service_measurements(
        measurements
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_current_user_quality_of_service_measurements_json(
    _In_ const web::json::value& serverMeasurementsJson
    )
{
    // Must join the session first before calling SetCurrentUserQualityOfServiceServerMeasurementsJson
    if (m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::logic_error;
    }

    m_memberCurrentUser->_Set_current_user_quality_of_service_server_measurements_json(serverMeasurementsJson);
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_current_user_member_custom_property_json(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson
    )
{
    if (name.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }

    // Must join the session first before calling SetCurrentUserMemberCustomPropertyJson
    if (m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::invalid_argument;
    }
    m_memberCurrentUser->_Set_member_custom_property_json(name, valueJson);

    return xbox_live_error_code::no_error;
}

std::error_code 
multiplayer_session::set_current_user_member_arbitration_results(
    _In_ const std::unordered_map<string_t, tournament_team_result>& results
    )
{
    // Must join the session first before calling set_current_user_member_arbitration_result
    if (m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::logic_error;
    }

    m_memberCurrentUser->_Set_arbitration_results(results);
    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::delete_current_user_member_custom_property_json(
    _In_ const string_t& name
    )
{
    if (name.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }

    // Must join the session first before calling DeleteCurrentUserMemberCustomPropertyJson
    if (m_memberCurrentUser == nullptr)
    {
        return xbox_live_error_code::logic_error;
    }
    m_memberCurrentUser->_Delete_custom_property_json(name);

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_matchmaking_target_session_constants_json(
    _In_ web::json::value matchmakingTargetSessionConstantsJson
    )
{
    m_multiplayerSessionProperties->_Set_matchmaking_target_session_constants_json(
        matchmakingTargetSessionConstantsJson
        );

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::set_session_custom_property_json(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson
    )
{
    if (name.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }
    m_multiplayerSessionProperties->_Set_session_custom_property_json(name, valueJson);

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::delete_session_custom_property_json(
    _In_ const string_t& name
    )
{
    if (name.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }

    m_multiplayerSessionProperties->_Delete_session_custom_property_json(name);

    return xbox_live_error_code::no_error;
}

std::error_code
multiplayer_session::add_member_reservation_helper(
    _In_ const string_t& xboxUserId,
    _In_ const web::json::value& memberCustomConstantsJson,
    _In_ bool addInitializePropertyToRequest,
    _In_ bool initializeRequested
    )
{
    if (xboxUserId.empty())
    {
        return xbox_live_error_code::invalid_argument;
    }
    web::json::value customConstantsJson;
    if (!memberCustomConstantsJson.is_null())
    {
        customConstantsJson = memberCustomConstantsJson;
    }

    std::shared_ptr<multiplayer_session_member_request> memberRequest = m_sessionRequest->add_member_request(
        true,
        false,
        xboxUserId,
        customConstantsJson,
        addInitializePropertyToRequest,
        initializeRequested
        );

    std::shared_ptr<multiplayer_session_member> member = std::make_shared<multiplayer_session_member>(
        false,
        0,
        xboxUserId,
        customConstantsJson
        );

    member->_Set_member_request(memberRequest);
    member->_Set_session_request(m_sessionRequest);
    m_members.push_back(member);

    return xbox_live_error_code::no_error;
}

xbox_live_result<std::shared_ptr<multiplayer_session_member>>
multiplayer_session::join_helper(
    _In_ web::json::value memberCustomConstantsJson,
    _In_ bool addInitializePropertyToRequest,
    _In_ bool initializeRequested,
    _In_ bool joinWithActiveStatus
    )
{
    if (m_joiningSession)
    {
        return xbox_live_result<std::shared_ptr<multiplayer_session_member>>(xbox_live_error_code::logic_error, "Failed trying to join the session more than once");
    }

    m_joiningSession = true;

    web::json::value customConstantsJson = web::json::value::null();
    if (!memberCustomConstantsJson.is_null())
    {
        customConstantsJson = memberCustomConstantsJson;
    }

    std::shared_ptr<multiplayer_session_member_request> memberRequest = m_sessionRequest->add_member_request(
        true,
        true,
        m_xboxUserId,
        customConstantsJson,
        addInitializePropertyToRequest,
        initializeRequested
        );

    if (joinWithActiveStatus)
    {
        memberRequest->set_is_active(true);
        memberRequest->set_write_is_active(true);
    }
    
    std::shared_ptr<multiplayer_session_member> member = std::make_shared<multiplayer_session_member>(
        true,
        0,
        m_xboxUserId,
        customConstantsJson
        );

    member->_Set_member_request(memberRequest);
    member->_Set_session_request(m_sessionRequest);
    m_members.push_back(member);
    m_memberCurrentUser = member;

    return xbox_live_result<std::shared_ptr<multiplayer_session_member>>(member);
}

void 
multiplayer_session::ensure_session_subscription_id_initialized()
{
    if (m_sessionSubscriptionGuid.empty())
    {
        m_sessionSubscriptionGuid = utils::string_t_from_internal_string(utils::create_guid(true));
    }
}

multiplay_metrics
multiplayer_session::_Convert_string_to_multiplayer_host_selection_metric(
    _In_ const string_t& value
    )
{
    if (value.empty())
    {
        return multiplay_metrics::latency;
    }
    else if (utils::str_icmp(value, _T("bandwidthUp")) == 0)
    {
        return multiplay_metrics::bandwidth_up;
    }
    else if (utils::str_icmp(value, _T("bandwidthDown")) == 0)
    {
        return multiplay_metrics::bandwidth_down;
    }
    else if (utils::str_icmp(value, _T("bandwidth")) == 0)
    {
        return multiplay_metrics::bandwidth;
    }
    else if (utils::str_icmp(value, _T("latency")) == 0)
    {
        return multiplay_metrics::latency;
    }

    return multiplay_metrics::unknown;
}

const xbox_live_result<string_t>
multiplayer_session::_Convert_multiplayer_host_selection_metric_to_string(
    _In_ multiplay_metrics multiplayMetric
    )
{
    switch (multiplayMetric)
    {
        case multiplay_metrics::unknown: return xbox_live_result<string_t>(_T("unknown"));

        case multiplay_metrics::bandwidth_up: return xbox_live_result<string_t>(_T("bandwidthUp"));

        case multiplay_metrics::bandwidth_down: return xbox_live_result<string_t>(_T("bandwidthDown"));

        case multiplay_metrics::bandwidth: return xbox_live_result<string_t>(_T("bandwidth"));

        case multiplay_metrics::latency: return xbox_live_result<string_t>(_T("latency"));

        default: return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Enum out of range");
    }
}

multiplayer_initialization_stage
multiplayer_session::_Convert_string_to_multiplayer_initialization_stage(
    _In_ const string_t& value
)
{
    if (value.empty())
    {
        return multiplayer_initialization_stage::none;
    }
    else if (utils::str_icmp(value, _T("joining")) == 0)
    {
        return multiplayer_initialization_stage::joining;
    }
    else if (utils::str_icmp(value, _T("failed")) == 0)
    {
        return multiplayer_initialization_stage::failed;
    }
    else if (utils::str_icmp(value, _T("evaluating")) == 0)
    {
        return multiplayer_initialization_stage::evaluating;
    }
    else if (utils::str_icmp(value, _T("measuring")) == 0)
    {
        return multiplayer_initialization_stage::measuring;
    }

    return multiplayer_initialization_stage::unknown;
}

xbox_live_result<matchmaking_status>
multiplayer_session::_Convert_string_to_matchmaking_status(
    _In_ const string_t& value
    )
{
    if (value.empty())
    {
        return xbox_live_result<matchmaking_status>(xbox_live_error_code::invalid_argument, "Value was empty");
    }

    if (utils::str_icmp(value, _T("searching")) == 0)
    {
        return xbox_live_result<matchmaking_status>(matchmaking_status::searching);
    }
    else if (utils::str_icmp(value, _T("expired")) == 0)
    {
        return xbox_live_result<matchmaking_status>(matchmaking_status::expired);
    }
    else if (utils::str_icmp(value, _T("found")) == 0)
    {
        return xbox_live_result<matchmaking_status>(matchmaking_status::found);
    }
    else if (utils::str_icmp(value, _T("canceled")) == 0)
    {
        return xbox_live_result<matchmaking_status>(matchmaking_status::canceled);
    }

    return xbox_live_result<matchmaking_status>(matchmaking_status::unknown);
}

const xbox_live_result<string_t>
multiplayer_session::_Convert_matchmaking_status_to_string(
    _In_ matchmaking_status matchmakingStatus
    )
{
    switch (matchmakingStatus)
    {
        case matchmaking_status::unknown: return xbox_live_result<string_t>(_T("unknown"));
             
        case matchmaking_status::searching: return xbox_live_result<string_t>(_T("searching"));
             
        case matchmaking_status::expired: return xbox_live_result<string_t>(_T("expired"));
             
        case matchmaking_status::found: return xbox_live_result<string_t>(_T("found"));
             
        case matchmaking_status::canceled: return xbox_live_result<string_t>(_T("canceled"));

        default: return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Enum out of range");
    }
}

write_session_status
multiplayer_session::convert_http_status_to_write_session_status(
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

std::vector<std::shared_ptr<multiplayer_session_member>>
multiplayer_session::_Deserialize_me_member(
    _In_ const web::json::value& json,
    _In_ std::error_code& errc
    )
{
    web::json::value membersInfo = utils::extract_json_field(json, _T("membersInfo"), errc, true);
    uint32_t first = utils::extract_json_int(membersInfo, _T("first"), errc, true);
    web::json::value memberJson = utils::extract_json_field(json, _T("members"), errc, true);
    uint32_t current = first;

    std::vector<std::shared_ptr<multiplayer_session_member>> members;

    auto member = multiplayer_session_member::_Deserialize(
        utils::extract_json_field(
            memberJson, 
            _T("me"),
            errc,
            true
            )
        );

    if (member.err())
    {
        errc = member.err();
    }
    std::shared_ptr<multiplayer_session_member> meMember = std::make_shared<multiplayer_session_member>(
        member.payload()
        );  

    meMember->_Set_member_id(current);

    members.push_back(meMember);

    return members;
}

std::vector<std::shared_ptr<multiplayer_session_member>>
multiplayer_session::_Deserialize_members_list(
    _In_ const web::json::value& json,
    _In_ std::error_code& errc
    )
{
    web::json::value membersInfo = utils::extract_json_field(json, _T("membersInfo"), errc, false);
    uint32_t first = utils::extract_json_int(membersInfo, _T("first"), errc, false);
    uint32_t count = utils::extract_json_int(membersInfo, _T("count"), errc, false);

    std::vector<std::shared_ptr<multiplayer_session_member>> members;
    uint32_t current = first;
    web::json::value membersJson = utils::extract_json_field(json, _T("members"), errc, false);
    for (uint32_t i = 0; i < count; i++)
    {
        stringstream_t stream;
        stream << current;
        web::json::value memberJsonIndex = utils::extract_json_field(membersJson, stream.str(), errc, true);
        
        auto member = multiplayer_session_member::_Deserialize(memberJsonIndex);
        if (member.err())
        {
            errc = member.err();
        }

        std::shared_ptr<multiplayer_session_member> currentMember = std::make_shared<multiplayer_session_member>(
            member.payload()
            );

        currentMember->_Set_member_id(current);
        members.push_back(currentMember);

        uint32_t next = utils::extract_json_int(memberJsonIndex, _T("next"), errc, false, current);
        if (next == current)
        {
            break;
        }

        current = next;
    }

    return members;
}

std::error_code 
multiplayer_session::_Populate_members_with_members_list(
    _In_ std::vector<std::shared_ptr<multiplayer_session_member>> members
    )
{
    std::error_code errc;
    for (auto& member : members)
    {
        errc = member->set_members_list(members);
    }
    return errc;
}

xbox_live_result<multiplayer_session_change_types>
multiplayer_session::compare_multiplayer_sessions(
    _In_ std::shared_ptr<multiplayer_session> currentSession,
    _In_ std::shared_ptr<multiplayer_session> oldSession
    )
{
    if (currentSession == nullptr || oldSession == nullptr)
    {
        return xbox_live_result<multiplayer_session_change_types>(xbox_live_error_code::invalid_argument, "Cannot compare a null session");
    }

    uint32_t currentType = static_cast<uint32_t>(multiplayer_session_change_types::none);

    if (utils::str_icmp(currentSession->session_properties()->host_device_token(), oldSession->session_properties()->host_device_token()) != 0)
    {
        currentType |= multiplayer_session_change_types::host_device_token_change;
    }

    if (currentSession->initialization_stage() != oldSession->initialization_stage())
    {
        currentType |= multiplayer_session_change_types::initialization_state_change;
    }

    auto currentMatchmakingServer = currentSession->matchmaking_server();
    auto oldMatchmakingServer = oldSession->matchmaking_server();
    if ((currentMatchmakingServer.is_null() != oldMatchmakingServer.is_null()) ||
        (!currentMatchmakingServer.is_null() && currentMatchmakingServer.status() != oldMatchmakingServer.status()) ||
        (!currentMatchmakingServer.is_null() && 
            !currentMatchmakingServer.target_session_ref().is_null() &&
            !oldMatchmakingServer.target_session_ref().is_null() &&
            !_Do_session_references_match(currentMatchmakingServer.target_session_ref(), oldMatchmakingServer.target_session_ref()))
        )
    {
        currentType |= multiplayer_session_change_types::matchmaking_status_change;
    }

    bool hasMemberChanged = false;
    bool memberStatusChanged = false;
    bool memberCustomPropertyChanged = false;

    if (currentSession->members().size() != oldSession->members().size())
    {
        hasMemberChanged = true;
    }

    for (uint32_t i = 0; i < currentSession->members().size(); i++)
    {
        std::shared_ptr<multiplayer_session_member> currentSessionMember = currentSession->members()[i];
        bool isMemberFound = false;
        for (uint32_t j = 0; j < oldSession->members().size(); j++)
        {
            std::shared_ptr<multiplayer_session_member> olderSessionMember = oldSession->members()[j];

            if (utils::str_icmp(currentSessionMember->xbox_user_id(),
                olderSessionMember->xbox_user_id()) == 0)
            {
                isMemberFound = true;

                if (currentSessionMember->status() != olderSessionMember->status())
                {
                    memberStatusChanged = true;
                }

                if (utils::str_icmp(currentSessionMember->member_custom_properties_json().serialize(),
                    olderSessionMember->member_custom_properties_json().serialize()) != 0)
                {
                    memberCustomPropertyChanged = true;
                }
            }
        }

        if (!isMemberFound)
        {
            hasMemberChanged = true;
        }

        if (memberStatusChanged && hasMemberChanged && memberCustomPropertyChanged)
        {
            break;
        }
    }

    if (hasMemberChanged)
    {
        currentType |= multiplayer_session_change_types::member_list_change;
    }

    if (memberStatusChanged)
    {
        currentType |= multiplayer_session_change_types::member_status_change;
    }

    if (memberCustomPropertyChanged)
    {
        currentType |= multiplayer_session_change_types::member_custom_property_change;
    }
    

    if (currentSession->session_properties()->closed() != oldSession->session_properties()->closed() ||
        currentSession->session_properties()->locked() != oldSession->session_properties()->locked() ||
        currentSession->session_properties()->join_restriction() != oldSession->session_properties()->join_restriction() ||
        (currentSession->members().size() == currentSession->session_constants()->max_members_in_session()) != 
        (oldSession->members().size() == oldSession->session_constants()->max_members_in_session())     // if the session is open again or closed again because the max member has changed
        )
    {
        currentType |= multiplayer_session_change_types::session_joinability_change;
    }

    if (utils::str_icmp(currentSession->session_properties()->session_custom_properties_json().serialize(), oldSession->session_properties()->session_custom_properties_json().serialize()) != 0)
    {
        currentType |= multiplayer_session_change_types::custom_property_change;
    }

    if (!currentSession->tournaments_server()._Is_same(oldSession->tournaments_server()))
    {
        currentType |= multiplayer_session_change_types::tournament_property_change;
    }

    if (!currentSession->arbitration_server()._Is_same(oldSession->arbitration_server()))
    {
        currentType |= multiplayer_session_change_types::arbitration_property_change;
    }

    return xbox_live_result<multiplayer_session_change_types>(static_cast<multiplayer_session_change_types>(currentType));
}

bool
multiplayer_session::_Do_session_references_match(
    _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef1,
    _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef2
    )
{
    return  utils::str_icmp(sessionRef1.service_configuration_id(), sessionRef2.service_configuration_id()) == 0 &&
        utils::str_icmp(sessionRef1.session_template_name(), sessionRef2.session_template_name()) == 0 &&
        utils::str_icmp(sessionRef1.session_name(), sessionRef2.session_name()) == 0;
}

xbox_live_result<multiplayer_session>
multiplayer_session::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_session returnResult;
    if ( json.is_null() ) return xbox_live_result<multiplayer_session>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value initializingJson = utils::extract_json_field(json, _T("initializing"), errc, false);
    web::json::value memberInfoJson = utils::extract_json_field(json, _T("membersInfo"), errc, false);

    returnResult.m_correlationId = utils::extract_json_string(json, _T("correlationId"), errc);
    returnResult.m_searchHandleId = utils::extract_json_string(json, _T("searchHandle"), errc, false);
    returnResult.m_startTime = utils::extract_json_time(json, _T("startTime"), errc);

    auto arbitrationJson = utils::extract_json_field(json, _T("arbitration"), errc, false);
    if (!arbitrationJson.is_null())
    {
        returnResult.m_arbitrationStatus = multiplayer_service::_Convert_string_to_arbitration_status(utils::extract_json_string(arbitrationJson, _T("status"), errc, false));
    }

    returnResult.m_branch = utils::extract_json_string(json, _T("branch"), errc);
    returnResult.m_changeNumber = utils::extract_json_int(json, _T("changeNumber"), errc, false, ULONG_MAX);

    auto sessionConstants = multiplayer_session_constants::_Deserialize(
        utils::extract_json_field(
            json, 
            _T("constants"),
            errc,
            true
            )
        );

    if (sessionConstants.err())
    {
        errc = sessionConstants.err();
    }
    returnResult.m_sessionConstants = std::make_shared<multiplayer_session_constants>(
        sessionConstants.payload()
        );

    returnResult.m_nextTimer = utils::extract_json_time(json, _T("nextTimer"), errc);
    returnResult.m_initializationStage = _Convert_string_to_multiplayer_initialization_stage(utils::extract_json_string(initializingJson, _T("stage"), errc));
    returnResult.m_initializationStageStartTime = utils::extract_json_time(initializingJson, _T("stageStartTime"), errc);
    returnResult.m_initializationEpisode = utils::extract_json_int(initializingJson, _T("episode"), errc);
    returnResult.m_hostCandidate = utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("hostCandidates"), errc, false);

    web::json::value jsonMembers = utils::extract_json_field(json, _T("members"), errc, false);
    if (jsonMembers.size() != 0)
    {
        if (returnResult.m_sessionConstants->capabilities_large())
        {
            returnResult.m_members = _Deserialize_me_member(json, errc);
        }
        else
        {
            returnResult.m_members = _Deserialize_members_list(json, errc);
        }
    }

    _Populate_members_with_members_list(returnResult.m_members);

    auto multiplayerSessionProperties = multiplayer_session_properties::_Deserialize(
        utils::extract_json_field(
            json, 
            _T("properties"), 
            errc,
            true
            )
        );

    if (multiplayerSessionProperties.err())
    {
        errc = multiplayerSessionProperties.err();
    }
    returnResult.m_multiplayerSessionProperties = std::make_shared<multiplayer_session_properties>(multiplayerSessionProperties.payload());

    returnResult.m_multiplayerSessionProperties->_Initialize(
        returnResult.m_sessionRequest,
        returnResult.m_members
        );
    returnResult.m_membersAccepted = utils::extract_json_int(memberInfoJson, _T("accepted"));

    auto sessionRoleTypes = multiplayer_session_role_types::_Deserialize(
        utils::extract_json_field(json, _T("roleTypes"), false)
        );

    if (sessionRoleTypes.err())
    {
        errc = sessionRoleTypes.err();
    }
    returnResult.m_sessionRoleTypes = std::make_shared<multiplayer_session_role_types>(sessionRoleTypes.payload());

    auto serversJson = utils::extract_json_field(json, _T("servers"), errc, false);
    auto serversMatchmakingJson = utils::extract_json_field(serversJson, _T("matchmaking"), errc, false);
    auto serversMatchmakingPropertiesJson = utils::extract_json_field(serversMatchmakingJson, _T("properties"), errc, false);

    if (!serversMatchmakingJson.is_null())
    {
        returnResult.m_matchmakingServer =
            multiplayer_session_matchmaking_server::_Deserialize(
                utils::extract_json_field(serversMatchmakingPropertiesJson, _T("system"), errc, false)
                ).payload();

        returnResult.m_hasMatchmakingServer = true;
    }

    auto serversArbitrationJson = utils::extract_json_field(serversJson, _T("arbitration"), errc, false);
    if (!serversArbitrationJson.is_null())
    {
        auto arbitrationServerResult = multiplayer_session_arbitration_server::_Deserialize(serversArbitrationJson);
        if (arbitrationServerResult.err())
        {
            errc = arbitrationServerResult.err();
        }
        returnResult.m_arbitrationServer = arbitrationServerResult.payload();
    }

    auto serversTournamentJson = utils::extract_json_field(serversJson, _T("tournaments"), errc, false);
    if (!serversTournamentJson.is_null())
    {
        auto tournamentServerResult = multiplayer_session_tournaments_server::_Deserialize(serversTournamentJson);
        if (tournamentServerResult.err())
        {
            errc = tournamentServerResult.err();
        }
        returnResult.m_tournamentsServer = tournamentServerResult.payload();

        if (returnResult.m_members.size() > 0 && returnResult.m_tournamentsServer.teams().size() > 0)
        {
            auto teamRefs = returnResult.m_tournamentsServer.teams();
            for (size_t i = 0; i < returnResult.m_members.size(); ++i)
            {
                auto member = returnResult.m_members[i];
                if (teamRefs.find(member->m_teamId) != teamRefs.end())
                {
                    member->m_tournamentTeamSessionRef = teamRefs[member->m_teamId];
                }
                else
                {
                    errc = xbox_live_error_code::json_error;
                }
            }
        }
    }

    returnResult.m_servers = std::move(serversJson);
    
    return xbox_live_result<multiplayer_session>(returnResult, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END