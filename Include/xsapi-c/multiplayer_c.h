// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
    #error C++11 required
#endif

#pragma once

#include "pal.h"

extern "C"
{

/// <summary>
/// Defines values that indicate the state of a tournament game.
/// DEPRECATED. This enumeration will be removed in a future release.
/// </summary>
/// <memof><see cref="XblTournamentGameResultWithRank"/></memof>
enum class XblTournamentGameResult : uint32_t
{
    /// <summary>
    /// No game.
    /// </summary>
    NoContest,

    /// <summary>
    /// Win.
    /// </summary>
    Win,

    /// <summary>
    /// Loss.
    /// </summary>
    Loss,

    /// <summary>
    /// Draw.
    /// </summary>
    Draw,

    /// <summary>
    /// Rank.
    /// </summary>
    Rank,

    /// <summary>
    /// Didn't show up.
    /// </summary>
    NoShow,
};

/// <summary>
/// Defines values that indicate the arbitration state of a tournament game.
/// DEPRECATED. This enumeration will be removed in a future release.
/// </summary>
/// <memof><see cref="XblMultiplayerArbitrationServer"/></memof>
enum class XblTournamentArbitrationState : uint32_t
{
    /// <summary>
    /// No arbitration state is set.
    /// </summary>
    None,

    /// <summary>
    /// Results were fully uploaded and complete.
    /// </summary>
    Completed,

    /// <summary>
    /// The match was canceled, such as in the case of forfeiting.
    /// </summary>
    Canceled,

    /// <summary>
    /// The match began, but no players or servers reported results before the arbitration deadline.
    /// </summary>
    NoResults,

    /// <summary>
    /// Some results were received, and results were compiled based on the incomplete data.
    /// </summary>
    PartialResults
};

/// <summary>
/// Defines values that indicate the source for a tournament game state.
/// DEPRECATED. This enumeration will be removed in a future release.
/// </summary>
/// <memof><see cref="XblMultiplayerArbitrationServer"/></memof>
/// <memof><see cref="XblMultiplayerTournamentsServer"/></memof>
enum class XblTournamentGameResultSource : uint32_t
{
    /// <summary>
    /// No game result source.
    /// </summary>
    None,

    /// <summary>
    /// Game result is determined by client arbitration.
    /// </summary>
    Arbitration,

    /// <summary>
    /// Game result is determined by game servers.
    /// </summary>
    Server,

    /// <summary>
    /// Game result is adjusted by tournament administrator.
    /// </summary>
    Adjusted,
};

/// <summary>
/// Defines values that indicate the status of a tournament game result.
/// DEPRECATED. This enumeration will be removed in a future release.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionMember"/></memof>
/// <seealso cref="XblMultiplayerSessionConstants"/>
enum class XblTournamentArbitrationStatus : uint32_t
{
    /// <summary>
    /// The system time is before the arbitration start time (`ArbitrationStartTime`), which is also the start time for the match.
    /// </summary>
    Waiting,

    /// <summary>
    /// The system time is after the arbitration start time (`ArbitrationStartTime`), and at least one player has become active.
    /// </summary>
    InProgress,

    /// <summary>
    /// The player reported results, so the player's role in the arbitration process is now done.  
    /// Occurs when arbitration succeeds, after the arbitration forfeit time (`ArbitrationStartTime` 
    /// plus the `ForfeitTimeout` delta) if no players have joined, or after arbitration time-out 
    /// (`ArbitrationStartTime` plus the `ArbitrationTimeout` delta).
    /// </summary>
    Complete,

    /// <summary>
    /// The player has become active at least once and is now participating in the match.
    /// </summary>
    Playing,

    /// <summary>
    /// The player was not able to upload results before arbitration time-out (`ArbitrationStartTime` plus the `ArbitrationTimeout` delta).
    /// </summary>
    Incomplete,

    /// <summary>
    /// The system time is after the arbitration start time (`ArbitrationStartTime`), but the player is not yet active.
    /// </summary>
    Joining
};

/// <summary>
/// Defines values that indicate the team session registration state for a tournament.
/// DEPRECATED. This enumeration will be removed in a future release.
/// </summary>
/// <memof><see cref="XblMultiplayerTournamentsServer"/></memof>
enum class XblTournamentRegistrationState : uint32_t
{
    /// <summary>
    /// The team registration state is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Registration was successfully received by the Tournament service and will be eventually processed.
    /// </summary>
    Pending,

    /// <summary>
    /// Registration for the team was withdrawn.
    /// </summary>
    Withdrawn,

    /// <summary>
    /// Registration could not be performed for the team.
    /// </summary>
    Rejected,

    /// <summary>
    /// Registration has been confirmed by the Tournament service.
    /// </summary>
    Registered,

    /// <summary>
    /// The team completed its participation in the tournament.
    /// </summary>
    Completed
};

/// <summary>
/// Defines values that indicate reasons why the team is under selected tournament registration state.
/// DEPRECATED. It will be removed in a future release
/// </summary>
/// <memof><see cref="XblMultiplayerTournamentsServer"/></memof>
enum class XblTournamentRegistrationReason : uint32_t
{
    /// <summary>
    /// The reason is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// The registration for this tournament is closed.
    /// </summary>
    RegistrationClosed,

    /// <summary>
    /// One of the team members is already registered for this tournament.
    /// </summary>
    MemberAlreadyRegistered,

    /// <summary>
    /// The tournament has reached its team registration limit.
    /// </summary>
    TournamentFull,

    /// <summary>
    /// The team has been eliminated from the tournament.
    /// </summary>
    TeamEliminated,

    /// <summary>
    /// The tournament is completed.
    /// </summary>
    TournamentCompleted
};

/// <summary>
/// Defines values that indicate the visibility or accessibility of a session.
/// </summary>
/// <memof><see cref="XblMultiplayerActivityDetails"/></memof>
/// <memof><see cref="XblMultiplayerSessionConstants"/></memof>
/// <memof><see cref="XblMultiplayerSessionInitArgs"/></memof>
/// <memof><see cref="XblMultiplayerSessionQuery"/></memof>
/// <memof><see cref="XblMultiplayerSessionQueryResult"/></memof>
/// <argof><see cref="XblMultiplayerSearchHandleGetVisibility"/></argof>
/// <argof><see cref="XblMultiplayerSessionConstantsSetVisibility"/></argof>
/// <remarks>
/// For more information,
/// see <see href="live-game-session-visibility-joinability.md">Game session visibility and joinability</see>
/// and the <see href="live-mpsd-details.md#visibility-and-joinability">Visibility and joinability</see> section 
/// of <see href="live-mpsd-details.md">Multiplayer Session advanced topics</see>.
/// </remarks> 
enum class XblMultiplayerSessionVisibility : uint32_t
{
    /// <summary>
    /// The status is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Ignore the session visibility filter.
    /// </summary>
    Any,

    /// <summary>
    /// The session is private and is not visible to players who aren't in the session.  
    /// Attempting to join causes the service to return HTTP_E_STATUS_FORBIDDEN (403).
    /// </summary>
    PrivateSession,

    /// <summary>
    /// The session is visible to players who aren't in the session, but the session is 
    /// read-only to them and they can't join.  
    /// Attempting to join causes the service to return HTTP_E_STATUS_BAD_REQUEST (400).
    /// </summary>
    Visible,

    /// <summary>
    /// The session is full and cannot be joined by anyone.  
    /// Attempting to join causes the service to return HTTP_E_STATUS_BAD_REQUEST (400).
    /// </summary>
    Full,

    /// <summary>
    /// The session is open and can be joined by anyone.
    /// </summary>
    Open
};

/// <summary>
/// Defines values that indicate the initialization stage of a session during managed initialization.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionInitializationInfo"/></memof>
/// <remarks>
/// For more information about managed initialization, see the "Managed initialization" section 
/// of <see href="live-matchmaking-target-session.md">Target session initialization and QoS</see>.
/// </remarks>
enum class XblMultiplayerInitializationStage : uint32_t
{
    /// <summary>
    /// Stage not known.
    /// </summary>
    Unknown,

    /// <summary>
    /// Stage not set.
    /// </summary>
    None,

    /// <summary> 
    /// Joining stage. Typically, matchmaking creates a session and adds users to the session.
    /// The client has until the joining timeout to join the session during this stage.
    /// </summary>
    Joining,

    /// <summary>
    /// Measuring stage. Quality of Service (QoS) measurement happens during this stage.
    /// If the title is manually managing QoS, the title handles this stage.
    /// Otherwise, the {% term xbox-live-party %} system handles this stage
    /// when calling `RegisterGameSession` or `RegisterMatchSession`.
    /// </summary>
    Measuring,

    /// <summary>
    /// Evaluating stage. If `externalEvaluation` is false, this stage is skipped.
    /// Otherwise, the title does its own evaluation.
    /// </summary>
    Evaluating,

    /// <summary>
    /// Failed stage. If the first initialization episode didn't succeed, the session can't be initialized.
    /// </summary>
    Failed
};

/// <summary>
/// Defines values that indicate the type of metric used to measure matchmaking Quality of Service (QoS) for a session.
/// </summary>
/// <memof><see cref="XblMultiplayerPeerToHostRequirements"/></memof>
/// <remarks>
/// For more information, see <see href="live-matchmaking-target-session.md">Target session initialization and QoS</see>.
/// </remarks>
enum class XblMultiplayerMetrics : uint32_t
{
    /// <summary>
    /// Unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Upstream (peer-to-host) bandwidth.
    /// </summary>
    BandwidthUp,

    /// <summary>
    /// Downstream (host-to-peer) bandwidth.
    /// </summary>
    BandwidthDown,

    /// <summary>
    /// Combined bandwidth.
    /// </summary>
    Bandwidth,

    /// <summary>
    /// Upstream (peer-to-host) latency.
    /// </summary>
    Latency
};

/// <summary>
/// Defines values that indicate the current network address translation (NAT) settings for
/// a console connecting to {% term xbox-live %}.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionMember"/></memof>
enum class XblNetworkAddressTranslationSetting : uint32_t
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    Unknown,

    /// <summary>
    /// Can connect with any consoles regardless of their NAT settings.
    /// </summary>
    Open,

    /// <summary>
    /// Can connect only with consoles that use Moderate or Open settings.
    /// </summary>
    Moderate,

    /// <summary>
    /// Can connect only with consoles that use Open NAT settings.
    /// </summary>
    Strict
};

/// <summary>
/// Defines values that indicate why Quality of Service (QoS) measurement failed during session initialization.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionMember"/></memof>
/// <argof><see cref="XblMultiplayerEventArgsFindMatchCompleted"/></argof>
/// <remarks>
/// For more information, see <see href="live-matchmaking-target-session.md">Target session initialization and QoS</see>.
/// </remarks>
enum class XblMultiplayerMeasurementFailure : uint32_t
{
    /// <summary>
    /// Unknown measurement failure.
    /// </summary>
    Unknown,

    /// <summary>
    /// No measurement failure.
    /// </summary>
    None,

    /// <summary>
    /// Measurement timed out.
    /// </summary>
    Timeout,

    /// <summary>
    /// Measurement of latency failed.
    /// </summary>
    Latency,

    /// <summary>
    /// Measurement of upstream (peer-to-host) bandwidth failed.
    /// </summary>
    BandwidthUp,

    /// <summary>
    /// Measurement of downstream (host-to-peer) bandwidth failed.
    /// </summary>
    BandwidthDown,

    /// <summary>
    /// Measurement failed for this player failed because measurement failed for another player in the group.
    /// </summary>
    Group,

    /// <summary>
    /// Measurement failed due to a network error; for example, the player was unreachable.
    /// </summary>
    Network,

    /// <summary>
    /// Measurement failed because the initialization episode failed.
    /// This likely happened because not enough users were in the session.
    /// </summary>
    Episode
};

/// <summary>
/// Defines values that indicate the current status of a session.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionQueryResult"/></memof>
/// <remarks>
/// For more information, see
/// the <see href="live-mpsd-details.md#session-user-states">Session user states</see> section 
/// of <see href="live-mpsd-details.md">Multiplayer Session advanced topics</see>.
/// </remarks>
enum class XblMultiplayerSessionStatus : uint32_t
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    Unknown,

    /// <summary>
    /// At least one player is active in the session.
    /// </summary>
    Active,

    /// <summary>
    /// No players are active in the session or all players left the session.
    /// </summary>
    Inactive,

    /// <summary>
    /// One or more players have not accepted the session invite.
    /// </summary>
    Reserved
};

/// <summary>
/// Defines values that indicate restrictions on the users who can join a session.
/// </summary>
enum class XblMultiplayerSessionRestriction : uint32_t
{
    /// <summary>
    /// Unknown restriction type.
    /// </summary>
    Unknown,

    /// <summary>
    /// No restrictions.
    /// </summary>
    None,

    /// <summary>
    /// Only players whose token `DeviceId` values match the `DeviceId` of a player 
    /// who is already in the session and active.
    /// </summary>
    Local,

    /// <summary>
    /// Only local players (as defined for `Local`) and players who are followed by an 
    /// existing (not reserved) member of the session can join without a reservation.
    /// </summary>
    Followed
};

/// <summary>
/// Defines values that indicate the status of a matchmaking request for a session.
/// </summary>
/// <memof><see cref="XblMultiplayerMatchmakingServer"/></memof>
enum class XblMatchmakingStatus : uint32_t
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    Unknown,

    /// <summary>
    /// The matchmaking search is not specified.  
    /// This status is optional and requires the `clientMatchmaking` capability.
    /// </summary>
    None,

    /// <summary>
    /// The matchmaking search is still searching.
    /// </summary>
    Searching,

    /// <summary>
    /// The matchmaking search has expired.
    /// </summary>
    Expired,

    /// <summary>
    /// The matchmaking search found a session.
    /// </summary>
    Found,

    /// <summary>
    /// The matchmaking search was canceled.
    /// </summary>
    Canceled
};

/// <summary>
/// Defines values that indicate the status of a member of a session.
/// </summary>
/// <memof><see cref="XblMultiplayerManagerMember"/></memof>
/// <memof><see cref="XblMultiplayerSessionMember"/></memof>
/// <argof><see cref="XblMultiplayerSessionCurrentUserSetStatus"/></argof>
enum class XblMultiplayerSessionMemberStatus : uint32_t
{
    /// <summary>
    /// The member is reserved for a specific Xbox user ID.  
    /// The specified member must join the session to fill the reservation.  
    /// If a reserved member doesn't join before the end of `JoinTimeout` (in 
    /// the <see cref="XblMultiplayerMemberInitialization"/> structure), the member is removed from the session.
    /// </summary>
    Reserved,

    /// <summary>
    /// The member is inactive in the current title.  
    /// The member may be active in another title, as specified by `ActiveTitleId` in 
    /// the <see cref="XblMultiplayerSessionMember"/> structure.  
    /// If an inactive member doesn't mark themselves as active before the end of `MemberInactiveTimeout` 
    /// (in the <see cref="XblMultiplayerSessionConstants"/> structure), the member is removed from the session.
    /// </summary>
    Inactive,

    /// <summary>
    /// When the shell launches the title to start a multiplayer game, the member is marked as ready.  
    /// If a ready member doesn't mark themselves as active before the end of `MemberReadyTimeout` (in 
    /// the <see cref="XblMultiplayerSessionConstants"/> structure), the member is marked as inactive.
    /// </summary>
    Ready,

    /// <summary>
    /// The member is active in the current title.
    /// </summary>
    Active
};

