// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

using namespace xbox::services;
using namespace xbox::services::legacy;
using namespace xbox::services::multiplayer;

#define MULTIPLAYER_SESSION_VERSION 1

XblMultiplayerSession::XblMultiplayerSession(
    _In_ uint64_t xuid,
    _In_ XblMultiplayerSessionReference sessionReference,
    _In_ const xsapi_internal_string& eTag,
    _In_ const xsapi_internal_string& responseDate,
    _In_ const JsonValue& json
) :
    m_xuid(xuid),
    m_eTag(eTag),
    m_sessionRetrievedTime(),
    m_info(),
    m_sessionReference(sessionReference),
    m_sessionConstants{},
    m_memberInitialization(),
    m_sessionProperties(),
    m_memberCurrentUser(nullptr),
    m_membersAccepted(0),
    m_writeSessionStatus(),
    m_joiningSession(false),
    m_newSession(false),
    m_deserializationError(),
    m_writePropertiesKeywords(false),
    m_writePropertiesTurns(false),
    m_writeInitializationStatus(false),
    m_initializationSucceeded(false),
    m_writeHostDeviceToken(false),
    m_writeMatchmakingServerConnectionPath(false),
    m_writeMatchmakingResubmit(false),
    m_writeServerConnectionStringCandidates(false),
    m_leaveSession(false),
    m_writeClosed(false),
    m_writeLocked(false),
    m_writeAllocateCloudCompute(false),
    m_writeRoleTypes(false),
    m_writeTimeouts(false),
    m_writeQosConnectivityMetrics(false),
    m_writeMemberInitialization(false),
    m_writePeerToPeerRequirements(false),
    m_writePeerToHostRequirements(false),
    m_writeMeasurementServerAddresses(false),
    m_writeJoinRestriction(false),
    m_writeReadRestriction(false),
    m_writeServersJson(false),
    m_writeMatchmakingTargetSessionConstants(false),
    m_writeSessionCustomPropertiesJson(false),
    m_writeConstants(false),
    m_memberRequestIndex(0)
{
    auto sessionDatetime = xbox::services::datetime::from_string(responseDate, xbox::services::datetime::date_format::RFC_1123);
    m_sessionRetrievedTime = utils::time_t_from_datetime(sessionDatetime);
    Initialize();
    Deserialize(json);
}

XblMultiplayerSession::XblMultiplayerSession(
    _In_ uint64_t xuid,
    _In_opt_ const XblMultiplayerSessionReference* sessionReference,
    _In_opt_ const XblMultiplayerSessionInitArgs* initArgs
) :
    m_xuid(xuid),
    m_sessionReference{},
    m_sessionConstants{},
    m_memberCurrentUser(nullptr),
    m_membersAccepted(0),
    m_joiningSession(false),
    m_newSession(true),
    m_writePropertiesKeywords(false),
    m_writePropertiesTurns(false),
    m_writeInitializationStatus(false),
    m_initializationSucceeded(false),
    m_writeHostDeviceToken(false),
    m_writeMatchmakingServerConnectionPath(false),
    m_writeMatchmakingResubmit(false),
    m_writeServerConnectionStringCandidates(false),
    m_leaveSession(false),
    m_writeClosed(false),
    m_writeLocked(false),
    m_writeAllocateCloudCompute(false),
    m_writeRoleTypes(false),
    m_writeTimeouts(false),
    m_writeQosConnectivityMetrics(false),
    m_writeMemberInitialization(false),
    m_writePeerToPeerRequirements(false),
    m_writePeerToHostRequirements(false),
    m_writeMeasurementServerAddresses(false),
    m_writeJoinRestriction(false),
    m_writeReadRestriction(false),
    m_writeServersJson(false),
    m_writeMatchmakingTargetSessionConstants(false),
    m_writeSessionCustomPropertiesJson(false),
    m_writeConstants(false),
    m_memberRequestIndex(0)
{
    if (sessionReference != nullptr)
    {
        m_sessionReference = *sessionReference;
    }
    Initialize();
    if (initArgs != nullptr)
    {
        if (initArgs->CustomJson)
        {
            m_constantsCustomJson = initArgs->CustomJson;
            m_sessionConstants.CustomJson = m_constantsCustomJson.data();
        }
        m_sessionConstants.MaxMembersInSession = initArgs->MaxMembersInSession;
        m_sessionConstants.Visibility = initArgs->Visibility;
        if (initArgs->InitiatorXuids && initArgs->InitiatorXuidsCount > 0)
        {
            m_initiatorXuids = xsapi_internal_vector<uint64_t>(initArgs->InitiatorXuids, initArgs->InitiatorXuids + initArgs->InitiatorXuidsCount);
            m_sessionConstants.InitiatorXuids = m_initiatorXuids.data();
            m_sessionConstants.InitiatorXuidsCount = m_initiatorXuids.size();
        }
        m_writeConstants = true;
    }
}

XblMultiplayerSession::XblMultiplayerSession(const XblMultiplayerSession& other)
    : m_xuid(other.m_xuid),
    m_eTag(other.m_eTag),
    m_sessionRetrievedTime(other.m_sessionRetrievedTime),
    m_info(other.m_info),
    m_initialization(other.m_initialization),
    m_sessionReference(other.m_sessionReference),
    m_hostCandidates(other.m_hostCandidates),
    m_sessionConstants(other.m_sessionConstants),
    m_initiatorXuids(other.m_initiatorXuids),
    m_memberInitialization(other.m_memberInitialization),
    m_constantsCustomJson(other.m_constantsCustomJson),
    m_constantsCloudComputePackageJson(other.m_constantsCloudComputePackageJson),
    m_constantsMeasurementServerAddressesJson(other.m_constantsMeasurementServerAddressesJson),
    m_sessionProperties(other.m_sessionProperties),
    m_keywords(other.m_keywords),
    m_sessionOwnerIndices(other.m_sessionOwnerIndices),
    m_turnCollection(other.m_turnCollection),
    m_serverConnectionStringCandidates(other.m_serverConnectionStringCandidates),
    m_matchmakingServerConnectionString(other.m_matchmakingServerConnectionString),
    m_matchmakingTargetSessionConstantsJson(other.m_matchmakingTargetSessionConstantsJson),
    m_sessionCustomPropertiesJson(other.m_sessionCustomPropertiesJson),
    m_roleTypes(other.m_roleTypes),
    m_members(other.m_members),
    m_membersAccepted(other.m_membersAccepted),
    m_serversJson(other.m_serversJson),
    m_matchmakingStatusDetails(other.m_matchmakingStatusDetails),
    m_lastTeamResultTeam(other.m_lastTeamResultTeam),
    m_writeSessionStatus(other.m_writeSessionStatus),
    m_newSession(other.m_newSession),
    m_deserializationError(other.m_deserializationError),
    m_sessionSubscriptionGuid(other.m_sessionSubscriptionGuid),
    m_writePropertiesKeywords(other.m_writePropertiesKeywords),
    m_writePropertiesTurns(other.m_writePropertiesTurns),
    m_writeInitializationStatus(other.m_writeInitializationStatus),
    m_initializationSucceeded(other.m_initializationSucceeded),
    m_writeHostDeviceToken(other.m_writeHostDeviceToken),
    m_writeMatchmakingServerConnectionPath(other.m_writeMatchmakingServerConnectionPath),
    m_writeMatchmakingResubmit(other.m_writeMatchmakingResubmit),
    m_writeServerConnectionStringCandidates(other.m_writeServerConnectionStringCandidates),
    m_leaveSession(other.m_leaveSession),
    m_writeClosed(other.m_writeClosed),
    m_writeLocked(other.m_writeLocked),
    m_writeAllocateCloudCompute(other.m_writeAllocateCloudCompute),
    m_writeRoleTypes(other.m_writeRoleTypes),
    m_writeTimeouts(other.m_writeTimeouts),
    m_writeQosConnectivityMetrics(other.m_writeQosConnectivityMetrics),
    m_writeMemberInitialization(other.m_writeMemberInitialization),
    m_writePeerToPeerRequirements(other.m_writePeerToPeerRequirements),
    m_writePeerToHostRequirements(other.m_writePeerToHostRequirements),
    m_writeMeasurementServerAddresses(other.m_writeMeasurementServerAddresses),
    m_writeJoinRestriction(other.m_writeJoinRestriction),
    m_writeReadRestriction(other.m_writeReadRestriction),
    m_writeServersJson(other.m_writeServersJson),
    m_writeMatchmakingTargetSessionConstants(other.m_writeMatchmakingTargetSessionConstants),
    m_writeSessionCustomPropertiesJson(other.m_writeSessionCustomPropertiesJson),
    m_writeConstants(other.m_writeConstants),
    m_memberRequestIndex(other.m_memberRequestIndex)
{
    m_joiningSession.exchange(other.m_joiningSession);

    for (auto& candidate : m_serverConnectionStringCandidates)
    {
        candidate = Make(candidate);
    }

    for (auto& keyword : m_keywords)
    {
        keyword = Make(keyword);
    }
    m_sessionProperties.Keywords = m_keywords.data();
    m_sessionProperties.SessionOwnerMemberIds = m_sessionOwnerIndices.data();
    m_sessionProperties.TurnCollection = m_turnCollection.data();
    m_sessionProperties.ServerConnectionStringCandidates = m_serverConnectionStringCandidates.data();
    m_sessionProperties.MatchmakingServerConnectionString = m_matchmakingServerConnectionString.data();
    m_sessionProperties.MatchmakingTargetSessionConstantsJson = m_matchmakingTargetSessionConstantsJson.data();
    m_sessionProperties.SessionCustomPropertiesJson = m_sessionCustomPropertiesJson.data();

    for (auto& member : m_members)
    {
        auto internalMember = Make<MultiplayerSessionMember>(*static_cast<MultiplayerSessionMember*>(member.Internal));
        member.Internal = internalMember;
        MultiplayerSessionMember::SetExternalMemberPointer(member);

        if (member.Xuid == m_xuid)
        {
            m_memberCurrentUser = &member;
        }

    }

    if (other.m_matchmakingServer != nullptr)
    {
        m_matchmakingServer = MakeShared<XblMultiplayerMatchmakingServer>(*other.m_matchmakingServer);
        m_matchmakingServer->StatusDetails = m_matchmakingStatusDetails.data();
    }

    m_sessionConstants.InitiatorXuids = m_initiatorXuids.data();
    m_sessionConstants.InitiatorXuidsCount = static_cast<uint32_t>(m_initiatorXuids.size());
    if (m_sessionConstants.MemberInitialization != nullptr)
    {
        m_sessionConstants.MemberInitialization = &m_memberInitialization;
    }
    m_sessionConstants.CustomJson = m_constantsCustomJson.data();
    m_sessionConstants.SessionCloudComputePackageConstantsJson = m_constantsCloudComputePackageJson.data();
    m_sessionConstants.MeasurementServerAddressesJson = m_constantsMeasurementServerAddressesJson.data();
}

XblMultiplayerSession::~XblMultiplayerSession()
{
    for (auto candidate : m_serverConnectionStringCandidates)
    {
        Delete(candidate);
    }

    for (auto keyword : m_keywords)
    {
        Delete(keyword);
    }

    for (auto& member : m_members)
    {
        Delete(static_cast<MultiplayerSessionMember*>(member.Internal));
    }
}

std::shared_ptr<xbox::services::RefCounter> XblMultiplayerSession::GetSharedThis()
{
    return shared_from_this();
}

void XblMultiplayerSession::Initialize()
{
    m_info = XblMultiplayerSessionInfo{};
    m_initialization = XblMultiplayerSessionInitializationInfo{};
    m_sessionProperties = XblMultiplayerSessionProperties{};
    m_sessionSubscriptionGuid = utils::create_guid(true);

    m_sessionCustomPropertiesJson = "{}";
    m_sessionProperties.SessionCustomPropertiesJson = m_sessionCustomPropertiesJson.data();

    m_constantsCustomJson = "";
    m_sessionConstants.CustomJson = m_constantsCustomJson.data();

    m_constantsCloudComputePackageJson = "";
    m_sessionConstants.SessionCloudComputePackageConstantsJson = m_constantsCloudComputePackageJson.data();

    m_constantsMeasurementServerAddressesJson = "";
    m_sessionConstants.MeasurementServerAddressesJson = m_constantsMeasurementServerAddressesJson.data();

    m_matchmakingTargetSessionConstantsJson = "";
    m_sessionProperties.MatchmakingTargetSessionConstantsJson = m_matchmakingTargetSessionConstantsJson.data();

    m_matchmakingServerConnectionString = "";
    m_sessionProperties.MatchmakingServerConnectionString = m_matchmakingServerConnectionString.data();

    // Skipping this one cause m_matchmakingServer is optional and nullptr
    // m_matchmakingServer->StatusDetails = m_matchmakingStatusDetails.data();

    // TODO remove this after fixing member management
    m_members.reserve(100);
}

const xsapi_internal_string XblMultiplayerSession::ETag() const
{
    return m_eTag;
}

