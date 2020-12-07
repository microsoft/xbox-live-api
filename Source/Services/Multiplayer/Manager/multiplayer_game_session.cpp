// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_manager_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerGameSession::MultiplayerGameSession():
    m_changeNumber(0),
    m_sessionReference{},
    m_sessionConstants{},
    m_memberInitialization{}
{
}

MultiplayerGameSession::MultiplayerGameSession(_In_ const MultiplayerGameSession& other):
    m_correlationId(other.m_correlationId),
    m_changeNumber(other.m_changeNumber),
    m_sessionReference(other.m_sessionReference),
    m_host(other.m_host),
    m_members(other.m_members),
    m_properties(other.m_properties),
    m_multiplayerClientManager(other.m_multiplayerClientManager),
    m_sessionConstants {},
    m_memberInitialization{}
{
    DeepCopyConstants(other.m_sessionConstants);
}

MultiplayerGameSession::MultiplayerGameSession(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ std::shared_ptr<MultiplayerMember> host,
    _In_ xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> members
    ):
    m_correlationId(session->SessionInfo().CorrelationId),
    m_changeNumber(session->SessionInfo().ChangeNumber),
    m_sessionReference(session->SessionReference()),
    m_host(std::move(host)),
    m_members(std::move(members)),
    m_sessionConstants{},
    m_memberInitialization{}
{
    XblMultiplayerSessionReadLockGuard sessionSafe(session);
    DeepCopyConstants(sessionSafe.SessionConstants());

    if (sessionSafe.SessionProperties().SessionCustomPropertiesJson != nullptr)
    {
        m_properties = sessionSafe.SessionProperties().SessionCustomPropertiesJson;
    }
}

const XblMultiplayerSessionReference&
MultiplayerGameSession::SessionReference() const
{
    return m_sessionReference;
}

const xsapi_internal_string&
MultiplayerGameSession::CorrelationId() const
{
    return m_correlationId;
}

uint64_t
MultiplayerGameSession::ChangeNumber() const
{
    return m_changeNumber;
}

std::shared_ptr<MultiplayerMember>
MultiplayerGameSession::Host() const
{
    return m_host;
}

void
MultiplayerGameSession::SetHost(
    _In_ std::shared_ptr<MultiplayerMember> hostMember
    )
{
    m_host = hostMember;
}

const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>&
MultiplayerGameSession::Members() const
{
    return m_members;
}

const XblMultiplayerSessionConstants&
MultiplayerGameSession::SessionConstants() const
{
    return m_sessionConstants;
}

const xsapi_internal_string&
MultiplayerGameSession::Properties() const
{
    return m_properties;
}

HRESULT
MultiplayerGameSession::SetProperties(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    return m_multiplayerClientManager->SetProperties(m_sessionReference, name, valueJson, context);
}

bool
MultiplayerGameSession::IsHost( 
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
MultiplayerGameSession::SetSynchronizedHost(
    _In_ const xsapi_internal_string& deviceToken,
    _In_opt_ context_t context
    )
{
    return m_multiplayerClientManager->SetSynchronizedHost(m_sessionReference, deviceToken, context);
}

HRESULT
MultiplayerGameSession::SetSynchronizedProperties(
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    return m_multiplayerClientManager->SetSynchronizedProperties(m_sessionReference, name, valueJson, context);
}

void
MultiplayerGameSession::SetMultiplayerClientManager(
    _In_ std::shared_ptr<MultiplayerClientManager> clientManager
    )
{
    m_multiplayerClientManager = clientManager;
}

void MultiplayerGameSession::DeepCopyConstants(const XblMultiplayerSessionConstants& other)
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