/// <summary>
/// Defines values that indicate the mode used when creating or writing to a multiplayer session.
/// </summary>
/// <argof><see cref="XblMultiplayerWriteSessionAsync"/></argof>
/// <argof><see cref="XblMultiplayerWriteSessionByHandleAsync"/></argof>
enum class XblMultiplayerSessionWriteMode : uint32_t
{
    /// <summary>
    /// Create a multiplayer session.  
    /// Fails if the session already exists.
    /// </summary>
    CreateNew,

    /// <summary>
    /// Either update or create a session.  
    /// Doesn't care whether the session exists.
    /// </summary>
    UpdateOrCreateNew,

    /// <summary>
    /// Updates an existing multiplayer session.  
    /// Fails if the session doesn't exist.
    /// </summary>
    UpdateExisting,

    /// <summary>
    /// Updates an existing multiplayer session.  
    /// Fails with HTTP_E_STATUS_PRECOND_FAILED (HTTP status 412) if the ETag on the local session doesn't match the ETag on the server.  
    /// Fails if the session does not exist.
    /// </summary>
    SynchronizedUpdate,
};

/// <summary>
/// Defines values that indicate the write status of a multiplayer session.
/// </summary>
enum class XblWriteSessionStatus : uint32_t
{
    /// <summary>
    /// Unknown write result.
    /// </summary>
    Unknown,

    /// <summary>
    /// User does not have permission to write to the session (HTTP status 403).
    /// </summary>
    AccessDenied,

    /// <summary>
    /// The write operation created the session (HTTP status 201).
    /// </summary>
    Created,

    /// <summary>
    /// A conflict occurred during the write operation (HTTP status 409).
    /// </summary>
    Conflict,

    /// <summary>
    /// The session was not found (HTTP status 404).
    /// </summary>
    HandleNotFound,

    /// <summary>
    /// The session was updated by another user (HTTP status 412).
    /// </summary>
    OutOfSync,

    /// <summary>
    /// The session was deleted successfully (HTTP status 204).
    /// </summary>
    SessionDeleted,

    /// <summary>
    /// The session was updated successfully (HTTP status 200).
    /// </summary>
    Updated
};

/// <summary>
/// Defines values that indicate change types for a multiplayer session.
/// </summary>
/// <argof><see cref="XblMultiplayerSessionSetSessionChangeSubscription"/></argof>
enum class XblMultiplayerSessionChangeTypes : uint32_t
{
    /// <summary>
    /// None.
    /// </summary>
    None = 0x0000,

    /// <summary>
    /// Changes to anything in the session.
    /// </summary>
    Everything = 0x0001,

    /// <summary>
    /// Changes to the host device token.
    /// </summary>
    HostDeviceTokenChange = 0x0002,

    /// <summary>
    /// Changes to the stage of initialization.
    /// </summary>
    InitializationStateChange = 0x0004,

    /// <summary>
    /// Changes to the matchmaking status, such as match found or match expired.
    /// </summary>
    MatchmakingStatusChange = 0x0008,

    /// <summary>
    /// A member joined the session.
    /// </summary>
    MemberListChange = 0x0010,

    /// <summary>
    /// A member left the session.
    /// </summary>
    MemberStatusChange = 0x0020,

    /// <summary>
    /// Changes to the joinability (<see cref="XblMultiplayerJoinability"/>) of the session.
    /// </summary>
    SessionJoinabilityChange = 0x0040,

    /// <summary>
    /// Changes in the custom properties of the session.
    /// </summary>
    CustomPropertyChange = 0x0080,

    /// <summary>
    /// Changes in the custom properties of any of the members.
    /// </summary>
    MemberCustomPropertyChange = 0x0100,

    /// <summary>
    /// Changes in tournament server properties, such as next game, last game, or registration.
    /// DEPRECATED. This value will be removed in a future release.
    /// </summary>
    TournamentPropertyChange = 0x0200,

    /// <summary>
    /// Changes in arbitration server properties, such as game results.
    /// DEPRECATED. This value will be removed in a future release.
    /// </summary>
    ArbitrationPropertyChange = 0x0400
};

DEFINE_ENUM_FLAG_OPERATORS(XblMultiplayerSessionChangeTypes);

/// <summary>
/// Defines values that indicate which multiplayer role settings are mutable.
/// </summary>
/// <memof><see cref="XblMultiplayerRoleType"/></memof>
/// <remarks>
/// Only the session owner can modify role settings and only those that are set 
/// in `XblMultiplayerRoleType::MutableRoleSettings`.  
/// You can set `XblMutableRoleSettings` in the session template.
/// </remarks>
enum class XblMutableRoleSettings : uint32_t
{
    /// <summary>
    /// None of the role settings are mutable.
    /// </summary>
    None = 0x0,

    /// <summary>
    /// Allows you to set the maximum number of players that can fill the role.
    /// </summary>
    Max = 0x1,

    /// <summary>
    /// Allows you to set the target number of players that should fill the role.
    /// </summary>
    Target = 0x2
};

DEFINE_ENUM_FLAG_OPERATORS(XblMutableRoleSettings);

#define XBL_TOURNAMENT_REFERENCE_DEFINITION_NAME_MAX_LENGTH     100
#define XBL_TOURNAMENT_REFERENCE_ORGANIZER_LENGTH               100

/// <summary>
/// Represents a reference to a tournament reference.
/// DEPRECATED. This structure will be removed in a future release.
/// </summary>
/// <memof><see cref="XblMultiplayerTournamentsServer"/></memof>
typedef struct XBL_DEPRECATED XblTournamentReference 
{
    /// <summary>
    /// The definition name of the tournament.
    /// </summary>
    _Null_terminated_ char DefinitionName[XBL_TOURNAMENT_REFERENCE_DEFINITION_NAME_MAX_LENGTH];

    /// <summary>
    /// The tournament ID specific to the tournament.
    /// </summary>
    _Null_terminated_ char TournamentId[XBL_GUID_LENGTH];

    /// <summary>
    /// The name of the tournament organizer.
    /// </summary>
    _Null_terminated_ char Organizer[XBL_TOURNAMENT_REFERENCE_ORGANIZER_LENGTH];

    /// <summary>
    /// The service configuration ID specific to the tournament.
    /// </summary>
    _Null_terminated_ char Scid[XBL_SCID_LENGTH];
} XblTournamentReference;


/// <summary>
/// Represents the result of a multiplayer game.
/// DEPRECATED. This structure will be removed in a future release.
/// </summary>
typedef struct XBL_DEPRECATED XblTournamentGameResultWithRank
{
    /// <summary>
    /// The result for the team.
    /// </summary>
    XblTournamentGameResult Result;

    /// <summary>
    /// The ranking of the result. Applies only when `Result` is `XblTournamentGameResult::Rank`.
    /// </summary>
    uint64_t Ranking;
} XblTournamentGameResultWithRank;

/// <summary>
/// Represents a team result for a multiplayer game.
/// DEPRECATED. This structure will be removed in a future release.
/// </summary>
typedef struct XBL_DEPRECATED XblTournamentTeamResult
{
    /// <summary>
    /// Name of the team.
    /// </summary>
    const char* Team;

    XBL_WARNING_PUSH
    XBL_WARNING_DISABLE_DEPRECATED
    /// <summary>
    /// The game result.
    /// </summary>
    XblTournamentGameResultWithRank GameResult;
    XBL_WARNING_POP

} XblTournamentTeamResult;

/// <summary>
/// Represents requirements for each connection between a host candidate and session members.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionConstants"/></memof>
/// <argof><see cref="XblMultiplayerSessionConstantsSetPeerToHostRequirements"/></argof>
/// <remarks>
/// For more information, see <see href="live-matchmaking-target-session.md">Target session initialization and QoS</see>.
/// </remarks>
typedef struct XblMultiplayerPeerToHostRequirements
{
    /// <summary>
    /// The maximum latency, in milliseconds, of the upstream (peer-to-host) connection.
    /// </summary>
    uint64_t LatencyMaximum;

    /// <summary>
    /// The minimum bandwidth, in kilobits per second, of the downstream (host-to-peer) connection.
    /// </summary>
    uint64_t BandwidthDownMinimumInKbps;

    /// <summary>
    /// The minimum bandwidth, in kilobits per second, of the upstream (peer-to-host) connection.
    /// </summary>
    uint64_t BandwidthUpMinimumInKbps;

    /// <summary>
    /// The metric used to select the host.
    /// </summary>
    XblMultiplayerMetrics HostSelectionMetric;
} XblMultiplayerPeerToHostRequirements;

/// <summary>
/// Represents requirements for a connection between session members.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionConstants"/></memof>
/// <argof><see cref="XblMultiplayerSessionConstantsSetPeerToPeerRequirements"/></argof>
typedef struct XblMultiplayerPeerToPeerRequirements
{
    /// <summary>
    /// The maximum latency, in milliseconds, for the peer-to-peer connection.
    /// </summary>
    uint64_t LatencyMaximum;

    /// <summary>
    /// The minimum bandwidth, in kilobits per second, for the peer-to-peer connection.
    /// </summary>
    uint64_t BandwidthMinimumInKbps;
} XblMultiplayerPeerToPeerRequirements;

/// <summary>
/// Represents requirements for a new Multiplayer service session.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionConstants"/></memof>
/// <argof><see cref="XblMultiplayerSessionConstantsSetMemberInitialization"/></argof>
typedef struct XblMultiplayerMemberInitialization
{
    /// <summary>
    /// Maximum time, in milliseconds, for the joining stage of the Quality of Service (QoS) process.
    /// </summary>
    uint64_t JoinTimeout;

    /// <summary>
    /// Maximum time, in milliseconds, for the measurement stage of the QoS process.
    /// </summary>
    uint64_t MeasurementTimeout;

    /// <summary>
    /// Maximum time, in milliseconds, for the evaluation stage of the QoS process.
    /// </summary>
    uint64_t EvaluationTimeout;

    /// <summary>
    /// When set to true, indicates that the title performs the evaluation stage.
    /// </summary>
    bool ExternalEvaluation;

    /// <summary>
    /// Minimum number of members for the session. Defaults to 2. Must be between 1 and `maxMemberCount`.  
    /// Applies only to the joining stage.
    /// </summary>
    uint32_t MembersNeededToStart;
} XblMultiplayerMemberInitialization;

/// <summary>
/// Represents the capabilities of a Multiplayer service session.
/// </summary>
/// <memof><see cref="XblMultiplayerSessionConstants"/></memof>
/// <argof><see cref="XblMultiplayerSessionConstantsSetCapabilities"/></argof>
/// <remarks>
/// Session capabilities are optional Boolean values that are set in the session template.  
/// If no capabilities are needed, an empty `SessionCapabilities` object should be in the template 
/// to prevent capabilities from being specified at session creation, unless the title 
/// requires dynamic session capabilities.<br/><br/>
/// For more information, see the <see href="live-mpsd-details.md#session-capabilities">Session capabilities</see> 
/// section of <see href="live-mpsd-details.md">Multiplayer Session advanced topics</see>.
/// </remarks>
typedef struct XblMultiplayerSessionCapabilities
{
    /// <summary>
    /// Indicates whether a session can enable metrics and session members can set `secureDeviceAddress`.
    /// If false, the session can't enable any metrics, and session members can't set `secureDeviceAddress`.
    /// </summary>
    bool Connectivity;

    /// <summary>
    /// If true, team capability is set on the session for a tournament.
    /// DEPRECATED. This member will be removed in a future release.
    /// </summary>
    XBL_DEPRECATED bool Team;

    /// <summary>
    /// If true, arbitration capability is set on the session for a tournament.
    /// DEPRECATED. This member will be removed in a future release.
    /// </summary>
    XBL_DEPRECATED bool Arbitration;

    /// <summary>
    /// If false (the default value), active users are required to remain online playing the title.
    /// If they don't, they are demoted to inactive status.  
    /// Set this flag to true to enable session members to stay active indefinitely.
    /// </summary>
    bool SuppressPresenceActivityCheck;

    /// <summary>
    /// Indicates whether the session represents actual gameplay rather than time in setup or 
    /// a menu, such a lobby or during matchmaking.  
    /// If true, the session is in gameplay mode.
    /// </summary>
    bool Gameplay;

    /// <summary>
    /// If true, the session can host 101 to 1000 users, which affects other session features.
    /// If false, the session can host 1 to 100 users. For more information, see 
    /// the <see href="live-mpsd-details.md#session-size">Session size</see> section 
    /// of <see href="live-mpsd-details.md">Multiplayer Session advanced topics</see>.
    /// </summary>
    bool Large;

    /// <summary>
    /// If true, a connection is required for a member to be marked as active. To enable session notifications 
    /// and detect disconnections, this member must be set to true. For more information, see 
    /// the <see href="live-mpsd-how-tos.md#sfmscn">Subscribe for MPSD session change notifications</see> section 
    /// of <see href="live-mpsd-how-tos.md">Multiplayer tasks</see>.
    /// </summary>
    bool ConnectionRequiredForActiveMembers;

    /// <summary>
    /// If true, the session supports calls from platforms without strong title identity.  
    /// This capability can't be set on large sessions.
    /// </summary>
    bool UserAuthorizationStyle;

    /// <summary>
    /// If true, the session supports crossplay between {% term platform_windows %} PC and Xbox.
    /// </summary>
    bool Crossplay;

    /// <summary>
    /// If true, the session can be linked to a search handle for searching.
    /// </summary>
    bool Searchable;

    /// <summary>
    /// If true, the session has owners. For a session to be searchable when `UserAuthorizationStyle` is 
    /// true, the session must have owners.
    /// </summary>
    bool HasOwners;
} XblMultiplayerSessionCapabilities;