const xsapi_internal_string& XblMultiplayerSession::ETagUnsafe() const
{
    return m_eTag;
}

const XblMultiplayerSessionInfo& XblMultiplayerSession::SessionInfo() const
{
    return m_info;
}

const XblMultiplayerSessionInitializationInfo& XblMultiplayerSession::InitializationInfo() const
{
    return m_initialization;
}

time_t XblMultiplayerSession::TimeOfSession() const
{
    return m_sessionRetrievedTime;
}
const XblMultiplayerSessionReference& XblMultiplayerSession::SessionReference() const
{
    return m_sessionReference;
}

const xsapi_internal_vector<XblDeviceToken>& XblMultiplayerSession::HostCandidates() const
{
    return m_hostCandidates;
}

const XblMultiplayerSessionConstants& XblMultiplayerSession::SessionConstantsUnsafe() const
{
    return m_sessionConstants;
}

const XblMultiplayerSessionProperties& XblMultiplayerSession::SessionPropertiesUnsafe() const
{
    return m_sessionProperties;
}

void XblMultiplayerSession::StateLock() const
{
    m_lockSession.lock();
}

void XblMultiplayerSession::StateUnlock() const
{
    m_lockSession.unlock();
}

const RoleTypes& XblMultiplayerSession::RoleTypesUnsafe() const
{
    return m_roleTypes;
}

const xsapi_internal_vector<XblMultiplayerSessionMember>& XblMultiplayerSession::MembersUnsafe() const
{
    return m_members;
}

const xsapi_internal_vector<const char*>& XblMultiplayerSession::ServerConnectionStringCandidatesUnsafe() const
{
    return m_serverConnectionStringCandidates;
}

const xsapi_internal_vector<uint32_t>& XblMultiplayerSession::TurnCollectionUnsafe() const
{
    return m_turnCollection;
}

const xsapi_internal_vector<const char*>& XblMultiplayerSession::KeywordsUnsafe() const
{
    return m_keywords;
}

const XblMultiplayerSessionMember* XblMultiplayerSession::CurrentUserUnsafe() const
{
    return m_memberCurrentUser;
}

MultiplayerSessionMember* XblMultiplayerSession::CurrentUserInternalUnsafe() const
{
    if (m_memberCurrentUser != nullptr)
    {
        return MultiplayerSessionMember::Get(m_memberCurrentUser);
    }
    return nullptr;
}

const XblMultiplayerSessionMember* XblMultiplayerSession::GetMemberUnsafe(uint32_t memberId) const
{
    const XblMultiplayerSessionMember* out = nullptr;
    for (const auto& member : m_members)
    {
        if (member.MemberId == memberId)
        {
            out = &member;
        }
    }
    return out;
}

uint32_t XblMultiplayerSession::MembersAccepted() const
{
    return m_membersAccepted;
}

const xsapi_internal_string XblMultiplayerSession::RawServersJson() const
{
    return m_serversJson;
}

const xsapi_internal_string& XblMultiplayerSession::RawServersJsonUnsafe() const
{
    return m_serversJson;
}

std::shared_ptr<const XblMultiplayerMatchmakingServer> XblMultiplayerSession::MatchmakingServer() const
{
    return m_matchmakingServer;
}

XblWriteSessionStatus XblMultiplayerSession::WriteStatus() const
{
    return m_writeSessionStatus;
}

HRESULT XblMultiplayerSession::DeserializationError() const
{
    return m_deserializationError;
}

HRESULT XblMultiplayerSession::SetServersJson(
    _In_ const xsapi_internal_string& serversJson
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };

    auto hr = JsonUtils::ValidateJson(serversJson.data());
    if (SUCCEEDED(hr))
    {
        m_serversJson = serversJson;
        m_writeServersJson = true;
    }
    return hr;
}

void XblMultiplayerSession::SetWriteSessionStatus(
    int32_t httpStatusCode
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_writeSessionStatus = ConvertHttpStatusToWriteSessionStatus(httpStatusCode);
}

HRESULT XblMultiplayerSession::AddMemberReservation(
    _In_ uint64_t xuid,
    _In_opt_z_ const char* memberCustomConstantsJson,
    _In_ bool initializeRequested
)
{
    if (memberCustomConstantsJson)
    {
        auto hr = JsonUtils::ValidateJson(memberCustomConstantsJson);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    xsapi_internal_stringstream memberId;
    memberId << "reserve_";
    memberId << m_memberRequestIndex++;

    m_members.push_back(MultiplayerSessionMember::Construct(false, memberId.str(), xuid, memberCustomConstantsJson, initializeRequested));
    MultiplayerSessionMember::SetExternalMemberPointer(m_members.back());

    return S_OK;
}

HRESULT XblMultiplayerSession::Join(
    _In_opt_z_ const char* memberCustomConstantsJson,
    _In_ bool initializeRequested,
    _In_ bool joinWithActiveStatus
)
{
    auto alreadyJoined = m_joiningSession.exchange(true);
    if (alreadyJoined)
    {
        return E_UNEXPECTED;
    }

    if (memberCustomConstantsJson)
    {
        auto hr = JsonUtils::ValidateJson(memberCustomConstantsJson);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_members.push_back(MultiplayerSessionMember::Construct(true, "me", m_xuid, memberCustomConstantsJson, initializeRequested));
    m_memberCurrentUser = &m_members.back();
    MultiplayerSessionMember::SetExternalMemberPointer(m_members.back());

    if (joinWithActiveStatus)
    {
        MultiplayerSessionMember::Get(m_memberCurrentUser)->SetStatus(XblMultiplayerSessionMemberStatus::Active);
    }

    return S_OK;
}

void XblMultiplayerSession::SetVisibility(
    _In_ XblMultiplayerSessionVisibility visibility
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionConstants.Visibility = visibility;
    m_writeConstants = true;
}

void
XblMultiplayerSession::SetMaxMembersInSession(
    _In_ uint32_t maxMembersInSession
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionConstants.MaxMembersInSession = maxMembersInSession;
    m_writeConstants = true;
}


HRESULT
XblMultiplayerSession::SetTimeouts(
    _In_ uint64_t memberReservedTimeout,
    _In_ uint64_t memberInactiveTimeout,
    _In_ uint64_t memberReadyTimeout,
    _In_ uint64_t sessionEmptyTimeout
    )
{
    // Call set_timeouts/SetTimeouts before writing a new session to the service
    if (!m_newSession)
    {
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionConstants.MemberReservedTimeout = memberReservedTimeout;
    m_sessionConstants.MemberInactiveTimeout = memberInactiveTimeout;
    m_sessionConstants.MemberReadyTimeout = memberReadyTimeout;
    m_sessionConstants.SessionEmptyTimeout = sessionEmptyTimeout;
    m_writeTimeouts = true;
    m_writeConstants = true;
    return S_OK;
}

HRESULT
XblMultiplayerSession::SetQosConnectivityMetrics(
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
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionConstants.EnableMetricsLatency = enableLatencyMetric;
    m_sessionConstants.EnableMetricsBandwidthUp = enableBandwidthUpMetric;
    m_sessionConstants.EnableMetricsBandwidthDown = enableBandwidthDownMetric;
    m_sessionConstants.EnableMetricsCustom = enableCustomMetric;
    m_writeQosConnectivityMetrics = true;
    m_writeConstants = true;
    return S_OK;
}

HRESULT
XblMultiplayerSession::SetMemberInitialization(
    _In_ const XblMultiplayerMemberInitialization& memberInitialization
)
{
    // Call set_member_initialization/SetMemberInitialization before writing a new session to the service
    if (!m_newSession)
    {
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_memberInitialization = memberInitialization;
    m_sessionConstants.MemberInitialization = &m_memberInitialization;
    m_writeMemberInitialization = true;
    m_writeConstants = true;
    return S_OK;
}

HRESULT
XblMultiplayerSession::SetPeerToPeerRequirements(
    _In_ const XblMultiplayerPeerToPeerRequirements& requirements
    )
{
    // Call set_peer_to_peer_requirements/SetPeerToPeerRequirements before writing a new session to the service
    if (!m_newSession)
    {
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionConstants.PeerToPeerRequirements = requirements;
    m_writePeerToPeerRequirements = true;
    m_writeConstants = true;
    return S_OK;
}

HRESULT XblMultiplayerSession::SetPeerToHostRequirements(
    _In_ const XblMultiplayerPeerToHostRequirements& requirements
    )
{
    // Call set_peer_to_host_requirements/SetPeerToHostRequirements before writing a new session to the service
    if (!m_newSession)
    {
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionConstants.PeerToHostRequirements = requirements;
    m_writePeerToHostRequirements = true;
    m_writeConstants = true;
    return S_OK;
}

HRESULT XblMultiplayerSession::SetMeasurementServerAddresses(
    _In_ const xsapi_internal_string& measurementServerAddresses
)
{
    if (!m_newSession)
    {
        return E_UNEXPECTED;
    }

    auto hr = JsonUtils::ValidateJson(measurementServerAddresses.data());
    if (SUCCEEDED(hr))
    {
        std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
        m_constantsMeasurementServerAddressesJson = measurementServerAddresses;
        m_sessionConstants.MeasurementServerAddressesJson = m_constantsMeasurementServerAddressesJson.data();
        m_writeMeasurementServerAddresses = true;
        m_writeConstants = true;
    }
    return hr;
}

HRESULT
XblMultiplayerSession::SetSessionCapabilities(
    _In_ const XblMultiplayerSessionCapabilities& capabilities
    )
{
    // Call set_session_capabilities/SetSessionCapabilities before writing a new session to the service
    if (!m_newSession)
    {
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionConstants.SessionCapabilities = capabilities;
    m_writeConstants = true;
    return S_OK;
}

HRESULT
XblMultiplayerSession::SetCloudComputePackageJson(
    _In_ const xsapi_internal_string& sessionCloudComputePackageConstantsJson
    )
{
    // Call set_cloud_compute_package_json/SetCloudComputePackageJson before writing a new session to the service
    if (!m_newSession)
    {
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_constantsCloudComputePackageJson = sessionCloudComputePackageConstantsJson;
    m_sessionConstants.SessionCloudComputePackageConstantsJson = m_constantsCloudComputePackageJson.data();
    m_writeConstants = true;
    return S_OK;
}

void
XblMultiplayerSession::SetInitializationStatus(
    _In_ bool initializationSucceeded
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_writeInitializationStatus = true;
    m_initializationSucceeded = initializationSucceeded;
}

void
XblMultiplayerSession::SetHostDeviceToken(
    _In_ const XblDeviceToken hostDeviceToken
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    utils::strcpy(m_sessionProperties.HostDeviceToken.Value, sizeof(m_sessionProperties.HostDeviceToken.Value), hostDeviceToken.Value);
    m_writeHostDeviceToken = true;
}

void
XblMultiplayerSession::SetHostDeviceToken(
    _In_ const xsapi_internal_string& hostDeviceToken
)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    utils::strcpy(m_sessionProperties.HostDeviceToken.Value, sizeof(m_sessionProperties.HostDeviceToken.Value), hostDeviceToken.data());
    m_writeHostDeviceToken = true;
}

void
XblMultiplayerSession::SetMatchmakingServerConnectionPath(
    _In_ const xsapi_internal_string& serverConnectionPath
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_matchmakingServerConnectionString = serverConnectionPath;
    m_sessionProperties.MatchmakingServerConnectionString = m_matchmakingServerConnectionString.data();
    m_writeMatchmakingServerConnectionPath = true;
}

void
XblMultiplayerSession::SetMatchmakingResubmit(
    _In_ bool matchResubmit
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionProperties.MatchmakingResubmit = matchResubmit;
    m_writeMatchmakingResubmit = true;
}

void
XblMultiplayerSession::SetClosed(
    _In_ bool closed
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionProperties.Closed = closed;
    m_writeClosed = true;
}

void
XblMultiplayerSession::SetLocked(
    _In_ bool locked
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionProperties.Locked = locked;
    m_writeLocked = true;
}

void
XblMultiplayerSession::SetAllocateCloudCompute(
    _In_ bool allocateCloudCompute
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionProperties.AllocateCloudCompute = allocateCloudCompute;
    m_writeAllocateCloudCompute = true;
}

HRESULT XblMultiplayerSession::SetServerConnectionStringCandidates(
    _In_reads_(serverConnectionStringCandidatesCount) const char** serverConnectionStringCandidates,
    _In_ size_t serverConnectionStringCandidatesCount
    )
{
    RETURN_HR_INVALIDARGUMENT_IF(serverConnectionStringCandidates == nullptr)
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    for (auto candidate : m_serverConnectionStringCandidates)
    {
        Delete(candidate);
    }
    m_serverConnectionStringCandidates.clear();
    for (uint32_t i = 0; i < serverConnectionStringCandidatesCount; ++i)
    {
        m_serverConnectionStringCandidates.push_back(Make(serverConnectionStringCandidates[i]));
    }

    m_sessionProperties.ServerConnectionStringCandidates = m_serverConnectionStringCandidates.data();
    m_sessionProperties.ServerConnectionStringCandidatesCount = m_serverConnectionStringCandidates.size();
    m_writeServerConnectionStringCandidates = true;
    return S_OK;
}

HRESULT
XblMultiplayerSession::SetSessionChangeSubscription(
    _In_ XblMultiplayerSessionChangeTypes changeTypes
    )
{
    if(m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    MultiplayerSessionMember::Get(m_memberCurrentUser)->SetSessionChangeSubscription(changeTypes, m_sessionSubscriptionGuid);
    return S_OK;
}

HRESULT
XblMultiplayerSession::Leave()
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Failed trying to leave and join the session at the same time
    if (m_joiningSession)
    {
        return E_UNEXPECTED;
    }

    for (auto iter = m_members.begin(); iter != m_members.end(); iter++)
    {
        if (iter->IsCurrentUser)
        {
            Delete(static_cast<MultiplayerSessionMember*>(iter->Internal));
            m_members.erase(iter);
            m_memberCurrentUser = nullptr;
            break;
        }
    }

    m_leaveSession = true;
    return S_OK;
}

HRESULT
XblMultiplayerSession::SetCurrentUserStatus(
    _In_ XblMultiplayerSessionMemberStatus status
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserStatus
    // Can not set member to ready
    // Can not set member to reserved.  Use AddMemberReservation instead
    if (m_memberCurrentUser == nullptr || status == XblMultiplayerSessionMemberStatus::Ready || status == XblMultiplayerSessionMemberStatus::Reserved)
    {
        return E_UNEXPECTED;
    }

    return MultiplayerSessionMember::Get(m_memberCurrentUser)->SetStatus(status);
}

HRESULT
XblMultiplayerSession::SetCurrentUserSecureDeviceAddressBase64(
    _In_ const xsapi_internal_string& value
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserSecureDeviceAddressBase64
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }

    MultiplayerSessionMember::Get(m_memberCurrentUser)->SetSecureDeviceBaseAddress64(value);
    return S_OK;
}

HRESULT
XblMultiplayerSession::SetCurrentUserRoleInfo(
    _In_ const xsapi_internal_vector<XblMultiplayerSessionMemberRole>& roles
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserRoleInfo
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }

    MultiplayerSessionMember::Get(m_memberCurrentUser)->SetRoles(roles);
    return S_OK;
}

HRESULT XblMultiplayerSession::SetMutableRoleSettings(
    _In_ String&& roleTypeName,
    _In_ String&& roleName,
    _In_opt_ uint32_t* maxCount,
    _In_opt_ uint32_t* targetCount
)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    auto hr = m_roleTypes.SetRoleSettings(std::move(roleTypeName), std::move(roleName), maxCount, targetCount);
    if (SUCCEEDED(hr))
    {
        m_writeRoleTypes = true;
    }
    return hr;
}

HRESULT XblMultiplayerSession::SetCurrentUserMembersInGroup(
    _In_ const xsapi_internal_vector<uint32_t>& membersInGroup
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserMembersInGroup
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }

    MultiplayerSessionMember::Get(m_memberCurrentUser)->SetMembersInGroup(membersInGroup);
    return S_OK;
}

HRESULT XblMultiplayerSession::SetCurrentUserGroups(
    _In_reads_(groupsCount) const char** groups,
    _In_ size_t groupsCount
)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserGroups
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }
    MultiplayerSessionMember::Get(m_memberCurrentUser)->SetGroups(groups, groupsCount);
    return S_OK;
}

HRESULT XblMultiplayerSession::SetCurrentUserEncounters(
    _In_reads_(encountersCount) const char** encounters,
    _In_ size_t encountersCount
)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserEncounters
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }
    MultiplayerSessionMember::Get(m_memberCurrentUser)->SetEncounters(encounters, encountersCount);
    return S_OK;
}

