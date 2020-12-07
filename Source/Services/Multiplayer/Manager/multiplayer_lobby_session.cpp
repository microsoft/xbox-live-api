// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerLobbySession::MultiplayerLobbySession(
    _In_ std::shared_ptr<MultiplayerClientManager> multiplayerClientManagerInstance
    ):
    m_multiplayerClientManager(std::move(multiplayerClientManagerInstance)),
    m_changeNumber(0),
    m_sessionReference{},
    m_sessionConstants{},
    m_memberInitialization{}
{
}

MultiplayerLobbySession::MultiplayerLobbySession():
    m_changeNumber(0),
    m_sessionReference{},
    m_sessionConstants{},
    m_memberInitialization{}
{
}

MultiplayerLobbySession::MultiplayerLobbySession(_In_ const MultiplayerLobbySession& other):
    m_multiplayerClientManager(other.m_multiplayerClientManager),
    m_correlationId(other.m_correlationId),
    m_changeNumber(other.m_changeNumber),
    m_sessionReference(other.m_sessionReference),
    m_host(other.m_host),
    m_members(other.m_members),
    m_localMembers(other.m_localMembers),
    m_customPropertiesJson(other.m_customPropertiesJson)
{
    DeepCopyConstants(other.m_sessionConstants);
}

MultiplayerLobbySession::MultiplayerLobbySession(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ std::shared_ptr<MultiplayerMember> host,
    _In_ const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& members,
    _In_ const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& localMembers
    ) :
    m_correlationId(session->SessionInfo().CorrelationId),
    m_changeNumber(session->SessionInfo().ChangeNumber),
    m_sessionReference(session->SessionReference()),
    m_host(std::move(host)),
    m_members(members),
    m_localMembers(localMembers)
{
    XblMultiplayerSessionReadLockGuard sessionSafe(session);
    m_customPropertiesJson = sessionSafe.SessionProperties().SessionCustomPropertiesJson;
    m_sessionConstants = sessionSafe.SessionConstants();
    DeepCopyConstants(sessionSafe.SessionConstants());
}

MultiplayerLobbySession::~MultiplayerLobbySession()
{
}

HRESULT
MultiplayerLobbySession::AddLocalUser(
    _In_ xbox_live_user_t user
    )
{
    if (m_multiplayerClientManager->LatestPendingRead() == nullptr)
    {
        m_multiplayerClientManager->Initialize();
    }
    return m_multiplayerClientManager->LobbyClient()->AddLocalUser(user, MultiplayerLocalUserLobbyState::Add);
}

HRESULT
MultiplayerLobbySession::RemoveLocalUser(
    _In_ xbox_live_user_t user
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->LobbyClient()->RemoveLocalUser(user);
}

HRESULT
MultiplayerLobbySession::SetLocalMemberProperties(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->LobbyClient()->SetLocalMemberProperties(user, name, valueJson, context);
}

HRESULT
MultiplayerLobbySession::DeleteLocalMemberProperties(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_string& name,
    _In_opt_ context_t context
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->LobbyClient()->DeleteLocalMemberProperties(user, name, context);
}

HRESULT
MultiplayerLobbySession::SetLocalMemberConnectionAddress(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_string& connectionAddress,
    _In_opt_ context_t context
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->LobbyClient()->SetLocalMemberConnectionAddress(user, connectionAddress, context);
}

const XblMultiplayerSessionReference&
MultiplayerLobbySession::SessionReference() const
{
    return m_sessionReference;
}

const xsapi_internal_string&
MultiplayerLobbySession::CorrelationId() const
{
    return m_correlationId;
}

uint64_t
MultiplayerLobbySession::ChangeNumber() const
{
    return m_changeNumber;
}

std::shared_ptr<MultiplayerMember>
MultiplayerLobbySession::Host() const
{
    return m_host;
}

void
MultiplayerLobbySession::SetHost(
    _In_ std::shared_ptr<MultiplayerMember> hostMember
    )
{
    m_host = hostMember;
}

const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>&
MultiplayerLobbySession::Members() const
{
    return m_members;
}

const XblMultiplayerSessionConstants&
MultiplayerLobbySession::SessionConstants() const
{
    return m_sessionConstants;
}

const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>&
MultiplayerLobbySession::LocalMembers() const
{
    return m_localMembers;
}

const xsapi_internal_string&
MultiplayerLobbySession::CustomPropertiesJson() const
{
    return m_customPropertiesJson;
}

HRESULT
MultiplayerLobbySession::SetProperties(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->SetProperties(m_sessionReference, name, valueJson, context);
}

bool
MultiplayerLobbySession::IsHost(
    _In_ uint64_t xuid
    )
{
    if (m_host == nullptr || m_members.size() == 0)
    {
        return false;
    }

    return xuid == m_host->Xuid();
}

HRESULT
MultiplayerLobbySession::SetSynchronizedHost(
    _In_ const xsapi_internal_string& hostDeviceToken,
    _In_opt_ context_t context
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->SetSynchronizedHost(m_sessionReference, hostDeviceToken, context);
}

HRESULT
MultiplayerLobbySession::SetSynchronizedProperties(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->SetSynchronizedProperties(m_sessionReference, name, valueJson, context);
}

void
MultiplayerLobbySession::SetMultiplayerClientManager(
    _In_ std::shared_ptr<MultiplayerClientManager> clientManager
    )
{
    m_multiplayerClientManager = clientManager;
}

#if HC_PLATFORM_IS_MICROSOFT
HRESULT
MultiplayerLobbySession::InviteFriends(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_string& contextStringId,
    _In_ const xsapi_internal_string& customActivationContext
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->InviteFriends(user, contextStringId, customActivationContext);
}
#endif

HRESULT
MultiplayerLobbySession::InviteUsers(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_vector<uint64_t>& xboxUserIds,
    _In_ const xsapi_internal_string& contextStringId,
    _In_ const xsapi_internal_string& customActivationContext
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager->LatestPendingRead() == nullptr, E_UNEXPECTED, "No user added. Call add_local_user() first.");
    return m_multiplayerClientManager->InviteUsers(user, xboxUserIds, contextStringId, customActivationContext);
}

void MultiplayerLobbySession::DeepCopyConstants(const XblMultiplayerSessionConstants& other)
{
    m_sessionConstants = other;
    m_initiatorXuids = xsapi_internal_vector<uint64_t>(other.InitiatorXuids, other.InitiatorXuids + other.InitiatorXuidsCount);
    m_sessionConstants.InitiatorXuids = m_initiatorXuids.data();
    if (m_sessionConstants.MemberInitialization)
    {
        m_memberInitialization = *m_sessionConstants.MemberInitialization;
        m_sessionConstants.MemberInitialization = &m_memberInitialization;
    }
    if (m_sessionConstants.CustomJson)
    {
        m_constantsCustomJson = m_sessionConstants.CustomJson;
        m_sessionConstants.CustomJson = m_constantsCustomJson.data();
    }
    if (m_sessionConstants.SessionCloudComputePackageConstantsJson)
    {
        m_constantsCloudComputePackageJson = m_sessionConstants.SessionCloudComputePackageConstantsJson;
        m_sessionConstants.SessionCloudComputePackageConstantsJson = m_constantsCloudComputePackageJson.data();
    }
    if (m_sessionConstants.MeasurementServerAddressesJson)
    {
        m_constantsMeasurementServerAddressesJson = m_sessionConstants.MeasurementServerAddressesJson;
        m_sessionConstants.MeasurementServerAddressesJson = m_constantsMeasurementServerAddressesJson.data();
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