/// <summary>
/// Represents constants for a multiplayer session.
/// </summary>
/// <remarks>
/// Session constants are set by the creator or by the session template only when a session is created.
/// </remarks>
typedef struct XblMultiplayerSessionConstants
{
    /// <summary>
    /// The maximum number of members in the session.
    /// </summary>
    uint32_t MaxMembersInSession;

    /// <summary>
    /// The visibility of the session.
    /// </summary>
    XblMultiplayerSessionVisibility Visibility;

    /// <summary>
    /// A collection of Xbox user IDs indicating who initiated the session. (Optional)
    /// </summary>
    uint64_t* InitiatorXuids;

    /// <summary>
    /// The number of entries in the `InitiatorXuids` array.
    /// </summary>
    size_t InitiatorXuidsCount;

    /// <summary>
    /// Any custom constants for the session, specified in a JSON string.  
    /// These constants can't be changed after the session is created. (Optional)
    /// </summary>
    const char* CustomJson;

    /// <summary>
    /// The Cloud Compute package constants for the session, specified in a JSON string.  
    /// These constants can't be changed after the session is created. (Optional)
    /// </summary>
    const char* SessionCloudComputePackageConstantsJson;

    /// <summary>
    /// Maximum time, in milliseconds, for a member with a reservation to join the session. 
    /// If the member doesn't join within this time, the reservation is removed.
    /// </summary>
    uint64_t MemberReservedTimeout;

    /// <summary>
    /// Maximum time, in milliseconds, for an inactive member to become active. 
    /// If an inactive member doesn't become active within this time, the member is removed from the session.
    /// </summary>
    uint64_t MemberInactiveTimeout;

    /// <summary>
    /// Maximum time, in milliseconds, for a member who is marked as ready to become active. 
    /// When the shell launches the title to start a multiplayer game, the member is marked as ready.
    /// If a member who is marked as ready doesn't become active within this time, the member becomes inactive.  
    /// </summary>
    uint64_t MemberReadyTimeout;

    /// <summary>
    /// Maximum time, in milliseconds, that the session can remain empty. 
    /// If no members join the session within this time, the session is deleted.
    /// </summary>
    uint64_t SessionEmptyTimeout;

    /// <summary>
    /// Delta, in milliseconds, from start time that represents the time at which results are finalized.  
    /// If no one (client or server) has reported at this time, we declare the match results incomplete.
    /// DEPRECATED. This member will be removed in a future release.
    /// </summary>
    XBL_DEPRECATED uint64_t ArbitrationTimeout;

    /// <summary>
    /// Delta, in milliseconds, from start time that represents the time at which, if the session has 
    /// no active users, the match is canceled.
    /// DEPRECATED. This member will be removed in a future release.
    /// </summary>
    XBL_DEPRECATED uint64_t ForfeitTimeout;

    /// <summary>
    /// If true, indicates that the title wants latency measured to help determine connectivity.  
    /// Requires `capabilities.connectivity` to be true.
    /// </summary>
    bool EnableMetricsLatency;

    /// <summary>
    /// If true, indicates that the title wants downstream (host-to-peer) bandwidth measured to help 
    /// determine connectivity. Requires `capabilities.connectivity` to be true.
    /// </summary>
    bool EnableMetricsBandwidthDown;

    /// <summary>
    /// If true, indicates that the title wants upstream (peer-to-host) bandwidth measured to help 
    /// determine connectivity. Requires `capabilities.connectivity` to be true.
    /// </summary>
    bool EnableMetricsBandwidthUp;

    /// <summary>
    /// If true, indicates that the title wants a custom measurement to help determine connectivity.  
    /// Requires `capabilities.connectivity` to be true.
    /// </summary>
    bool EnableMetricsCustom;

    /// <summary>
    /// If set, the session expects the client system or title to perform initialization after session creation.  
    /// Timeouts and initialization stages are automatically tracked by the session, including 
    /// initial Quality of Service (QoS) measurements if any metrics are set.
    /// </summary>
    XblMultiplayerMemberInitialization* MemberInitialization;

    /// <summary>
    /// QoS requirements for a connection between session members.
    /// </summary>
    XblMultiplayerPeerToPeerRequirements PeerToPeerRequirements;

    /// <summary>
    /// QoS requirements for a connection between a host candidate and session members.
    /// </summary>
    XblMultiplayerPeerToHostRequirements PeerToHostRequirements;

    /// <summary>
    /// The set of potential server connection strings that should be evaluated.
    /// </summary>
    const char* MeasurementServerAddressesJson;

    /// <summary>
    /// Indicates whether the matchmaking status fields can be written to.
    /// </summary>
    bool ClientMatchmakingCapable;

    /// <summary>
    /// The capabilities of the session.
    /// </summary>
    XblMultiplayerSessionCapabilities SessionCapabilities;
} XblMultiplayerSessionConstants;

#define XBL_MULTIPLAYER_DEVICE_TOKEN_MAX_LENGTH             40 
#define XBL_MULTIPLAYER_SESSION_TEMPLATE_NAME_MAX_LENGTH    100
#define XBL_MULTIPLAYER_SESSION_NAME_MAX_LENGTH             XBL_MULTIPLAYER_SESSION_TEMPLATE_NAME_MAX_LENGTH

/// <summary>
/// Represents a handle ID of a multiplayer session.
/// </summary>
/// <argof><see cref="XblMultiplayerSetTransferHandleResult"/></argof>
/// <remarks>
/// Multiplayer Session Directory (MPSD) can create various handles that refer to a session.  
/// They are immutable and can only be created, read, and deleted.
/// Note that this handle ID references to a service side object.
/// </remarks>
typedef struct XblMultiplayerSessionHandleId
{
    /// <summary>
    /// The ID of the handle that MSPD created.
    /// </summary>
    _Null_terminated_ char value[XBL_GUID_LENGTH];
} XblMultiplayerSessionHandleId;

/// <summary>
/// Represents a reference to a multiplayer session.
/// </summary>
typedef struct XblMultiplayerSessionReference
{
    /// <summary>
    /// The service configuration ID (SCID) specific to the title.
    /// </summary>
    _Null_terminated_ char Scid[XBL_SCID_LENGTH];

    /// <summary>
    /// The name of the template for the session.
    /// </summary>
    _Null_terminated_ char SessionTemplateName[XBL_MULTIPLAYER_SESSION_TEMPLATE_NAME_MAX_LENGTH];

    /// <summary>
    /// The name of the session.
    /// </summary>
    _Null_terminated_ char SessionName[XBL_MULTIPLAYER_SESSION_NAME_MAX_LENGTH];
} XblMultiplayerSessionReference;

#define XBL_MULTIPLAYER_SESSION_REFERENCE_URI_MAX_LENGTH  (44 + XBL_SCID_LENGTH + XBL_MULTIPLAYER_SESSION_TEMPLATE_NAME_MAX_LENGTH + XBL_MULTIPLAYER_SESSION_NAME_MAX_LENGTH)

/// <summary>
/// Represents a URI path representation of a session reference.  
/// </summary>
/// <remarks>
/// The format of the URI path 
/// is `/serviceconfigs/{scid}/sessiontemplates/{session-template-name}/sessions/{session-name}`, where 
/// `{scid}` is the service configuration ID specific to the title.
/// </remarks>
/// <argof><see cref="XblMultiplayerSessionReferenceToUriPath"/></argof>
typedef struct XblMultiplayerSessionReferenceUri
{
    /// <summary>
    /// The URI path.
    /// </summary>
    _Null_terminated_ char value[XBL_MULTIPLAYER_SESSION_REFERENCE_URI_MAX_LENGTH];
} XblMultiplayerSessionReferenceUri;

/// <summary>
/// Creates an <see cref="XblMultiplayerSessionReference"/> object from a service configuration ID (SCID), session 
/// template name, and session name.
/// </summary>
/// <param name="scid">The SCID that the session is a part of. The SCID is case-sensitive, so paste it directly 
/// from Partner Center.</param>
/// <param name="sessionTemplateName">The name of the session template.</param>
/// <param name="sessionName">The name of the session.</param>
/// <returns>A reference to the multiplayer session.</returns>
STDAPI_(XblMultiplayerSessionReference) XblMultiplayerSessionReferenceCreate(
    _In_z_ const char* scid,
    _In_z_ const char* sessionTemplateName,
    _In_z_ const char* sessionName
) XBL_NOEXCEPT;

/// <summary>
/// Returns the session reference parsed from a URI.
/// </summary>
/// <param name="path">The URI path.</param>
/// <param name="sessionReference">Passes back the session reference.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionReferenceParseFromUriPath(
    _In_ const char* path,
    _Out_ XblMultiplayerSessionReference* sessionReference
) XBL_NOEXCEPT;

/// <summary>
/// Returns the URI path representation of a session reference.
/// </summary>
/// <param name="sessionReference">A session reference.</param>
/// <param name="sessionReferenceUri">Passes back the URI representation of the session reference.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionReferenceToUriPath(
    _In_ const XblMultiplayerSessionReference* sessionReference,
    _Out_ XblMultiplayerSessionReferenceUri* sessionReferenceUri
) XBL_NOEXCEPT;

/// <summary>
/// Verifies whether an <see cref="XblMultiplayerSessionReference"/> object is well formed.
/// </summary>
/// <param name="sessionReference">The session reference.</param>
/// <returns>Returns true if session is well formed, false if session is not well formed.</returns>
/// <remarks>
/// An <see cref="XblMultiplayerSessionReference"/> object is considered to be well formed if none 
/// of the fields are empty strings.
/// </remarks>
STDAPI_(bool) XblMultiplayerSessionReferenceIsValid(
    _In_ const XblMultiplayerSessionReference* sessionReference
) XBL_NOEXCEPT;

/// <summary>
/// Represents the matchmaking server that supports a multiplayer session.
/// </summary>
typedef struct XblMultiplayerMatchmakingServer
{
    /// <summary>
    /// The status of the matchmaking server.
    /// </summary>
    XblMatchmakingStatus Status;

    /// <summary>
    /// The status details, if any, of the matchmaking server.
    /// </summary>
    const char* StatusDetails;

    /// <summary>
    /// The typical wait time, in seconds.
    /// </summary>
    uint32_t TypicalWaitInSeconds;

    /// <summary>
    /// A reference to the target session.
    /// </summary>
    XblMultiplayerSessionReference TargetSessionRef;
} XblMultiplayerMatchmakingServer;

/// <summary>
/// Represents the arbitration server that supports a multiplayer session.
/// DEPRECATED. This structure will be removed in a future release.
/// </summary>
typedef struct XBL_DEPRECATED XblMultiplayerArbitrationServer
{
    /// <summary>
    /// The start time of the match. This time is also when arbitration starts.
    /// </summary>
    time_t ArbitrationStartTime;

    /// <summary>
    /// The state of the result.
    /// </summary>
    XblTournamentArbitrationState ResultState;

    /// <summary>
    /// The source of the result.
    /// </summary>
    XblTournamentGameResultSource ResultSource;

    /// <summary>
    /// A value from 0 through 100 that indicates the confidence level of the result.
    /// </summary>
    uint32_t ResultConfidenceLevel;

    XBL_WARNING_PUSH
    XBL_WARNING_DISABLE_DEPRECATED
    /// <summary>
    /// The results of the game.
    /// </summary>
    XblTournamentTeamResult* Results;
    XBL_WARNING_POP

    /// <summary>
    /// The number of results.
    /// </summary>
    size_t ResultsCount;
} XblMultiplayerArbitrationServer;

/// <summary>
/// Represents a team in a tournament.
/// DEPRECATED. This structure will be removed in a future release.
/// </summary>
typedef struct XBL_DEPRECATED XblMultiplayerTournamentTeam
{
    /// <summary>
    /// ID of the team.
    /// </summary>
    const char* TeamId;

    /// <summary>
    /// Session reference of the session the team is in.
    /// </summary>
    XblMultiplayerSessionReference TeamSessionReference;
} XblMultiplayerTournamentTeam;

/// <summary>
/// Represents the tournament server that supports a multiplayer session.
/// DEPRECATED. This structure will be removed in a future release.
/// </summary>
typedef struct XBL_DEPRECATED XblMultiplayerTournamentsServer 
{
    XBL_WARNING_PUSH
    XBL_WARNING_DISABLE_DEPRECATED
    /// <summary>
    /// The tournament reference.
    /// </summary>
    XblTournamentReference TournamentReference;
    XBL_WARNING_POP

    XBL_WARNING_PUSH
    XBL_WARNING_DISABLE_DEPRECATED
    /// <summary>
    /// The teams in the tournament.
    /// </summary>
    XblMultiplayerTournamentTeam* Teams;
    XBL_WARNING_POP

    /// <summary>
    /// The number of teams in the tournament.
    /// </summary>
    size_t TeamsCount;

    /// <summary>
    /// The registration state of the team.
    /// </summary>
    XblTournamentRegistrationState RegistrationState;

    /// <summary>
    /// The reason for the registration state.
    /// </summary>
    XblTournamentRegistrationReason RegistrationReason;

    /// <summary>
    /// The start time of the next game in the tournament.
    /// </summary>
    time_t NextGameStartTime;

    /// <summary>
    /// The session reference of the next game in the tournament.
    /// </summary>
    XblMultiplayerSessionReference NextGameSessionReference;

    /// <summary>
    /// The end time of the last game in the tournament.
    /// </summary>
    time_t LastGameEndTime;

    XBL_WARNING_PUSH
    XBL_WARNING_DISABLE_DEPRECATED
    /// <summary>
    /// The result of the last game in the tournament.
    /// </summary>
    XblTournamentTeamResult LastTeamResult;
    XBL_WARNING_POP

    /// <summary>
    /// The source of the result of the last game in the tournament.
    /// </summary>
    XblTournamentGameResultSource LastGameResultSource;
} XblMultiplayerTournamentsServer;

/// <summary>
/// Represents a category of roles for a multiplayer session.
/// </summary>
/// <memof><see cref="XblMultiplayerRole"/></memof>
/// <argof><see cref="XblMultiplayerSessionRoleTypes"/></argof>
/// <remarks>
/// For more information, see <see href="live-multiplayer-roles.md">Multiplayer roles</see>.
/// </remarks>
typedef struct XblMultiplayerRoleType
{
    /// <summary>
    /// Name of the role type.
    /// </summary>
    const char* Name;

    /// <summary>
    /// If true, only the owner of the session can assign this role to members.
    /// </summary>
    bool OwnerManaged;

    /// <summary>
    /// The settings (for roles in this role type) that can be modified throughout the life of the session.  
    /// Exclude role settings to lock them.
    /// </summary>
    XblMutableRoleSettings MutableRoleSettings;

    /// <summary>
    /// A collection of roles for this role type.
    /// </summary>
    struct XblMultiplayerRole* Roles;

    /// <summary>
    /// The number of roles in the `Roles` array.
    /// </summary>
    size_t RoleCount;
} XblMultiplayerRoleType;

/// <summary>
/// Represents role info for a multiplayer role.
/// </summary>
typedef struct XblMultiplayerRole
{
    /// <summary>
    /// The role type that this role belongs too.
    /// </summary>
    XblMultiplayerRoleType* RoleType;

    /// <summary>
    /// Name of the role.  
    /// Unique with a role type.
    /// </summary>
    const char* Name;

    /// <summary>
    /// Member xbox_user_ids currently assigned for this role.
    /// </summary>
    uint64_t* MemberXuids;

    /// <summary>
    /// Number of slots occupied for this role.
    /// </summary>
    uint32_t MemberCount;

    /// <summary>
    /// Number of target slots assigned for this role.
    /// </summary>
    uint32_t TargetCount;

    /// <summary>
    /// Maximum number of slots available for this role.
    /// </summary>
    uint32_t MaxMemberCount;
} XblMultiplayerRole;

/// <summary>
/// Represents a session member's role in the session.
/// </summary>
typedef struct XblMultiplayerSessionMemberRole
{
    /// <summary>
    /// Role type this role belongs to.
    /// </summary>
    const char* roleTypeName;

    /// <summary>
    /// Name of the role.
    /// </summary>
    const char* roleName;
} XblMultiplayerSessionMemberRole;

/// <summary>
/// Represents a users current multiplayer activity, along with some details about the corresponding session.
/// </summary>
typedef struct XblMultiplayerActivityDetails
{
    /// <summary>
    /// Session reference containing identifying information for the session.
    /// </summary>
    XblMultiplayerSessionReference SessionReference;

    /// <summary>
    /// HandleId corresponding to this activity.
    /// </summary>
    char HandleId[XBL_GUID_LENGTH];

    /// <summary>
    /// TitleId that should be launched in order to join this activity.
    /// </summary>
    uint32_t TitleId;

    /// <summary>
    /// The visibility state of the session. Whether other users can see, or join, etc.
    /// </summary>
    XblMultiplayerSessionVisibility Visibility;

    /// <summary>
    /// The join restriction of the session, which applies if visibility is "open".
    /// </summary>
    XblMultiplayerSessionRestriction JoinRestriction;

    /// <summary>
    /// Indicates whether the session is temporarily closed for joining.
    /// </summary>
    bool Closed;

    /// <summary>
    /// Xbox User ID of the member whose activity this is.
    /// </summary>
    uint64_t OwnerXuid;

    /// <summary>
    /// Number of total slots.
    /// </summary>
    uint32_t MaxMembersCount;

    /// <summary>
    /// Number of slots occupied.
    /// </summary>
    uint32_t MembersCount;

    /// <summary>
    /// String containing custom session properties JSON blob.
    /// </summary>
    const char* CustomSessionPropertiesJson;
} XblMultiplayerActivityDetails;

