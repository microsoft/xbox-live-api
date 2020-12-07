// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerClientPendingRequest::MultiplayerClientPendingRequest() :
    m_context(nullptr),
    m_requestType(PendingRequestType::NonSynchronizedChanges),
    m_localUserLobbyState(MultiplayerLocalUserLobbyState::Unknown),
    m_joinability(XblMultiplayerJoinability::None)
{
}

PendingRequestType
MultiplayerClientPendingRequest::RequestType() const
{
    return m_requestType;
}

context_t
MultiplayerClientPendingRequest::Context()
{
    return m_context;
}

uint32_t
MultiplayerClientPendingRequest::Identifier() const
{
    return m_identifier;
}

// Local User properties
std::shared_ptr<MultiplayerLocalUser>
MultiplayerClientPendingRequest::LocalUser()
{
    return m_localUser;
}

void
MultiplayerClientPendingRequest::SetLocalUser(
    _In_ std::shared_ptr<MultiplayerLocalUser> user
    )
{
    m_localUser = user;
}

MultiplayerLocalUserLobbyState
MultiplayerClientPendingRequest::LobbyState()
{
    return m_localUserLobbyState;
}

void
MultiplayerClientPendingRequest::SetLobbyState(
    _In_ MultiplayerLocalUserLobbyState userState
    )
{
    m_localUserLobbyState = userState;
}

const xsapi_internal_string&
MultiplayerClientPendingRequest::LobbyHandleId() const
{
    return m_lobbyHandleId;
}

void
MultiplayerClientPendingRequest::SetLobbyHandleId(_In_ const xsapi_internal_string& handleId)
{
    m_lobbyHandleId = handleId;
}

const xsapi_internal_string&
MultiplayerClientPendingRequest::LocalUserSecureDeivceAddress() const
{
    return m_localUserSecureDeivceAddress;
}

void
MultiplayerClientPendingRequest::SetLocalUserConnectionAddress(
    _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
    _In_ const xsapi_internal_string& connectionAddress,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_localUser = localUser;
    m_localUserSecureDeivceAddress = utils::format_secure_device_address(connectionAddress);
}

const xsapi_internal_map<xsapi_internal_string, JsonDocument>&
MultiplayerClientPendingRequest::LocalUserProperties() const
{
    return m_localUserProperties;
}

void
MultiplayerClientPendingRequest::SetLocalUserProperties(
    _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_localUser = localUser;
    JsonUtils::CopyFrom(m_localUserProperties[name], valueJson);
}

// Session non-synchronized properties
XblMultiplayerJoinability
MultiplayerClientPendingRequest::Joinability()
{
    return m_joinability;
}

void MultiplayerClientPendingRequest::SetJoinability(
    _In_ XblMultiplayerJoinability value,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_joinability = value;
}

const xsapi_internal_map<xsapi_internal_string, JsonDocument>&
MultiplayerClientPendingRequest::SessionProperties() const
{
    return m_sessionProperties;
}

void
MultiplayerClientPendingRequest::SetSessionProperties(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    m_context = context;
    JsonUtils::CopyFrom(m_sessionProperties[name], valueJson);
}

// Session synchronized properties
const xsapi_internal_string&
MultiplayerClientPendingRequest::SynchronizedHostDeviceToken() const
{
    return m_synchronizedHostDeviceToken;
}

void
MultiplayerClientPendingRequest::SetSynchronizedHostDeviceToken(
    _In_ const xsapi_internal_string& hostDeviceToken,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_requestType = PendingRequestType::SynchronizedChanges;
    m_synchronizedHostDeviceToken = hostDeviceToken;
}

const xsapi_internal_map<xsapi_internal_string, JsonDocument>&
MultiplayerClientPendingRequest::SynchronizedSessionProperties() const
{
    return m_synchronizedSessionProperties;
}

void
MultiplayerClientPendingRequest::SetSynchronizedSessionProperties(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_requestType = PendingRequestType::SynchronizedChanges;
    JsonUtils::CopyFrom(m_synchronizedSessionProperties[name], valueJson);
}

void
MultiplayerClientPendingRequest::AppendPendingChanges(
    _In_ std::shared_ptr<XblMultiplayerSession> sessionToCommit,
    _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
    _In_ bool isGameInProgress
    )
{
    // Apply local user properties
    if (localUser != nullptr && m_localUser != nullptr && localUser->Xuid() == m_localUser->Xuid())
    {
        XblMultiplayerSessionReadLockGuard sessionToCommitSafe(sessionToCommit);
        if (!m_localUserSecureDeivceAddress.empty())
        {
            // Assign connection address to the localUser instance as it's used in other places locally.
            localUser->SetConnectionAddress(m_localUserSecureDeivceAddress);
            if (sessionToCommitSafe.CurrentUser() != nullptr)
            {
                sessionToCommitSafe.CurrentUserInternal()->SetSecureDeviceBaseAddress64(m_localUserSecureDeivceAddress);
            }
        }

        if (m_localUserProperties.size() > 0 && sessionToCommitSafe.CurrentUser() != nullptr)
        {
            for (const auto& prop : m_localUserProperties)
            {
                sessionToCommitSafe.CurrentUserInternal()->SetCustomPropertyJson(prop.first, prop.second);
            }
        }

        localUser->SetWriteChangesToService(false);
    }

    // Apply session non-sync properties

    if (m_joinability != XblMultiplayerJoinability::None)
    {
        MultiplayerManagerUtils::SetJoinability(m_joinability, sessionToCommit, isGameInProgress);
    }

    if (m_sessionProperties.size() > 0)
    {
        for (const auto& prop : m_sessionProperties)
        {
            sessionToCommit->SetSessionCustomPropertyJson(prop.first, prop.second);
        }
    }

    // Apply session sync properties

    if (!m_synchronizedHostDeviceToken.empty())
    {
        sessionToCommit->SetHostDeviceToken(m_synchronizedHostDeviceToken.data());
    }

    if (m_synchronizedSessionProperties.size() > 0)
    {
        for (const auto& prop : m_synchronizedSessionProperties)
        {
            sessionToCommit->SetSessionCustomPropertyJson(prop.first, prop.second);
        }
    }
}

std::atomic<uint32_t> MultiplayerClientPendingRequest::s_nextUniqueIdentifier{ 0 };

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END