HRESULT XblMultiplayerSession::SetCurrentUserServerMeasurementsJson(
    _In_ const xsapi_internal_string& serverMeasurementsJson
)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserServerMeasurementsJson
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }
    return MultiplayerSessionMember::Get(m_memberCurrentUser)->SetServerMeasurementsJson(serverMeasurementsJson);
}

HRESULT
XblMultiplayerSession::SetCurrentUserQosMeasurementsJson(
    _In_ const xsapi_internal_string& serverMeasurementsJson
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserQosMeasurementsJson
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }
    return MultiplayerSessionMember::Get(m_memberCurrentUser)->SetQosMeasurementsJson(serverMeasurementsJson);
}

HRESULT
XblMultiplayerSession::SetCurrentUserMemberCustomPropertyJson(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson
)
{
    if (name.empty())
    {
        return E_INVALIDARG;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling SetCurrentUserMemberCustomPropertyJson
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }

    return MultiplayerSessionMember::Get(m_memberCurrentUser)->SetCustomPropertyJson(name, valueJson);
}

HRESULT
XblMultiplayerSession::DeleteCurrentUserMemberCustomPropertyJson(
    _In_ const xsapi_internal_string& name
    )
{
    if (name.empty())
    {
        return E_INVALIDARG;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    // Must join the session first before calling DeleteCurrentUserMemberCustomPropertyJson
    if (m_memberCurrentUser == nullptr)
    {
        return E_UNEXPECTED;
    }
    MultiplayerSessionMember::Get(m_memberCurrentUser)->DeleteCustomPropertyJson(name);

    return S_OK;
}

HRESULT
XblMultiplayerSession::SetMatchmakingTargetSessionConstantsJson(
    _In_ const xsapi_internal_string& matchmakingTargetSessionConstantsJson
    )
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };

    auto hr = JsonUtils::ValidateJson(matchmakingTargetSessionConstantsJson.data());
    if (SUCCEEDED(hr))
    {
        m_matchmakingTargetSessionConstantsJson = matchmakingTargetSessionConstantsJson;
        m_sessionProperties.MatchmakingTargetSessionConstantsJson = m_matchmakingTargetSessionConstantsJson.data();
        m_writeMatchmakingTargetSessionConstants = true;
    }
    return hr;
}

HRESULT
XblMultiplayerSession::SetSessionCustomPropertyJson(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& customProperty
)
{
    if (name.empty())
    {
        return E_INVALIDARG;
    }

    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };

    JsonDocument customProperties;
    customProperties.Parse(m_sessionCustomPropertiesJson.data());
    auto hr = JsonUtils::SetMember(customProperties, name, customProperty);
    XSAPI_ASSERT(SUCCEEDED(hr));
    if (SUCCEEDED(hr))
    {
        m_sessionCustomPropertiesJson = JsonUtils::SerializeJson(customProperties);
        m_sessionProperties.SessionCustomPropertiesJson = m_sessionCustomPropertiesJson.data();
        m_writeSessionCustomPropertiesJson = true;
    }

    return hr;
}

HRESULT
XblMultiplayerSession::DeleteSessionCustomPropertyJson(
    _In_ const xsapi_internal_string& name
    )
{
    return SetSessionCustomPropertyJson(name, JsonValue());
}

HRESULT XblMultiplayerSession::SetKeywords(
    _In_ const char** keywords,
    _In_ size_t keywordsCount
)
{
    RETURN_HR_INVALIDARGUMENT_IF(keywords == nullptr)
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    for (auto keyword : m_keywords)
    {
        Delete(keyword);
    }
    m_keywords.clear();
    for (uint32_t i = 0; i < keywordsCount; ++i)
    {
        m_keywords.push_back(Make(keywords[i]));
    }
    m_sessionProperties.Keywords = m_keywords.data();
    m_sessionProperties.KeywordCount = m_keywords.size();
    m_writePropertiesKeywords = true;
    return S_OK;
}

void XblMultiplayerSession::SetJoinRestriction(
    _In_ XblMultiplayerSessionRestriction joinRestriction
)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionProperties.JoinRestriction = joinRestriction;
    m_writeJoinRestriction = true;
}

void XblMultiplayerSession::SetReadRestriction(
    _In_ XblMultiplayerSessionRestriction readRestriction
)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };
    m_sessionProperties.ReadRestriction = readRestriction;
    m_writeReadRestriction = true;
}

XblMultiplayerMetrics XblMultiplayerSession::ConvertStringToMultiplayerHostSelectionMetric(
    _In_ const xsapi_internal_string& value
    )
{
    if (value.empty())
    {
        return XblMultiplayerMetrics::Latency;
    }
    else if (utils::str_icmp_internal(value, "bandwidthUp") == 0)
    {
        return XblMultiplayerMetrics::BandwidthUp;
    }
    else if (utils::str_icmp_internal(value, "bandwidthDown") == 0)
    {
        return XblMultiplayerMetrics::BandwidthDown;
    }
    else if (utils::str_icmp_internal(value, "bandwidth") == 0)
    {
        return XblMultiplayerMetrics::Bandwidth;
    }
    else if (utils::str_icmp_internal(value, "latency") == 0)
    {
        return XblMultiplayerMetrics::Latency;
    }

    return XblMultiplayerMetrics::Unknown;
}

xsapi_internal_string
XblMultiplayerSession::ConvertMultiplayerHostSelectionMetricToString(
    _In_ XblMultiplayerMetrics multiplayMetric
    )
{
    switch (multiplayMetric)
    {
        case XblMultiplayerMetrics::Unknown: return "unknown";
        case XblMultiplayerMetrics::BandwidthUp: return "bandwidthUp";
        case XblMultiplayerMetrics::BandwidthDown: return "bandwidthDown";
        case XblMultiplayerMetrics::Bandwidth: return "bandwidth";
        case XblMultiplayerMetrics::Latency: return "latency";
        default: 
        {
            XSAPI_ASSERT(false);
            return "unknown";
        };
    }
}

XblMultiplayerInitializationStage
XblMultiplayerSession::ConvertStringToMultiplayerInitializationStage(
    _In_ const xsapi_internal_string& value
)
{
    if (value.empty())
    {
        return XblMultiplayerInitializationStage::None;
    }
    else if (utils::str_icmp_internal(value, "joining") == 0)
    {
        return XblMultiplayerInitializationStage::Joining;
    }
    else if (utils::str_icmp_internal(value, "failed") == 0)
    {
        return XblMultiplayerInitializationStage::Failed;
    }
    else if (utils::str_icmp_internal(value, "evaluating") == 0)
    {
        return XblMultiplayerInitializationStage::Evaluating;
    }
    else if (utils::str_icmp_internal(value, "measuring") == 0)
    {
        return XblMultiplayerInitializationStage::Measuring;
    }

    return XblMultiplayerInitializationStage::Unknown;
}

XblMatchmakingStatus
XblMultiplayerSession::ConvertStringToMatchmakingStatus(
    _In_ const xsapi_internal_string& value
    )
{
    XSAPI_ASSERT(!value.empty());
    if (utils::str_icmp_internal(value, "searching") == 0)
    {
        return XblMatchmakingStatus::Searching;
    }
    else if (utils::str_icmp_internal(value, "expired") == 0)
    {
        return XblMatchmakingStatus::Expired;
    }
    else if (utils::str_icmp_internal(value, "found") == 0)
    {
        return XblMatchmakingStatus::Found;
    }
    else if (utils::str_icmp_internal(value, "canceled") == 0)
    {
        return XblMatchmakingStatus::Canceled;
    }
    return XblMatchmakingStatus::Unknown;
}

xsapi_internal_string
XblMultiplayerSession::ConvertMatchmakingStatusToString(
    _In_ XblMatchmakingStatus matchmakingStatus
    )
{
    switch (matchmakingStatus)
    {
        case XblMatchmakingStatus::Unknown: return "unknown";
        case XblMatchmakingStatus::Searching: return "searching";
        case XblMatchmakingStatus::Expired: return "expired";
        case XblMatchmakingStatus::Found: return "found";
        case XblMatchmakingStatus::Canceled: return "canceled";
        default: 
        {
            XSAPI_ASSERT(false);
            return "unknown";
        }
    }
}

XblWriteSessionStatus
XblMultiplayerSession::ConvertHttpStatusToWriteSessionStatus(
_In_ int32_t httpStatusCode
)
{
    switch (httpStatusCode)
    {
        case 200: return XblWriteSessionStatus::Updated;
        case 201: return XblWriteSessionStatus::Created;
        case 204: return XblWriteSessionStatus::SessionDeleted;
        case 401: return XblWriteSessionStatus::AccessDenied;
        case 404: return XblWriteSessionStatus::HandleNotFound;
        case 409: return XblWriteSessionStatus::Conflict;
        case 412: return XblWriteSessionStatus::OutOfSync;
        default: return XblWriteSessionStatus::Unknown;
    }
}

