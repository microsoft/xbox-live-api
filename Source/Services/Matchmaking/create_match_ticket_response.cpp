// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "matchmaking_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN
const xsapi_internal_string& CreateMatchTicketResponse::MatchTicketId() const
{
    return m_matchTicketId;
}

const std::chrono::seconds& CreateMatchTicketResponse::EstimatedWaitTime() const
{
    return m_estimatedWaitTime;
}

CreateMatchTicketResponse::CreateMatchTicketResponse()
{}

CreateMatchTicketResponse::CreateMatchTicketResponse(
    _In_ const xsapi_internal_string matchTicketId,
    _In_ std::chrono::seconds estimatedWaitTime
)
    : m_matchTicketId(matchTicketId), 
    m_estimatedWaitTime(estimatedWaitTime)
{}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END