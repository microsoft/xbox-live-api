// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "matchmaking_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN
HopperStatisticsResponse::HopperStatisticsResponse()
{}

HopperStatisticsResponse::HopperStatisticsResponse(
    _In_ xsapi_internal_string hopperName,
    _In_ std::chrono::seconds estimatedWaitTime,
    _In_ uint32_t playersWaitingToMatch
)
    : m_hopperName(hopperName), 
    m_estimatedWaitTime(estimatedWaitTime), 
    m_playersWaitingToMatch(playersWaitingToMatch)
{
}

const xsapi_internal_string& HopperStatisticsResponse::HopperName() const
{
    return m_hopperName;
}

const std::chrono::seconds& HopperStatisticsResponse::EstimatedWaitTime() const
{
    return m_estimatedWaitTime;
}

uint32_t HopperStatisticsResponse::PlayersWaitingToMatch() const
{
    return m_playersWaitingToMatch;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END