HRESULT XblMultiplayerSession::DeserializeMembers(
    _In_ const JsonValue& json
)
{
    if (!json.IsNull() && json.IsObject())
    {
        uint32_t first = 0;
        uint32_t last = 0;
        if (json.HasMember("membersInfo"))
        {
            const JsonValue& membersInfo = json["membersInfo"];
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(membersInfo, "first", first, true));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(membersInfo, "next", last, true));
        }
        else
        {
            //required
            return WEB_E_INVALID_JSON_STRING;
        }

        for (uint32_t current = first; current != last;)
        {
            XblMultiplayerSessionMember member;
            if (json.HasMember("members"))
            {
                const JsonValue& membersJson = json["members"];
                // In large sessions the member json only contains "me" member
                auto currentIdString = m_sessionConstants.SessionCapabilities.Large ? "me" : utils::uint32_to_internal_string(current);
                if (membersJson.IsObject() && membersJson.HasMember(currentIdString.c_str()))
                {
                    const JsonValue& memberJson = membersJson[currentIdString.c_str()];
                    member = MultiplayerSessionMember::Deserialize(memberJson).Payload();

                    member.MemberId = current;

                    if (member.Xuid == m_xuid)
                    {
                        member.IsCurrentUser = true;
                    }

                    m_members.push_back(std::move(member));
                    if (m_members.back().IsCurrentUser)
                    {
                        m_memberCurrentUser = &m_members.back();
                    }
                    MultiplayerSessionMember::SetExternalMemberPointer(m_members.back());

                    if (m_sessionConstants.SessionCapabilities.Large)
                    {
                        break;
                    }

                    current = last;
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(memberJson, "next", current, false));
                }
                else
                {
                    //required
                    return WEB_E_INVALID_JSON_STRING;
                }
            }

        }
        
    }

    return S_OK;
}

XblMultiplayerSessionChangeTypes
XblMultiplayerSession::CompareMultiplayerSessions(
    _In_ std::shared_ptr<XblMultiplayerSession> other
    )
{
    XblMultiplayerSessionChangeTypes currentType = XblMultiplayerSessionChangeTypes::None;
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };

    if (utils::str_icmp_internal(m_sessionProperties.HostDeviceToken.Value, other->m_sessionProperties.HostDeviceToken.Value) != 0)
    {
        currentType |= XblMultiplayerSessionChangeTypes::HostDeviceTokenChange;
    }

    if (m_initialization.Stage != other->m_initialization.Stage)
    {
        currentType |= XblMultiplayerSessionChangeTypes::InitializationStateChange;
    }

    if ((m_matchmakingServer == nullptr) != (other->m_matchmakingServer == nullptr))
    {
        currentType |= XblMultiplayerSessionChangeTypes::MatchmakingStatusChange;
    }
    else if (m_matchmakingServer != nullptr &&
        ((m_matchmakingServer->Status != other->m_matchmakingServer->Status) || !(m_matchmakingServer->TargetSessionRef == other->m_matchmakingServer->TargetSessionRef))
        )
    {
        currentType |= XblMultiplayerSessionChangeTypes::MatchmakingStatusChange;
    }

    bool hasMemberChanged = false;
    bool memberStatusChanged = false;
    bool memberCustomPropertyChanged = false;

    if (m_members.size() != other->m_members.size())
    {
        hasMemberChanged = true;
    }

    for (const auto& currentMember : m_members)
    {
        bool isMemberFound = false;
        for (const auto& olderSessionMember : other->m_members)
        {
            if (currentMember.Xuid == olderSessionMember.Xuid)
            {
                isMemberFound = true;

                if (currentMember.Status != olderSessionMember.Status)
                {
                    memberStatusChanged = true;
                }

                MultiplayerSessionMemberReadLockGuard memberSafe(MultiplayerSessionMember::Get(&currentMember));
                MultiplayerSessionMemberReadLockGuard olderSessionMemberSafe(MultiplayerSessionMember::Get(&olderSessionMember));
                if (utils::str_icmp(JsonUtils::SerializeJson(memberSafe.CustomPropertiesJson()).c_str(),
                    JsonUtils::SerializeJson(olderSessionMemberSafe.CustomPropertiesJson()).c_str()) != 0)
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
        currentType |= XblMultiplayerSessionChangeTypes::MemberListChange;
    }

    if (memberStatusChanged)
    {
        currentType |= XblMultiplayerSessionChangeTypes::MemberStatusChange;
    }

    if (memberCustomPropertyChanged)
    {
        currentType |= XblMultiplayerSessionChangeTypes::MemberCustomPropertyChange;
    }

    XblMultiplayerSessionReadLockGuard otherSafe(other);
    if (m_sessionProperties.Closed != other->m_sessionProperties.Closed ||
        m_sessionProperties.Locked != other->m_sessionProperties.Locked ||
        m_sessionProperties.JoinRestriction != other->m_sessionProperties.JoinRestriction ||
        (m_members.size() == m_sessionConstants.MaxMembersInSession) != 
        (otherSafe.Members().size() == other->m_sessionConstants.MaxMembersInSession)     // if the session is open again or closed again because the max member has changed
        )
    {
        currentType |= XblMultiplayerSessionChangeTypes::SessionJoinabilityChange;
    }

    if (utils::str_icmp_internal(m_sessionCustomPropertiesJson, other->m_sessionCustomPropertiesJson) != 0)
    {
        currentType |= XblMultiplayerSessionChangeTypes::CustomPropertyChange;
    }

    return static_cast<XblMultiplayerSessionChangeTypes>(currentType);
}

HRESULT XblMultiplayerSession::Deserialize(
    _In_ const JsonValue& json
)
{
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "contractVersion", m_info.ContractVersion));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "correlationId", m_info.CorrelationId, sizeof(m_info.CorrelationId)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "searchHandle", m_info.SearchHandleId, sizeof(m_info.SearchHandleId)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonStringToCharArray(json, "branch", m_info.Branch, sizeof(m_info.Branch)));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(json, "changeNumber", m_info.ChangeNumber, false));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "startTime", m_info.StartTime));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "nextTimer", m_info.NextTimer));

    if (json.IsObject() && json.HasMember("constants"))
    {
        DeserializeSessionConstants(json["constants"]);
    }
    else
    {
        //required
        DeserializeSessionConstants(JsonValue());
        return WEB_E_INVALID_JSON_STRING;
    }

    if (json.IsObject() && json.HasMember("initializing"))
    {
        const JsonValue& initializingJson = json["initializing"];
        xsapi_internal_string stage;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(initializingJson, "stage", stage));
        m_initialization.Stage = ConvertStringToMultiplayerInitializationStage(stage);
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(initializingJson, "stageStartTime", m_initialization.StageStartTime));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(initializingJson, "episode", m_initialization.Episode));
    }
    else
    {
        m_initialization.Stage = XblMultiplayerInitializationStage::None;
        m_initialization.StageStartTime = utils::time_t_from_datetime(xbox::services::datetime());
        m_initialization.Episode = 0;
    }
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblDeviceToken>([](const JsonValue& json)
    {
        if (!json.IsString())
        {
            return Result<XblDeviceToken>(WEB_E_INVALID_JSON_STRING);
        }
        XblDeviceToken token{};
        utils::strcpy(token.Value, sizeof(token.Value), json.GetString());
        return Result<XblDeviceToken>(token);
    }, json, "hostCandidates", m_hostCandidates, false));

    if (json.IsObject() && json.HasMember("roleTypes"))
    {
        auto roleTypesResult{ RoleTypes::Deserialize(json["roleTypes"]) };
        RETURN_HR_IF_FAILED(roleTypesResult.Hresult());
        m_roleTypes = roleTypesResult.ExtractPayload();
    }

    DeserializeMembers(json);

    if (json.IsObject() && json.HasMember("properties"))
    {
        DeserializeSessionProperties(json["properties"]);
    }
    else
    {
        //required
        DeserializeSessionProperties(JsonValue());
        return WEB_E_INVALID_JSON_STRING;
    }

    if (json.IsObject() && json.HasMember("membersInfo"))
    {
        const JsonValue& memberInfoJson = json["membersInfo"];
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(memberInfoJson, "accepted", m_membersAccepted));
    }
    else
    {
        m_membersAccepted = 0;
    }

    if (json.IsObject() && json.HasMember("servers"))
    {
        const JsonValue& serversJson = json["servers"];

        if (serversJson.IsObject())
        {
            if (serversJson.HasMember("matchmaking"))
            {
                const JsonValue& serversMatchmakingJson = serversJson["matchmaking"];
                if (!serversMatchmakingJson.IsNull())
                {
                    DeserializeMatchmakingServer(serversMatchmakingJson);
                }
            }
        }

        m_serversJson = JsonUtils::SerializeJson(serversJson);
    }
    else
    {
        m_serversJson = "";
    }

    return S_OK;
}

void XblMultiplayerSession::SerializeSessionProperties(_Out_ JsonValue& jsonProperties, _In_ JsonDocument::AllocatorType& allocator)
{
    jsonProperties.SetObject();
    JsonValue jsonPropertiesSystem(rapidjson::kObjectType);
    if (m_writePropertiesKeywords)
    {
        JsonValue keywordsJson;
        JsonUtils::SerializeVector<const char*>(JsonUtils::JsonUtf8Serializer, m_keywords, keywordsJson, allocator);
        jsonPropertiesSystem.AddMember("keywords", keywordsJson, allocator);
    }

    if (m_writePropertiesTurns)
    {
        JsonValue turnJson;
        JsonUtils::SerializeVector<uint32_t>(JsonUtils::JsonIntSerializer, m_turnCollection, turnJson, allocator);
        jsonPropertiesSystem.AddMember("turn", turnJson, allocator);
    }

    if (m_writeJoinRestriction && m_sessionProperties.JoinRestriction != XblMultiplayerSessionRestriction::Unknown)
    {
        auto joinRestrictionToString = Serializers::StringFromMultiplayerSessionRestriction(m_sessionProperties.JoinRestriction);
        jsonPropertiesSystem.AddMember("joinRestriction", JsonValue(joinRestrictionToString.c_str(), allocator).Move(), allocator);
    }

    if (m_writeReadRestriction && m_sessionProperties.ReadRestriction != XblMultiplayerSessionRestriction::Unknown)
    {
        auto readRestrictionToString = Serializers::StringFromMultiplayerSessionRestriction(m_sessionProperties.ReadRestriction);
        jsonPropertiesSystem.AddMember("readRestriction", JsonValue(readRestrictionToString.c_str(), allocator).Move(), allocator);
    }

    if (m_writeClosed)
    {
        jsonPropertiesSystem.AddMember("closed", m_sessionProperties.Closed, allocator);
    }

    if (m_writeLocked)
    {
        jsonPropertiesSystem.AddMember("locked", m_sessionProperties.Locked, allocator);
    }

    if (m_writeAllocateCloudCompute)
    {
        jsonPropertiesSystem.AddMember("allocateCloudCompute", m_sessionProperties.AllocateCloudCompute, allocator);
    }

    if (m_writeMatchmakingTargetSessionConstants || m_writeMatchmakingServerConnectionPath)
    {
        JsonValue jsonMatchmaking(rapidjson::kObjectType);
        if (m_writeMatchmakingTargetSessionConstants)
        {
            JsonDocument targetSessionConstraintsJson{ &allocator };
            targetSessionConstraintsJson.Parse(m_matchmakingTargetSessionConstantsJson.c_str());
            jsonMatchmaking.AddMember("targetSessionConstants", targetSessionConstraintsJson, allocator);
        }

        if (m_writeMatchmakingServerConnectionPath)
        {
            jsonMatchmaking.AddMember("serverConnectionString", JsonValue(m_sessionProperties.MatchmakingServerConnectionString, allocator).Move(), allocator);
        }

        jsonPropertiesSystem.AddMember("matchmaking", jsonMatchmaking, allocator);
    }

    if (m_writeMatchmakingResubmit)
    {
        jsonPropertiesSystem.AddMember("matchmakingResubmit", m_sessionProperties.MatchmakingResubmit, allocator);
    }

    if (m_writeInitializationStatus)
    {
        jsonPropertiesSystem.AddMember("initializationSucceeded", m_initializationSucceeded, allocator);
    }

    if (m_writeHostDeviceToken)
    {
        jsonPropertiesSystem.AddMember("host", JsonValue(m_sessionProperties.HostDeviceToken.Value, allocator).Move(), allocator);
    }

    if (m_writeServerConnectionStringCandidates)
    {
        JsonValue serverConnectionStringCandidatesJson;
        JsonUtils::SerializeVector<const char*>(JsonUtils::JsonUtf8Serializer, m_serverConnectionStringCandidates, serverConnectionStringCandidatesJson, allocator);
        jsonPropertiesSystem.AddMember("serverConnectionStringCandidates", serverConnectionStringCandidatesJson, allocator);
    }

    if (!jsonPropertiesSystem.IsNull() && !jsonPropertiesSystem.ObjectEmpty())
    {
        jsonProperties.AddMember("system", jsonPropertiesSystem, allocator);
    }

    if (m_writeSessionCustomPropertiesJson)
    {
        JsonDocument customJson{ &allocator };
        customJson.Parse(m_sessionCustomPropertiesJson.c_str());
        jsonProperties.AddMember("custom", customJson, allocator);
    }
}