/// <summary>
/// Token that represents a unique device participating in the session.  
/// It's a case-insensitive string that can be used for equality comparisons.
/// </summary>
typedef struct XblDeviceToken
{
    /// <summary>
    /// The unique device.
    /// </summary>
    _Null_terminated_ char Value[XBL_MULTIPLAYER_DEVICE_TOKEN_MAX_LENGTH];
} XblDeviceToken;

/// <summary>
/// Represents a read only reference to member in a multiplayer session.
/// </summary>
typedef struct XblMultiplayerSessionMember
{
    /// <summary>
    /// Id for this member.  
    /// Unique within the context of the session which this member is part of.
    /// </summary>
    uint32_t MemberId;

    /// <summary>
    /// Id of this members' team in a tournament.
    /// DEPRECATED. It will be removed in a future release
    /// </summary>
    XBL_DEPRECATED const char* TeamId;

    /// <summary>
    /// Initial team assignment from SmartMatch.
    /// </summary>
    const char* InitialTeam;

    /// <summary>
    /// Arbitration Status of a member in a tournament.
    /// DEPRECATED. It will be removed in a future release
    /// </summary>
    XBL_DEPRECATED XblTournamentArbitrationStatus ArbitrationStatus;

    /// <summary>
    /// Xbox User ID of the member.  
    /// Only known if the member has accepted.
    /// </summary>
    uint64_t Xuid;

    /// <summary>
    /// JSON string that specify the custom constants for the member.
    /// </summary>
    const char* CustomConstantsJson;

    /// <summary>
    /// The base64 encoded secure device address of the member. (Optional)
    /// </summary>
    const char* SecureDeviceBaseAddress64;

    /// <summary>
    /// An array of roles for this member. (Optional)
    /// </summary>
    const XblMultiplayerSessionMemberRole* Roles;

    /// <summary>
    /// Number of entries in the Roles array.
    /// </summary>
    size_t RolesCount;

    /// <summary>
    /// JSON string that specify the custom properties for the member.
    /// </summary>
    const char* CustomPropertiesJson;

    /// <summary>
    /// The Gamertag of the member. (Optional)  
    /// Only known if the member has accepted.
    /// </summary>
    char Gamertag[XBL_GAMERTAG_CHAR_SIZE];

    /// <summary>
    /// The status of this member.
    /// </summary>
    XblMultiplayerSessionMemberStatus Status;

    /// <summary>
    /// Only true if this member is ready for turn.
    /// </summary>
    bool IsTurnAvailable;

    /// <summary>
    /// Indicates if this MultiplayerSessionMember is for the current user.
    /// </summary>
    bool IsCurrentUser;

    /// <summary>
    /// Indicates to run QoS initialization for this user.  
    /// Defaults to false.  
    /// Ignored if there is not a "memberInitialization" section for the session.
    /// </summary>
    bool InitializeRequested;

    /// <summary>
    /// When match adds a user to a session, it can provide some context around how and why they were matched into the session.  
    /// This is a copy of the user's serverMeasurements from the matchmaking session.
    /// </summary>
    const char* MatchmakingResultServerMeasurementsJson;

    /// <summary>
    /// QoS measurements by game-server connection string.  
    /// Like all fields, "serverMeasurements" must be updated as a whole, so it should be set once when measurement is complete.  
    /// If empty, it means that none of the measurements completed within the "serverMeasurementTimeout".
    /// </summary>
    const char* ServerMeasurementsJson;

    /// <summary>
    /// A collection of memberIds in my group.  
    /// If a "initializationGroup" list is set, the member's own index will always be added if it isn't already present.  
    /// During managed initialization, if any members in the list fail, this member will also fail.
    /// </summary>
    const uint32_t* MembersInGroupIds;

    /// <summary>
    /// The number of entries in the MembersInGroupIds array.
    /// </summary>
    size_t MembersInGroupCount;

    /// <summary>
    /// QoS measurements by secure device address.  
    /// Like all fields, "measurements" must be updated as a whole.  
    /// It should be set once when measurement is complete, not incrementally.  
    /// If a "measurements" object is set, it can't contain an entry for the member's own address.
    /// </summary>
    const char* QosMeasurementsJson;

    /// <summary>
    /// This is set when the member uploads a secure device address.  
    /// It's a case-insensitive string that can be used for equality comparisons.
    /// </summary>
    XblDeviceToken DeviceToken;

    /// <summary>
    /// This is the device's NAT setting when the member uploads a secure device address.
    /// </summary>
    XblNetworkAddressTranslationSetting Nat;

    /// <summary>
    /// If the member is active, this is the title ID in which they are active.
    /// </summary>
    uint32_t ActiveTitleId;

    /// <summary>
    /// This value is only useful to read when the title is manually managing their own QoS.  
    /// If the "memberInitialization" section is set and the member was added with "initialize":true, 
    /// this is set to the initialization episode that the member will participate in otherwise it is 0.  
    /// Users join sessions in batches.  
    /// The initialization episode number indicates a set of users that QoS needs to be performed against.  
    /// Initialization episode 1 is a special value used for the members added to a new session at create time.
    /// </summary>
    uint32_t InitializationEpisode;

    /// <summary>
    /// The time the user joined the session.  
    /// If "reserved" is true, this is the time the reservation was made.
    /// </summary>
    time_t JoinTime;

    /// <summary>
    /// The cause of why the initialization failed, or XblMultiplayerMeasurementFailure::None if there was no failure.  
    /// Set when transitioning out of the "joining" or "measuring" stage if this member doesn't pass.
    /// </summary>
    XblMultiplayerMeasurementFailure InitializationFailureCause;

    /// <summary>
    /// An array of group names for the current user indicating which groups that user was part of during a multiplayer session.
    /// </summary>
    const char** Groups;

    /// <summary>
    /// The number of items in the Groups array.
    /// </summary>
    size_t GroupsCount;

    /// <summary>
    /// Gets a list of group names for the current user indicating which groups that user encountered during a multiplayer session.
    /// </summary>
    const char** Encounters;

    /// <summary>
    /// The number of items in the Groups array.
    /// </summary>
    size_t EncountersCount;

    /// <summary>
    /// The tournament team session reference.
    /// DEPRECATED. It will be removed in a future release
    /// </summary>
    XBL_DEPRECATED XblMultiplayerSessionReference TournamentTeamSessionReference;

    /// <summary>
    /// Internal use only.
    /// </summary>
    void* Internal;
} XblMultiplayerSessionMember;

/// <summary>
/// A set of properties associated with this session.  
/// Any player can modify these properties.
/// </summary>
typedef struct XblMultiplayerSessionProperties
{
    /// <summary>
    /// A collection of keywords associated with the session (Optional, might be empty).
    /// </summary>
    const char** Keywords;

    /// <summary>
    /// The number of keywords.
    /// </summary>
    size_t KeywordCount;

    /// <summary>
    /// Restricts who can join "open" sessions. (Has no effect on reservations, which means it has no impact on "private" and "visible" sessions.)  
    /// Defaults to "none".  
    /// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.  
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can join without a reservation.
    /// </summary>
    XblMultiplayerSessionRestriction JoinRestriction;

    /// <summary>
    /// Restricts who can read "open" sessions. (Has no effect on reservations, which means it has no impact on "private" and "visible" sessions.)  
    /// Defaults to "none".  
    /// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.  
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can read without a reservation.  
    /// The read restriction applies to sessions with "open" or "visible" visibility and determines who can read the session without an invite.  
    /// The read restriction must be at least as accessible as the join restriction, i.e. 'joinRestriction' can't be set to "followed" without also setting 'readRestriction'."
    /// </summary>
    XblMultiplayerSessionRestriction ReadRestriction;

    /// <summary>
    /// A collection of session MemberIds indicating whose turn it is.
    /// </summary>
    uint32_t* TurnCollection;

    /// <summary>
    /// The number of entries in the TurnCollection array.
    /// </summary>
    size_t TurnCollectionCount;

    /// <summary>
    /// A JSON string representing the target session constants.
    /// </summary>
    const char* MatchmakingTargetSessionConstantsJson;

    /// <summary>
    /// JSON string that specify the custom properties for the session.  
    /// These can be changed anytime.  
    /// When changing, call multiplayer_service::write_session to write the changes to the service.
    /// </summary>
    const char* SessionCustomPropertiesJson;

    /// <summary>
    /// Force a specific connection string to be used.  
    /// This is useful for session in progress join scenarios.
    /// </summary>
    const char* MatchmakingServerConnectionString;

    /// <summary>
    /// The ordered list of connection strings that the session could use to connect to a game server.  
    /// Generally titles should use the first on the list, but sophisticated titles could use 
    /// a custom mechanism for choosing one of the others (e.g. based on load).
    /// </summary>
    const char** ServerConnectionStringCandidates;

    /// <summary>
    /// The number of entries in the ServerConnectionStringCandidates array.
    /// </summary>
    size_t ServerConnectionStringCandidatesCount;

    /// <summary>
    /// Session MemberIds of owners of the session.
    /// </summary>
    uint32_t* SessionOwnerMemberIds;

    /// <summary>
    /// The number of entries in the SessionOwnerMemberIds array.
    /// </summary>
    size_t SessionOwnerMemberIdsCount;

    /// <summary>
    /// Device token of the host.  
    /// Must match the "deviceToken" of at least one member, otherwise this field is deleted.  
    /// If "peerToHostRequirements" is set and "host" is set, the measurement stage assumes the given host is the correct host and only measures metrics to that host.
    /// </summary>
    XblDeviceToken HostDeviceToken;

    /// <summary>
    /// Controls whether a session is joinable, independent of visibility, join restriction, and available space in the session.  
    /// Does not affect reservations.  
    /// Defaults to false.
    /// </summary>
    bool Closed;

    /// <summary>
    /// If true, it would allow the members of the session to be locked, such that if a user leaves they are able to 
    /// come back into the session but no other user could take that spot. Defaults to false.
    /// </summary>
    bool Locked;

    /// <summary>
    /// Setting to true by a client triggers a Xbox Live Compute allocation attempt by MPSD.  
    /// Defaults to false.
    /// </summary>
    bool AllocateCloudCompute;

    /// <summary>
    /// True if the match that was found didn't work out and needs to be resubmitted.  
    /// Set to false to signal that the match did work, and the matchmaking service can release the session.
    /// </summary>
    bool MatchmakingResubmit;
} XblMultiplayerSessionProperties;

/// <summary>
/// Basic info about a local multiplayer session.
/// </summary>
typedef struct XblMultiplayerSessionInfo
{
    /// <summary>
    /// The contract version of the session.
    /// </summary>
    uint32_t ContractVersion;

    /// <summary>
    /// The branch of the session used to scope change numbers.
    /// </summary>
    char Branch[XBL_GUID_LENGTH];

    /// <summary>
    /// The change number of the session.
    /// </summary>
    uint64_t ChangeNumber;

    /// <summary>
    /// A unique ID to the session used to query trace logs for entries that relate to the session.
    /// </summary>
    char CorrelationId[XBL_GUID_LENGTH];

    /// <summary>
    /// The time that the session began.
    /// </summary>
    time_t StartTime;

    /// <summary>
    /// If any timeouts are in progress, this is the date when the next timer will fire.
    /// </summary>
    time_t NextTimer;

    /// <summary>
    /// A unique search handle ID to the session.
    /// </summary>
    char SearchHandleId[XBL_GUID_LENGTH];
} XblMultiplayerSessionInfo;

/// <summary>
/// Present during member initialization.
/// </summary>
typedef struct XblMultiplayerSessionInitializationInfo
{
    /// <summary>
    /// The 'stage' goes from "joining" to "measuring" to "evaluating".  
    /// If episode #1 fails, then 'stage' is set to "failed" and the session cannot be initialized.  
    /// Otherwise, when an initialization episode completes, the 'initializing' object is removed.  
    /// If 'autoEvaluate' is set, "evaluating" is skipped. If neither 'metrics' nor 'measurementServerAddresses' is set, "measuring" is skipped.
    /// </summary>
    XblMultiplayerInitializationStage Stage;

    /// <summary>
    /// The time with the initialization stage started.
    /// </summary>
    time_t StageStartTime;

    /// <summary>
    /// If member_initialization set and Initialize is true on the member, then the member gets assigned to an InitializingEpisode.  
    /// An episode is a set of users that need to have QoS metrics applied to them.  
    /// Will be 0 when the InitializingEpisode is not set.  
    /// This value is only useful when manually managing QoS.
    /// </summary>
    uint32_t Episode;
} XblMultiplayerSessionInitializationInfo;

/// <summary>
/// Arguments passed to the event handler when a session change occurs.
/// </summary>
typedef struct XblMultiplayerSessionChangeEventArgs
{
    /// <summary>
    /// The session that triggered this event.
    /// </summary>
    XblMultiplayerSessionReference SessionReference;

    /// <summary>
    /// The branch of the session used to scope change numbers.
    /// </summary>
    char Branch[XBL_GUID_LENGTH];

    /// <summary>
    /// The change number of the session.
    /// </summary>
    uint64_t ChangeNumber;
} XblMultiplayerSessionChangeEventArgs;

/// <summary>
/// Queries the visible multiplayer sessions based on the configuration of this request.
/// </summary>
typedef struct XblMultiplayerSessionQuery
{
    /// <summary>
    /// The service configuration id that the session is a part of.
    /// </summary>
    char Scid[XBL_SCID_LENGTH];

    /// <summary>
    /// The maximum number of items to return.
    /// </summary>
    uint32_t MaxItems;

    /// <summary>
    /// Include private sessions to the result.
    /// </summary>
    bool IncludePrivateSessions;

    /// <summary>
    /// Include sessions that the user hasn't accepted.  
    /// Must specify xboxUserIdFilter to use.
    /// </summary>
    bool IncludeReservations;

    /// <summary>
    /// Include inactive sessions to the result.  
    /// Must specify xboxUserIdFilter to use.
    /// </summary>
    bool IncludeInactiveSessions;

    /// <summary>
    /// Filter result to just sessions these Xbox User IDs in it. (Optional)  
    /// You must specify at least one Xuid filter OR a keyword filter.
    /// </summary>
    uint64_t* XuidFilters;

    /// <summary>
    /// The number of Xuids in the XuidsFilters array.
    /// </summary>
    size_t XuidFiltersCount;

    /// <summary>
    /// Filter result to just sessions with this keyword. (Optional)  
    /// You must specify at least one Xuid filter OR a keyword filter.
    /// </summary>
    const char* KeywordFilter;

    /// <summary>
    /// The name of the template for the multiplayer session to filter on.
    /// </summary>
    char SessionTemplateNameFilter[XBL_MULTIPLAYER_SESSION_TEMPLATE_NAME_MAX_LENGTH];

    /// <summary>
    /// Filter result to just sessions with the specified visibility.
    /// </summary>
    XblMultiplayerSessionVisibility VisibilityFilter;

    /// <summary>
    /// Filter result to just sessions with this major version or less of the contract.  
    /// Use 0 to ignore.
    /// </summary>
    uint32_t ContractVersionFilter;
} XblMultiplayerSessionQuery;

