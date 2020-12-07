// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/matchmaking_c.h"
#include "xsapi-cpp/multiplayer.h"

namespace xbox { namespace services {
    class xbox_live_context;
    /// <summary>
    /// Contains classes and enumerations that let you match
    /// players for a multiplayer session.
    /// </summary>
    namespace matchmaking {

/// <summary>
/// Defines values used to indicate whether a match ticket is for a new
/// game session or an existing session.
/// </summary>
enum class preserve_session_mode
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    unknown,

    /// <summary>
    /// Always use an existing game session. This is for matching more players
    /// for a game session that is already created or in progress.
    /// </summary>
    always,

    /// <summary>
    /// Never use an existing game session. This is for matching players
    /// for a new game session.
    /// </summary>
    never
};

/// <summary>
/// Defines values used to indicate the status of the match request.
/// </summary>
enum class ticket_status
{
    /// <summary>
    /// The status of the match request has not been returned by the server yet
    /// or the server returned an unrecognized response.
    /// </summary>
    unknown,

    /// <summary>
    /// Matchmaking has not found a match and the search
    /// request has expired according to its give up duration.
    /// </summary>
    expired,

    /// <summary>
    /// Matchmaking has not found a match yet and it is
    /// still searching.
    /// </summary>
    searching,

    /// <summary>
    /// Matchmaking has found a match and the ticket contains a
    /// reference to the session that is to be created.
    /// </summary>
    found,

    /// <summary>
    /// Matchmaking has been canceled for this ticket.
    /// </summary>
    canceled
};

/// <summary>
/// Represents a server response to a create match ticket request.
/// </summary>
class create_match_ticket_response
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    inline create_match_ticket_response();

    /// <summary>
    /// Internal function
    /// </summary>
    inline create_match_ticket_response(
        XblCreateMatchTicketResponse response
        );

    /// <summary>
    /// Ticket ID of a match request.
    /// </summary>
    inline string_t match_ticket_id() const;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    inline std::chrono::seconds estimated_wait_time() const;

private:
    XblCreateMatchTicketResponse m_createMatchTicketResponse;
};

/// <summary>
/// Represents a server response to a request for match ticket details.
/// </summary>
class match_ticket_details_response
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    inline match_ticket_details_response();

    /// <summary>
    /// Internal function
    /// </summary>
    inline match_ticket_details_response(
        std::shared_ptr<char> buffer
        );

    /// <summary>
    /// Status of a match request.
    /// </summary>
    inline ticket_status match_status() const;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    inline std::chrono::seconds estimated_wait_time() const;

    /// <summary>
    /// An enum value to specify whether the match should preserve the session on which the match has been requested.
    /// </summary>
    inline preserve_session_mode preserve_session() const;

    /// <summary>
    /// The session on which the match was requested.
    /// </summary>
    inline xbox::services::multiplayer::multiplayer_session_reference ticket_session() const;

    /// <summary>
    /// The session on which a match request has been found.
    /// </summary>
    inline xbox::services::multiplayer::multiplayer_session_reference target_session() const;

    /// <summary>
    /// The attributes of a match request.
    /// </summary>
    inline web::json::value ticket_attributes() const;

private:
    static inline ticket_status convert_string_to_ticket_status(_In_ const string_t& value);

    static inline preserve_session_mode convert_string_to_preserve_session_mode(_In_ const string_t& value);

    std::shared_ptr<char> m_buffer;
    XblMatchTicketDetailsResponse m_matchTicketDetailsResponse;
};

/// <summary>
/// Represents a server response to a hopper statistics request.
/// </summary>
class hopper_statistics_response
{
public:

    /// <summary>
    /// Internal function
    /// </summary>
    inline hopper_statistics_response();

    /// <summary>
    /// Internal function
    /// </summary>
    inline hopper_statistics_response(
        std::shared_ptr<char> buffer
        );