void XblMultiplayerSession::SerializeSessionConstants(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator)
{
    if (!m_writeConstants)
    {
        return;
    }

    json.SetObject();

    JsonValue systemJson(rapidjson::kObjectType);
    systemJson.AddMember("version", MULTIPLAYER_SESSION_VERSION, allocator);
    if (m_sessionConstants.MaxMembersInSession > 0)
    {
        systemJson.AddMember("maxMembersCount", m_sessionConstants.MaxMembersInSession, allocator);
    }

    JsonValue systemCapabilitiesJson(rapidjson::kObjectType);
    if (m_sessionConstants.SessionCapabilities.Connectivity)
    {
        systemCapabilitiesJson.AddMember("connectivity", true, allocator);
    }

    if (m_sessionConstants.SessionCapabilities.SuppressPresenceActivityCheck)
    {
        systemCapabilitiesJson.AddMember("suppressPresenceActivityCheck", true, allocator);
    }

    if (m_sessionConstants.SessionCapabilities.Gameplay)
    {
        systemCapabilitiesJson.AddMember("gameplay", true, allocator);
    }

    if (m_sessionConstants.SessionCapabilities.Large)
    {
        systemCapabilitiesJson.AddMember("large", true, allocator);
    }

    if (m_sessionConstants.SessionCapabilities.UserAuthorizationStyle)
    {
        systemCapabilitiesJson.AddMember("userAuthorizationStyle", true, allocator);
    }

    if (m_sessionConstants.SessionCapabilities.ConnectionRequiredForActiveMembers)
    {
        systemCapabilitiesJson.AddMember("connectionRequiredForActiveMembers", true, allocator);
    }

    if (m_sessionConstants.SessionCapabilities.Crossplay)
    {
        systemCapabilitiesJson.AddMember("crossPlay", true, allocator);
    }
    
    if (m_sessionConstants.SessionCapabilities.Searchable)
    {
        systemCapabilitiesJson.AddMember("searchable", true, allocator);
    }

    if (m_sessionConstants.SessionCapabilities.HasOwners)
    {
        systemCapabilitiesJson.AddMember("hasOwners", true, allocator);
    }

    if (!systemCapabilitiesJson.IsNull() && !systemCapabilitiesJson.ObjectEmpty())
    {
        systemJson.AddMember("capabilities", systemCapabilitiesJson, allocator);
    }

    if (m_sessionConstants.Visibility != XblMultiplayerSessionVisibility::Any && m_sessionConstants.Visibility != XblMultiplayerSessionVisibility::Unknown)
    {
        auto visibilityString = Serializers::StringFromMultiplayerSessionVisibility(m_sessionConstants.Visibility);
        systemJson.AddMember("visibility", JsonValue(visibilityString.c_str(), allocator).Move(), allocator);
    }

    if (m_sessionConstants.InitiatorXuids != nullptr && m_sessionConstants.InitiatorXuidsCount > 0)
    {
        std::sort(m_sessionConstants.InitiatorXuids, m_sessionConstants.InitiatorXuids + m_sessionConstants.InitiatorXuidsCount);
        JsonValue jsonArray(rapidjson::kArrayType);
        for (uint32_t i = 0; i < m_sessionConstants.InitiatorXuidsCount; ++i)
        {
            jsonArray.PushBack(JsonValue(utils::uint64_to_internal_string(m_sessionConstants.InitiatorXuids[i]).c_str(), allocator).Move(), allocator);
        }
        systemJson.AddMember("initiators", jsonArray, allocator);
    }

    if (m_writeTimeouts)
    {
        JsonValue reservedRemovalTimeoutJson;
        JsonUtils::SerializeUInt52ToJson(m_sessionConstants.MemberReservedTimeout, reservedRemovalTimeoutJson);
        systemJson.AddMember("reservedRemovalTimeout", reservedRemovalTimeoutJson, allocator);

        JsonValue inactiveRemovalTimeoutJson;
        JsonUtils::SerializeUInt52ToJson(m_sessionConstants.MemberInactiveTimeout, inactiveRemovalTimeoutJson);
        systemJson.AddMember("inactiveRemovalTimeout", inactiveRemovalTimeoutJson, allocator);
        
        JsonValue readyRemovalTimeoutJson;
        JsonUtils::SerializeUInt52ToJson(m_sessionConstants.MemberReadyTimeout, readyRemovalTimeoutJson);
        systemJson.AddMember("readyRemovalTimeout", readyRemovalTimeoutJson, allocator);

        JsonValue sessionEmptyTimeoutJson;
        JsonUtils::SerializeUInt52ToJson(m_sessionConstants.SessionEmptyTimeout, sessionEmptyTimeoutJson);
        systemJson.AddMember("sessionEmptyTimeout", sessionEmptyTimeoutJson, allocator);
    }

    if (m_writeQosConnectivityMetrics)
    {
        JsonValue systemMetricsJson(rapidjson::kObjectType);
        systemMetricsJson.AddMember("latency", m_sessionConstants.EnableMetricsLatency, allocator);
        systemMetricsJson.AddMember("bandwidthDown", m_sessionConstants.EnableMetricsBandwidthDown, allocator);
        systemMetricsJson.AddMember("bandwidthUp", m_sessionConstants.EnableMetricsBandwidthUp, allocator);
        systemMetricsJson.AddMember("custom", m_sessionConstants.EnableMetricsCustom, allocator);
        systemJson.AddMember("metrics", systemMetricsJson, allocator);
    }

    if (m_writeMemberInitialization)
    {
        JsonValue memberInitializationJson{ rapidjson::kObjectType };

        JsonValue joinTimeoutJson;
        JsonUtils::SerializeUInt52ToJson(m_sessionConstants.MemberInitialization->JoinTimeout, joinTimeoutJson);
        memberInitializationJson.AddMember("joinTimeout", joinTimeoutJson, allocator);

        JsonValue measurementTimeoutJson;
        JsonUtils::SerializeUInt52ToJson(m_sessionConstants.MemberInitialization->MeasurementTimeout, measurementTimeoutJson);
        memberInitializationJson.AddMember("measurementTimeout", measurementTimeoutJson, allocator);

        if (m_sessionConstants.MemberInitialization->ExternalEvaluation)
        {
            JsonValue evaluationTimeoutJson;
            JsonUtils::SerializeUInt52ToJson(m_sessionConstants.MemberInitialization->EvaluationTimeout, evaluationTimeoutJson);
            memberInitializationJson.AddMember("evaluationTimeout", evaluationTimeoutJson, allocator);
        }

        memberInitializationJson.AddMember("externalEvaluation", m_sessionConstants.MemberInitialization->ExternalEvaluation, allocator);
        memberInitializationJson.AddMember("membersNeededToStart", m_sessionConstants.MemberInitialization->MembersNeededToStart, allocator);

        systemJson.AddMember("memberInitialization", memberInitializationJson, allocator);
    }

    if (m_writePeerToPeerRequirements)
    {
        JsonValue peerToPeerRequirementsJson(rapidjson::kObjectType);

        JsonValue latencyMaxJson;
        JsonUtils::SerializeUInt52ToJson(m_sessionConstants.PeerToPeerRequirements.LatencyMaximum, latencyMaxJson);
        peerToPeerRequirementsJson.AddMember("latencyMaximum", latencyMaxJson, allocator);

        peerToPeerRequirementsJson.AddMember("bandwidthMinimum", m_sessionConstants.PeerToPeerRequirements.BandwidthMinimumInKbps, allocator);
        systemJson.AddMember("peerToPeerRequirements", peerToPeerRequirementsJson, allocator);
    }

    if (m_writePeerToHostRequirements)
    {
        JsonValue peerToHostRequirementsJson(rapidjson::kObjectType);

        JsonValue latencyMaxJson;
        JsonUtils::SerializeUInt52ToJson(m_sessionConstants.PeerToHostRequirements.LatencyMaximum, latencyMaxJson);
        peerToHostRequirementsJson.AddMember("latencyMaximum",latencyMaxJson, allocator);

        peerToHostRequirementsJson.AddMember("bandwidthDownMinimum", m_sessionConstants.PeerToHostRequirements.BandwidthDownMinimumInKbps, allocator);
        peerToHostRequirementsJson.AddMember("bandwidthUpMinimum", m_sessionConstants.PeerToHostRequirements.BandwidthUpMinimumInKbps, allocator);
        peerToHostRequirementsJson.AddMember(
            "hostSelectionMetric", 
            JsonValue(XblMultiplayerSession::ConvertMultiplayerHostSelectionMetricToString(m_sessionConstants.PeerToHostRequirements.HostSelectionMetric).c_str(), allocator).Move(), 
            allocator
        );

        systemJson.AddMember("peerToHostRequirements", peerToHostRequirementsJson, allocator);
    }

    if (m_writeMeasurementServerAddresses)
    {
        JsonDocument measurementServerAddressesJson;
        measurementServerAddressesJson.Parse(m_sessionConstants.MeasurementServerAddressesJson);
        systemJson.AddMember("measurementServerAddresses", measurementServerAddressesJson, allocator);
    }

    if (m_sessionConstants.SessionCloudComputePackageConstantsJson != nullptr && m_sessionConstants.SessionCloudComputePackageConstantsJson[0] != 0)
    {
        JsonDocument cloudComputePackage{ &allocator };
        cloudComputePackage.Parse(m_sessionConstants.SessionCloudComputePackageConstantsJson);
        systemJson.AddMember("cloudComputePackage", cloudComputePackage, allocator);
    }

    json.AddMember("system", systemJson, allocator);

    if (m_sessionConstants.CustomJson != nullptr && m_sessionConstants.CustomJson[0] != 0)
    {
        JsonDocument customJson{ &allocator };
        customJson.Parse(m_sessionConstants.CustomJson);
        json.AddMember("custom", customJson, allocator);
    }
    else
    {
        json.AddMember("custom", JsonValue(rapidjson::kObjectType), allocator);
    }
}

void XblMultiplayerSession::Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator)
{
    json.SetObject();
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };

    if (m_newSession && m_writeConstants)
    {
        JsonValue serializedSessionConstants;
        SerializeSessionConstants(serializedSessionConstants, allocator);
        json.AddMember("constants", serializedSessionConstants, allocator);
    }

    if (m_writeRoleTypes)
    {
        json.AddMember("roleTypes", m_roleTypes.Serialize(allocator), allocator);
    }

    JsonValue serializedSessionProperties;
    SerializeSessionProperties(serializedSessionProperties, allocator);
    if (!serializedSessionProperties.IsNull() && !serializedSessionProperties.ObjectEmpty())
    {
        json.AddMember("properties", serializedSessionProperties, allocator);
    }

    if (!m_members.empty() || m_leaveSession)
    {
        JsonValue memberListJson(rapidjson::kObjectType);
        for (const auto& member : m_members)
        {
            auto internalMember = MultiplayerSessionMember::Get(&member);

            JsonValue memberJson{ rapidjson::kObjectType };
            internalMember->Serialize(memberJson, allocator);
            if (memberJson.MemberCount())
            {
                memberListJson.AddMember(JsonValue(internalMember->MemberId().c_str(), allocator).Move(), memberJson, allocator);
            }
        }

        if (m_leaveSession)
        {
            // Write "me" : null to leave session.
            memberListJson.AddMember("me", JsonValue{ rapidjson::kNullType }, allocator);
        }

        if (memberListJson.MemberCount())
        {
            json.AddMember("members", memberListJson, allocator);
        }
    }

    if (m_writeServersJson)
    {
        JsonDocument serversJson{};
        serversJson.Parse(m_serversJson.c_str());
        JsonUtils::SetMember(json, allocator, "servers", serversJson);
    }
}

bool XblMultiplayerSession::operator==(const XblMultiplayerSession& rhs) const
{
    return m_sessionReference == rhs.m_sessionReference;
}

bool XblMultiplayerSession::IsHost(
    _In_ const xsapi_internal_string& memberDeviceToken,
    _In_ const std::shared_ptr<XblMultiplayerSession>& session
    )
{
    if (memberDeviceToken.empty() || session == nullptr)
    {
        return false;
    }

    return utils::str_icmp(memberDeviceToken.data(), session->m_sessionProperties.HostDeviceToken.Value) == 0;
}

bool XblMultiplayerSession::IsPlayerInSession(
    _In_ uint64_t xboxUserId,
    _In_ const std::shared_ptr<XblMultiplayerSession>& session
    )
{
    if (session == nullptr)
    {
        return false;
    }

    for (const auto& member : session->m_members)
    {
        if (xboxUserId == member.Xuid)
        {
            return true;
        }
    }

    return false;
}

const XblMultiplayerSessionMember* XblMultiplayerSession::GetPlayerInSession(
    _In_ uint64_t xboxUserId,
    _In_ std::shared_ptr<XblMultiplayerSession> session
    )
{
    if (session == nullptr)
    {
        return nullptr;
    }

    for (const auto& member : session->m_members)
    {
        if (xboxUserId == member.Xuid)
        {
            return &member;
        }
    }
    return nullptr;
}