/// <summary>
/// Session information returned from a XblMultiplayerQuerySessionsAsync call.
/// </summary>
typedef struct XblMultiplayerSessionQueryResult
{
    /// <summary>
    /// The time that the session began.
    /// </summary>
    time_t StartTime;

    /// <summary>
    /// Session reference for the session.
    /// </summary>
    XblMultiplayerSessionReference SessionReference;

    /// <summary>
    /// The current status of the session.
    /// </summary>
    XblMultiplayerSessionStatus Status;

    /// <summary>
    /// The visibility state of the session.  
    /// Whether other users can see, or join, etc.
    /// </summary>
    XblMultiplayerSessionVisibility Visibility;

    /// <summary>
    /// Indicates if it is my turn.
    /// </summary>
    bool IsMyTurn;

    /// <summary>
    /// Xbox User ID of the member.
    /// </summary>
    uint64_t Xuid;

    /// <summary>
    /// Approximate number of non-reserved members.
    /// </summary>
    uint32_t AcceptedMemberCount;

    /// <summary>
    /// Join restriction for the session.
    /// </summary>
    XblMultiplayerSessionRestriction JoinRestriction;
} XblMultiplayerSessionQueryResult;

/// <summary>
/// A handle to an search details object that is associated with a MPSD sessions.  
/// The object has filterable and queryable attributes about the session.
/// </summary>
typedef struct XblMultiplayerSearchHandleDetails* XblMultiplayerSearchHandle;

#define XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH 100

/// <summary>
/// A searchable tag that can be attached to a multiplayer session search handle when it is created.  
/// Tags must be alphanumeric and start with a letter.  
/// They're case-insensitive.
/// </summary>
typedef struct XblMultiplayerSessionTag
{
    /// <summary>
    /// The multiplayer search handle.
    /// </summary>
    char value[XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH];
} XblMultiplayerSessionTag;

/// <summary>
/// An associative attribute that can be attached to a multiplayer session search handle when it is created.  
/// Attribute names be lower-case alphanumeric, and start with a letter.
/// </summary>
typedef struct XblMultiplayerSessionStringAttribute
{
    /// <summary>
    /// Name of the attribute.
    /// </summary>
    char name[XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH];

    /// <summary>
    /// Attribute value.
    /// </summary>
    char value[XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH];
} XblMultiplayerSessionStringAttribute;

/// <summary>
/// An associative attribute that can be attached to a multiplayer session search handle when it is created.
/// Attribute names be lower-case alphanumeric, and start with a letter.
/// </summary>
typedef struct XblMultiplayerSessionNumberAttribute
{
    /// <summary>
    /// Name of the attribute.
    /// Attribute names be lower-case alphanumeric, and start with a letter.
    /// </summary>
    char name[XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH];

    /// <summary>
    /// Attribute value.
    /// </summary>
    double value;
} XblMultiplayerSessionNumberAttribute;

/// <summary>
/// Handle to a local multiplayer session.  
/// This handle will be used to query and change the local session object.  
/// Anytime the changes made to the local session object will not be reflected in the multiplayer service
/// until a subsequent call to <see cref="XblMultiplayerWriteSessionAsync"/>.
/// </summary>
typedef struct XblMultiplayerSession* XblMultiplayerSessionHandle;

/// <summary>
/// Optional args when creating a new local multiplayer session.
/// </summary>
typedef struct XblMultiplayerSessionInitArgs
{
    /// <summary>
    /// The maximum number of members in this session.  
    /// This value can only be set if the maximum is not specified in the title's multiplayer session template.  
    /// If the maximum is specified in the title's multiplayer session template, then set to 0 to ignore this parameter.
    /// </summary>
    uint32_t MaxMembersInSession;

    /// <summary>
    /// The visibility of this session.
    /// </summary>
    XblMultiplayerSessionVisibility Visibility;

    /// <summary>
    /// A collection of Xbox User IDs indicating who initiated the session. (Optional)
    /// </summary>
    const uint64_t* InitiatorXuids;

    /// <summary>
    /// The number of Xuids in the Initiator XuidsArray.
    /// </summary>
    size_t InitiatorXuidsCount;

    /// <summary>
    /// JSON that specifies the custom constants for the session.These can not be changed after the session is created. (Optional)
    /// </summary>
    _Null_terminated_ const char* CustomJson;
} XblMultiplayerSessionInitArgs;

/// <summary>
/// Creates a new local multiplayer session.
/// </summary>
/// <param name="xuid">The Xbox User ID of the user who is creating this session.</param>
/// <param name="sessionReference">A reference that uniquely identifies the session.</param>
/// <param name="initArgs">Additional args used to initialize the session. Must also include the sessionReference if these are included.</param>
/// <returns>Handle to a local multiplayer session.</returns>
/// <remarks>
/// You must call <see cref="XblMultiplayerWriteSessionAsync"/> after this to write batched local changes to the service.  
/// If this is called without XblMultiplayerWriteSessionAsync, this will only create a local session object, but does not commit it to the service.  
/// When the local session object is no longer needed, call <see cref="XblMultiplayerSessionCloseHandle"/>.
/// </remarks>
STDAPI_(XblMultiplayerSessionHandle) XblMultiplayerSessionCreateHandle(
    _In_ uint64_t xuid,
    _In_opt_ const XblMultiplayerSessionReference* sessionReference,
    _In_opt_ const XblMultiplayerSessionInitArgs* initArgs
) XBL_NOEXCEPT;

/// <summary>
/// Increments the reference count to a local session object.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="duplicatedHandle">Passes back the duplicated handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionDuplicateHandle(
    _In_ XblMultiplayerSessionHandle handle,
    _Out_ XblMultiplayerSessionHandle* duplicatedHandle
) XBL_NOEXCEPT;

/// <summary>
/// Decrements the reference count to a local session object.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSessionCloseHandle(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Arbitration status of a tournament.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>Returns the status of a tournament.</returns>
STDAPI_XBL_DEPRECATED_(XblTournamentArbitrationStatus) XblMultiplayerSessionArbitrationStatus(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// The time when the server returned the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The time when the server returned the session.</returns>
/// <remarks>
/// Note that this is not part of the remote session state, it only indicates when the local session was created.
/// </remarks>
STDAPI_(time_t) XblMultiplayerSessionTimeOfSession(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Get the info about session initialization.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The initialization info used to create the session.</returns>
/// <remarks>
/// If the session is not doing member initialization, nullptr will be returned.
/// </remarks>
STDAPI_(const XblMultiplayerSessionInitializationInfo*) XblMultiplayerSessionGetInitializationInfo(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Returns an OR'd set of XblMultiplayerSessionChangeTypes values representing the aspects of the session 
/// that the current xboxlivecontext is subscribed to, of XblMultiplayerSessionChangeTypes::None if there is none.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>An OR'd set of XblMultiplayerSessionChangeTypes values representing the aspects of the session.</returns>
STDAPI_(XblMultiplayerSessionChangeTypes) XblMultiplayerSessionSubscribedChangeTypes(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Host candidates are an ordered list of device tokens, ordered by preference as specified by XblMultiplayerMetrics
/// in the session constants.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="deviceTokens">Passes back a pointer to the array of candidate device tokens.  
/// The memory for the pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <param name="deviceTokensCount">Passes back the size of the deviceTokens array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionHostCandidates(
    _In_ XblMultiplayerSessionHandle handle,
    _Out_ const XblDeviceToken** deviceTokens,
    _Out_ size_t* deviceTokensCount
) XBL_NOEXCEPT;

/// <summary>
/// The uniquely identifying information for the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>A pointer to the multiplayer session reference.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
STDAPI_(const XblMultiplayerSessionReference*) XblMultiplayerSessionSessionReference(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// A set of constants associated with this session.  
/// These can only be set when creating the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>A pointer to the constant values used in the multiplayer session.</returns>
STDAPI_(const XblMultiplayerSessionConstants*) XblMultiplayerSessionSessionConstants(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Sets the maximum number of members in this session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="maxMembersInSession">Value to set for max members.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSessionConstantsSetMaxMembersInSession(
    _In_ XblMultiplayerSessionHandle handle,
    uint32_t maxMembersInSession
) XBL_NOEXCEPT;

/// <summary>
/// Sets the visibility of this session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="visibility">New visibility value.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSessionConstantsSetVisibility(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerSessionVisibility visibility
) XBL_NOEXCEPT;

/// <summary>
/// Sets the timeouts for the session.  
/// This can only be set when creating a new session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="memberReservedTimeout">The timeout for a member reservation, in milliseconds.  
/// A value of 0 is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
/// <param name="memberInactiveTimeout">The timeout for a member to be considered inactive, in milliseconds.  
/// A value of 0 is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
/// <param name="memberReadyTimeout">The timeout for a member to be considered ready, in milliseconds.  
/// A value of 0 is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
/// <param name="sessionEmptyTimeout">The timeout for an empty session, in milliseconds.  
/// A value of 0 is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionConstantsSetTimeouts(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ uint64_t memberReservedTimeout,
    _In_ uint64_t memberInactiveTimeout,
    _In_ uint64_t memberReadyTimeout,
    _In_ uint64_t sessionEmptyTimeout
) XBL_NOEXCEPT;

/// <summary>
/// Sets the arbitration timeouts for the session.  
/// This can only be set when creating a new session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="arbitrationTimeout">The timeout for arbitration, in milliseconds representing the point at which results are finalized.</param>
/// <param name="forfeitTimeout">The timeout for forfeit, in milliseconds representing the point at which, 
/// if the session has no active users, the match is canceled.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblMultiplayerSessionConstantsSetArbitrationTimeouts(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ uint64_t arbitrationTimeout,
    _In_ uint64_t forfeitTimeout
) XBL_NOEXCEPT;

/// <summary>
/// Enables or disables connectivity metrics for the session.  
/// This can only be set when creating a new session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="enableLatencyMetric">True to enable the measuring of latency, and false to disable latency measurement.</param>
/// <param name="enableBandwidthDownMetric">True to enable the measuring of bandwidth down, and false to disable bandwidth down measurement.</param>
/// <param name="enableBandwidthUpMetric">True to enable the measuring of bandwidth up, and false to disable bandwidth up measurement.</param>
/// <param name="enableCustomMetric">True to enable custom metrics, and false to disable them.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// For ones that are enabled, they must be sufficient to satisfy the QoS requirements.
/// </remarks>
STDAPI XblMultiplayerSessionConstantsSetQosConnectivityMetrics(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ bool enableLatencyMetric,
    _In_ bool enableBandwidthDownMetric,
    _In_ bool enableBandwidthUpMetric,
    _In_ bool enableCustomMetric
) XBL_NOEXCEPT;

/// <summary>
/// If a 'memberInitialization' object is set, the session expects the client system or title to perform 
/// initialization following session creation and/or as new members join the session.  
/// This can only be set when creating a new session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="memberInitialization">Object specifying member initialization parameters.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The timeouts and initialization stages are automatically tracked by the session, including QoS measurements if any metrics are set.  
/// These timeouts override the session's reservation and ready timeouts for members that have 'initializationEpisode' set.
/// </remarks>
STDAPI XblMultiplayerSessionConstantsSetMemberInitialization(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerMemberInitialization memberInitialization
) XBL_NOEXCEPT;

/// <summary>
/// These thresholds apply to each pairwise connection for all members in a session.  
/// This can only be set when creating a new session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="requirements">Object specifying the peer to peer requirements for the session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionConstantsSetPeerToPeerRequirements(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerPeerToPeerRequirements requirements
) XBL_NOEXCEPT;

/// <summary>
/// These thresholds apply to each connection from a host candidate.  
/// This can only be set when creating a new session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="requirements">Object specifying the peer to host requirements for the session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionConstantsSetPeerToHostRequirements(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerPeerToHostRequirements requirements
) XBL_NOEXCEPT;

/// <summary>
/// The set of potential server connection strings that should be evaluated.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="measurementServerAddressesJson">The Json that represent the measurement server addresses.  
/// Example JSON:
/// {
///   "server farm a": {
///     "secureDeviceAddress": "r5Y=" // Base-64 encoded secure-device-address
///   },  
///   "datacenter b" : {
///     "secureDeviceAddress": "rwY="
///   }
/// }
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionConstantsSetMeasurementServerAddressesJson(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ const char* measurementServerAddressesJson
) XBL_NOEXCEPT;

/// <summary>
/// Sets the capabilities constants for the session.  
/// This can only be set when creating a new session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="capabilities">A collection of MultiplayerSessionCapabilities flags that apply to the MultiplayerSessionConstant's capabilities JSON object.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionConstantsSetCapabilities(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerSessionCapabilities capabilities
) XBL_NOEXCEPT;

/// <summary>
/// This can only be set when creating a new session.  
/// Can only be specified if the 'cloudCompute' capability is set.  
/// Enables clients to request that a cloud compute instance be allocated on behalf of the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="sessionCloudComputePackageConstantsJson">Cloud compute instance to be allocated on behalf of the session.  
/// Example Json:
/// {
///   "crossSandbox": true, // True if the cloud compute resources are provisioned to be sandbox-agnostic, false if they are provisioned per-sandbox.
///   "titleId" : "4567", //The title ID and GSI set of the cloud compute package to allocate.
///   "gsiSet" : "128ce92a-45d0-4319-8a7e-bd8e940114ec"
/// }
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionConstantsSetCloudComputePackageJson(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ const char* sessionCloudComputePackageConstantsJson
) XBL_NOEXCEPT;

/// <summary>
/// Get the properties associated with the session.  
/// Any player can modify these properties.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>A pointer to the set of properties associated with this session.</returns>
STDAPI_(const XblMultiplayerSessionProperties*) XblMultiplayerSessionSessionProperties(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// A collection of keywords associated with the session. (Optional, might be empty)
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="keywords">The keywords to associate with the session. Overwrites existing keywords.</param>
/// <param name="keywordsCount">The size of the keywords array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionPropertiesSetKeywords(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ const char** keywords,
    _In_ size_t keywordsCount
) XBL_NOEXCEPT;

/// <summary>
/// Restricts who can join "open" sessions. (Has no effect on reservations, which means it has no impact on "private" and "visible" sessions.)
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="joinRestriction">New join restriction value.</param>
/// <returns></returns>
/// <remarks>
/// Defaults to "none".  
/// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.  
/// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can join without a reservation.
/// </remarks>
STDAPI_(void) XblMultiplayerSessionPropertiesSetJoinRestriction(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerSessionRestriction joinRestriction
) XBL_NOEXCEPT;

/// <summary>
/// Restricts who can read "open" sessions. (Has no effect on reservations, which means it has no impact on "private" and "visible" sessions.)
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="readRestriction">New read restriction value.</param>
/// <returns></returns>
/// <remarks>
/// Defaults to "none".  
/// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.  
/// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can read without a reservation.
/// </remarks>
STDAPI_(void) XblMultiplayerSessionPropertiesSetReadRestriction(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerSessionRestriction readRestriction
) XBL_NOEXCEPT;

/// <summary>
/// Sets the collection of session MemberIds indicating whose turn it is.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="turnCollectionMemberIds">Collection of MemberIds whose turn it is.</param>
/// <param name="turnCollectionMemberIdsCount">The size of the turnCollectionMemberIds array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionPropertiesSetTurnCollection(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ const uint32_t* turnCollectionMemberIds,
    _In_ size_t turnCollectionMemberIdsCount
) XBL_NOEXCEPT;