    /// <summary>
    /// Name of the hopper in which a match was requested.
    /// </summary>
    inline string_t hopper_name() const;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    inline std::chrono::seconds estimated_wait_time() const;

    /// <summary>
    /// The number of players in the hopper waiting to be matched.
    /// </summary>
    inline uint32_t players_waiting_to_match() const;

private:
    std::shared_ptr<char> m_buffer;
    XblHopperStatisticsResponse m_hopperStatisticsResponse;
};

/// <summary>
/// Represents the Matchmaking Service.
/// </summary>
class matchmaking_service
{
public:
    /// <summary>
    /// Sends a matchmaking request to the server and returns the match ticket with a ticket id.
    /// </summary>
    /// <param name="ticketSessionReference">The multiplayer session to use for the match.</param>
    /// <param name="matchmakingServiceConfigurationId">The service configuration ID for the match.</param>
    /// <param name="hopperName">The name of the hopper.</param>
    /// <param name="ticketTimeout">The maximum time to wait for players to join the session.</param>
    /// <param name="preserveSession">Indicates if the session should be preserved.</param>
    /// <param name="ticketAttributesJson">The ticket attributes for the session. (Optional)</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, a new match ticket
    /// object is returned. The match ticket object contains server returned information such as ticket id and wait
    /// time, and also contains copies of the title specified data from the ticket data object.</returns>
    /// <remarks>Calls V103 POST /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}</remarks>
    inline pplx::task<xbox_live_result<create_match_ticket_response>> create_match_ticket(
        _In_ const xbox::services::multiplayer::multiplayer_session_reference& ticketSessionReference,
        _In_ const string_t& matchmakingServiceConfigurationId,
        _In_ const string_t& hopperName,
        _In_ const std::chrono::seconds& ticketTimeout,
        _In_ preserve_session_mode preserveSession,
        _In_ const web::json::value& ticketAttributesJson = web::json::value()
        );

    /// <summary>
    /// Deletes a the match ticket on the server.
    /// </summary>
    /// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
    /// <param name="hopperName">The name of the hopper where the match ticket is located.</param>
    /// <param name="ticketId">The id of the ticket to delete on the server.</param>
    /// <returns>The async object for notifying when the operation has been completed.</returns>
    /// <remarks>Calls V103 DELETE /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/tickets/{ticketId}</remarks>
    inline pplx::task<xbox_live_result<void>> delete_match_ticket(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& hopperName,
        _In_ const string_t& ticketId
        );

    /// <summary>
    /// Retrieves the properties of a match ticket from the server.
    /// </summary>
    /// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
    /// <param name="hopperName">The name of the hopper where the match ticket is located.</param>
    /// <param name="ticketId">The ticket id of the match ticket to retrieve.</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, the match
    /// ticket object with the data for the ticket, including ticket id and wait time information, is returned
    /// returned from the server.</returns>
    /// <remarks>Calls V103 GET /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/tickets/{ticketId}</remarks>
    inline pplx::task<xbox_live_result<match_ticket_details_response>> get_match_ticket_details(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& hopperName,
        _In_ const string_t& ticketId
        );

    /// <summary>
    /// Gets statistics about a hopper such as how many players are in it.
    /// </summary>
    /// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
    /// <param name="hopperName">The name of the hopper to query stats for.</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, an object
    /// containing statistics about the hopper is returned.</returns>
    /// <remarks>Calls V103 GET /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/stats</remarks>
    inline pplx::task<xbox_live_result<hopper_statistics_response>> get_hopper_statistics(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& hopperName
        );

    inline matchmaking_service(const matchmaking_service& other);
    inline matchmaking_service& operator=(matchmaking_service other);
    inline ~matchmaking_service();

private:
    inline matchmaking_service (_In_ XblContextHandle contextHandle);

    XblContextHandle m_xblContext;
    friend xbox_live_context;
};

}}}

#include "impl/matchmaking.hpp"