const XblMultiplayerSessionMember* XblMultiplayerSession::HostMember(
    _In_ std::shared_ptr<XblMultiplayerSession> session
)
{
    if (session == nullptr)
    {
        return nullptr;
    }

    for (const auto& member : session->m_members)
    {
        if (utils::str_icmp(member.DeviceToken.Value, session->m_sessionProperties.HostDeviceToken.Value) == 0)
        {
            return &member;
        }
    }
    return nullptr;
}

HRESULT XblMultiplayerSession::SetTurnCollection(_In_ const xsapi_internal_vector<uint32_t>& turnCollection)
{
    std::lock_guard<std::recursive_mutex> lock{ m_lockSession };

    if (turnCollection.empty())
    {
        return E_INVALIDARG;
    }

    m_turnCollection = turnCollection;
    m_sessionProperties.TurnCollection = m_turnCollection.data();
    m_sessionProperties.TurnCollectionCount = m_turnCollection.size();
    m_writePropertiesTurns = true;
    return S_OK;
}

bool XblMultiplayerSession::HasSessionPropertyChanged(
    _In_ const std::shared_ptr<XblMultiplayerSession>& session1,
    _In_ const std::shared_ptr<XblMultiplayerSession>& session2,
    _In_ const xsapi_internal_string& _propertyName
    )
{
    if (session1 == nullptr && session2 == nullptr)
    {
        return true;
    }

    if (session1 == nullptr || session2 == nullptr)
    {
        return false;
    }

    auto propertyName = _propertyName;

    JsonDocument customProp1;
    JsonDocument customProp2;
    customProp1.Parse(session1->m_sessionCustomPropertiesJson.data());
    customProp2.Parse(session2->m_sessionCustomPropertiesJson.data());

    if (!customProp1.HasParseError() &&
        !customProp2.HasParseError())
    {
        xsapi_internal_string prop1;
        xsapi_internal_string prop2;

        bool isInProp1 = SUCCEEDED(JsonUtils::ExtractJsonString(customProp1, propertyName, prop1, true));
        bool isInProp2 = SUCCEEDED(JsonUtils::ExtractJsonString(customProp2, propertyName, prop2, true));

        if ((isInProp1 && !isInProp2) ||
            (!isInProp1 && isInProp2))
        {
            return true;
        }

        if (isInProp1 && isInProp2)
        {
            return utils::str_icmp(prop1.c_str(), prop2.c_str()) != 0;
        }
    }

    return false;
}

bool XblMultiplayerSession::DoSessionsMatch(
    _In_ std::shared_ptr<XblMultiplayerSession> lhs,
    _In_ std::shared_ptr<XblMultiplayerSession> rhs
    )
{
    return lhs != nullptr && rhs != nullptr && *rhs == *lhs;
}

HRESULT XblMultiplayerSession::DeserializeMatchmakingServer(
    _In_ const JsonValue& serversMatchmakingJson
)
{
    if (serversMatchmakingJson.IsObject() && serversMatchmakingJson.HasMember("properties"))
    {
        const JsonValue& serversMatchmakingPropertiesJson = serversMatchmakingJson["properties"];

        if (serversMatchmakingPropertiesJson.IsObject() && serversMatchmakingPropertiesJson.HasMember("system"))
        {
            const JsonValue& json = serversMatchmakingPropertiesJson["system"];

            if (!json.IsNull())
            {
                m_matchmakingServer = MakeShared<XblMultiplayerMatchmakingServer>();

                xsapi_internal_string status;
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "status", status));
                m_matchmakingServer->Status = XblMultiplayerSession::ConvertStringToMatchmakingStatus(status);
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "statusDetails", m_matchmakingStatusDetails));
                m_matchmakingServer->StatusDetails = m_matchmakingStatusDetails.data();
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(json, "typicalWait", m_matchmakingServer->TypicalWaitInSeconds));

                if (json.IsObject() && json.HasMember("targetSessionRef"))
                {
                    m_matchmakingServer->TargetSessionRef = Serializers::DeserializeSessionReference(json["targetSessionRef"]).Payload();
                }
                else
                {
                    m_matchmakingServer->TargetSessionRef = XblMultiplayerSessionReference();
                }
            }
        }
    }

    return S_OK;
}

HRESULT XblMultiplayerSession::DeserializeSessionProperties(
    _In_ const JsonValue& json
)
{
    if (json.IsObject() && json.HasMember("system"))
    {
        const JsonValue& systemJson = json["system"];

        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<const char*>(JsonUtils::JsonUtf8Extractor, systemJson, "keywords", m_keywords, false));
        m_sessionProperties.KeywordCount = m_keywords.size();
        m_sessionProperties.Keywords = m_keywords.data();

        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<uint32_t>(JsonUtils::JsonIntExtractor, systemJson, "owners", m_sessionOwnerIndices, false));
        m_sessionProperties.SessionOwnerMemberIdsCount = m_sessionOwnerIndices.size();
        m_sessionProperties.SessionOwnerMemberIds = m_sessionOwnerIndices.data();

        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<uint32_t>(JsonUtils::JsonIntExtractor, systemJson, "turn", m_turnCollection, false));
        m_sessionProperties.TurnCollectionCount = static_cast<uint32_t>(m_turnCollection.size());
        m_sessionProperties.TurnCollection = m_turnCollection.data();

        xsapi_internal_string joinRestrictionString;
        xsapi_internal_string readRestrictionString;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(systemJson, "joinRestriction", joinRestrictionString));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(systemJson, "readRestriction", readRestrictionString));
        if (!joinRestrictionString.empty())
        {
            m_sessionProperties.JoinRestriction = Serializers::MultiplayerSessionRestrictionFromString(joinRestrictionString);
        }

        if (!readRestrictionString.empty())
        {
            m_sessionProperties.ReadRestriction = Serializers::MultiplayerSessionRestrictionFromString(readRestrictionString);
        }

        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemJson, "closed", m_sessionProperties.Closed));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemJson, "locked", m_sessionProperties.Locked));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemJson, "allocateCloudCompute", m_sessionProperties.AllocateCloudCompute));
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemJson, "matchmakingResubmit", m_sessionProperties.MatchmakingResubmit));

        if (systemJson.IsObject() && systemJson.HasMember("matchmaking"))
        {
            const JsonValue& systemMatchmakingJson = systemJson["matchmaking"];

            if (systemMatchmakingJson.IsObject() && systemMatchmakingJson.HasMember("targetSessionConstants"))
            {
                m_matchmakingTargetSessionConstantsJson = JsonUtils::SerializeJson(
                    systemMatchmakingJson["targetSessionConstants"]
                );
            }

            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(systemMatchmakingJson, "serverConnectionString", m_matchmakingServerConnectionString));
            m_sessionProperties.MatchmakingServerConnectionString = m_matchmakingServerConnectionString.data();
        }

        m_sessionProperties.MatchmakingTargetSessionConstantsJson = m_matchmakingTargetSessionConstantsJson.data();

        xsapi_internal_string host;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(systemJson, "host", host));
        utils::strcpy(
            m_sessionProperties.HostDeviceToken.Value,
            sizeof(m_sessionProperties.HostDeviceToken),
            host.c_str()
        );

        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<const char*>(JsonUtils::JsonUtf8Extractor, systemJson, "serverConnectionStringCandidates", m_serverConnectionStringCandidates, false));
        m_sessionProperties.ServerConnectionStringCandidatesCount = static_cast<uint32_t>(m_serverConnectionStringCandidates.size());
        m_sessionProperties.ServerConnectionStringCandidates = m_serverConnectionStringCandidates.data();
    }
    else
    {
        //required
        return WEB_E_INVALID_JSON_STRING;
    }

    if (json.IsObject() && json.HasMember("custom"))
    {
        m_sessionCustomPropertiesJson = JsonUtils::SerializeJson(json["custom"]);
    }
    m_sessionProperties.SessionCustomPropertiesJson = m_sessionCustomPropertiesJson.data();

    return S_OK;
}

HRESULT XblMultiplayerSession::DeserializeSessionConstants(
    _In_ const JsonValue& json
)
{
    if (!json.IsObject()) 
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    if (json.HasMember("system"))
    {
        const JsonValue& systemJson = json["system"];

        if (systemJson.IsObject())
        {
            if (systemJson.HasMember("cloudComputePackage"))
            {
                m_constantsCloudComputePackageJson = JsonUtils::SerializeJson(systemJson["cloudComputePackage"]);
                m_sessionConstants.SessionCloudComputePackageConstantsJson = m_constantsCloudComputePackageJson.data();
            }

            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(systemJson, "maxMembersCount", m_sessionConstants.MaxMembersInSession));

            xsapi_internal_string visibility;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(systemJson, "visibility", visibility));
            m_sessionConstants.Visibility = Serializers::MultiplayerSessionVisibilityFromString(visibility);

            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<uint64_t>(JsonUtils::JsonXuidExtractor, systemJson, "initiators", m_initiatorXuids, false));
            m_sessionConstants.InitiatorXuids = m_initiatorXuids.data();
            m_sessionConstants.InitiatorXuidsCount = m_initiatorXuids.size();

            if (systemJson.HasMember("capabilities"))
            {
                const JsonValue& systemCapabilitiesJson = systemJson["capabilities"];
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "connectivity", m_sessionConstants.SessionCapabilities.Connectivity, false));
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "suppressPresenceActivityCheck", m_sessionConstants.SessionCapabilities.SuppressPresenceActivityCheck, false));
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "gameplay", m_sessionConstants.SessionCapabilities.Gameplay, false));
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "large", m_sessionConstants.SessionCapabilities.Large, false));
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "connectionRequiredForActiveMembers", m_sessionConstants.SessionCapabilities.ConnectionRequiredForActiveMembers, false));
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "userAuthorizationStyle", m_sessionConstants.SessionCapabilities.UserAuthorizationStyle, false));
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "crossPlay", m_sessionConstants.SessionCapabilities.Crossplay, false));
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "hasOwners", m_sessionConstants.SessionCapabilities.HasOwners, false));
                 RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemCapabilitiesJson, "searchable", m_sessionConstants.SessionCapabilities.Searchable, false));
            }
            else
            {
                m_sessionConstants.SessionCapabilities.Connectivity = false;
                m_sessionConstants.SessionCapabilities.SuppressPresenceActivityCheck = false;
                m_sessionConstants.SessionCapabilities.Gameplay = false;
                m_sessionConstants.SessionCapabilities.Large = false;
                m_sessionConstants.SessionCapabilities.ConnectionRequiredForActiveMembers = false;
                m_sessionConstants.SessionCapabilities.UserAuthorizationStyle = false;
                m_sessionConstants.SessionCapabilities.Crossplay = false;
                m_sessionConstants.SessionCapabilities.HasOwners = false;
                m_sessionConstants.SessionCapabilities.Searchable = false;
            }

            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(systemJson, "reservedRemovalTimeout", m_sessionConstants.MemberReservedTimeout, false));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(systemJson, "inactiveRemovalTimeout", m_sessionConstants.MemberInactiveTimeout, false));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(systemJson, "readyRemovalTimeout", m_sessionConstants.MemberReadyTimeout, false));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(systemJson, "sessionEmptyTimeout", m_sessionConstants.SessionEmptyTimeout, false));

            if (systemJson.HasMember("metrics"))
            {
                const JsonValue& systemMetricsJson = systemJson["metrics"];
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemMetricsJson, "latency", m_sessionConstants.EnableMetricsLatency, false));
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemMetricsJson, "bandwidthDown", m_sessionConstants.EnableMetricsBandwidthDown, false));
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemMetricsJson, "bandwidthUp", m_sessionConstants.EnableMetricsBandwidthUp, false));
                RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(systemMetricsJson, "custom", m_sessionConstants.EnableMetricsCustom, false));
            }
            else
            {
                m_sessionConstants.EnableMetricsLatency = false;
                m_sessionConstants.EnableMetricsBandwidthDown = false;
                m_sessionConstants.EnableMetricsBandwidthUp = false;
                m_sessionConstants.EnableMetricsCustom = false;
            }

            if (systemJson.HasMember("memberInitialization"))
            {
                const JsonValue& memberInitializationJson = systemJson["memberInitialization"];
                if (!memberInitializationJson.IsNull())
                {
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(memberInitializationJson, "joinTimeout", m_memberInitialization.JoinTimeout));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(memberInitializationJson, "measurementTimeout", m_memberInitialization.MeasurementTimeout));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(memberInitializationJson, "evaluationTimeout", m_memberInitialization.EvaluationTimeout));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(memberInitializationJson, "externalEvaluation", m_memberInitialization.ExternalEvaluation));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(memberInitializationJson, "membersNeededToStart", m_memberInitialization.MembersNeededToStart));
                    m_sessionConstants.MemberInitialization = &m_memberInitialization;
                }
            }

            if (systemJson.HasMember("peerToHostRequirements"))
            {
                const JsonValue& peerToHostRequirementsJson = systemJson["peerToHostRequirements"];
                if (!peerToHostRequirementsJson.IsNull())
                {
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(peerToHostRequirementsJson, "latencyMaximum", m_sessionConstants.PeerToHostRequirements.LatencyMaximum));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(peerToHostRequirementsJson, "bandwidthDownMinimum", m_sessionConstants.PeerToHostRequirements.BandwidthDownMinimumInKbps));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(peerToHostRequirementsJson, "bandwidthUpMinimum", m_sessionConstants.PeerToHostRequirements.BandwidthUpMinimumInKbps));
                    xsapi_internal_string hostSelectionMetric;
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "hostSelectionMetric", hostSelectionMetric));
                    m_sessionConstants.PeerToHostRequirements.HostSelectionMetric = XblMultiplayerSession::ConvertStringToMultiplayerHostSelectionMetric(hostSelectionMetric);
                }
            }

            if (systemJson.HasMember("peerToPeerRequirements"))
            {
                const JsonValue& peerToPeerRequirementsJson = systemJson["peerToPeerRequirements"];
                if (!peerToPeerRequirementsJson.IsNull())
                {
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(peerToPeerRequirementsJson, "latencyMaximum", m_sessionConstants.PeerToPeerRequirements.LatencyMaximum));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(peerToPeerRequirementsJson, "bandwidthMinimum", m_sessionConstants.PeerToPeerRequirements.BandwidthMinimumInKbps));
                }
            }

            if (systemJson.HasMember("measurementServerAddresses"))
            {
                m_constantsMeasurementServerAddressesJson = JsonUtils::SerializeJson(systemJson["measurementServerAddresses"]);
                m_sessionConstants.MeasurementServerAddressesJson = m_constantsMeasurementServerAddressesJson.data();
            }
        }
    }
    else
    {
        //required
        return WEB_E_INVALID_JSON_STRING;
    }

    if (json.HasMember("custom"))
    {
        m_constantsCustomJson = JsonUtils::SerializeJson(json["custom"]);
        m_sessionConstants.CustomJson = m_constantsCustomJson.data();
    }

    return S_OK;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