/// <summary>
/// A set of role types associated with this session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="roleTypes">Passes back a pointer to an array of roleTypes.  
/// The memory for the pointer remains valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <param name="roleTypesCount">Passes back the number of roles types in the roleTypes array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionRoleTypes(
    _In_ XblMultiplayerSessionHandle handle,
    _Out_ const XblMultiplayerRoleType** roleTypes,
    _Out_ size_t* roleTypesCount
) XBL_NOEXCEPT;

/// <summary>
/// Fetches the XblMultiplayerRole object by role type name and role name.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="roleTypeName">Name of the role type the role belongs to.</param>
/// <param name="roleName">Name of the role.</param>
/// <param name="role">Passes back a pointer to the role object with info about the role.  
/// The memory for the pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.  
/// If no such role exists, nullptr will be passed back.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Note that newly created sessions will not have role info populated until the XblMultiplayerWriteSessionAsync is called.
/// </remarks>
STDAPI XblMultiplayerSessionGetRoleByName(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* roleTypeName,
    _In_z_ const char* roleName,
    _Out_ const XblMultiplayerRole** role
) XBL_NOEXCEPT;

/// <summary>
/// Sets the max member count and/or target member counts for a role.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="roleTypeName">Name of the role type for the role being modified.</param>
/// <param name="roleName">Name of the role to modify.</param>
/// <param name="maxMemberCount">The maximum number of members that can have the role.</param>
/// <param name="targetMemberCount">The target number of members for the role.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Only the session owner can modify role settings and only those that are mutable (see XblMultiplayerRoleType.MutableRoleSettings).  
/// In your session template, you also need to set 'hasOwners' capability and 'ownerManaged' to true for the specific role type 
/// that you want to modify.
/// </remarks>
STDAPI XblMultiplayerSessionSetMutableRoleSettings(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* roleTypeName,
    _In_z_ const char* roleName,
    _In_opt_ uint32_t* maxMemberCount,
    _In_opt_ uint32_t* targetMemberCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets the collection of members that are in the session or entering the session together.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="members">Passes back a pointer to array of session member objects.
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <param name="membersCount">Passes back the size of the returned array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Call XblMultiplayerSessionJoin or XblMultiplayerSessionLeave to add or remove yourself from this list.  
/// Call XblMultiplayerSessionAddMemberReservation to add a reservation for another user on this list.
/// </remarks>
STDAPI XblMultiplayerSessionMembers(
    _In_ XblMultiplayerSessionHandle handle,
    _Out_ const XblMultiplayerSessionMember** members,
    _Out_ size_t* membersCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets the session member with a specified MemberId.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="memberId">The Id of the member to fetch.</param>
/// <returns>A pointer to a member in a multiplayer session. (Read Only)  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
/// <remarks>
/// If there is no member with the specified Id, nullptr is returned.
/// </remarks>
STDAPI_(const XblMultiplayerSessionMember*) XblMultiplayerSessionGetMember(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ uint32_t memberId
) XBL_NOEXCEPT;

/// <summary>
/// A multiplayer session server that contains properties associated with a target session reference.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The matchmaking server supporting the multiplayer session.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
STDAPI_(const XblMultiplayerMatchmakingServer*) XblMultiplayerSessionMatchmakingServer(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// A tournament session servers that contains properties associated with a tournament reference.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The tournament server supporting the multiplayer session.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED
STDAPI_XBL_DEPRECATED_(const XblMultiplayerTournamentsServer*) XblMultiplayerSessionTournamentsServer(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;
XBL_WARNING_POP

/// <summary>
/// An arbitration server that contains properties associated with a tournament games results.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The arbitration server supporting the multiplayer session.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED
STDAPI_XBL_DEPRECATED_(const XblMultiplayerArbitrationServer*) XblMultiplayerSessionArbitrationServer(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;
XBL_WARNING_POP

/// <summary>
/// The number of members that have accepted and are added to the session and are no longer pending.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The number of members that have accepted.</returns>
STDAPI_(uint32_t) XblMultiplayerSessionMembersAccepted(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// A JSON string containing a collection of servers for this multiplayer session.  
/// This Json object has the info that is parsed to create the XblMultiplayerArbitrationServer, 
/// XblMultiplayerMatchmakingServer, and XblMultiplayerTournamentsServer objects.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The JSON string containing a collection of servers for this multiplayer session.
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
STDAPI_(const char*) XblMultiplayerSessionRawServersJson(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Sets the JSON string containing a collection of servers for this multiplayer session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="rawServersJson">Json describing the session servers.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionSetRawServersJson(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* rawServersJson
) XBL_NOEXCEPT;

/// <summary>
/// The ETag returned with this session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The ETag.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
STDAPI_(const char*) XblMultiplayerSessionEtag(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Returns the current User in the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The current User in the session.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
/// <remarks>
/// A nullptr will be returned if there is no current user in the session.
/// </remarks>
STDAPI_(const XblMultiplayerSessionMember*) XblMultiplayerSessionCurrentUser(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Gets some basic info about the session.  
/// Represents the info in the root of the MPSD session document.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>A pointer to the basic info about the local multiplayer session.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</returns>
STDAPI_(const XblMultiplayerSessionInfo*) XblMultiplayerSessionGetInfo(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// After call XblMultiplayerWriteSessionAsync, the status of the write.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>The write status of the Multiplayer session.</returns>
STDAPI_(XblWriteSessionStatus) XblMultiplayerSessionWriteStatus(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Add a new member reservation on the session for the specified xuid and member constants.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="xuid">The xuid to add a reservation for.</param>
/// <param name="memberCustomConstantsJson">The custom constants to set for this member.  
/// This is the only time the member's constants can be set. (Optional)</param>
/// <param name="initializeRequested">True if the system should perform managed initialization, and false otherwise.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionAddMemberReservation(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ uint64_t xuid,
    _In_opt_z_ const char* memberCustomConstantsJson,
    _In_ bool initializeRequested
) XBL_NOEXCEPT;

/// <summary>
/// Joins the local user to the session, sets the user to active.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="memberCustomConstantsJson">The custom constants to set for this member.  
/// This is the only time the member's constants can be set.</param>
/// <param name="initializeRequested">True if the system should perform managed initialization, and false otherwise.</param>
/// <param name="joinWithActiveStatus">True if player should join with active status.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionJoin(
    _In_ XblMultiplayerSessionHandle handle,
    _In_opt_z_ const char* memberCustomConstantsJson,
    _In_ bool initializeRequested,
    _In_ bool joinWithActiveStatus
) XBL_NOEXCEPT;

/// <summary>
/// This can only be set when creating a new session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="initializationSucceeded">True if initialization succeeded, and false otherwise.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSessionSetInitializationSucceeded(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ bool initializationSucceeded
) XBL_NOEXCEPT;

/// <summary>
/// Sets the device token of the host.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="hostDeviceToken">The host device token.</param>
/// <returns></returns>
/// <remarks>
/// If "peerToHostRequirements" is set and this is set, the measurement stage assumes the given host is the correct host and only measures metrics to that host.
/// Note that host device tokens are generated from a session member's secure device address, so ensure that the secure device address is set
/// (see <see cref="XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64"/>) for the desired host prior to calling this method.
/// </remarks>
STDAPI_(void) XblMultiplayerSessionSetHostDeviceToken(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblDeviceToken hostDeviceToken
) XBL_NOEXCEPT;

/// <summary>
/// Forces a specific server connection string to be used, useful in preserveSession=always cases.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="serverConnectionPath">The server connection path.  
/// Setting this path can be useful when the session is preserved.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSessionSetMatchmakingServerConnectionPath(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* serverConnectionPath
) XBL_NOEXCEPT;

/// <summary>
/// If set to true, makes the session "closed", meaning that new users will not be able to join unless they already have a reservation.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="closed">True if the session should be closed, false otherwise.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSessionSetClosed(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ bool closed
) XBL_NOEXCEPT;

/// <summary>
/// Sets if a session is locked or not.  
/// If locked, members that leave the session will be able to come back into the session, allowing no additional users to take that spot.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="locked">Set true, if the session should be lock, false otherwise.</param>
/// <returns></returns>
/// <remarks>
/// If the session is locked, it must also be set to closed.
/// </remarks>
STDAPI_(void) XblMultiplayerSessionSetLocked(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ bool locked
) XBL_NOEXCEPT;

/// <summary>
/// If this property is set an allocation of the 'cloudComputePackage' will be attempted.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="allocateCloudCompute">True if a cloud compute package should be allocated and false otherwise.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSessionSetAllocateCloudCompute(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ bool allocateCloudCompute
) XBL_NOEXCEPT;

/// <summary>
/// Sets if a match needs to be resubmitted or not.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="matchResubmit">Set true, if the match that was found didn't work out and needs to be resubmitted.  
/// Set false, if the match did work and the matchmaking service can release the session. </param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSessionSetMatchmakingResubmit(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ bool matchResubmit
) XBL_NOEXCEPT;

/// <summary>
/// The ordered list of case-insensitive connection strings that the session could use to connect to a game server.  
/// Generally titles should use the first on the list, but sophisticated titles could use 
/// a custom mechanism for choosing one of the others (e.g. based on load).
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="serverConnectionStringCandidates">The collection of connection paths.</param>
/// <param name="serverConnectionStringCandidatesCount">The size of the candidates array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionSetServerConnectionStringCandidates(
    _In_ XblMultiplayerSessionHandle handle,
    _In_reads_(serverConnectionStringCandidatesCount) const char** serverConnectionStringCandidates,
    _In_ size_t serverConnectionStringCandidatesCount
) XBL_NOEXCEPT;

/// <summary>
/// Configures the set of session changes that this client will be subscribed to.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="changeTypes">Or'd set of XblMultiplayerSessionChangeType enum values representing the change types to subscribe to.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Set to MultiplayerSessionChangeTypes::None to clear the subscription.
/// </remarks>
STDAPI XblMultiplayerSessionSetSessionChangeSubscription(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerSessionChangeTypes changeTypes
) XBL_NOEXCEPT;

/// <summary>
/// Call if the user who either created or got the session leaves the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If the session is deleted as a result of this action, a 204 response will be returned.
/// </remarks>
STDAPI XblMultiplayerSessionLeave(
    _In_ XblMultiplayerSessionHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Set the current user to active or inactive.  
/// The member must first be joined to the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="status">Indicates the current user status.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// You cannot set the user to reserved or ready in this manner.  
/// Use <see cref="XblMultiplayerSessionAddMemberReservation"/> to add a member reservation.
/// </remarks>
STDAPI XblMultiplayerSessionCurrentUserSetStatus(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ XblMultiplayerSessionMemberStatus status
) XBL_NOEXCEPT;

/// <summary>
/// Set the base64 encoded secure device address of the member.  
/// The member must first be joined to the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="value">Indicates the value of the current user's secure device address encoded in base64.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// On platforms that don't have a secure device address, call XblFormatSecureDeviceAddress 
/// to generate a value that can be used by this function. Note that setting a secure device address is required
/// to manually set a session host.
/// </remarks>
STDAPI XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ const char* value
) XBL_NOEXCEPT;

#if HC_PLATFORM != HC_PLATFORM_XDK && HC_PLATFORM != HC_PLATFORM_UWP
/// <summary>
/// A formatted secure device address.
/// </summary>
typedef struct XblFormattedSecureDeviceAddress
{
    /// <summary>
    /// The secure device address.
    /// </summary>
    char value[4096];
} XblFormattedSecureDeviceAddress;

/// <summary>
/// Formats a secure device address given a unique device id for platforms that don't support SDAs.
/// </summary>
/// <param name="deviceId">A unique id to represent this device for the lifetime of the local game process/instance.</param>
/// <param name="address">Passes back the formatted secure device address.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Formats the deviceId string according to the following recommendation:  
///
///     Generate 16 random bytes(e.g., a GUID) to be used for the lifetime of the local game process/instance
///     to uniquely represent the device.  Format the 'secureDeviceAddress' to be a string with the prefix "AAAAAAAA"
///     (8 capital letter As) followed by the 32 character hexadecimal representation of the random bytes(upper 
///     or lower case letters don't matter, nor does the byte order if the bytes are an actual structured GUID).  
///     For example, "AAAAAAAA00112233445566778899AABBCCDDEEFF".  
///
/// Call XblMultiplayerSessionCurrentUserSetSecureDeviceAddressBase64 using the formatted SDA to set the SDA for the user.
/// </remarks>
STDAPI XblFormatSecureDeviceAddress(
    _In_ const char* deviceId,
    _Inout_ XblFormattedSecureDeviceAddress* address
) XBL_NOEXCEPT;
#endif

/// <summary>
/// Set the role info of the local member.  
/// The member must first be joined to the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="roles">Indicates a collection of roles for the user.</param>
/// <param name="rolesCount">The size of the role array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionCurrentUserSetRoles(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ const XblMultiplayerSessionMemberRole* roles,
    _In_ size_t rolesCount
) XBL_NOEXCEPT;

/// <summary>
/// Set a collection of members in the group.  
/// The member must first be joined to the session.
/// </summary>
/// <param name="session">Handle to the multiplayer session.</param>
/// <param name="memberIds">Array of memberIds that are in the local users group.</param>
/// <param name="memberIdsCount">Size of the memberIds array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionCurrentUserSetMembersInGroup(
    _In_ XblMultiplayerSessionHandle session,
    _In_reads_(memberIdsCount) uint32_t* memberIds,
    _In_ size_t memberIdsCount
) XBL_NOEXCEPT;

/// <summary>
/// Sets a string vector of group names for the current user indicating which groups that user was part of during a multiplayer session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="groups">Array of group names.</param>
/// <param name="groupsCount">Size of the groups array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionCurrentUserSetGroups(
    _In_ XblMultiplayerSessionHandle handle,
    _In_reads_(groupsCount) const char** groups,
    _In_ size_t groupsCount
) XBL_NOEXCEPT;

/// <summary>
/// Sets a list of group names for the current user indicating which groups that user encountered during a multiplayer session.  
/// An encounter is a brief interaction with another group.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="encounters">Array of encounter strings.</param>
/// <param name="encountersCount">Size of the encounters array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionCurrentUserSetEncounters(
    _In_ XblMultiplayerSessionHandle handle,
    _In_reads_(encountersCount) const char** encounters,
    _In_ size_t encountersCount
) XBL_NOEXCEPT;

/// <summary>
/// Sets a collection of XblMultiplayerQosMeasurements for the members.  
/// This is only useful when the title is manually managing QoS. (If the platform is automatically performing QoS, this does not need to be called)
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="measurements">Json representing the QoS measurements of members.  
/// Example Json:
/// {
///   "e69c43a8": { // Device token of peer
///     "latency": 5953,  // Milliseconds
///     "bandwidthDown" : 19342,  // Kilobits per second
///     "bandwidthUp" : 944,  // Kilobits per second
///     "custom" : { }
///   },
///   ... // additional device tokens
/// }
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionCurrentUserSetQosMeasurements(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* measurements
) XBL_NOEXCEPT;

/// <summary>
/// Sets measurements JSON for the servers.  
/// This is only useful when the title is manually managing QoS. (If the platform is automatically performing QoS, this does not need to be called)
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="measurements">The JSON that represents the server measurements.  
/// Example Json:
/// {
///   "server farm a": {
///     "latency": 233  // Milliseconds
///   }
/// }
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionCurrentUserSetServerQosMeasurements(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* measurements
) XBL_NOEXCEPT;

/// <summary>
/// Set a custom property on the current user to the specified JSON string.  
/// The member must first be joined to the session.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="name">The name of the property to set.</param>
/// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionCurrentUserSetCustomPropertyJson(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* name,
    _In_z_ const char* valueJson
) XBL_NOEXCEPT;

