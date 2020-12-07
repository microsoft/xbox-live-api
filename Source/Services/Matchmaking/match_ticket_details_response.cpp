// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "matchmaking_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN
const XblTicketStatus MatchTicketDetailsResponse::MatchStatus() const
{
    return m_matchStatus;
}

const std::chrono::seconds& MatchTicketDetailsResponse::EstimatedWaitTime() const
{
    return m_estimatedWaitTime;
}

const XblPreserveSessionMode MatchTicketDetailsResponse::PreserveSession() const
{
    return m_preserveSession;
}

const XblMultiplayerSessionReference MatchTicketDetailsResponse::TicketSession() const
{
    return m_ticketSession;
}

const XblMultiplayerSessionReference MatchTicketDetailsResponse::TargetSession() const
{
    return m_targetSession;
}

const JsonValue& MatchTicketDetailsResponse::TicketAttributes() const
{
    return m_ticketAttributes;
}

MatchTicketDetailsResponse::MatchTicketDetailsResponse()
{
}

MatchTicketDetailsResponse::MatchTicketDetailsResponse(
    _In_ XblTicketStatus matchStatus,
    _In_ std::chrono::seconds estimatedWaitTime,
    _In_ XblPreserveSessionMode preserveSession,
    _In_ XblMultiplayerSessionReference ticketSession,
    _In_ XblMultiplayerSessionReference targetSession,
    _In_ const JsonValue& ticketAttributes
) :
    m_matchStatus(matchStatus),
    m_estimatedWaitTime(estimatedWaitTime),
    m_preserveSession(preserveSession),
    m_ticketSession(ticketSession),
    m_targetSession(targetSession)
{
    JsonUtils::CopyFrom(m_ticketAttributes, ticketAttributes);
}

MatchTicketDetailsResponse::MatchTicketDetailsResponse(const MatchTicketDetailsResponse& other)
    :
    m_matchStatus(other.m_matchStatus),
    m_estimatedWaitTime(other.m_estimatedWaitTime),
    m_preserveSession(other.m_preserveSession),
    m_ticketSession(other.m_ticketSession),
    m_targetSession(other.m_targetSession)
{
    JsonUtils::CopyFrom(m_ticketAttributes, other.m_ticketAttributes);
}

MatchTicketDetailsResponse& MatchTicketDetailsResponse::operator =(const MatchTicketDetailsResponse& other)
{
    if (this != &other)
    {
        m_matchStatus = other.m_matchStatus;
        m_estimatedWaitTime = other.m_estimatedWaitTime;
        m_preserveSession = other.m_preserveSession;
        m_ticketSession = other.m_ticketSession;
        m_targetSession = other.m_targetSession;
        JsonUtils::CopyFrom(m_ticketAttributes, other.m_ticketAttributes);
    }
    return *this;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END