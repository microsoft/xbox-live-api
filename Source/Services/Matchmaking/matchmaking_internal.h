#pragma once

#include "xsapi-c/matchmaking_c.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN

class MatchmakingService : public std::enable_shared_from_this<MatchmakingService> 
{

public:
    MatchmakingService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
    );

    HRESULT CreateMatchTicket(
        _In_ const XblMultiplayerSessionReference& ticketSessionReference,
        _In_ const xsapi_internal_string& matchmakingServiceConfigurationId,
        _In_ const xsapi_internal_string& hopperName,
        _In_ const std::chrono::seconds& ticketTimeout,
        _In_ XblPreserveSessionMode preserveSession,
        _In_ JsonValue& ticketAttributesJson,
        _In_ XAsyncBlock* async
    );

    HRESULT DeleteMatchTicketAsync(
        _In_ const xsapi_internal_string& serviceConfigurationId,
        _In_ const xsapi_internal_string& hopperName,
        _In_ const xsapi_internal_string& ticketId,
        _In_ XAsyncBlock* async
    );

    HRESULT GetMatchTicketDetailsAsync(
        _In_ const xsapi_internal_string& serviceConfigurationId,
        _In_ const xsapi_internal_string& hopperName,
        _In_ const xsapi_internal_string& ticketId,
        _In_ XAsyncBlock* async
    );

    HRESULT GetHopperStatistics(
        _In_ const xsapi_internal_string& serviceConfigurationId,
        _In_ const xsapi_internal_string& hopperName,
        _In_ XAsyncBlock* async
    );

private:
    static xsapi_internal_string GetMatchmakingSubPath(
        _In_ const xsapi_internal_string& serviceConfigId,
        _In_ const xsapi_internal_string& hopperName,
        _In_ const xsapi_internal_string& ticketId
    );

    static xsapi_internal_string GetHopperSubPath(
        _In_ const xsapi_internal_string& serviceConfigId,
        _In_ const xsapi_internal_string& hopperName
    );

    static xsapi_internal_string ConvertPreserveSessionModeToString(
        _In_ XblPreserveSessionMode preserve_session
    );

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_contextSettings;
};

/// <summary>
/// Represents a server response to a hopper statistics request.
/// </summary>
class HopperStatisticsResponse 
{
public:
    HopperStatisticsResponse();

    HopperStatisticsResponse(
        _In_ xsapi_internal_string hopperName,
        _In_ std::chrono::seconds estimatedWaitTime,
        _In_ uint32_t playersWaitingToMatch
    );

    /// <summary>
    /// Name of the hopper in which a match was requested.
    /// </summary>
    const xsapi_internal_string& HopperName() const;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    const std::chrono::seconds& EstimatedWaitTime() const;

    /// <summary>
    /// The number of players in the hopper waiting to be matched.
    /// </summary>
    uint32_t PlayersWaitingToMatch() const;

private:
    /// <summary>
    /// Name of the hopper in which a match was requested.
    /// </summary>
    xsapi_internal_string m_hopperName;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    std::chrono::seconds m_estimatedWaitTime{};

    /// <summary>
    /// The number of players in the hopper waiting to be matched.
    /// </summary>
    uint32_t m_playersWaitingToMatch{ 0 };
};

/// <summary>
/// Represents a server response to a request for match ticket details.
/// </summary>
class MatchTicketDetailsResponse
{
public:
    MatchTicketDetailsResponse();

    MatchTicketDetailsResponse (
        _In_ XblTicketStatus matchStatus,
        _In_ std::chrono::seconds estimatedWaitTime,
        _In_ XblPreserveSessionMode preserveSession,
        _In_ XblMultiplayerSessionReference ticketSession,
        _In_ XblMultiplayerSessionReference targetSession,
        _In_ const JsonValue& ticketAttributes
    );

    MatchTicketDetailsResponse(const MatchTicketDetailsResponse& other);

    MatchTicketDetailsResponse& operator =(const MatchTicketDetailsResponse& other);

    /// <summary>
    /// Status of a match request.
    /// </summary>
    const XblTicketStatus MatchStatus() const;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    const std::chrono::seconds& EstimatedWaitTime() const;

    /// <summary>
    /// An enum value to specify whether the match should preserve the session on which the match has been requested.
    /// </summary>
    const XblPreserveSessionMode PreserveSession() const;

    /// <summary>
    /// The session on which the match was requested.
    /// </summary>
    const XblMultiplayerSessionReference TicketSession() const;

    /// <summary>
    /// The session on which a match request has been found.
    /// </summary>
    const XblMultiplayerSessionReference TargetSession() const;

    /// <summary>
    /// The attributes of a match request.
    /// </summary>
    const JsonValue& TicketAttributes() const;

    static XblTicketStatus ConvertStringToTicketStatus(_In_ const xsapi_internal_string& value);
    static XblPreserveSessionMode ConvertStringToPreserveSessionMode(_In_ const xsapi_internal_string& value);

private:

    /// <summary>
    /// Status of a match request.
    /// </summary>
    XblTicketStatus m_matchStatus{ XblTicketStatus::Unknown };

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    std::chrono::seconds m_estimatedWaitTime{};

    /// <summary>
    /// An enum value to specify whether the match should preserve the session on which the match has been requested.
    /// </summary>
    XblPreserveSessionMode m_preserveSession{ XblPreserveSessionMode::Unknown };

    /// <summary>
    /// The session on which the match was requested.
    /// </summary>
    XblMultiplayerSessionReference m_ticketSession{};

    /// <summary>
    /// The session on which a match request has been found.
    /// </summary>
    XblMultiplayerSessionReference m_targetSession{};

    /// <summary>
    /// The attributes of a match request.
    /// </summary>
    JsonDocument m_ticketAttributes;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END