/// <summary>
/// Delete a custom property on the current user.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="name">The name of the property to delete.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionCurrentUserDeleteCustomPropertyJson(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* name
) XBL_NOEXCEPT;

/// <summary>
/// Sets the properties of the matchmaking.  
/// This should only be set by a client acting as a matchmaking service.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="matchmakingTargetSessionConstantsJson">A JSON string representing the target session constants.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionSetMatchmakingTargetSessionConstantsJson(
    _In_ XblMultiplayerSessionHandle handle,
    _In_ const char* matchmakingTargetSessionConstantsJson
) XBL_NOEXCEPT;

/// <summary>
/// Set a session custom property to the specified JSON string.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="name">The name of the property to set.</param>
/// <param name="valueJson">
/// The JSON value to assign to the property. This must be a valid JSON string.
/// Examples include "\"JsonString\"", "{\"name\":\"A\"}", or "true".
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionSetCustomPropertyJson(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* name,
    _In_z_ const char* valueJson
) XBL_NOEXCEPT;

/// <summary>
/// Deletes a session custom property.
/// </summary>
/// <param name="handle">Handle to the multiplayer session.</param>
/// <param name="name">The name of the property to delete.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSessionDeleteCustomPropertyJson(
    _In_ XblMultiplayerSessionHandle handle,
    _In_z_ const char* name
) XBL_NOEXCEPT;

/// <summary>
/// Checks the deltas between 2 sessions and returns an Or'ed MultiplayerSessionChangeType.  
/// Useful to compare a session object passed to XblMultiplayerWriteSessionAsync with the session object returned.
/// </summary>
/// <param name="currentSessionHandle">A session to compare to the other.</param>
/// <param name="oldSessionHandle">A session to compare to the other.</param>
/// <returns>The Or'ed change types for a multiplayer session.</returns>
STDAPI_(XblMultiplayerSessionChangeTypes) XblMultiplayerSessionCompare(
    _In_ XblMultiplayerSessionHandle currentSessionHandle,
    _In_ XblMultiplayerSessionHandle oldSessionHandle
) XBL_NOEXCEPT;

/// <summary>
/// Increments the reference count to a local search handle details object.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="duplicatedHandle">Passes back the duplicated handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleDuplicateHandle(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ XblMultiplayerSearchHandle* duplicatedHandle
) XBL_NOEXCEPT;

/// <summary>
/// Decrements the reference count to a local search handle details object.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerSearchHandleCloseHandle(
    _In_ XblMultiplayerSearchHandle handle
) XBL_NOEXCEPT;

/// <summary>
/// Get the session reference for the session the search handle is associated with.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="sessionRef">Passes back the associated session reference.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetSessionReference(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ XblMultiplayerSessionReference* sessionRef
) XBL_NOEXCEPT;

/// <summary>
/// Get the Id for the search handle object.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="id">Passes back a search handle Id.  
/// The memory for the pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetId(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const char** id
) XBL_NOEXCEPT;

/// <summary>
/// Get a pointer to an array of Xuids who own the session associated with the search handle.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="xuids">Passes back a pointer to array of xuids.  
/// The memory for the pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <param name="xuidsCount">Passes back the count of xuids in the array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetSessionOwnerXuids(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const uint64_t** xuids,
    _Out_ size_t* xuidsCount
) XBL_NOEXCEPT;

/// <summary>
/// Get a pointer to an array of tags attached with the search handle.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="tags">Passes back a pointer to array of tags.  
/// The memory for the pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <param name="tagsCount">Passes back the count of tags in the array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetTags(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const XblMultiplayerSessionTag** tags,
    _Out_ size_t* tagsCount
) XBL_NOEXCEPT;

/// <summary>
/// Get a pointer to an array of attributes attached with the search handle.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="attributes">Passes back a pointer to array of attributes.  
/// The memory for the pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <param name="attributesCount">Passes back the count of attributes in the array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetStringAttributes(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const XblMultiplayerSessionStringAttribute** attributes,
    _Out_ size_t* attributesCount
) XBL_NOEXCEPT;

/// <summary>
/// Get a pointer to an array of attributes attached with the search handle.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="attributes">Passes back a pointer to array of attributes.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <param name="attributesCount">Passes back the count of attributes in the attributes array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetNumberAttributes(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const XblMultiplayerSessionNumberAttribute** attributes,
    _Out_ size_t* attributesCount
) XBL_NOEXCEPT;

/// <summary>
/// Get visibility of the session associated with the search handle.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="visibility">Passes back the visibility of the associated session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetVisibility(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ XblMultiplayerSessionVisibility* visibility
) XBL_NOEXCEPT;

/// <summary>
/// Get join restriction of the session associated with the search handle.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="joinRestriction">Passes back the join restriction of the associated session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetJoinRestriction(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ XblMultiplayerSessionRestriction* joinRestriction
) XBL_NOEXCEPT;

/// <summary>
/// Get whether or not the session associated with the search handle is temporarily closed for joining.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="closed">Passes back whether the session is closed or not.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetSessionClosed(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ bool* closed
) XBL_NOEXCEPT;

/// <summary>
/// Get the current and max number of members in the associated session.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="maxMembers">Passes back the max members allowed in session.</param>
/// <param name="currentMembers">Passes back the current number of members in session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetMemberCounts(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_opt_ size_t* maxMembers,
    _Out_opt_ size_t* currentMembers
) XBL_NOEXCEPT;

/// <summary>
/// Get the creation time of the search handle.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="creationTime">Passes back the time the search handle was created in MPSD (not the local object).</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetCreationTime(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ time_t* creationTime
) XBL_NOEXCEPT;

/// <summary>
/// Get the custom session properties for the associated session.
/// </summary>
/// <param name="handle">Handle to the search handle details.</param>
/// <param name="customPropertiesJson">Passes back the custom properties JSON string.  
/// The memory for the returned pointer will remain valid for the life of the XblMultiplayerSessionHandle object until it is closed.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSearchHandleGetCustomSessionPropertiesJson(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const char** customPropertiesJson
) XBL_NOEXCEPT;

/// <summary>
/// Writes a new or updated multiplayer session to the service.  
/// The session must have a valid session reference.  
/// If it was not created with one, use XblMultiplayerWriteSessionByHandleAsync instead.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="multiplayerSession">A MultiplayerSession object that has been modified with the changes to write.</param>
/// <param name="writeMode">The type of write operation.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// In the async callback, call XblMultiplayerWriteSessionResult() to get a XblMultiplayerSessionHandle handle.
/// Use that handle to call XblMultiplayerSessionWriteStatus() to get the write status.
/// The call to XblMultiplayerWriteSessionAsync() will only fail if the args passed to it are invalid or in very rare 
/// cases where it could not start the async task.
/// </remarks>
/// <rest>Calls V105 PUT /serviceconfigs/{serviceConfigurationId}/sessionTemplates/{sessiontemplateName}/sessions/{sessionName}</rest>
STDAPI XblMultiplayerWriteSessionAsync(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionHandle multiplayerSession,
    _In_ XblMultiplayerSessionWriteMode writeMode,
    _Inout_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of a XblMultiplayerWriteSessionAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="handle">Passes back a handle to a new instance of a local multiplayer session object.
/// The XblMultiplayerSessionHandle must be released by the caller by calling <see cref="XblMultiplayerSessionCloseHandle"/>.
/// Use XblMultiplayerSession* APIs to get session data from the handle.
/// If the updated session object is not needed, passing nullptr will cause the new multiplayer
/// session object to be cleaned up immediately.
/// </param>
/// <returns>HRESULT return code for this API operation.  
/// It will be a failure HRESULT if there was a network error or failure HTTP status code unless its a 412 (Precondition Failed).
/// A 412 returns success since the service also returns latest session state, so you must call XblMultiplayerSessionWriteStatus() to get the 
/// write status and call XblMultiplayerSession* APIs to get session data from the handle.
/// </returns>
/// <remarks>
/// Note that if you leave a session that you are the last member of and the sessionEmptyTimeout 
/// is equal to 0, then the session will be deleted immediately.
/// Call XblMultiplayerSessionWriteStatus() to get the write status.
/// </remarks>
STDAPI XblMultiplayerWriteSessionResult(
    _Inout_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandle* handle
) XBL_NOEXCEPT;

/// <summary>
/// Writes a new or updated multiplayer session to the service, using the specified handle to the session.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="multiplayerSession">A MultiplayerSession object that has been modified with the changes to write.</param>
/// <param name="writeMode">The type of write operation.</param>
/// <param name="handleId">The ID of the handle that should be used when writing the session.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// A handle is a service-side pointer to a session.  The handle ID is a GUID identifier of the handle.  
/// Callers will usually get the handleId either from another player's XblMultiplayerActivityDetails via 
/// the XblMultiplayerGetActivitiesForUsersAsync() API, or from an invite.  
///
/// Use this method only if your multiplayer session object doesn't have a valid XblMultiplayerSessionReference, as
/// a handle's lifetime may be shorter than that of the session it points to.
/// </remarks>
STDAPI XblMultiplayerWriteSessionByHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionHandle multiplayerSession,
    _In_ XblMultiplayerSessionWriteMode writeMode,
    _In_ const char* handleId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of a XblMultiplayerWriteSessionAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="handle">Passes back a handle to a new instance of a local multiplayer session object.  
/// It must be release by the caller with <see cref="XblMultiplayerSessionCloseHandle"/>.  
/// If the updated session object is not needed, passing nullptr will cause the new multiplayer session object to be cleaned up immediately.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Note that if you leave a session that you are the last member of and the sessionEmptyTimeout
/// is equal to 0, then the session will be deleted immediately and a nullptr will be returned.
/// </remarks>
STDAPI XblMultiplayerWriteSessionByHandleResult(
    _Inout_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandle* handle
) XBL_NOEXCEPT;

/// <summary>
/// Gets an existing session object with all its attributes from the server.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="sessionReference">A XblMultiplayerSessionReference object containing identifying information for the session.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <rest>Calls V102 GET /serviceconfigs/{serviceConfigurationId}/sessionTemplates/{sessiontemplateName}/sessions/{sessionName}</rest>
STDAPI XblMultiplayerGetSessionAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerSessionReference* sessionReference,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of an XblMultiplayerGetSessionResult call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="handle">Passes back a handle to a new instance of a local multiplayer session object.
/// It must be release by the caller with <see cref="XblMultiplayerSessionCloseHandle"/>.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If the session does not exist, this API will return __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND) and a null
/// XblMultiplayerSessionHandle.
/// </remarks>
STDAPI XblMultiplayerGetSessionResult(
    _In_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandle* handle
) XBL_NOEXCEPT;

/// <summary>
/// Gets a session object with all its attributes from the server, given a session handle id.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="handleId">A multiplayer handle id, which uniquely identifies the session.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// A handle is a service-side pointer to a session. The handleId is a GUID identifier of the handle.  
/// Callers will usually get the handleId either from another player's XblMultiplayerActivityDetails, or from an invite.
/// </remarks>
/// <rest>Calls GET /handles/{handleId}/session</rest>
STDAPI XblMultiplayerGetSessionByHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ const char* handleId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of an XblMultiplayerGetSessionByHandleAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="handle">Passes back a handle to a new instance of a local multiplayer session object.  
/// It must be release by the caller with <see cref="XblMultiplayerSessionCloseHandle"/>.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If the session does not exist, this API will return __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND) and a null
/// XblMultiplayerSessionHandle.
/// </remarks>
STDAPI XblMultiplayerGetSessionByHandleResult(
    _In_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandle* handle
) XBL_NOEXCEPT;

/// <summary>
/// Retrieve a list of sessions with various filters.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="sessionQuery">A session query object that defines the parameters for the query.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <rest>Calls V102 GET /serviceconfigs/{scid}/sessions or /serviceconfigs/{scid}/sessiontemplates/{session-template-name}/sessions</rest>
STDAPI XblMultiplayerQuerySessionsAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerSessionQuery* sessionQuery,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the number of sessions that matched a session query.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="sessionCount">Passes back the number of matching sessions.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerQuerySessionsResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* sessionCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets the results of a session query.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="sessionCount">The number of sessions that matched the query.
/// Use <see cref="XblMultiplayerQuerySessionsResultCount"/> to get the count required.</param>
/// <param name="sessions">A caller allocated array to pass back the session query results.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerQuerySessionsResult(
    _In_ XAsyncBlock* async,
    _In_ size_t sessionCount,
    _Out_writes_(sessionCount) XblMultiplayerSessionQueryResult* sessions
) XBL_NOEXCEPT;

/// <summary>
/// Sets the passed session as the user's current activity, which will be displayed in Xbox dashboard user experiences
/// (e.g. friends and gamercard) as associated with the currently running title.  
/// If the session is joinable, it may also be displayed as such in those user experiences.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="sessionReference">An XblMultiplayerSessionReference for the session of the activity.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSetActivityAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerSessionReference* sessionReference,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Clears the user's current activity session for the specified serviceConfigurationId.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="scid">The Service Configuration ID (SCID) in which to clear activity. The SCID is considered case sensitive so paste it directly from the Partner Center</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerClearActivityAsync(
    _In_ XblContextHandle xblContext,
    _In_z_ const char* scid,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// The access rights the caller has to the origin session are extended to the target session.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="targetSessionReference">Target XblMultiplayerSessionReference for the session you want to extend the access rights for.</param>
/// <param name="originSessionReference">Origin XblMultiplayerSessionReference for the session that grants access to the target session.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// For example, in a title with a lobby session and a game session, the title could put a transfer handle 
/// linking the lobby to the game inside the lobby session.  
/// Users invited to the lobby can use the handle to join the game session as well.  
/// The transfer handle is deleted once the target session is deleted.
/// </remarks>
STDAPI XblMultiplayerSetTransferHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionReference targetSessionReference,
    _In_ XblMultiplayerSessionReference originSessionReference,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of a XblMultiplayerSetTransferHandleAsync operation.  
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="handleId">Passes back the multiplayer session handle ID.  
/// This handle holds the unique ID for a MPSD transfer handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSetTransferHandleResult(
    _In_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandleId* handleId
) XBL_NOEXCEPT;

/// <summary>
/// Create a search handle associated with an existing session.  
/// This makes the session queryable.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="sessionRef">Session reference to create a search handle for.</param>
/// <param name="tags">Optional set of tags to attach to search handle.</param>
/// <param name="tagsCount">Count of tags.</param>
/// <param name="numberAttributes">Optional attributes to attach to search handle.</param>
/// <param name="numberAttributesCount">Count of number attributes.</param>
/// <param name="stringAttributes">Optional attributes to attach to search handle.</param>
/// <param name="stringAttributesCount">Count of string attributes.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerCreateSearchHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerSessionReference* sessionRef,
    _In_reads_opt_(tagsCount) const XblMultiplayerSessionTag* tags,
    _In_ size_t tagsCount,
    _In_reads_opt_(numberAttributesCount) const XblMultiplayerSessionNumberAttribute* numberAttributes,
    _In_ size_t numberAttributesCount,
    _In_reads_opt_(stringAttributesCount) const XblMultiplayerSessionStringAttribute* stringAttributes,
    _In_ size_t stringAttributesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the result from an XblMultiplayerCreateSearchHandleAsync.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="handle">Passes back a handle to the local search handle object.  