RoleTypes::RoleTypes(const RoleTypes& other) noexcept
    : m_values{ other.m_values }
{
    for (auto& roleType : m_values)
    {
        roleType.Name = Make(roleType.Name);
        roleType.Roles = MakeArray(roleType.Roles, roleType.RoleCount);

        for (size_t i = 0; i < roleType.RoleCount; ++i)
        {
            roleType.Roles[i].RoleType = &roleType;
            roleType.Roles[i].Name = Make(roleType.Roles[i].Name);
            roleType.Roles[i].MemberXuids = MakeArray(roleType.Roles[i].MemberXuids, roleType.Roles[i].MemberCount);
        }
    }
}

RoleTypes& RoleTypes::operator=(RoleTypes other) noexcept
{
    std::swap(other.m_values, m_values);
    return *this;
}

RoleTypes::~RoleTypes() noexcept
{
    for (auto& roleType : m_values)
    {
        Delete(roleType.Name);
        for (size_t i = 0; i < roleType.RoleCount; ++i)
        {
            Delete(roleType.Roles[i].Name);
            Delete(roleType.Roles[i].MemberXuids);
        }
        Delete(roleType.Roles);
    }
}

Result<RoleTypes> RoleTypes::Deserialize(const JsonValue& json) noexcept
{
    if (json.IsNull())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    RoleTypes roleTypes{};
    roleTypes.m_values.reserve(json.MemberCount());

    for (const auto& roleTypeJson : json.GetObject())
    {
        roleTypes.m_values.push_back(XblMultiplayerRoleType{});
        auto& roleType{ roleTypes.m_values.back() };

        roleType.Name = Make(roleTypeJson.name.GetString());
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(roleTypeJson.value, "ownerManaged", roleType.OwnerManaged, false));

        Vector<String> mutableSettingsVector;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<String>(JsonUtils::JsonStringExtractor, roleTypeJson.value, "mutableRoleSettings", mutableSettingsVector, false));
        for (auto& setting : mutableSettingsVector)
        {
            if (utils::str_icmp_internal(setting, "max") == 0)
            {
                roleType.MutableRoleSettings |= XblMutableRoleSettings::Max;
            }
            else if (utils::str_icmp_internal(setting, "target") == 0)
            {
                roleType.MutableRoleSettings |= XblMutableRoleSettings::Target;
            }
        }

        if (roleTypeJson.value.HasMember("roles"))
        {
            const auto& rolesJson = roleTypeJson.value["roles"];
            if (rolesJson.IsObject())
            {
                roleType.Roles = MakeArray<XblMultiplayerRole>(rolesJson.MemberCount());

                for (const auto& roleJson : rolesJson.GetObject())
                {
                    auto& role{ roleType.Roles[roleType.RoleCount++] };

                    role.RoleType = &roleType;
                    role.Name = Make(roleJson.name.GetString());
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(roleJson.value, "count", role.MemberCount, false));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(roleJson.value, "max", role.MaxMemberCount, false));
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(roleJson.value, "target", role.TargetCount, false));
                    if (role.MemberCount > 0)
                    {
                        Vector<uint64_t> memberXuidsVector;
                        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<uint64_t>(JsonUtils::JsonXuidExtractor, roleJson.value, "memberXuids", memberXuidsVector, true));
                        role.MemberXuids = MakeArray(memberXuidsVector);
                    }
                }
            }
        }
    }

    return roleTypes;
}

JsonValue RoleTypes::Serialize(JsonDocument::AllocatorType& a) const noexcept
{
    JsonValue roleTypesJson{ rapidjson::Type::kObjectType };
    for (const auto& roleType : m_values)
    {
        JsonValue roleTypeJson{ rapidjson::kObjectType };
        JsonValue rolesJson{ rapidjson::kObjectType };

        for (size_t i = 0; i < roleType.RoleCount; ++i)
        {
            auto& role = roleType.Roles[i];
            JsonValue roleJson{ rapidjson::kObjectType };
            if (role.MaxMemberCount)
            {
                roleJson.AddMember("max", role.MaxMemberCount, a);
            }
            if (role.TargetCount > 0)
            {
                roleJson.AddMember("target", role.TargetCount, a);
            }
            rolesJson.AddMember(JsonValue{ role.Name, a }.Move(), roleJson, a);
        }
        roleTypeJson.AddMember("roles", rolesJson, a);
        roleTypesJson.AddMember(JsonValue{ roleType.Name, a }.Move(), roleTypeJson, a);
    }
    return roleTypesJson;
}

const Vector<XblMultiplayerRoleType>& RoleTypes::Values() const noexcept
{
    return m_values;
}

HRESULT RoleTypes::SetRoleSettings(
    String&& roleTypeName,
    String&& roleName,
    uint32_t* maxCount,
    uint32_t* targetCount
) noexcept
{
    auto role{ GetRole(std::move(roleTypeName), std::move(roleName)) };
    RETURN_HR_INVALIDARGUMENT_IF_NULL(role);

    bool maxMutable{ (role->RoleType->MutableRoleSettings | XblMutableRoleSettings::Max) != XblMutableRoleSettings::None };
    bool targetMutable{ (role->RoleType->MutableRoleSettings | XblMutableRoleSettings::Target) != XblMutableRoleSettings::None };
    RETURN_HR_IF((maxCount && !maxMutable) || (targetCount && !targetMutable), E_UNEXPECTED);

    if (maxCount)
    {
        role->MaxMemberCount = *maxCount;
    }
    if (targetCount)
    {
        role->TargetCount = *targetCount;
    }

    return S_OK;
}

XblMultiplayerRole* RoleTypes::GetRole(
    String&& roleTypeName,
    String&& roleName
) const noexcept
{
    // Could store these in a map to make lookup quicker but the collection of roles should be small
    for (const auto& roleType : m_values)
    {
        if (Stricmp(roleType.Name, roleTypeName.data()) == 0)
        {
            for (size_t i = 0; i < roleType.RoleCount; ++i)
            {
                if (Stricmp(roleType.Roles[i].Name, roleName.data()) == 0)
                {
                    return &roleType.Roles[i];
                }
            }
            break;
        }
    }
    return nullptr;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END

STDAPI_(XblMultiplayerSessionHandle) XblMultiplayerSessionCreateHandle(
    _In_ uint64_t xuid,
    _In_opt_ const XblMultiplayerSessionReference* sessionReference,
    _In_opt_ const XblMultiplayerSessionInitArgs* initArgs
) XBL_NOEXCEPT
try
{
    auto session = MakeShared<XblMultiplayerSession>(xuid, sessionReference, initArgs);
    session->AddRef();
    return session.get();
}
CATCH_RETURN_WITH(nullptr)

STDAPI XblMultiplayerSessionDuplicateHandle(
    _In_ XblMultiplayerSessionHandle session,
    _Out_ XblMultiplayerSessionHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(session == nullptr || duplicatedHandle == nullptr);

    session->AddRef();
    *duplicatedHandle = session;

    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblMultiplayerSessionCloseHandle(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session)
    {
        session->DecRef();
    }
}
CATCH_RETURN_WITH(;)

STDAPI_(XblTournamentArbitrationStatus) XblMultiplayerSessionArbitrationStatus(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(session);
    return XblTournamentArbitrationStatus::Incomplete;
}
CATCH_RETURN_WITH(XblTournamentArbitrationStatus::Incomplete)

STDAPI_(time_t) XblMultiplayerSessionTimeOfSession(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return 0;
    }

    return session->TimeOfSession();
}
CATCH_RETURN()

STDAPI_(const XblMultiplayerSessionInitializationInfo*) XblMultiplayerSessionGetInitializationInfo(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return &session->InitializationInfo();
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(XblMultiplayerSessionChangeTypes) XblMultiplayerSessionSubscribedChangeTypes(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return XblMultiplayerSessionChangeTypes::None;
    }

    if (session->CurrentUserUnsafe() == nullptr)
    {
        return XblMultiplayerSessionChangeTypes::None;
    }
    return MultiplayerSessionMember::Get(session->CurrentUserUnsafe())->SubscribedChangeTypes();
}
CATCH_RETURN_WITH(XblMultiplayerSessionChangeTypes::None)

