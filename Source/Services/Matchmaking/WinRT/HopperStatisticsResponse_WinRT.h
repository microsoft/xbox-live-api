//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi\multiplayer.h"
#include "xsapi\matchmaking.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_BEGIN

/// <summary>
/// Represents a server response to a hopper statistics request.
/// </summary>
public ref class HopperStatisticsResponse sealed
{
public:
    /// <summary>
    /// Name of the hopper in which a match was requested.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(HopperName, hopper_name)

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(EstimatedWaitTime, estimated_wait_time)

    /// <summary>
    /// The number of players in the hopper waiting to be matched.
    /// </summary>
    DEFINE_PROP_GET_OBJ(PlayersWaitingToMatch, players_waiting_to_match, uint32)

internal:
    HopperStatisticsResponse(
        _In_ xbox::services::matchmaking::hopper_statistics_response cppObj
        ) :
        m_cppObj(std::move(cppObj))
    {
    }

private:
    xbox::services::matchmaking::hopper_statistics_response m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_END