/// If this parameter is null, the local search handle object will be freed immediately.  
/// Note that this does not destroy the search handle on the MPSD service side, that must be done with XblMultiplayerDeleteSearchHandleAsync.  
/// If this is non-null, the handle must later be closed with <see cref="XblMultiplayerSearchHandleCloseHandle"/>.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerCreateSearchHandleResult(
    _In_ XAsyncBlock* async,
    _Out_opt_ XblMultiplayerSearchHandle* handle
) XBL_NOEXCEPT;

/// <summary>
/// Delete a search handle from the MPSD service.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="handleId">Id of the search handle to delete.  
/// Obtained from either the search handle or session handle.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Note that this will not destroy local search handle objects; the lifetime of those objects is still managed by XblMultiplayerSearchHandleCloseHandle.  
/// Once the search handle object is deleted from service, the associated session will no longer be searchable.
/// </remarks>
STDAPI XblMultiplayerDeleteSearchHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ const char* handleId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Search for sessions by their associated search handles.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="scid">The Service Configuration ID (SCID) within which to query for search handles. The SCID is considered case sensitive so paste it directly from the Partner Center</param>
/// <param name="sessionTemplateName">The name of the template to query for search handles.</param>
/// <param name="orderByAttribute">This specifies the attribute to sort the search handles by. Pass empty string to default to ordering by 'Timestamp asc'.</param>
/// <param name="orderAscending">Pass true to order ascending, false to order descending.</param>
/// <param name="searchFilter">The query string to get the search handles for.</param>
/// <param name="socialGroup">The social group to get the search handles for.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// **Filtering search handles:**  
/// 
/// The query syntax is an OData like syntax with only the following operators supported EQ, NE, GE, GT, LE and LT along with the logical operators of AND and OR.  
/// 
///     Example 1:
///     To filter for search handles for a specific XboxUserId use
///         "MemberXuids/any(d:d eq '12345678')" or "OwnerXuids/any(d:d eq '12345678')"
///     
///     Example 2:
///     To filter for search handles for a title defined string metadata use
///         "Strings/stringMetadataType eq 'value'"
///     
///     Example 3:
///     To filter for search handles for a title defined numbers metadata AND a tag type value use
///         "Numbers/numberMetadataType eq 53 AND Tags/tagType eq 'value'"
/// 
/// **Empty filter and social group:**
/// 
/// Since searchFilter and socialGroup are optional, please make sure to pass in a nullptr if they aren't needed/used. Passing in an empty string "" will **not** work.
/// </remarks>
STDAPI XblMultiplayerGetSearchHandlesAsync(
    _In_ XblContextHandle xblContext,
    _In_z_ const char* scid,
    _In_z_ const char* sessionTemplateName,
    _In_opt_z_ const char* orderByAttribute,
    _In_ bool orderAscending,
    _In_opt_z_ const char* searchFilter,
    _In_opt_z_ const char* socialGroup,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Get the number of search handles returned from an XblMultiplayerGetSearchHandlesAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="searchHandleCount">Passes back the number of search results.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetSearchHandlesResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* searchHandleCount
) XBL_NOEXCEPT;

/// <summary>
/// Get handles to the local search handle objects returned from XblMultiplayerGetSearchHandlesAsync call.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="searchHandles">A caller allocated array to pass back the search handle results.  
/// Each handle must later be closed with <see cref="XblMultiplayerSearchHandleCloseHandle"/>.</param>
/// <param name="searchHandlesCount">Size of search handles array.  
/// Use <see cref="XblMultiplayerGetSearchHandlesResultCount"/> to get the count required.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetSearchHandlesResult(
    _In_ XAsyncBlock* async,
    _Out_writes_(searchHandlesCount) XblMultiplayerSearchHandle* searchHandles,
    _In_ size_t searchHandlesCount
) XBL_NOEXCEPT;

/// <summary>
/// A handle that corresponds to an outstanding invite that has been sent.
/// </summary>
typedef struct XblMultiplayerInviteHandle
{
    /// <summary>
    /// The outstanding invite GUID.
    /// </summary>
    _Null_terminated_ char Data[XBL_GUID_LENGTH];
} XblMultiplayerInviteHandle;

/// <summary>
/// Invites the specified users to a session.  
/// This will result in a notification being shown to each invited user using standard invite text.  
/// If a user accepts that notification the title will be activated.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="sessionReference">An XblMultiplayerSessionReference object representing the session the target users will be invited to.</param>
/// <param name="xuids">The list of xbox user IDs who will be invited.</param>
/// <param name="xuidsCount">Size of the xuids array.</param>
/// <param name="titleId">The ID of the title that the invited user will activate in order to join the session.</param>
/// <param name="contextStringId">The custom context string ID.  
/// This string ID is defined during Xbox Live ingestion to identify the invitation text that is additional to the standard invitation text.  
/// The ID string must be prefixed with "///".  
/// Pass nullptr if you don't want a custom string added to the invite.</param>
/// <param name="customActivationContext">The activation context string. A game defined string that is passed to the invited game client and interpreted as desired. (Optional)</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSendInvitesAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerSessionReference* sessionReference,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_ uint32_t titleId,
    _In_opt_z_ const char* contextStringId,
    _In_opt_z_ const char* customActivationContext,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Invites the specified users to a session.  
/// This will result in a notification being shown to each invited user using standard invite text.  
/// If a user accepts that notification the title will be activated.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="handlesCount">The number of handles in the handles array. Size should be equal to the number of invites requested.</param>
/// <param name="handles">A caller allocated array to pass back the invite handle results.
/// The handle ID strings corresponding to the invites that have been sent.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerSendInvitesResult(
    _In_ XAsyncBlock* async,
    _In_ size_t handlesCount,
    _Out_writes_(handlesCount) XblMultiplayerInviteHandle* handles
) XBL_NOEXCEPT;

/// <summary>
/// DEPRECATED - Call <see cref="XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync"/>, which also 
/// populates `CustomSessionProperties` in the result.<br/>  
/// Queries for the current activity for a social group of players associated with a particular "owner" player.  
/// </summary>
/// <param name="xboxLiveContext">{% term xbox-live %} context for the local player.</param>
/// <param name="scid">The service configuration identifier (SCID) within which to query for activities. 
/// The SCID is case-sensitive, so paste it directly from Partner Center.</param>
/// <param name="socialGroupOwnerXuid">The player whose social group will be used for the query.</param>
/// <param name="socialGroup">The social group (such as "people" or "favorites") to use to get the list of users.</param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// No paging or continuation is available. The Multiplayer service limits the number of items returned to 100.
/// </remarks>
STDAPI XblMultiplayerGetActivitiesForSocialGroupAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char* scid,
    _In_ uint64_t socialGroupOwnerXuid,
    _In_ const char* socialGroup,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Queries for the current activity for a social group of players associated with a particular "owner" player.  
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local player.</param>
/// <param name="scid">The service configuration identifier (SCID) within which to query for activities. 
/// The SCID is case-sensitive, so paste it directly from Partner Center.</param>
/// <param name="socialGroupOwnerXuid">The player whose social group will be used for the query.</param>
/// <param name="socialGroup">The social group (such as "people" or "favorites") to use to get the list of users.</param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// No paging or continuation is available. The Multiplayer service limits the number of items returned to 100.
/// </remarks>
STDAPI XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync(
    _In_ XblContextHandle xblContext,
    _In_ const char* scid,
    _In_ uint64_t socialGroupOwnerXuid,
    _In_ const char* socialGroup,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the number of <see cref="XblMultiplayerActivityDetails"/> objects returned.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="activityCount">The number of activity objects returned.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetActivitiesForSocialGroupResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* activityCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of a call to <see cref="XblMultiplayerGetActivitiesForSocialGroupAsync"/>.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="activityCount">The number of activity objects returned.</param>
/// <param name="activities">A caller-allocated array for the activity objects returned.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetActivitiesForSocialGroupResult(
    _In_ XAsyncBlock* async,
    _In_ size_t activityCount,
    _Out_writes_(activityCount) XblMultiplayerActivityDetails* activities
) XBL_NOEXCEPT;

/// <summary>
/// Gets the size of <see cref="XblMultiplayerActivityDetails"/> objects returned.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the multiplayer activity details.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of a call to <see cref="XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync"/>.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="bufferSize">The size of the provided `buffer`.</param>
/// <param name="buffer">A caller-allocated byte buffer to write results into.</param>
/// <param name="ptrToBuffer">Strongly typed pointer to `buffer`.
/// Do not free; its lifecycle is tied to `buffer`.</param>
/// <param name="ptrToBufferCount">Number of entries in `buffer`.</param>
/// <param name="bufferUsed">Number of bytes actually written to `buffer`.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMultiplayerActivityDetails** ptrToBuffer,
    _Out_ size_t* ptrToBufferCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// DEPRECATED - Call <see cref="XblMultiplayerGetActivitiesWithPropertiesForUsersAsync"/>, which also 
/// populates `CustomSessionProperties` in the result.<br/>  
/// Queries for the current activity for a set of players specified by Xbox user ID.  
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local user.</param>
/// <param name="scid">The service configuration identifier (SCID) within which to query for activities. 
/// The SCID is case-sensitive, so paste it directly from Partner Center.</param>
/// <param name="xuids">The list of Xbox user IDs to find activities for.</param>
/// <param name="xuidsCount">The size of the `xuids` array.</param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// No paging or continuation is available. The Multiplayer service limits the number of items returned to 100.
/// </remarks>
STDAPI XblMultiplayerGetActivitiesForUsersAsync(
    _In_ XblContextHandle xblContext,
    _In_ const char* scid,
    _In_reads_(xuidsCount) const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Queries for the current activity for a set of players specified by Xbox user ID.  
/// </summary>
/// <param name="xblContext">{% term xbox-live %} context for the local user.</param>
/// <param name="scid">The service configuration identifier (SCID) within which to query for activities. 
/// The SCID is case-sensitive, so paste it directly from Partner Center.</param>
/// <param name="xuids">The list of Xbox user IDs to find activities for.</param>
/// <param name="xuidsCount">The size of the `xuids` array.</param>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// No paging or continuation is available. The Multiplayer service limits the number of items returned to 100.
/// </remarks>
STDAPI XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(
    _In_ XblContextHandle xblContext,
    _In_ const char* scid,
    _In_reads_(xuidsCount) const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the number of <see cref="XblMultiplayerActivityDetails"/> objects returned.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="activityCount">The number of activity objects returned.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetActivitiesForUsersResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* activityCount
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of a call to <see cref="XblMultiplayerGetActivitiesForUsersAsync"/>.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="activityCount">The number of activity objects returned.</param>
/// <param name="activities">A caller-allocated array for the activity objects returned.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetActivitiesForUsersResult(
    _In_ XAsyncBlock* async,
    _In_ size_t activityCount,
    _Out_writes_(activityCount) XblMultiplayerActivityDetails* activities
) XBL_NOEXCEPT;

/// <summary>
/// Gets the size of <see cref="XblMultiplayerActivityDetails"/> objects returned.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="resultSizeInBytes">Returns the size in bytes required to store the result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetActivitiesWithPropertiesForUsersResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Gets the result of a call to <see cref="XblMultiplayerGetActivitiesWithPropertiesForUsersAsync"/>.
/// </summary>
/// <param name="async">The `XAsyncBlock` for this operation.</param>
/// <param name="bufferSize">The size of the provided `buffer`.</param>
/// <param name="buffer">A caller-allocated byte buffer to write results into.</param>
/// <param name="ptrToBuffer">Strongly typed pointer to `buffer`.
/// Do not free; its lifecycle is tied to `buffer`.</param>
/// <param name="ptrToBufferCount">Number of entries in `buffer`.</param>
/// <param name="bufferUsed">Number of bytes actually written to `buffer`.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerGetActivitiesWithPropertiesForUsersResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMultiplayerActivityDetails** ptrToBuffer,
    _Out_ size_t* ptrToBufferCount,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Explicitly starts or stops the multiplayer service connectivity via RTA. Enabling the RTA subscription enables:
/// 1. Callbacks when the local Users's sessions change, using the MultiplayerSession object. Handlers are added
/// with <see cref="XblMultiplayerAddSessionChangedHandler"/>.
/// 2. Automatic removal of members from sessions when the RTA connection underlying this multiplayer subscription is broken.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="subscriptionsEnabled">True to enable subscriptions and false to stop them.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This method immediately enables the RTA connection, but the in order to receive session changed callbacks, the session
/// must be written again after enabling subscriptions.
/// </remarks>
STDAPI XblMultiplayerSetSubscriptionsEnabled(
    _In_ XblContextHandle xblContext,
    _In_ bool subscriptionsEnabled
) XBL_NOEXCEPT;

/// <summary>
/// Indicates whether multiplayer subscriptions are currently enabled. Note that subscriptions can be enabled/disabled
/// explicitly using <see cref="XblMultiplayerSetSubscriptionsEnabled"/>, but they will also be enabled automatically
/// if a session changed handler is added.
/// </summary>
/// <param name="xblHandle">Xbox live context for the local user.</param>
/// <returns>Returns true if enabled, false if not enabled.</returns>
STDAPI_(bool) XblMultiplayerSubscriptionsEnabled(
    _In_ XblContextHandle xblHandle
) XBL_NOEXCEPT;

/// <summary>
/// A callback method to be called when a session changes.
/// </summary>
/// <param name="context">Caller context to be passed the handler.</param>
/// <param name="args">Arguments to be passed the handler.</param>
/// <returns></returns>
typedef void CALLBACK XblMultiplayerSessionChangedHandler(
    _In_opt_ void* context,
    _In_ XblMultiplayerSessionChangeEventArgs args
);

/// <summary>
/// Registers an event handler for notifications when a multiplayer session changes. If the RTA subscription has not
/// been explicitly enabled with <see cref="XblMultiplayerSetSubscriptionsEnabled" />, adding session changed handlers will
/// enable it automatically. Use the returned XblFunctionContext to unregister the handler.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns>The function context token that was registered for the event.</returns>
STDAPI_(XblFunctionContext) XblMultiplayerAddSessionChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionChangedHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for multiplayer session change notifications.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerRemoveSessionChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

/// <summary>
/// A callback method to be called when a rta subscription is lost.
/// </summary>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns></returns>
typedef void CALLBACK XblMultiplayerSessionSubscriptionLostHandler(
    _In_opt_ void* context
);

/// <summary>
/// Registers an event handler for notifications when a multiplayer subscription is lost.  
/// Use the returned XblFunctionContext to unregister the handler.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns>The function context token that was registered for the event.</returns>
STDAPI_(XblFunctionContext) XblMultiplayerAddSubscriptionLostHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerSessionSubscriptionLostHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for multiplayer subscription lost notifications.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerRemoveSubscriptionLostHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

/// <summary>
/// A callback method to be called when the multiplayer connection id changes.
/// </summary>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns></returns>
typedef void CALLBACK XblMultiplayerConnectionIdChangedHandler(
    _In_opt_ void* context
);

/// <summary>
/// Registers an event handler for notifications when the multiplayer connection id changes.  
/// Use the returned XblFunctionContext to unregister the handler.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context to be passed the handler.</param>
/// <returns>The function context token that was registered for the event.</returns>
STDAPI_(XblFunctionContext) XblMultiplayerAddConnectionIdChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblMultiplayerConnectionIdChangedHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for multiplayer connection id changed notifications.
/// </summary>
/// <param name="xblContext">Xbox live context for the local user.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerRemoveConnectionIdChangedHandler(
    _In_ XblContextHandle xblContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

}