STDAPI XblMultiplayerSessionHostCandidates(
    _In_ XblMultiplayerSessionHandle session,
    _Out_ const XblDeviceToken** deviceTokens,
    _Out_ size_t* deviceTokensCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(deviceTokens == nullptr || deviceTokensCount == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);

    *deviceTokens = session->HostCandidates().data();
    *deviceTokensCount = session->HostCandidates().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI_(const XblMultiplayerSessionReference*) XblMultiplayerSessionSessionReference(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return &session->SessionReference();
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(const XblMultiplayerSessionConstants*) XblMultiplayerSessionSessionConstants(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return &session->SessionConstantsUnsafe();
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(void) XblMultiplayerSessionConstantsSetMaxMembersInSession(
    _In_ XblMultiplayerSessionHandle session,
    uint32_t maxMembersInSession
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetMaxMembersInSession(maxMembersInSession);
}
CATCH_RETURN_WITH(;)

STDAPI_(void) XblMultiplayerSessionConstantsSetVisibility(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerSessionVisibility visibility
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetVisibility(visibility);
}
CATCH_RETURN_WITH(;)

STDAPI XblMultiplayerSessionConstantsSetTimeouts(
    _In_ XblMultiplayerSessionHandle session,
    _In_ uint64_t memberReservedTimeout,
    _In_ uint64_t memberInactiveTimeout,
    _In_ uint64_t memberReadyTimeout,
    _In_ uint64_t sessionEmptyTimeout
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetTimeouts(memberReservedTimeout, memberInactiveTimeout, memberReadyTimeout, sessionEmptyTimeout);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionConstantsSetArbitrationTimeouts(
    _In_ XblMultiplayerSessionHandle session,
    _In_ uint64_t arbitrationTimeout,
    _In_ uint64_t forfeitTimeout
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(session);
    UNREFERENCED_PARAMETER(arbitrationTimeout);
    UNREFERENCED_PARAMETER(forfeitTimeout);

    return E_NOTIMPL;
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionConstantsSetQosConnectivityMetrics(
    _In_ XblMultiplayerSessionHandle session,
    _In_ bool enableLatencyMetric,
    _In_ bool enableBandwidthDownMetric,
    _In_ bool enableBandwidthUpMetric,
    _In_ bool enableCustomMetric
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetQosConnectivityMetrics(enableLatencyMetric, enableBandwidthDownMetric, enableBandwidthUpMetric, enableCustomMetric);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionConstantsSetMemberInitialization(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerMemberInitialization memberInitialization
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetMemberInitialization(std::move(memberInitialization));
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionConstantsSetPeerToPeerRequirements(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerPeerToPeerRequirements requirements
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetPeerToPeerRequirements(std::move(requirements));
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionConstantsSetPeerToHostRequirements(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerPeerToHostRequirements requirements
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetPeerToHostRequirements(std::move(requirements));
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson(
    _In_ XblMultiplayerSessionHandle session,
    _In_ const char* measurementServerAddressesJson
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(session == nullptr || measurementServerAddressesJson == nullptr);
    return session->SetMeasurementServerAddresses(measurementServerAddressesJson);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionConstantsSetCapabilities(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerSessionCapabilities capabilities
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetSessionCapabilities(std::move(capabilities));
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionConstantsSetCloudComputePackageJson(
    _In_ XblMultiplayerSessionHandle session,
    _In_ const char* sessionCloudComputePackageConstantsJson
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetCloudComputePackageJson(sessionCloudComputePackageConstantsJson);
}
CATCH_RETURN()

STDAPI_(const XblMultiplayerSessionProperties*) XblMultiplayerSessionSessionProperties(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return &session->SessionPropertiesUnsafe();
}
CATCH_RETURN_WITH(nullptr)

STDAPI XblMultiplayerSessionPropertiesSetKeywords(
    _In_ XblMultiplayerSessionHandle session,
    _In_ const char** keywords,
    _In_ size_t keywordsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetKeywords(keywords, keywordsCount);
}
CATCH_RETURN()

STDAPI_(void) XblMultiplayerSessionPropertiesSetJoinRestriction(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerSessionRestriction joinRestriction
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetJoinRestriction(joinRestriction);
}
CATCH_RETURN_WITH(;)

STDAPI_(void) XblMultiplayerSessionPropertiesSetReadRestriction(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerSessionRestriction readRestriction
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetReadRestriction(readRestriction);
}
CATCH_RETURN_WITH(;)

STDAPI XblMultiplayerSessionPropertiesSetTurnCollection(
    _In_ XblMultiplayerSessionHandle session,
    _In_ const uint32_t* turnCollectionMemberIds,
    _In_ size_t turnCollectionMemberIdsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetTurnCollection(xsapi_internal_vector<uint32_t>(turnCollectionMemberIds, turnCollectionMemberIds + turnCollectionMemberIdsCount));
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionRoleTypes(
    _In_ XblMultiplayerSessionHandle session,
    _Out_ const XblMultiplayerRoleType** roleTypes,
    _Out_ size_t* roleTypesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF(roleTypes == nullptr || roleTypesCount == nullptr);

    *roleTypes = session->RoleTypesUnsafe().Values().data();
    *roleTypesCount = session->RoleTypesUnsafe().Values().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionGetRoleByName(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* roleTypeName,
    _In_z_ const char* roleName,
    _Out_ const XblMultiplayerRole** role
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF(roleTypeName == nullptr || roleName == nullptr || role == nullptr);
    *role = session->RoleTypesUnsafe().GetRole(roleTypeName, roleName);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionSetMutableRoleSettings(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* roleTypeName,
    _In_z_ const char* roleName,
    _In_opt_ uint32_t* maxMemberCount,
    _In_opt_ uint32_t* targetMemberCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF(roleTypeName == nullptr || roleName == nullptr);
    return session->SetMutableRoleSettings(roleTypeName, roleName, maxMemberCount, targetMemberCount);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionMembers(
    _In_ XblMultiplayerSessionHandle session,
    _Out_ const XblMultiplayerSessionMember** members,
    _Out_ size_t* membersCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF(members == nullptr || membersCount == nullptr);

    *members = session->MembersUnsafe().data();
    *membersCount = session->MembersUnsafe().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI_(const XblMultiplayerSessionMember*) XblMultiplayerSessionGetMember(
    _In_ XblMultiplayerSessionHandle session,
    _In_ uint32_t memberId
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    // Could store these in a map to improve lookup but member count should not be large
    auto& members = session->MembersUnsafe();
    for (const auto& member : members)
    {
        if (member.MemberId == memberId)
        {
            return &member;
        }
    }
    return nullptr;
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(const XblMultiplayerMatchmakingServer*) XblMultiplayerSessionMatchmakingServer(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return session->MatchmakingServer().get();
}
CATCH_RETURN_WITH(nullptr)

XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED
STDAPI_(const XblMultiplayerTournamentsServer*) XblMultiplayerSessionTournamentsServer(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(session);
    return nullptr;
}
CATCH_RETURN_WITH(nullptr)
XBL_WARNING_POP

XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED
STDAPI_(const XblMultiplayerArbitrationServer*) XblMultiplayerSessionArbitrationServer(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(session);
    return nullptr;
}
CATCH_RETURN_WITH(nullptr)
XBL_WARNING_POP

STDAPI_(uint32_t) XblMultiplayerSessionMembersAccepted(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return 0;
    }

    return session->MembersAccepted();
}
CATCH_RETURN()

STDAPI_(const char*) XblMultiplayerSessionRawServersJson(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return session->RawServersJsonUnsafe().data();
}
CATCH_RETURN_WITH(nullptr)

STDAPI XblMultiplayerSessionSetRawServersJson(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* rawServersJson
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetServersJson(rawServersJson);
}
CATCH_RETURN()

STDAPI_(const char*) XblMultiplayerSessionEtag(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return session->ETagUnsafe().data();
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(const XblMultiplayerSessionMember*) XblMultiplayerSessionCurrentUser(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return session->CurrentUserUnsafe();
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(const XblMultiplayerSessionInfo*) XblMultiplayerSessionGetInfo(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return nullptr;
    }

    return &session->SessionInfo();
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(XblWriteSessionStatus) XblMultiplayerSessionWriteStatus(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        // TODO: is this correct?
        return XblWriteSessionStatus::Unknown;
    }
    else
    {
        return session->WriteStatus();
    }
}
CATCH_RETURN_WITH(XblWriteSessionStatus::Unknown)

STDAPI XblMultiplayerSessionAddMemberReservation(
    _In_ XblMultiplayerSessionHandle session,
    _In_ uint64_t xuid,
    _In_opt_z_ const char* memberCustomConstantsJson,
    _In_ bool initializeRequested
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->AddMemberReservation(xuid, memberCustomConstantsJson, initializeRequested);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionJoin(
    _In_ XblMultiplayerSessionHandle session,
    _In_opt_z_ const char* memberCustomConstantsJson,
    _In_ bool initializeRequested,
    _In_ bool joinWithActiveStatus
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->Join(memberCustomConstantsJson, initializeRequested, joinWithActiveStatus);
}
CATCH_RETURN()

STDAPI_(void) XblMultiplayerSessionSetInitializationSucceeded(
    _In_ XblMultiplayerSessionHandle session,
    _In_ bool initializationSucceeded
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetInitializationStatus(initializationSucceeded);
}
CATCH_RETURN_WITH(;)

STDAPI_(void) XblMultiplayerSessionSetHostDeviceToken(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblDeviceToken hostDeviceToken
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetHostDeviceToken(hostDeviceToken);
}
CATCH_RETURN_WITH(;)

STDAPI_(void) XblMultiplayerSessionSetMatchmakingServerConnectionPath(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* serverConnectionPath
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    return session->SetMatchmakingServerConnectionPath(serverConnectionPath);
}
CATCH_RETURN_WITH(;)

STDAPI_(void) XblMultiplayerSessionSetClosed(
    _In_ XblMultiplayerSessionHandle session,
    _In_ bool closed
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetClosed(closed);
}
CATCH_RETURN_WITH(;)

STDAPI_(void) XblMultiplayerSessionSetLocked(
    _In_ XblMultiplayerSessionHandle session,
    _In_ bool locked
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetLocked(locked);
}
CATCH_RETURN_WITH(;)

STDAPI_(void) XblMultiplayerSessionSetAllocateCloudCompute(
    _In_ XblMultiplayerSessionHandle session,
    _In_ bool allocateCloudCompute
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetAllocateCloudCompute(allocateCloudCompute);
}
CATCH_RETURN_WITH(;)

STDAPI_(void) XblMultiplayerSessionSetMatchmakingResubmit(
    _In_ XblMultiplayerSessionHandle session,
    _In_ bool matchResubmit
) XBL_NOEXCEPT
try
{
    if (session == nullptr)
    {
        return;
    }

    session->SetMatchmakingResubmit(matchResubmit);
}
CATCH_RETURN_WITH(;)

STDAPI XblMultiplayerSessionSetServerConnectionStringCandidates(
    _In_ XblMultiplayerSessionHandle session,
    _In_reads_(serverConnectionStringCandidatesCount) const char** serverConnectionStringCandidates,
    _In_ size_t serverConnectionStringCandidatesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetServerConnectionStringCandidates(serverConnectionStringCandidates, serverConnectionStringCandidatesCount);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionSetSessionChangeSubscription(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerSessionChangeTypes changeTypes
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetSessionChangeSubscription(changeTypes);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionLeave(
    _In_ XblMultiplayerSessionHandle session
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->Leave();
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserSetStatus(
    _In_ XblMultiplayerSessionHandle session,
    _In_ XblMultiplayerSessionMemberStatus status
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetCurrentUserStatus(status);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64(
    _In_ XblMultiplayerSessionHandle session,
    _In_ const char* value
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    return session->SetCurrentUserSecureDeviceAddressBase64(value);
}
CATCH_RETURN()

#if HC_PLATFORM != HC_PLATFORM_XDK && HC_PLATFORM != HC_PLATFORM_UWP
STDAPI XblFormatSecureDeviceAddress(
    _In_ const char* deviceId,
    _Inout_ XblFormattedSecureDeviceAddress* address
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(address);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(deviceId);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(deviceId);

    auto sda = utils::format_secure_device_address(deviceId);
    utils::strcpy(address->value, sizeof(address->value), sda.c_str());
    return S_OK;
}
CATCH_RETURN()
#endif

STDAPI XblMultiplayerSessionCurrentUserSetRoles(
    _In_ XblMultiplayerSessionHandle session,
    _In_ const XblMultiplayerSessionMemberRole* roles,
    _In_ size_t rolesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(roles);
    return session->SetCurrentUserRoleInfo(xsapi_internal_vector<XblMultiplayerSessionMemberRole>(roles, roles + rolesCount));
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserSetMembersInGroup(
    _In_ XblMultiplayerSessionHandle session,
    _In_reads_(memberIdsCount) uint32_t* memberIds,
    _In_ size_t memberIdsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(memberIds);
    return session->SetCurrentUserMembersInGroup(xsapi_internal_vector<uint32_t>(memberIds, memberIds + memberIdsCount));
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserSetGroups(
    _In_ XblMultiplayerSessionHandle session,
    _In_reads_(groupsCount) const char** groups,
    _In_ size_t groupsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(groups);
    return session->SetCurrentUserGroups(groups, groupsCount);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserSetEncounters(
    _In_ XblMultiplayerSessionHandle session,
    _In_reads_(encountersCount) const char** encounters,
    _In_ size_t encountersCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(encounters);
    return session->SetCurrentUserEncounters(encounters, encountersCount);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserSetQosMeasurements(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* measurements
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(measurements);
    return session->SetCurrentUserQosMeasurementsJson(measurements);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserSetServerQosMeasurements(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* measurements
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(measurements);
    return session->SetCurrentUserServerMeasurementsJson(measurements);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserSetCustomPropertyJson(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* name,
    _In_z_ const char* valueJson
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF(name == nullptr || valueJson == nullptr);

    JsonDocument valueJsonObject;
    auto hr = JsonUtils::ValidateJson(valueJson, valueJsonObject);
    if (SUCCEEDED(hr))
    {
        hr = session->SetCurrentUserMemberCustomPropertyJson(name, valueJsonObject);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* name
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(name);
    return session->DeleteCurrentUserMemberCustomPropertyJson(name);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson(
    _In_ XblMultiplayerSessionHandle session,
    _In_ const char* matchmakingTargetSessionConstantsJson
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(session == nullptr || matchmakingTargetSessionConstantsJson == nullptr);
    return session->SetMatchmakingTargetSessionConstantsJson(matchmakingTargetSessionConstantsJson);
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionSetCustomPropertyJson(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* name,
    _In_z_ const char* valueJson
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF(name == nullptr || valueJson == nullptr);

    JsonDocument valueJsonObject;
    auto hr = JsonUtils::ValidateJson(valueJson, valueJsonObject);
    if (SUCCEEDED(hr))
    {
        hr = session->SetSessionCustomPropertyJson(name, valueJsonObject);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerSessionDeleteCustomPropertyJson(
    _In_ XblMultiplayerSessionHandle session,
    _In_z_ const char* name
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(session);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(name);
    return session->DeleteSessionCustomPropertyJson(name);
}
CATCH_RETURN()

STDAPI_(XblMultiplayerSessionChangeTypes) XblMultiplayerSessionCompare(
    _In_ XblMultiplayerSessionHandle currentSessionHandle,
    _In_ XblMultiplayerSessionHandle oldSessionHandle
) XBL_NOEXCEPT
try
{
    if (currentSessionHandle == nullptr || oldSessionHandle == nullptr)
    {
        return XblMultiplayerSessionChangeTypes::None;
    }
    return currentSessionHandle->CompareMultiplayerSessions(oldSessionHandle->shared_from_this());
}
CATCH_RETURN_WITH(XblMultiplayerSessionChangeTypes::None)
