// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if !XSAPI_NO_PPL

#include "xsapi-cpp/real_time_activity.h"
#include "xsapi-c/multiplayer_c.h"

namespace xbox { namespace services {
    class xbox_live_context;

    namespace social {
        class reputation_service;
    }
}}

namespace xbox { namespace services { 
    /// <summary>
    /// Contains classes and enumerations for managing a multiplayer session.
    /// </summary>
    namespace multiplayer {

/// <summary>
/// Defines values used to indicate status for visibility or accessibility of a session.
/// </summary>
enum class multiplayer_session_visibility
{
    /// <summary> Unknown </summary>
    unknown,

    /// <summary> Ignore the SessionVisibility filter.</summary>
    any,

    /// <summary>
    /// The session is private and it's not visible to other users who
    /// aren't in the session. Joining a visible or private session is a HTTP_E_STATUS_FORBIDDEN.
    /// </summary>
    private_session,

    /// <summary>
    /// The session is visible to other users who aren't in the session, but the session is read-only to them and they can't join.
    /// Joining an visible causes the service to return HTTP_E_STATUS_BAD_REQUEST (403).
    /// </summary>
    visible,

    /// <summary>
    /// The session is full and cannot be joined by anyone.  
    /// Joining an open but full session causes the service to return HTTP_E_STATUS_BAD_REQUEST (400).
    /// </summary>
    full,

    /// <summary> The session is open and can be joined by anyone. </summary>
    open
};

/// <summary>
/// Defines values used to indicate status for the initialization stage of a session during managed initialization.
/// </summary>
enum class multiplayer_initialization_stage
{
    /// <summary> Unknown </summary>
    unknown,

    /// <summary> Initialization stage not set.</summary>
    none,

    /// <summary> 
    /// Joining initialization stage.  
    /// Typically matchmaking creates session and puts users into it.  
    /// SPC has up to the joining timeout to join the session during this phase. 
    /// </summary>
    joining,

    /// <summary>
    /// Measuring initialization stage.  Stage where QoS measurement happens.  
    /// If the title is manually managing QoS, then title will do this stage.  
    /// Otherwise the Party system will do this when calling RegisterGameSession or RegisterMatchSession.
    /// </summary>
    measuring,

    /// <summary>
    /// Evaluating initialization stage.
    /// If auto evaluate is true, then this stage is skipped.  
    /// Otherwise the title will do its own evaluation.  
    /// This stage is applied even with the SPC is managing QoS.
    /// </summary>
    evaluating,

    /// <summary>
    /// Failed initialization stage.
    /// If episode 1 didn't succeed, then goes into failed permanently.
    /// </summary>
    failed
};

/// <summary>
/// Defines values used to indicate the type of metric used to measure matchmaking QoS for a session.
/// </summary>
enum class multiplay_metrics
{
    /// <summary> Unknown metric </summary>
    unknown,

    /// <summary> Bandwidth host selection metric </summary>
    bandwidth_up,

    /// <summary> Bandwidth down host selection metric </summary>
    bandwidth_down,

    /// <summary> Bandwidth host selection metric </summary>
    bandwidth,

    /// <summary> Latency host selection metric </summary>
    latency
};

/// <summary>
/// Defines values used to indicate the current network address translation (NAT) settings for a console connecting to Xbox Live.
/// </summary>
enum class network_address_translation_setting
{
    /// <summary> The server returned an unrecognized response. </summary>
    unknown,

    /// <summary> Can connect with any other consoles regardless of their NAT setting. </summary>
    open,

    /// <summary> Consoles using Moderate NAT settings can only connect with other consoles using Moderate or Open settings. </summary>
    moderate,

    /// <summary> Consoles using Strict NAT settings can only connect with other consoles using Open NAT settings. </summary>
    strict
};

/// <summary>
/// Defines values used to indicate types measurement failures for a session member on the network.
/// </summary>
enum class multiplayer_measurement_failure
{
    /// <summary> Unknown measurement failure </summary>
    unknown,

    /// <summary> This player has no measurement failure. </summary>
    none,

    /// <summary> This player failed because timeout measurement test failed. </summary>
    timeout,

    /// <summary> This player failed because latency measurement test failed. </summary>
    latency,

    /// <summary> This player failed because bandwidth up measurement test failed. </summary>
    bandwidth_up,

    /// <summary> This player failed because bandwidth down measurement test failed. </summary>
    bandwidth_down,

    /// <summary> This player failed cause someone failed in their group failed. </summary>
    group,

    /// <summary> This player failed due to a network error such as the user was unreachable. </summary>
    network,

    /// <summary> This player failed because your episode failed.   This likely happened because there wasn't enough users in the session. </summary>
    episode
};

/// <summary>
/// Defines values used to indicate current status values for a session.
/// </summary>
enum class multiplayer_session_status
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    unknown,

    /// <summary>
    /// The session is active and there is at least one user.
    /// </summary>
    active,

    /// <summary>
    /// The session is inactive. This means no users in the session are
    /// active or all users left the session.
    /// </summary>
    inactive,

    /// <summary>
    /// The session is reserved. This means one for more users have not
    /// accepted the session invite.
    /// </summary>
    reserved
};

/// <summary>
/// Defines values used to indicate restrictions on the users who can join a session.
/// </summary>
enum class multiplayer_session_restriction
{
    /// <summary> The unrecognized restriction type. </summary>
    unknown,

    /// <summary> Default value, no restriction </summary>
    none,

    /// <summary> If "local", only users whose token's DeviceId matches someone else already in the session and "active": true. </summary>
    local,

    /// <summary> If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can join without a reservation. </summary>
    followed
};

/// <summary>
/// Defines values used to indicate status for a matchmaking request for a session.
/// </summary>
enum class matchmaking_status
{
    /// <summary> The server returned an unrecognized response. </summary>
    unknown,

    /// <summary> Indicates the matchmaking search is not specified.  This status is optional and requires the clientMatchmaking capability. </summary>
    none,

    /// <summary> Indicates the matchmaking search is still searching. </summary>
    searching,

    /// <summary> Indicates the matchmaking search has expired. </summary>
    expired,

    /// <summary> Indicates the matchmaking search has found a session. </summary>
    found,

    /// <summary> Indicates the matchmaking search has been canceled. </summary>
    canceled
};

/// <summary>
/// Defines values used to indicate status for member of a session.
/// </summary>
enum class multiplayer_session_member_status
{
    /// <summary>
    /// Member is reserved for a specific Xbox User ID.  
    /// This specific member must join the session to fill the reservation.  
    /// If a reserved member doesn't join before the JoinTimeout they will be removed.
    /// </summary>
    reserved,

    /// <summary>
    /// The member is inactive in the current title.  
    /// The member may be active in another title as specified by ActiveTitleId.
    /// If an inactive member doesn't mark themselves as Active within the MemberInactiveTimeout they will be removed from the session.
    /// </summary>
    inactive,

    /// <summary>
    /// When the shell launches the title to start a multiplayer game, the member is marked as ready.
    /// If a ready member doesn't mark themselves as Active within the MemberReadyTimeout they will be marked as inactive.
    /// </summary>
    ready,

    /// <summary> The member is active in the current title. </summary>
    active
};

/// <summary>
/// Defines values used to indicate the mode used when creating or writing to a new Multiplayer service session.
/// </summary>
enum class multiplayer_session_write_mode
{
    /// <summary>
    /// Create a new multiplayer session.  Fails if the session already exists.
    /// </summary>
    create_new,
    
    /// <summary>
    /// Either update or create a new session. Doesn't care whether the session exists.
    /// </summary>
    update_or_create_new,
    
    /// <summary>
    /// Updates an existing multiplayer session; fails if the session doesn't exist.  
    /// </summary>
    update_existing,
    
    /// <summary>
    /// Updates an existing multiplayer session.  Fails with HTTP_E_STATUS_PRECOND_FAILED (HTTP status 412) if eTag on local session doesn't match eTag on server. 
    /// Fails if the session does not exist.
    /// </summary>
    synchronized_update,
};

enum class write_session_status
{
    /// <summary>
    /// Unknown write result
    /// </summary>
    unknown,

    /// <summary>
    /// HTTP Result 403- User does not have proper permission to write a session
    /// </summary>
    access_denied,

    /// <summary>
    /// HTTP Result 201- Write created session successfully
    /// </summary>
    created,

    /// <summary>
    /// HTTP Result 409- Conflict occurred during write about session document
    /// </summary>
    conflict,

    /// <summary>
    /// HTTP Result 404- Session not found
    /// </summary>
    handle_not_found,

    /// <summary>
    /// HTTP Result 412- Session document is not the most recent
    /// </summary>
    out_of_sync,

    /// <summary>
    /// HTTP Result 204- Session deleted successfully
    /// </summary>
    session_deleted,

    /// <summary>
    /// HTTP Result 200- Session updated successfully
    /// </summary>
    updated
};

/// <summary>
/// Defines values used to indicate change types for a multiplayer session.
/// </summary>
enum multiplayer_session_change_types
{
    /// <summary>
    /// None 
    /// </summary>
    none = 0x0000,

    /// <summary>
    /// Changes to anything in the session.
    /// </summary>
    everything = 0x0001,

    /// <summary>
    /// Changes to the host device token.
    /// </summary>
    host_device_token_change = 0x0002,

    /// <summary>
    /// Changes to the stage of initialization has changed.
    /// </summary>
    initialization_state_change = 0x0004,

    /// <summary>
    /// Changes to the matchmaking status (e.g. match found or expired)
    /// </summary>
    matchmaking_status_change = 0x0008,

    /// <summary>
    /// A member joined the session
    /// </summary>
    member_list_change = 0x0010,

    /// <summary>
    /// A member left the session
    /// </summary>
    member_status_change = 0x0020,

    /// <summary>
    /// Changes to the joinability of the session.
    /// </summary>
    session_joinability_change = 0x0040,

    /// <summary>
    /// Changes within properties/custom
    /// </summary>
    custom_property_change = 0x0080,

    /// <summary>
    /// Changes within member/properties/custom, for any of the members.
    /// </summary>
    member_custom_property_change = 0x0100,
};

/// <summary>
/// Defines values used to indicate mutable_role_setting types for a multiplayer role.
/// Note: Only the session owner can modify role settings and only those that are set as multiplayer_role_type::mutable_role_settings().
/// The mutable_role_settings can be set in the session template.
/// </summary>
enum class mutable_role_setting
{
    /// <summary>
    /// Allows you to set a max count for the multiplayer role
    /// </summary>
    max,

    /// <summary>
    /// Allows you to set a target count for the multiplayer role
    /// </summary>
    target
};

/// <summary>
/// Represents requirements that apply to each connection between a host candidate and session members.
/// </summary>
class multiplayer_peer_to_host_requirements
{
public:
    /// <summary>
    /// The maximum latency for the peer to host connection.
    /// </summary>
    inline std::chrono::milliseconds latency_maximum() const;

    /// <summary>
    /// The minimum bandwidth down in kilobits per second for the peer to host connection.
    /// </summary>
    inline uint64_t bandwidth_down_minimum_in_kilobits_per_second() const;

    /// <summary>
    /// The minimum bandwidth up in kilobits per second for the peer to host connection.
    /// </summary>
    inline uint64_t bandwidth_up_minimum_in_kilobits_per_second() const;

    /// <summary>
    /// Indicates which metric was used to select the host.
    /// </summary>
    inline multiplay_metrics host_selection_metric() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_peer_to_host_requirements(_In_ const XblMultiplayerPeerToHostRequirements& requirements);

private:
    XblMultiplayerPeerToHostRequirements m_requirements;
};

/// <summary>
/// Represents requirements for a connection between session members.
/// </summary>
class multiplayer_peer_to_peer_requirements
{
public:
    /// <summary>
    /// The maximum latency for the peer to peer connection.
    /// </summary>
    inline uint64_t bandwidth_minimum_in_kilobits_per_second() const;

    /// <summary>
    /// The minimum bandwidth in kilobits per second for the peer to peer connection.
    /// </summary>
    inline std::chrono::milliseconds latency_maximum() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_peer_to_peer_requirements(_In_ const XblMultiplayerPeerToPeerRequirements& requirements);
private:
    XblMultiplayerPeerToPeerRequirements m_requirements;
};

/// <summary>
/// Used to configure requirements and initialize a new Multiplayer session.
/// </summary>
class multiplayer_member_initialization
{
public:
    /// <summary>
    /// Indicates if the ManagedInitializion object is set.
    /// </summary>
    inline bool member_initialization_set() const;

    /// <summary>
    /// Returns the timeout for the first stage of the QoS process which is the joining stage. 
    /// </summary>
    inline std::chrono::milliseconds join_timeout() const;

    /// <summary>
    /// Returns the timeout for the measurement stage of the QoS process. 
    /// </summary>
    inline std::chrono::milliseconds measurement_timeout() const;

    /// <summary>
    /// Returns the timeout for the evaluation stage of the QoS process. 
    /// </summary>
    inline std::chrono::milliseconds evaluation_timeout() const;

    /// <summary>
    /// This is an optional evaluate stage for title.  The title can do evaluation when set to true.
    /// </summary>
    inline bool external_evaluation() const;

    /// <summary>
    /// Defaults to 2. Must be between 1 and maxMemberCount. Only applies to initialization episode zero.
    /// </summary>
    inline uint32_t members_need_to_start() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_member_initialization(_In_opt_ const XblMultiplayerMemberInitialization* memberInitialization);

private:
    bool m_initializationSet{ false };
    XblMultiplayerMemberInitialization m_memberInitialization{};
};

/// <summary>
/// Represents the capabilities of a multiplayer session.
/// </summary>
/// <remarks>
/// Session capabilities are boolean values that are optionally set in the session template. If no capabilities are needed, an empty SessionCapabilities object should be in the template to prevent capabilities from being specified on session creation, unless the title requires dynamic session capabilities.
/// </remarks>
class multiplayer_session_capabilities
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    inline multiplayer_session_capabilities();

    /// <summary>
    /// If false, the session can't enable any metrics and the session members can not set their SecureDeviceAddress.  
    /// </summary>
    inline bool connectivity() const;

    /// <summary>
    /// If false, the session can't enable any metrics and the session members can not set their SecureDeviceAddress.  
    /// </summary>
    inline void set_connectivity(_In_ bool connectivity);

    /// <summary>
    /// By default (if false), active users are required to remain online playing the title, otherwise they get demoted to 
    /// inactive status. Setting this flag to true disables this check so that members stay active indefinitely.
    /// </summary>
    inline bool suppress_presence_activity_check() const;

    /// <summary>
    /// By default (if false), active users are required to remain online playing the title, otherwise they get demoted to 
    /// inactive status. Setting this flag to true disables this check so that members stay active indefinitely.
    /// </summary>
    inline void set_suppress_presence_activity_check(_In_ bool suppressPresenceActivityCheck);

    /// <summary>
    /// Indicates whether the session represents actual gameplay, as opposed to setup/menu time like a lobby or matchmaking.
    /// If true, then the session is in gameplay mode.
    /// </summary>
    inline bool gameplay() const;

    /// <summary>
    /// Indicates whether the session represents actual gameplay, as opposed to setup/menu time like a lobby or matchmaking.
    /// If true, then the session is in gameplay mode.
    /// </summary>
    inline void set_gameplay(_In_ bool gameplay);

    /// <summary>
    /// If true, this session can host a large number of users, which has impact on other session properties (see documentation)
    /// </summary>
    inline bool large() const;

    /// <summary>
    /// If true, this session can host a large number of users, which has impact on other session properties (see documentation)
    /// </summary>
    inline void set_large(_In_ bool large);

    /// <summary>
    /// If true, this connection is required to have a member be active (see documentation)
    /// </summary>
    inline bool connection_required_for_active_members() const;

    /// <summary>
    /// If true, this connection is required to have a member be active (see documentation)
    /// </summary>
    inline void set_connection_required_for_active_members(_In_ bool connectionRequired);

    /// <summary>
    /// Session supports calls from platforms without strong title identity. This capability can't be set on large sessions.
    /// </summary>
    inline bool user_authorization_style() const;

    /// <summary>
    /// Session supports calls from platforms without strong title identity. This capability can't be set on large sessions.
    /// </summary>
    inline void set_user_authorization_style(_In_ bool userAuthorizationStyle);

    /// <summary>
    /// Session supports cross play between PC and Xbox
    /// </summary>
    inline bool crossplay() const;

    /// <summary>
    /// Session supports cross play between PC and Xbox
    /// </summary>
    inline void set_crossplay(_In_ bool crossplay);

    /// <summary>
    /// True, if the session can be linked to a search handle for searching.
    /// </summary>
    inline bool searchable() const;

    /// <summary>
    /// Allows the session to be linked to a search handle for searching.
    /// </summary>
    inline void set_searchable(_In_ bool searchable);

    /// <summary>
    /// True, if the session has owners. If you have user_authorization_style set, then in order to be searchable, you must have owners set.
    /// </summary>
    inline bool has_owners() const;

    /// <summary>
    /// If you have user_authorization_style set, then in order to be searchable, you must have owners set.
    /// </summary>
    inline void set_has_owners(_In_ bool hasOwners);

private:
    XblMultiplayerSessionCapabilities m_capabilities;
    friend class multiplayer_session;
};

/// <summary>
/// Represents matchmaking quality of service (QoS) measurements for the network used by a session member.
/// </summary>
class multiplayer_quality_of_service_measurements
{
public:
    /// <summary>
    /// Creates a new MultiplayerQualityOfServiceMeasurements object.
    /// </summary>
    /// <param name="memberDeviceToken">The device token of the member that this measurement is for.</param>
    /// <param name="latency">The time of the latency measurement.</param>
    /// <param name="bandwidthDownInKilobitsPerSecond">The bandwidth down in kilobits per second.</param>
    /// <param name="bandwidthUpInKilobitsPerSecond">The bandwidth up in kilobits per second.</param>
    /// <param name="customJson">JSON string that specify the custom properties.</param>
    inline multiplayer_quality_of_service_measurements(
        _In_ const string_t& memberDeviceToken,
        _In_ std::chrono::milliseconds latency,
        _In_ uint64_t bandwidthDownInKilobitsPerSecond,
        _In_ uint64_t bandwidthUpInKilobitsPerSecond,
        _In_ const string_t& customJson
    );

    /// <summary>
    /// The device token of the member that this measurement is for.
    /// </summary>
    inline const string_t& member_device_token() const;

    /// <summary>
    /// The time of the latency measurement.
    /// </summary>
    inline const std::chrono::milliseconds& latency() const;
    
    /// <summary>
    /// The bandwidth down in kilobits per second.
    /// </summary>
    inline uint64_t bandwidth_down_in_kilobits_per_second() const;

    /// <summary>
    /// The bandwidth up in kilobits per second.
    /// </summary>
    inline uint64_t bandwidth_up_in_kilobits_per_second() const;

    /// <summary>
    /// JSON string that specify the custom properties.
    /// </summary>
    inline const web::json::value& custom_json() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_quality_of_service_measurements(
        _In_ const string_t& memberDeviceToken,
        _In_ const web::json::value& measurementsJson
    );

private:
    string_t m_memberDeviceToken;
    std::chrono::milliseconds m_latency;
    uint64_t m_bandwidthDown;
    uint64_t m_bandwidthUp;
    web::json::value m_customJson;
    web::json::value m_measurementsJson;

    friend class multiplayer_session;
};

/// <summary>
/// Represents constant values for a multiplayer session.
/// </summary>
class multiplayer_session_constants
{
public:
    /// <summary>
    /// The maximum number of members in this session.
    /// </summary>
    inline uint32_t max_members_in_session() const;

    /// <summary>
    /// The maximum number of members in this session.
    /// </summary>
    inline void set_max_members_in_session(_In_ uint32_t maxMembersInSession);

    /// <summary>
    /// The visibility of this session.
    /// </summary>
    inline multiplayer_session_visibility visibility() const;

    /// <summary>
    /// The visibility of this session.
    /// </summary>
    inline void set_visibility(_In_ multiplayer_session_visibility visibility);
    
    /// <summary>
    /// A collection of Xbox User IDs indicating who initiated the session. (Optional)
    /// </summary>
    inline std::vector<string_t> initiator_xbox_user_ids() const;
    
    /// <summary>
    /// JSON string that specify the custom constants for the session.  These can not be changed after the session is created. (Optional)
    /// </summary>
    inline web::json::value session_custom_constants_json() const;

    /// <summary>
    /// JSON string that specify the cloud compute package constants for the session.  These can not be changed after the session is created. (Optional)
    /// </summary>
    inline web::json::value session_cloud_compute_package_constants_json() const;

    /// <summary>
    /// If a member reservation does not join within this timeout, then reservation is removed.
    /// </summary>
    inline std::chrono::milliseconds member_reserved_time_out() const;
    
    /// <summary>
    /// If an inactive member reservation does not become active within this timeout, then the inactive member is removed from the session.
    /// </summary>
    inline std::chrono::milliseconds member_inactive_timeout() const;
    
    /// <summary>
    /// If a member who is marked as ready doesn't mark themselves as active within this timeout, then member becomes inactive.
    /// When the shell launches the title to start a multiplayer game, the member is marked as ready.
    /// </summary>
    inline std::chrono::milliseconds member_ready_timeout() const;
    
    /// <summary>
    /// If the session is empty for this timeout, then the session is deleted.
    /// </summary>
    inline std::chrono::milliseconds session_empty_timeout() const;

    /// <summary>
    /// Indicates if the title wants latency measured for determining connectivity
    /// Requires CapabilitiesConnectivity capability.
    /// </summary>
    inline bool enable_metrics_latency() const;

    /// <summary>
    /// Indicates if the title wants bandwidth down measured for determining connectivity
    /// Requires CapabilitiesConnectivity capability.
    /// </summary>
    inline bool enable_metrics_bandwidth_down() const;

    /// <summary>
    /// Indicates if the title wants bandwidth up measured for determining connectivity
    /// Requires CapabilitiesConnectivity capability.
    /// </summary>
    inline bool enable_metrics_bandwidth_up() const;
    
    /// <summary>
    /// Indicates if the title wants a custom measurement measured for determining connectivity
    /// Requires CapabilitiesConnectivity capability.
    /// </summary>
    inline bool enable_metrics_custom() const;

    /// <summary>
    /// If a MemberInitialization object is set, the session expects the client system or title to perform
    /// initialization following session creation. The timeouts and initialization stages are automatically tracked by
    /// the session, including initial QoS if any metrics are set.
    /// </summary>
    inline multiplayer_member_initialization member_initialization() const;

    /// <summary>
    /// Peer to peer QoS requirements
    /// </summary>
    inline multiplayer_peer_to_peer_requirements peer_to_peer_requirements() const;
    
    /// <summary>
    /// Peer to host QoS requirements
    /// </summary>
    inline multiplayer_peer_to_host_requirements peer_to_host_requirements() const;
    
    /// <summary>
    /// The set of potential server connection strings that should be evaluated.
    /// </summary>
    inline web::json::value measurement_server_addresses_json() const;

    /// <summary>
    /// Indicates whether the matchmaking status fields can be written to.
    /// </summary>
    inline bool client_matchmaking_capable() const;

    /// <summary>
    /// If false, the session can't enable any metrics and the session members can not set their SecureDeviceAddress.  
    /// </summary>
    inline bool capabilities_connectivity() const;

    /// <summary>
    /// By default (if false), active users are required to remain online playing the title, otherwise they get demoted to 
    /// inactive status. Setting this flag to true disables this check so that members stay active indefinitely.
    /// </summary>
    inline bool capabilities_suppress_presence_activity_check() const;

    /// <summary>
    /// Indicates whether the session represents actual gameplay, as opposed to setup/menu time like a lobby or matchmaking.
    /// If true, then the session is in gameplay mode.
    /// </summary>
    inline bool capabilities_gameplay() const;

    /// <summary>
    /// If true, this session can host a large number of users, which has impact on other session properties.
    /// </summary>
    inline bool capabilities_large() const;

    /// <summary>
    /// If true, this connection is required to have a member be active (see documentation)
    /// </summary>
    inline bool capabilities_connection_required_for_active_member() const;

    /// <summary>
    /// Session supports cross play between PC and Xbox
    /// </summary>
    inline bool capabilities_crossplay() const;

    /// <summary>
    /// Session supports calls from platforms without strong title identity. This capability can't be set on large sessions.
    /// Using this capability will cause both 'readRestriction' and 'joinRestriction' to default to "local".
    /// </summary>
    inline bool capabilities_user_authorization_style() const;

    /// <summary>
    /// True, if the session can be linked to a search handle for searching.
    /// </summary>
    inline bool capabilities_searchable() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_constants(_In_ XblMultiplayerSessionHandle sessionHandle);

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_constants(bool isLobbySession);

    inline ~multiplayer_session_constants();

private:
    multiplayer_session_constants(const multiplayer_session_constants&) = delete;
    multiplayer_session_constants& operator=(multiplayer_session_constants) = delete;

    XblMultiplayerSessionHandle m_sessionHandle;
    const XblMultiplayerSessionConstants* m_constants;
};

/// <summary>
/// Represents a reference to a multiplayer session.
/// </summary>
class multiplayer_session_reference
{
public:
    /// <summary>
    /// Constructs a null MultiplayerSession object.
    /// </summary>
    inline multiplayer_session_reference();

    /// <summary>
    /// Constructs the MultiplayerSession object with data about the session.
    /// </summary>
    /// <param name="serviceConfigurationId">A service configuration ID appropriate for the title.</param>
    /// <param name="sessionTemplateName">The name of the template for the session to be based on.</param>
    /// <param name="sessionName">A unique name for the session.</param>
    inline multiplayer_session_reference(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& sessionTemplateName,
        _In_ const string_t& sessionName
    );

    /// <summary>
    /// The service configuration ID specific to the title.
    /// </summary>
    inline string_t service_configuration_id() const;

    /// <summary>
    /// The name of the template for the session.
    /// </summary>
    inline string_t session_template_name() const;

    /// <summary>
    /// The name of the session.
    /// </summary>
    inline string_t session_name() const;

    /// <summary>
    /// Whether this object has been properly constructed
    /// </summary>
    inline bool is_null() const;

    /// <summary>
    /// Returns a URI path representation of the session reference.
    /// </summary>
    inline string_t to_uri_path() const;

    /// <summary>
    /// Returns the session reference parsed from URI.
    /// </summary>
    static inline multiplayer_session_reference parse_from_uri_path(_In_ const string_t& path);

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_reference(_In_ const XblMultiplayerSessionReference& reference);

private:
    mutable XblMultiplayerSessionReference m_reference;
    friend class multiplayer_session;
    friend class multiplayer_service;
    friend class multiplayer_search_handle_request;
    friend class social::reputation_service;
};

/// <summary>
/// Represents the matchmaking server supporting the multiplayer session.
/// </summary>
class multiplayer_session_matchmaking_server
{
public:
    /// <summary>
    /// The Matchmaking Status of the Multiplayer Session Server.
    /// </summary>
    inline matchmaking_status status() const;

    /// <summary>
    /// The Matchmaking Status Details of the Multiplayer Session Server.
    /// </summary>
    inline string_t status_details() const;

    /// <summary>
    /// The Typical Wait of the Multiplayer Session Server.
    /// </summary>
    inline std::chrono::seconds typical_wait() const;

    /// <summary>
    /// The Target Session Reference of the Multiplayer Session Server.
    /// </summary>
    inline multiplayer_session_reference target_session_ref() const;

    /// <summary>
    /// Returns true if this object is blank
    /// </summary>
    inline bool is_null() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_matchmaking_server(_In_ XblMultiplayerSessionHandle sessionHandle);
    inline multiplayer_session_matchmaking_server(const multiplayer_session_matchmaking_server& other);
    inline multiplayer_session_matchmaking_server& operator=(multiplayer_session_matchmaking_server other);
    inline ~multiplayer_session_matchmaking_server();

private:
    XblMultiplayerSessionHandle m_sessionHandle;
    const XblMultiplayerMatchmakingServer* m_server;
};

/// <summary>
/// Represents role info for a multiplayer role.
/// </summary>
class multiplayer_role_info
{
public:
    inline multiplayer_role_info();

    /// <summary>
    /// Member xbox_user_ids currently assigned for this role.
    /// </summary>
    inline const std::vector<string_t>& member_xbox_user_ids() const;

    /// <summary>
    /// Number of slots occupied for this role.
    /// </summary>
    inline uint32_t members_count() const;

    /// <summary>
    /// Number of target slots assigned for this role.
    /// </summary>
    inline uint32_t target_count() const;

    /// <summary>
    /// Maximum number of slots available for this role.
    /// </summary>
    inline uint32_t max_members_count() const;

    /// <summary>
    /// Set the max member count for this role.
    /// Note: Only the session owner can modify role settings and only those that are multiplayer_role_type::mutable_role_settings()
    /// In your session template, you also need to set 'hasOwners' capability and 'ownerManaged' to true for the specific role type 
    /// that you want to modify the mutable_role_setting off.
    /// </summary>
    /// <param name="maxCount">The max member count for this role.</param>
    inline void set_max_members_count(_In_ uint32_t maxCount);

    /// <summary>
    /// Set the target member count for this role.
    /// Note: Only the session owner can modify role settings and only those that are multiplayer_role_type::mutable_role_settings()
    /// In your session template, you also need to set 'hasOwners' capability and 'ownerManaged' to true for the specific role type 
    /// that you want to modify the mutable_role_setting off.
    /// </summary>
    /// <param name="targetCount">The max member count for this role.</param>
    inline void set_target_count(_In_ uint32_t targetCount);

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_role_info(const XblMultiplayerRole* role);

private:
    std::vector<string_t> m_memberXuids;
    uint32_t m_maxMembersCount{ 0 };
    uint32_t m_membersCount{ 0 };
    uint32_t m_targetCount{ 0 };
};

/// <summary>
/// Represents a collection of roles for this role type.
/// </summary>
class multiplayer_role_type
{
public:
    inline multiplayer_role_type();

    /// <summary>
    /// True if ownerManaged is set on the roleType.
    /// </summary>
    inline bool owner_managed() const;

    /// <summary>
    /// Mutable role settings for this role.
    /// </summary>
    inline const std::vector<mutable_role_setting>& mutable_role_settings() const;

    /// <summary>
    /// A collection of roles for this role type.
    /// </summary>
    inline const std::unordered_map<string_t, multiplayer_role_info>& roles() const;

    /// <summary>
    /// Set a collection of roles for this role type.
    /// Note: Only the session owner can modify role settings and only those that are multiplayer_role_type::mutable_role_settings()
    /// In your session template, you also need to set 'hasOwners' capability and 'ownerManaged' to true for the specific role type 
    /// that you want to modify the mutable_role_setting off.
    /// </summary>
    inline void set_roles(_In_ const std::unordered_map<string_t, multiplayer_role_info>& roles);

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_role_type(const XblMultiplayerRoleType* roleType);

private:
    bool m_ownerManaged{ false };
    std::vector<mutable_role_setting> m_mutableRoleSettings;
    std::unordered_map<string_t, multiplayer_role_info> m_roles;
};

/// <summary>
/// Represents session role type values for a multiplayer session.
/// </summary>
class multiplayer_session_role_types
{
public:
    /// <summary>
    /// A collection of role types.
    /// </summary>
    inline const std::unordered_map<string_t, multiplayer_role_type>& role_types() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_role_types(const XblMultiplayerRoleType* roleTypes, size_t roleTypesCount);

private:
    std::unordered_map<string_t, multiplayer_role_type> m_roleTypes;
};

/// <summary>
/// Represents a users current multiplayer activity, along with some details about the corresponding session.
/// </summary>
class multiplayer_activity_details
{
public:
    /// <summary>
    /// Object containing identifying information for the session.
    /// </summary>
    inline multiplayer_session_reference session_reference() const;

    /// <summary>
    /// HandleId corresponding to this activity.
    /// </summary>
    inline string_t handle_id() const;

    /// <summary>
    /// TitleId that should be launched in order to join this activity.
    /// </summary>
    inline uint32_t title_id() const;

    /// <summary>
    /// The visibility state of the session. Whether other users can see, or join, etc.
    /// </summary>
    inline multiplayer_session_visibility visibility() const;

    /// <summary>
    /// The join restriction of the session, which applies if visibility is "open".
    /// </summary>
    inline multiplayer_session_restriction join_restriction() const;

    /// <summary>
    /// Indicates whether the session is temporarily closed for joining.
    /// </summary>
    inline bool closed() const;

    /// <summary>
    /// Xbox User ID of the member whose activity this is. 
    /// </summary>
    inline string_t owner_xbox_user_id() const;

    /// <summary>
    /// Number of total slots.
    /// </summary>
    inline uint32_t max_members_count() const;

    /// <summary>
    /// Number of slots occupied.
    /// </summary>
    inline uint32_t members_count() const;

    /// <summary>
    /// Custom session properties for the session.
    /// </summary>
    inline web::json::value custom_session_properties_json() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_activity_details(const XblMultiplayerActivityDetails& activityDetails);
    inline multiplayer_activity_details(const multiplayer_activity_details&);
    inline multiplayer_activity_details& operator=(multiplayer_activity_details);

    /// <summary>
    /// Internal function
    /// </summary>
    inline ~multiplayer_activity_details();

private:
    XblMultiplayerActivityDetails m_activityDetails;
};

/// <summary>
/// Represents a users current search handle, along with some details about the corresponding session.
/// </summary>
class multiplayer_search_handle_details
{
public:
    /// <summary>
    /// Object containing identifying information for the session.
    /// </summary>
    inline multiplayer_session_reference session_reference() const;

    /// <summary>
    /// HandleId corresponding to this activity.
    /// </summary>
    inline string_t handle_id() const;

    /// <summary>
    /// Owners of the session.
    /// </summary>
    inline std::vector<string_t> session_owner_xbox_user_ids() const;

    /// <summary>
    /// The tags that are currently set on the session.
    /// </summary>
    inline std::vector<string_t> tags() const;

    /// <summary>
    /// The numbers metadata that is currently set on the session.
    /// </summary>
    inline std::unordered_map<string_t, double> numbers_metadata() const;

    /// <summary>
    /// The strings metadata that is currently set on the session.
    /// </summary>
    inline std::unordered_map<string_t, string_t> strings_metadata() const;

    /// <summary>
    /// Deprecated. Note that the role types are still exposed through the multiplayer_session object.
    /// </summary>
    _XSAPICPP_DEPRECATED inline std::unordered_map<string_t, multiplayer_role_type> role_types() const;

    /// <summary>
    /// The visibility state of the session. Whether other users can see, or join, etc.
    /// </summary>
    inline multiplayer_session_visibility visibility() const;

    /// <summary>
    /// The join restriction of the session, which applies if visibility is "open".
    /// </summary>
    inline multiplayer_session_restriction join_restriction() const;

    /// <summary>
    /// Indicates whether the session is temporarily closed for joining.
    /// </summary>
    inline bool closed() const;

    /// <summary>
    /// Number of total slots.
    /// </summary>
    inline uint32_t max_members_count() const;

    /// <summary>
    /// Number of slots occupied.
    /// </summary>
    inline uint32_t members_count() const;

    /// <summary>
    /// Custom session properties for the session.
    /// </summary>
    inline web::json::value custom_session_properties_json() const;

    /// <summary>
    /// The time when the search handle was created.
    /// </summary>
    inline utility::datetime handle_creation_time() const;

    inline multiplayer_search_handle_details(XblMultiplayerSearchHandle handle);
    inline multiplayer_search_handle_details(const multiplayer_search_handle_details&);
    inline multiplayer_search_handle_details& operator=(multiplayer_search_handle_details);
    inline ~multiplayer_search_handle_details();

private:
    XblMultiplayerSearchHandle m_handle;
};

/// <summary>
/// Represents a reference to a multiplayer session. It
/// contains mostly just ids.
/// </summary>
class multiplayer_session_states
{
public:
    /// <summary>
    /// The time that the session began.
    /// </summary>
    inline utility::datetime start_time() const;

    /// <summary>
    /// Object containing identifying information for the session.
    /// </summary>
    inline multiplayer_session_reference session_reference() const;

    /// <summary>
    /// The current status of the session.
    /// </summary>
    inline multiplayer_session_status status() const;

    /// <summary>
    /// The visibility state of the session. Whether other users can see, or join, etc.
    /// </summary>
    inline multiplayer_session_visibility visibility() const;

    /// <summary>
    /// Indicates if it is my turn.
    /// </summary>
    inline bool is_my_turn() const;

    /// <summary>
    /// Xbox User ID of the member. 
    /// </summary>
    inline string_t xbox_user_id() const;

    /// <summary>
    /// Approximate number of non-reserved members.
    /// </summary>
    inline uint32_t accepted_member_count() const;

    /// <summary>
    /// Join restriction for the session.
    /// </summary>
    inline multiplayer_session_restriction join_restriction() const;

    /// <summary>
    /// DEPRECATED - Keywords can be obtained with get_current_session.
    /// </summary>
    _XSAPICPP_DEPRECATED inline std::vector<string_t> keywords() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_states(_In_ const XblMultiplayerSessionQueryResult& state);

private:
    XblMultiplayerSessionQueryResult m_state;
};

/// <summary>
/// Represents a reference to member in a multiplayer session.
/// </summary>
class multiplayer_session_member
{
public:
    /// <summary>
    /// Id for this member.
    /// </summary>
    inline uint32_t member_id() const;

    /// <summary>
    /// Initial team assignment from SmartMatch.
    /// </summary>
    inline string_t initial_team() const;

    /// <summary>
    /// Xbox User ID of the member.  Only known if the member has accepted.
    /// </summary>
    inline string_t xbox_user_id() const;
    
    /// <summary>
    /// JSON string that specify the custom constants for the member.
    /// </summary>
    inline web::json::value member_custom_constants_json() const;
    
    /// <summary>
    /// The base64 encoded secure device address of the member. (Optional)
    /// </summary>
    inline string_t secure_device_base_address64() const;

    /// <summary>
    /// A collection of role types to role names for this member. (Optional)
    /// </summary>
    inline std::unordered_map<string_t, string_t> roles() const;

    /// <summary>
    /// JSON string that specify the custom properties for the member.
    /// </summary>
    inline web::json::value member_custom_properties_json() const;

    /// <summary>
    /// The Gamertag of the member.  Only known if the member has accepted. (Optional) 
    /// </summary>
    inline string_t gamertag() const;

    /// <summary>
    /// The status of this member.
    /// </summary>
    inline multiplayer_session_member_status status() const;

    /// <summary>
    /// Only true if this member is ready for turn.
    /// </summary>
    inline bool is_turn_available() const;

    /// <summary>
    /// Indicates if this MultiplayerSessionMember is for the current user.
    /// </summary>
    inline bool is_current_user() const;

    /// <summary>
    /// Indicates to run QoS initialization for this user. Defaults to false.
    /// Ignored if there is not a "memberInitialization" section for the session.
    /// </summary>
    inline bool initialize_requested() const;

    /// <summary>
    /// When match adds a user to a session, it can provide some context around how and why they were matched into the session.
    /// This is a copy of the user's serverMeasurements from the matchmaking session.
    /// </summary>
    inline web::json::value matchmaking_result_server_measurements_json() const;
    
    /// <summary>
    /// QoS measurements by game-server connection string. 
    /// Like all fields, "serverMeasurements" must be updated as a whole, so it should be set once when measurement is complete.
    /// If empty, it means that none of the measurements completed within the "serverMeasurementTimeout".
    /// </summary>
    inline web::json::value member_server_measurements_json() const;

    /// <summary>
    /// A collection of members in my group
    /// If a "initializationGroup" list is set, the member's own index will always be added if it isn't already present.
    /// During managed initialization, if any members in the list fail, this member will also fail.
    /// </summary>
    inline std::vector<std::shared_ptr<multiplayer_session_member>> members_in_group() const;

    /// <summary>
    /// A collection of members in my group
    /// If a "initializationGroup" list is set, the member's own index will always be added if it isn't already present.
    /// During managed initialization, if any members in the list fail, this member will also fail.
    /// </summary>
    inline std::error_code set_members_list(_In_ const std::vector<std::shared_ptr<multiplayer_session_member>>& members);

    /// <summary>
    /// QoS measurements by secure device address. Like all fields, "measurements" must be updated as a whole. It should be set once when measurement is complete, not incrementally.
    /// If a "measurements" object is set, it can't contain an entry for the member's own address.
    /// </summary>
    inline std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>> member_measurements() const;
    
    /// <summary>
    /// This is set when the member uploads a secure device address. It's a case-insensitive string that can be used for equality comparisons.
    /// </summary>
    inline string_t device_token() const;
    
    /// <summary>
    /// This is the device's NAT setting when the member uploads a secure device address.
    /// </summary>
    inline network_address_translation_setting nat() const;

    /// <summary>
    /// If the member is active, this is the title ID in which they are active.
    /// </summary>
    inline uint32_t active_title_id() const;
    
    /// <summary>
    /// This value is only useful to read when the title is manually managing their own QoS.
    /// If the "memberInitialization" section is set and the member was added with "initialize":true, 
    /// this is set to the initialization episode that the member will participate in otherwise it is 0.
    /// Users join sessions in batches.  
    /// The initialization episode number indicates a set of users that QoS needs to be performed against.
    /// Initialization episode 1 is a special value used for the members added to a new session at create time.
    /// </summary>
    inline uint32_t initialization_episode() const;

    /// <summary>
    /// The time the user joined the session. If "reserved" is true, this is the time the reservation was made.
    /// </summary>
    inline utility::datetime join_time() const;
    
    /// <summary>
    /// The cause of why the initialization failed, or MultiplayerMeasurementFailure::None if there was no failure.
    /// Set when transitioning out of the "joining" or "measuring" stage if this member doesn't pass.
    /// </summary>
    inline multiplayer_measurement_failure initialization_failure_cause() const;

    /// <summary>
    /// Gets or sets a string vector of group names for the current user indicating which groups that user was part of during a multiplayer session.
    /// </summary>
    inline std::vector<string_t> groups() const;

    /// <summary>
    /// Gets or sets a string vector of group names for the current user indicating which groups that user was part of during a multiplayer session.
    /// </summary>
    inline void set_groups(_In_ const std::vector<string_t>& groups);

    /// <summary>
    /// Gets a list of group names for the current user indicating which groups that user encountered during a multiplayer session.
    /// </summary>
    inline std::vector<string_t> encounters() const;

    /// <summary>
    /// Gets a list of group names for the current user indicating which groups that user encountered during a multiplayer session.
    /// </summary>
    inline void set_encounters(_In_ const std::vector<string_t>& encounters);

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_member(_In_ XblMultiplayerSessionHandle session, _In_ const XblMultiplayerSessionMember* member);
    inline ~multiplayer_session_member();

private:
    multiplayer_session_member(const multiplayer_session_member&) = delete;
    multiplayer_session_member& operator=(multiplayer_session_member) = delete;

    XblMultiplayerSessionHandle m_session;
    const XblMultiplayerSessionMember* m_member;
};

/// <summary>
/// Represents multiplayer session properties.
/// </summary>
class multiplayer_session_properties
{
public:
    /// <summary>
    /// A collection of keywords associated with the session. (Optional, might be empty)
    /// When changing, call multiplayer_service::write_session to write the changes to the service.
    /// </summary>
    inline std::vector<string_t> keywords() const;

    /// <summary>
    /// A collection of keywords associated with the session. (Optional, might be empty)
    /// When changing, call multiplayer_service::write_session to write the changes to the service.
    /// </summary>
    inline void set_keywords(_In_ const std::vector<string_t>& keywords);

    /// <summary>
    /// Restricts who can join "open" sessions. (Has no effect on reservations, which means it has no impact on "private" and "visible" sessions.)
    /// Defaults to "none".
    /// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can join without a reservation.
    /// </summary>
    inline multiplayer_session_restriction join_restriction() const;
    
    /// <summary>
    /// Restricts who can join "open" sessions. (Has no effect on reservations, which means it has no impact on "private" and "visible" sessions.)
    /// Defaults to "none".
    /// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can join without a reservation.
    /// </summary>
    inline std::error_code set_join_restriction(_In_ multiplayer_session_restriction joinRestriction);

    /// <summary>
    /// Restricts who can read "open" sessions. (Has no effect on reservations, which means it has no impact on "private" and "visible" sessions.)
    /// Defaults to "none".
    /// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can read without a reservation.
    /// The read restriction applies to sessions with "open" or "visible" visibility and determines who can read the session without an invite.
    /// The read restriction must be at least as accessible as the join restriction, i.e. 'joinRestriction' can't be set to "followed" without also setting 'readRestriction'."
    /// </summary>
    inline multiplayer_session_restriction read_restriction() const;

    /// <summary>
    /// Restricts who can read "open" sessions. (Has no effect on reservations, which means it has no impact on "private" and "visible" sessions.)
    /// Defaults to "none".
    /// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can read without a reservation.
    /// </summary>
    inline std::error_code set_read_restriction(_In_ multiplayer_session_restriction readRestriction);

    /// <summary>
    /// A collection of MultiplayerSessionMember objects indicating whose turn it is.
    /// When changing, call multiplayer_service::write_session to write the changes to the service.
    /// </summary>
    inline std::vector<std::shared_ptr<multiplayer_session_member>> turn_collection() const;
    
    /// <summary>
    /// A collection of MultiplayerSessionMember objects indicating whose turn it is.
    /// When changing, call multiplayer_service::write_session to write the changes to the service.
    /// </summary>
    inline std::error_code set_turn_collection(_In_ const std::vector<std::shared_ptr<multiplayer_session_member>>& turnCollection);

    /// <summary>
    /// A JSON string representing the target session constants.
    /// </summary>
    inline web::json::value matchmaking_target_session_constants_json() const;

    /// <summary>
    /// JSON string that specify the custom properties for the session.  These can be changed anytime.
    /// When changing, call multiplayer_service::write_session to write the changes to the service.
    /// </summary>
    inline web::json::value session_custom_properties_json() const;

    /// <summary>
    /// Force a specific connection string to be used.  This is useful for session in progress join scenarios.
    /// </summary>
    inline string_t matchmaking_server_connection_string() const;
    
    /// <summary>
    /// The ordered list of connection strings that the session could use to connect to a game server. Generally titles should use the first on
    /// the list, but sophisticated titles could use a custom mechanism for choosing one of the others (e.g. based on load).
    /// </summary>
    inline std::vector<string_t> server_connection_string_candidates() const;

    /// <summary>
    /// Member index of owners of the session.
    /// </summary>
    inline std::vector<uint32_t> session_owner_indices() const;
    
    /// <summary>
    /// Device token of the host.
    /// Must match the "deviceToken" of at least one member, otherwise this field is deleted.
    /// If "peerToHostRequirements" is set and "host" is set, the measurement stage assumes the given host is the correct host and only measures metrics to that host.
    /// </summary>
    inline string_t host_device_token() const;

    /// <summary>
    /// Controls whether a session is joinable, independent of visibility, joinrestriction, and available space in the session.
    /// Does not affect reservations.  Defaults to false.
    /// </summary>
    inline bool closed() const;

    /// <summary>
    /// If true, it would allow the members of the session to be locked, such that if a user leaves they are able to 
    /// come back into the session but no other user could take that spot. Defaults to false.
    /// </summary>
    inline bool locked() const;

    /// <summary>
    /// Setting to true by a client triggers a Xbox Live Compute allocation attempt by MPSD.
    /// Defaults to false.
    /// </summary>
    inline bool allocate_cloud_compute() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_properties(_In_ XblMultiplayerSessionHandle session);
    inline ~multiplayer_session_properties();

private:
    multiplayer_session_properties(const multiplayer_session_properties&) = delete;
    multiplayer_session_properties& operator=(multiplayer_session_properties) = delete;

    XblMultiplayerSessionHandle m_session;
    const XblMultiplayerSessionProperties* m_properties;
};

/// <summary>
/// Represents a multiplayer session.
/// </summary>
class multiplayer_session
{
public:
    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only create a local session 
    /// object but does not commit it to the service.  
    /// Creates a new MultiplayerSession without any constants or session reference.  This override is intended 
    /// to be used when the session (serviceconfigid/template/name) are not known.  A MultiplayerSession created
    /// using this constructor must we retrieved/written using the "ByHandle" overrides.  
    /// (e.g. WriteSessionByHandleAsync and GetCurrentSessionByHandleAsync)
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user who is creating this session</param>
    /// <param name="multiplayerSessionReference">A reference that uniquely identifies the session.</param>
    inline multiplayer_session(
        _In_ const string_t& xboxUserId
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only create a local session object but does not commit it to the service.
    /// Creates a new MultiplayerSession without any constants, which allows the request to simply use whatever constants are already specified in the session
    /// template on the service.  Those constants are returned in the response session data.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user who is creating this session</param>
    /// <param name="multiplayerSessionReference">A reference that uniquely identifies the session.</param>
    inline multiplayer_session(
        _In_ const string_t& xboxUserId,
        _In_ multiplayer_session_reference sessionReference
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only create a local session object but does not commit it to the service.
    /// Creates a new MultiplayerSession using the specified session constants.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user who is creating this session</param>
    /// <param name="multiplayerSessionReference">A reference that uniquely identifies the session.</param>
    /// <param name="maxMembersInSession">The maximum number of members in this session. This value can only be set if the maximum is not specified in the title's multiplayer session template.
    /// If the maximum is specified in the title's multiplayer session template, then set to 0 to ignore this parameter</param>
    /// <param name="multiplayerSessionVisibility">The visibility of this session</param>
    /// <param name="initiatorXboxUserIds">A collection of Xbox User IDs indicating who initiated the session (Optional)</param>
    /// <param name="sessionCustomConstantsJson">JSON that specify the custom constants for the session.  These can not be changed after the session is created.  (Optional)</param>
    inline multiplayer_session(
        _In_ const string_t& xboxUserId,
        _In_ multiplayer_session_reference multiplayerSessionReference,
        _In_ uint32_t maxMembersInSession,
        _In_ multiplayer_session_visibility multiplayerSessionVisibility,
        _In_ std::vector<string_t> initiatorXboxUserIds = std::vector<string_t>(),
        _In_ const web::json::value& sessionCustomConstantsJson = web::json::value()
        );

    /// <summary>
    /// A unique ID to the session used to query trace logs for entries that relate to the session.  
    /// </summary>
    inline string_t multiplayer_correlation_id() const;

    /// <summary>
    /// A unique search handle ID to the session.
    /// </summary>
    inline string_t search_handle_id() const;

    /// <summary>
    /// The time that the session began.
    /// </summary>
    inline utility::datetime start_time() const;

    /// <summary>
    /// If any timeouts are in progress, this is the date when the next timer will fire.
    /// </summary>
    inline utility::datetime date_of_next_timer() const;
    
    /// <summary>
    /// The date when the server returned the session.
    /// </summary>
    inline utility::datetime date_of_session() const;

    /// <summary>
    /// Present during managed initialization.
    /// The "stage" goes from "joining" to "measuring" to "evaluating".
    /// If episode 1 fails, then "stage" is set to "failed" and the session cannot be initialized.
    /// Otherwise, when an initialization episode completes, the "initialization" object is removed.
    /// If "externalEvaluation" is not set, "evaluating" is skipped. If "metrics" isn't set, "measuring" is skipped.
    /// </summary>
    inline multiplayer_initialization_stage initialization_stage() const;

    /// <summary>
    /// The time with the initialization stage started.
    /// </summary>
    inline utility::datetime initializing_stage_start_time() const;

    /// <summary>
    /// If MemberInitialization set and Initialize is true on the member, then the member gets assigned to an InitializingEpisode.  
    /// An episode is a set of users that need to have QoS metrics applied to them.  
    /// Will be 0 when the InitializingEpisode is not set.
    /// This value is only useful when manually managing QoS.
    /// </summary>
    inline uint32_t intializing_episode() const;

    /// <summary>
    /// Returns an OR'd set of MultiplayerSessionChangeTypes values representing the aspects of 
    /// the session that the current xboxlivecontext is subscribed to, of None if there is none.
    /// </summary>
    inline multiplayer_session_change_types subscribed_change_types() const;

    /// <summary>
    /// Host candidates are an ordered list of device tokens, ordered by preference as specified by MultiplayerSessionConstants::PeerToHostRequirements::HostSelectionMetric.
    /// </summary>
    inline std::vector<string_t> host_candidates() const;
    
    /// <summary>
    /// The uniquely identifying information for the session.
    /// </summary>
    inline multiplayer_session_reference session_reference() const;
    
    /// <summary>
    /// A set of constants associated with this session.  These can only be set when creating the session.
    /// </summary>
    inline std::shared_ptr<multiplayer_session_constants> session_constants() const;
    
    /// <summary>
    /// A set of properties associated with this session.  Any player can modify these properties.
    /// </summary>
    inline std::shared_ptr<multiplayer_session_properties> session_properties() const;

    /// <summary>
    /// A set of role types associated with this session.
    /// </summary>
    inline std::shared_ptr<multiplayer_session_role_types> session_role_types() const;
    
    /// <summary>
    /// A collection of members that are in the session or entering the session together. 
    /// Call MultiplayerSession::Join or MultiplayerSession::Leave to add or remove yourself from this list.  
    /// Call MultiplayerSession::AddMemberReservation to add a reservation for another user on this list.
    /// Call multiplayer_service::write_session to write these changes to the service.
    /// </summary>
    inline std::vector<std::shared_ptr<multiplayer_session_member>> members() const;
    
    /// <summary>
    /// A multiplayer session servers that contains properties associated with a target session reference.
    /// </summary>
    inline multiplayer_session_matchmaking_server matchmaking_server() const;

    /// <summary>
    /// The number of members that have accepted and are added to the session and are no longer pending.
    /// </summary>
    inline uint32_t members_accepted() const;
    
    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// A JSON string containing a collection of servers for this multiplayer session.
    /// </summary>
    inline web::json::value servers_json() const;
    
    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// A JSON string containing a collection of servers for this multiplayer session.
    /// </summary>
    inline void set_servers_json(_In_ const web::json::value& serversJson);
    
    /// <summary>
    /// The ETag returned with this session.
    /// </summary>
    inline string_t e_tag() const;
    
    /// <summary>
    /// Returns the current User in the session.  A nullptr will be returned if there is no current user in the session.
    /// </summary>
    inline std::shared_ptr<multiplayer_session_member> current_user() const;
    
    /// <summary>
    /// The branch of the session used to scope change numbers. 
    /// </summary>
    inline string_t branch() const;

    /// <summary>
    /// The change number of the session.
    /// </summary>
    inline uint64_t change_number() const;
    
    /// <summary>
    /// On writing a session, the status of the write
    /// </summary>
    inline write_session_status write_status() const;

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Add a new member reservation on the session for the specified xboxUserId and member constants.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID to add a reservation for.</param>
    /// <param name="memberCustomConstantsJson">The custom constants to set for this member.  This is the only time the member's constants can be set. (Optional)</param>
    inline std::error_code add_member_reservation(
        _In_ const string_t& xboxUserId,
        _In_ const web::json::value& memberCustomConstantsJson = web::json::value()
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Add a new member reservation on the session for the specified xboxUserId and member constants.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID to add a reservation for.</param>
    /// <param name="memberCustomConstantsJson">The custom constants to set for this member.  This is the only time the member's constants can be set. (Optional)</param>
    /// <param name="initializeRequested">True if the system should perform managed initialization, and false otherwise.</param>
    inline std::error_code add_member_reservation(
        _In_ const string_t& xboxUserId,
        _In_ const web::json::value& memberCustomConstantsJson,
        _In_ bool initializeRequested
        );

    /// <summary>
    /// Joins this user to the session, sets the user to active.
    /// </summary>
    /// <param name="memberCustomConstantsJson">The custom constants to set for this member.  
    /// This is the only time the member's constants can be set. </param>
    /// <param name="initializeRequested">True if the system should perform managed    
    /// initialization, and false otherwise.</param>
    /// <param name="joinWithActiveStatus">True if player should join with active status.</param>
    /// <param name="addInitializePropertyToRequest">True to write the 'initialize' property to the request. False otherwise and that means initializeRequested will be ignored.</param>
    inline xbox_live_result<std::shared_ptr<multiplayer_session_member>> join(
        _In_ const web::json::value& memberCustomConstantsJson = web::json::value(),
        _In_ bool initializeRequested = true,
        _In_ bool joinWithActiveStatus = true,
        _In_ bool addInitializePropertyToRequest = true
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// Sets the visibility of the session.
    /// </summary>
    /// <param name="visibility">Set the visibility setting of the session.</param>    
    inline void set_visibility(_In_ multiplayer_session_visibility visibility);

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// Sets the max member count of the session.
    /// </summary>
    /// <param name="maxMembersInSession">Set the max member count of the session.</param>    
    inline void set_max_members_in_session(_In_ uint32_t maxMembersInSession);

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Sets the max member count per role. 
    /// Note: Only the session owner can modify role settings and only those that are multiplayer_role_type::mutable_role_settings()
    /// In your session template, you also need to set 'hasOwners' capability and 'ownerManaged' to true for the specific role type 
    /// that you want to modify the mutable_role_setting off.
    /// </summary>
    /// <param name="roleTypes">A map of role type names to multiplayer role type</param>
    inline std::error_code set_mutable_role_settings(
        _In_ const std::unordered_map<string_t, multiplayer_role_type>& roleTypes
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// Sets the timeouts for the session.
    /// </summary>
    /// <param name="memberReservedTimeout">The timeout for a member reservation, in milliseconds. A value of 0 is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
    /// <param name="memberInactiveTimeout">The timeout for a member to be considered inactive, in milliseconds. A value of 0 is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
    /// <param name="memberReadyTimeout">The timeout for a member to be considered ready, in milliseconds. A value of 0 is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
    /// <param name="sessionEmptyTimeout">The timeout for an empty session, in milliseconds. A value of 0 is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
    inline std::error_code set_timeouts(
        _In_ std::chrono::milliseconds memberReservedTimeout,
        _In_ std::chrono::milliseconds memberInactiveTimeout,
        _In_ std::chrono::milliseconds memberReadyTimeout,
        _In_ std::chrono::milliseconds sessionEmptyTimeout
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// Enables or disables connectivity metrics for the session.
    /// For ones that are enabled, they must be sufficient to satisfy the QoS requirements.
    /// </summary>
    /// <param name="enableLatencyMetric">True to enable the measuring of latency, and false to disable latency measurement.</param>
    /// <param name="enableBandwidthDownMetric">True to enable the measuring of bandwidth down, and false to disable bandwidth down measurement.</param>
    /// <param name="enableBandwidthUpMetric">True to enable the measuring of bandwidth up, and false to disable bandwidth up measurement.</param>
    /// <param name="enableCustomMetric">True to enable custom metrics, and false to disable them.</param>
    inline std::error_code set_quality_of_service_connectivity_metrics(
        _In_ bool enableLatencyMetric,
        _In_ bool enableBandwidthDownMetric,
        _In_ bool enableBandwidthUpMetric,
        _In_ bool enableCustomMetric
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// If a 'memberInitialization' object is set, the session expects the client system or title to perform 
    /// initialization following session creation and/or as new members join the session.
    /// The timeouts and initialization stages are automatically tracked by the session, including QoS 
    /// measurements if any metrics are set. These timeouts override the session's reservation and ready 
    /// timeouts for members that have 'initializationEpisode' set.
    /// </summary>
    /// <param name="joinTimeout">The period of time, in milliseconds, that the Xbox system waits for a member to join the session.</param>
    /// <param name="measurementTimeout">The period of time, in milliseconds, that the Xbox system waits for a measuring operation during managed initialization.</param>
    /// <param name="evaluationTimeout">The period of time, in milliseconds, that the Xbox system waits for an evaluation.</param>
    /// <param name="externalEvaluation">False if the Xbox system should auto-evaluate the session service, and true if the title performs the evaluation.</param>
    /// <param name="membersNeededToStart">The number of members needed to start the session, for initialization episode zero only.</param>
    inline std::error_code set_member_initialization(
        _In_ std::chrono::milliseconds joinTimeout,
        _In_ std::chrono::milliseconds measurementTimeout,
        _In_ std::chrono::milliseconds evaluationTimeout,
        _In_ bool externalEvaluation,
        _In_ uint32_t membersNeededToStart
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// These thresholds apply to each pairwise connection for all members in a session.
    /// </summary>
    /// <param name="latencyMaximum">The maximum latency, in milliseconds, between session members.</param>
    /// <param name="bandwidthMinimumInKilobitsPerSecond">The minimum bandwidth, in kilobits per second, between members.</param>
    inline std::error_code set_peer_to_peer_requirements(
        _In_ std::chrono::milliseconds latencyMaximum,
        _In_ uint32_t bandwidthMinimumInKilobitsPerSecond
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// These thresholds apply to each connection from a host candidate.
    /// </summary>
    /// <param name="latencyMaximum">The maximum latency time, in milliseconds.</param>
    /// <param name="bandwidthDownMinimumInKilobitsPerSecond">The minimum bandwidth, in kilobits per second, for information sent from the host to the session member.</param>
    /// <param name="bandwidthUpMinimumInKilobitsPerSecond">The minimum bandwidth, in kilobits per second, for information sent from the session member to the host.</param>
    /// <param name="hostSelectionMetric">An enumeration value indicating the metric for the Xbox system to use in selecting a host.</param>
    inline std::error_code set_peer_to_host_requirements(
        _In_ std::chrono::milliseconds latencyMaximum,
        _In_ uint32_t bandwidthDownMinimumInKilobitsPerSecond,
        _In_ uint32_t bandwidthUpMinimumInKilobitsPerSecond,
        _In_ multiplay_metrics hostSelectionMetric
        );
    
    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.    
    /// </summary>
    /// <param name="capabilities">A collection of MultiplayerSessionCapabilities flags that apply to the MultiplayerSessionConstant's capabilities JSON object</param>
    inline std::error_code set_session_capabilities(
        _In_ const multiplayer_session_capabilities& capabilities
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// Can only be specified if the 'cloudCompute' capability is set. Enables clients to request that a cloud compute instance be allocated on behalf of the session.
    /// </summary>
    /// <param name="sessionCloudComputePackageConstantsJson">Cloud compute instance be allocated on behalf of the session.</param>
    inline std::error_code set_cloud_compute_package_json(
        _In_ const web::json::value& sessionCloudComputePackageConstantsJson
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// The set of potential server connection strings that should be evaluated. 
    /// </summary>
    /// <param name="initializationSucceeded">True if initialization succeeded, and false otherwise.</param>
    inline void set_initialization_status(
        _In_ bool initializationSucceeded
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Sets the device token of the host.
    /// If "peerToHostRequirements" is set and this is set, the measurement stage assumes the given host is the correct host and only measures metrics to that host.
    /// </summary>
    /// <param name="hostDeviceToken">The host device token.</param>
    inline void set_host_device_token(
        _In_ const string_t& hostDeviceToken
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Forces a specific server connection string to be used, useful in preserveSession=always cases.
    /// </summary>
    /// <param name="serverConnectionPath">The server connection path. Setting this path can be useful when the session is preserved.</param>
    inline void set_matchmaking_server_connection_path(
        _In_ const string_t& serverConnectionPath
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// If set to true, makes the session "closed", meaning that new users will not be able to join unless they already have a reservation.
    /// </summary>
    inline void set_closed(
        _In_ bool closed
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// If set to true, it would allow the members of the session to be locked, such that if a user leaves they are able to come back into the session but
    /// no other user could take that spot. If the session is locked, it must also be set to closed.
    /// </summary>
    inline void set_locked(
        _In_ bool locked
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// If set to true, makes the session "closed", meaning that new users will not be able to join unless they already have a reservation.
    /// </summary>
    inline void set_allocate_cloud_compute(
        _In_ bool allocateCloudCompute
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set matchResubmit to true if the match that was found didn't work out and needs to be resubmitted. 
    /// Set matchResubmit to false to signal that the match did work, and the matchmaking service can release the session.
    /// </summary>
    /// <param name="matchResubmit ">True if the match that was found was not successful and needs to be resubmitted.</param>
    inline void set_matchmaking_resubmit(
        _In_ bool matchResubmit
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// The ordered list of case-insensitive connection strings that the session could use to connect to 
    /// a game server. Generally titles should use the first on the list, but sophisticated titles could use 
    /// a custom mechanism for choosing one of the others (e.g. based on load).
    /// </summary>
    /// <param name="serverConnectionStringCandidates">The collection of connection paths.</param>
    inline void set_server_connection_string_candidates(
        _In_ const std::vector<string_t>& serverConnectionStringCandidates
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local 
    /// session object but does not commit it to the service. Configures the set of session changes that 
    /// this client will be subscribed to.  Set to "MultiplayerSessionChangeTypes::None" to clear the subscription.
    /// </summary>
    /// <param name="changeTypes">Or'd set of MultiplayerSessionChangeType enum values representing the change types to subscribe to.</param>
    inline std::error_code set_session_change_subscription(
        _In_ multiplayer_session_change_types changeTypes
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// With the user who either created or got the session, leave the session. If the session is deleted as a result of this action, a 204 response with a nullptr for the MultiplayerSession object will be returned. 
    /// </summary>
    inline std::error_code leave();

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set the current user to active or inactive.  
    /// You cannot set the user to reserved or ready in this manner.  
    /// Use AddMemberReservation() to add a member reservation.
    /// The member must first be joined to the session.
    /// </summary>
    /// <param name="status">Indicates the current user status.</param>
    inline std::error_code set_current_user_status(
        _In_ multiplayer_session_member_status status
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set the base64 encoded secure device address of the member
    /// The member must first be joined to the session.
    /// </summary>
    /// <param name="value">Indicates the value of the current user's secure device address encoded in base64.</param>
    inline std::error_code set_current_user_secure_device_address_base64(
        _In_ const string_t& value
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set the role info of the member.
    /// The member must first be joined to the session.
    /// </summary>
    /// <param name="roles">Indicates a collection of role types to role names for the current user.</param>
    inline std::error_code set_current_user_role_info(
        _In_ const std::unordered_map<string_t, string_t>& roles
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set a collection of members in the group
    /// The member must first be joined to the session.
    /// </summary>
    /// <param name="value">Indicates the value of the current user's secure device address encoded in base64.</param>
    inline std::error_code set_current_user_members_in_group(
        _In_ const std::vector<std::shared_ptr<multiplayer_session_member>>& membersInGroup
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Sets a collection of MultiplayerQualityOfServiceMeasurements for the members.  
    /// This is only useful when the title is manually managing QoS.
    /// If the platform is automatically performing QoS, this does not need to be called.
    /// </summary>
    /// <param name="measurements">A collection of objects representing the QoS measurements.</param>
    inline std::error_code set_current_user_quality_of_service_measurements(
        _In_ std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>> measurements
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Sets measurements JSON for the servers.  
    /// This is only useful when the title is manually managing QoS.
    /// If the platform is automatically performing QoS, this does not need to be called.
    /// </summary>
    /// <param name="serverMeasurementsJson">The JSON that represents the server measurements.</param>
    inline std::error_code set_current_user_quality_of_service_measurements_json(
        _In_ const web::json::value& serverMeasurementsJson
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set a custom property on the current user to the specified JSON string
    /// The member must first be joined to the session.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    inline std::error_code set_current_user_member_custom_property_json(
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson = web::json::value()
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Delete a custom property on the current user
    /// </summary>
    /// <param name="name">The name of the property to set</param>
    inline std::error_code delete_current_user_member_custom_property_json(
        _In_ const string_t& name
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Sets the properties of the matchmaking.  This should only be set by a client acting as a matchmaking service. 
    /// </summary>
    /// <param name="matchmakingTargetSessionConstantsJson">A JSON string representing the target session constants.</param>
    inline std::error_code set_matchmaking_target_session_constants_json(
        _In_ const web::json::value& matchmakingTargetSessionConstantsJson
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set a session custom property to the specified JSON string.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    inline std::error_code set_session_custom_property_json(
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson = web::json::value()
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Deletes a session custom property. 
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    inline std::error_code delete_session_custom_property_json(
        _In_ const string_t& name
        );

    /// <summary>
    /// Static compare method that allows comparison between 2 sessions and returns a Or'ed MultiplayerSessionChangeType.
    /// </summary>
    /// <param name="currentSession">A session to compare to the other.</param>
    /// <param name="oldSession">A session to compare to the other.</param>
    /// <returns>An OR'ed MultiplayerSessionChangeType that contains all of the differences.</returns>
    inline static xbox_live_result<multiplayer_session_change_types> compare_multiplayer_sessions(
        _In_ std::shared_ptr<multiplayer_session> currentSession,
        _In_ std::shared_ptr<multiplayer_session> oldSession
        );

    /// <summary>
    /// Static method that converts an HTTP Status code to a write_session_status
    /// </summary>
    /// <param name="httpStatusCode">Status code of a http result</param>
    /// <returns>A write_session_status which gives more specific information about the status code in regards to the Write Session Call</returns>
    inline static write_session_status convert_http_status_to_write_session_status(
        _In_ int32_t httpStatusCode
        );

    /// <summary>
    /// Internal function.
    /// </summary>
    inline multiplayer_session(XblMultiplayerSessionHandle handle);
    inline ~multiplayer_session();

private:
    multiplayer_session(const multiplayer_session&) = delete;
    multiplayer_session& operator=(multiplayer_session) = delete;

    XblMultiplayerSessionHandle m_handle;
    const XblMultiplayerSessionInfo* m_sessionInfo;
    mutable std::shared_ptr<multiplayer_session_constants> m_sessionConstants;
    mutable std::shared_ptr<multiplayer_session_properties> m_sessionProperties;

    friend class multiplayer_service;
};

/// <summary>
/// Arguments passed to the event handler when a session change occurs.
/// </summary>
class multiplayer_session_change_event_args
{
public:
    /// <summary>
    /// The session that triggered this event.
    /// </summary>
    inline multiplayer_session_reference session_reference() const;

    /// <summary>
    /// The branch of the session used to scope change numbers. 
    /// </summary>
    inline string_t branch() const;

    /// <summary>
    /// The change number of the session.
    /// </summary>
    inline uint64_t change_number() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_session_change_event_args(const XblMultiplayerSessionChangeEventArgs& args);

private:
    XblMultiplayerSessionChangeEventArgs m_args;
};

/// <summary>
/// Gets the visible multiplayer sessions based on the configuration of this request.
/// </summary>
class multiplayer_get_sessions_request
{
public:
    /// <summary>Creates a GetSessionsRequest object.</summary>
    /// <param name="serviceConfigurationId">The service configuration id that the sessions part of.</param>
    /// <param name="maxItems">The maximum number of items to return.</param>
    inline multiplayer_get_sessions_request(
        _In_ string_t serviceConfigurationId,
        _In_ uint32_t maxItems = 0
        );

    /// <summary>
    /// The service configuration id that the sessions part of.
    /// </summary>
    inline string_t service_configuration_id();

    /// <summary>
    /// The maximum number of items to return.
    /// </summary>
    inline uint32_t max_items();

    /// <summary>
    /// Include private sessions to the result.
    /// </summary>
    inline bool include_private_sessions();

    /// <summary>
    /// Sets the query to include private sessions in the result.
    /// </summary>
    /// <param name="includePrivateSessions">Whether to include private sessions.</param>
    inline void set_include_private_sessions(_In_ bool includePrivateSessions);

    /// <summary>
    /// Include sessions that the user hasn't accepted.  Must specify xboxUserIdFilter to use.
    /// </summary>
    inline bool include_reservations();

    /// <summary>
    /// Sets the query to include session reservations for members in the result.
    /// </summary>
    /// <param name="includeResevations">Whether to include reservations.</param>
    inline void set_include_reservations(_In_ bool includeResevations);

    /// <summary>
    /// Include inactive sessions to the result.  Must specify xboxUserIdFilter to use.
    /// </summary>
    inline bool include_inactive_sessions();

    /// <summary>
    /// Sets the query to include inactive sessions in the result.
    /// </summary>
    /// <param name="includeInactiveSessions">Whether to include inactive sessions.</param>
    inline void set_include_inactive_sessions(_In_ bool includeInactiveSessions);

    /// <summary>
    /// Filter result to just sessions this Xbox User ID in it.
    /// </summary>
    inline string_t xbox_user_id_filter();

    /// <summary>
    /// Sets the xbox user id filter. (The xboxUserIdFilter, xboxUserIdsFilter, or keywordFilter must be specified)
    /// </summary>
    /// <param name="filter">The xbox user id to filter by.</param>
    inline void set_xbox_user_id_filter(_In_ const string_t& filter);

    /// <summary>
    /// Filter result to just sessions these Xbox User IDs in it.
    /// </summary>
    inline std::vector<string_t> xbox_user_ids_filter();

    /// <summary>
    /// Sets the xbox user ids filter. (The xboxUserIdFilter, xboxUserIdsFilter, or keywordFilter must be specified)
    /// </summary>
    /// <param name="filter">The xbox user ids to filter by.</param>
    inline void set_xbox_user_ids_filter(_In_ const std::vector<string_t>& filter);
    
    /// <summary>
    /// Filter result to just sessions with this keyword.
    /// </summary>
    inline string_t keyword_filter();

    /// <summary>
    /// Sets the xbox user ids filter. (The xboxUserIdFilter, xboxUserIdsFilter, or keywordFilter must be specified)
    /// </summary>
    /// <param name="filter">The keyword to filter by.</param>
    inline void set_keyword_filter(_In_ const string_t& filter);

    /// <summary>
    /// The name of the template for the multiplayer session to filter on.
    /// </summary>
    inline string_t session_template_name_filter();

    /// <summary>
    /// Sets the session template to filter by.
    /// </summary>
    /// <param name="filter">The session template name to filter by.</param>
    inline void set_session_template_name_filter(_In_ const string_t& filter);

    /// <summary>
    /// Filter result to just sessions with the specified visibility.
    /// </summary>
    inline multiplayer_session_visibility visibility_filter();

    /// <summary>
    /// Sets the session template to filter by.
    /// </summary>
    /// <param name="filter">The session template name to filter by.</param>
    inline void set_visibility_filter(_In_ multiplayer_session_visibility filter);

    /// <summary>
    /// Filter result to just sessions with this major version or less of the contract. (use 0 to ignore)
    /// </summary>
    inline uint32_t contract_version_filter();

    /// <summary>
    /// Sets the contract version to filter by.
    /// </summary>
    /// <param name="filter">The contract version to filter by.</param>
    inline void set_contract_version_filter(_In_ uint32_t filter);

private:
    XblMultiplayerSessionQuery m_request;
    std::vector<uint64_t> m_xuidFilters;
    std::string m_keywordFilter;
    friend class multiplayer_service;
};

/// <summary>
/// Queries for the all search handles that references the searchable sessions given the specific query.
/// There is no paging or continuation, and the multiplayer service will limit the number of items returned to 100.
/// </summary>
class multiplayer_query_search_handle_request
{
public:
    /// <summary> Creates a multiplayer_query_search_handle_request object.
    /// <param name="serviceConfigurationId">The scid within which to query for search handles.</param>
    /// <param name="sessionTemplateName">The name of the template to query for search handles.</param>
    /// </summary>
    inline multiplayer_query_search_handle_request(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& sessionTemplateName
        );

    /// <summary>
    /// The service configuration id that the sessions part of.
    /// </summary>
    inline const string_t& service_configuration_id() const;

    /// <summary>
    /// The name of the template that the sessions part of.
    /// </summary>
    inline const string_t& session_template_name() const;

    /// <summary>
    /// The attribute to sort the search handles by.
    /// </summary>
    inline const string_t& order_by() const;

    /// <summary>
    /// Specify the attribute to sort the search handles by.
    /// Valid values are "Timestamp desc", "Timestamp asc" or any Numbers search attribute followed by 'asc or 'desc' (ex: 'Numbers/gamerank asc')
    /// </summary>
    /// <param name="orderBy">Pass empty string to default to ordering by 'Timestamp asc'.</param>
    inline void set_order_by(_In_ const string_t& orderBy);

    /// <summary>
    /// The order to sort the search handles by.
    /// </summary>
    inline bool order_ascending() const;

    /// <summary>
    /// Specify the order to sort the search handles by.
    /// </summary>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    inline void set_order_ascending(_In_ bool orderAscending);

    /// <summary>
    /// The filter to search for.
    /// </summary>
    inline const string_t& search_filter() const;

    /// <summary>
    /// Specify the filter to search for.
    /// The filter syntax is an OData like syntax with only the following operators supported EQ, NE, GE, GT, LE and LT along with the logical operators of AND and OR.
    ///
    /// Example 1:
    /// To search for search handles for a specific XboxUserId use
    ///     "MemberXuids/any(d:d eq '12345678')" or "OwnerXuids/any(d:d eq '12345678')"
    ///
    /// Example 2:
    /// To search for search handles for a title defined string metadata use
    ///     "Strings/stringMetadataType eq 'value'"
    ///
    /// Example 3:
    /// To search for search handles for a title defined numbers metadata AND a tag type value use
    ///     "Numbers/numberMetadataType eq 53 AND Tags/tagType eq 'value'"
    /// </summary>
    /// <param name="searchFilter">The filter string to search for.</param>
    inline void set_search_filter(_In_ const string_t& searchFilter);

    /// <summary>
    /// The social group to get the search handles for.
    /// </summary>
    inline const string_t& social_group() const;

    /// <summary>
    /// Specify the social group to get the search handles for.
    /// </summary>
    /// <param name="socialGroup">The social group to use in order to get the list of users. (e.g. "people" or "favorites")</param>
    inline void set_social_group(_In_ const string_t& socialGroup);

private:
    string_t m_scid;
    string_t m_sessionTemplateName;
    string_t m_orderBy;
    bool m_orderAscending{ false };
    string_t m_searchFilter;
    string_t m_socialGroup;
};

/// <summary>
/// Sets the search handle based on the configuration of this request.
/// </summary>
class multiplayer_search_handle_request
{
public:
    /// <summary> Creates a multiplayer_search_handle_request object.</summary>
    /// <param name="sessionRef">The session referenceid that the sessions part of.</param>
    inline multiplayer_search_handle_request(
        _In_ multiplayer_session_reference sessionRef
    );

    /// <summary>
    /// The session reference that the sessions part of.
    /// </summary>
    inline multiplayer_session_reference session_reference() const;

    /// <summary>
    /// Filter result to just sessions with the tags set.
    /// </summary>
    inline std::vector<string_t> tags() const;

    /// <summary>
    /// Sets the tags to filter by.
    /// </summary>
    /// <param name="filter">The tags to filter by.</param>
    inline void set_tags(_In_ const std::vector<string_t>& value);

    /// <summary>
    /// Filter result to just sessions with the numbers metadata.
    /// </summary>
    inline std::unordered_map<string_t, double> numbers_metadata() const;

    /// <summary>
    /// Sets the numbers metadata to filter by.
    /// </summary>
    /// <param name="filter">The tags to filter by.</param>
    inline void set_numbers_metadata(_In_ const std::unordered_map<string_t, double>& metadata);

    /// <summary>
    /// Filter result to just sessions with the strings metadata.
    /// </summary>
    inline std::unordered_map<string_t, string_t> strings_metadata() const;

    /// <summary>
    /// Sets the strings metadata to filter by.
    /// </summary>
    /// <param name="filter">The tags to filter by.</param>
    inline void set_strings_metadata(_In_ const std::unordered_map<string_t, string_t>& metadata);

private:
    XblMultiplayerSessionReference m_sessionRef;
    std::vector<XblMultiplayerSessionTag> m_tags;
    std::vector<XblMultiplayerSessionStringAttribute> m_stringAttributes;
    std::vector<XblMultiplayerSessionNumberAttribute> m_numberAttributes;

    friend class multiplayer_service;
};

/// <summary>
/// Used to handle interactions with an Xbox Live service endpoint on a server. 
/// </summary>
class multiplayer_service
{
public:
    /// <summary>
    /// Writes a new or updated multiplayer session to the service.
    /// The passed multiplayerSession must have a valid multiplayer_session_reference set on it.
    /// </summary>
    /// <param name="multiplayerSession">A MultiplayerSession object that has been modified with the changes to write.</param>
    /// <param name="multiplayerSessionWriteMode">The type of write operation</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, a MultiplayerSession
    /// object is returned that contains the response returned from the server. Note that if you leave a session that you are the 
    /// the last member of and the sessionEmptyTimeout is equal to 0, then the session will be deleted immediately and a nullptr will be returned. </returns>
    /// <remarks>Calls V105 PUT /serviceconfigs/{serviceConfigurationId}/sessionTemplates/{sessiontemplateName}/sessions/{sessionName}</remarks>
    inline pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> write_session(
        _In_ std::shared_ptr<multiplayer_session> multiplayerSession,
        _In_ multiplayer_session_write_mode writeMode
        );

    /// <summary>
    /// Writes a new or updated multiplayer session to the service, using the specified handle to the session.  
    /// A handle is a service-side pointer to a session.  The handleid is a GUID identifier of the handle.  Callers will
    /// usually get the handleid either from another player's MultiplayerActivityDetails, or from an invite.
    ///
    /// Use this method only if your MultiplayerSession object doesn't have a multiplayer_session_reference, as  
    /// a handle's lifetime may be shorter than that of the session it points to.
    /// 
    /// </summary>
    /// <param name="multiplayerSession">A MultiplayerSession object that has been modified with the changes to write</param>
    /// <param name="multiplayerSessionWriteMode">The type of write operation</param>
    /// <param name="handleId">The ID of the handle that should be used when writing the session.</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, a MultiplayerSession
    /// object is returned that contains the response returned from the server. The returned MultiplayerSession will
    /// contain a multiplayer_session_reference, so may be used when calling write_session. Note that if you leave a session that you are the 
    /// the last member of and the sessionEmptyTimeout is equal to 0, then the session will be deleted immediately and a nullptr will be returned. </returns>
    /// <remarks>Calls V105 PUT /handles/{handleid}/session</remarks>
    inline pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> write_session_by_handle(
        _In_ std::shared_ptr<multiplayer_session> multiplayerSession,
        _In_ multiplayer_session_write_mode multiplayerSessionWriteMode,
        _In_ const string_t& handleId
        );

    /// <summary>
    /// Gets a session object with all its attributes from the server.
    /// </summary>
    /// <param name="sessionReference">A multiplayer_session_reference object containing identifying information for the session.</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, a MultiplayerSession
    /// object is returned that contains the response returned from the server.</returns>
    /// <remarks>Calls V102 GET /serviceconfigs/{serviceConfigurationId}/sessionTemplates/{sessiontemplateName}/sessions/{sessionName}</remarks>
    inline pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> get_current_session(
        _In_ multiplayer_session_reference sessionReference
        );

    /// <summary>
    /// Gets a session object with all its attributes from the server, given a session handle id.
    /// A handle is a service-side pointer to a session.  The handleid is a GUID identifier of the handle.  Callers will
    /// usually get the handleid either from another player's MultiplayerActivityDetails, or from an invite.
    /// </summary>
    /// <param name="handleId">A multiplayer handle id, which uniquely identifies the session.</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, a MultiplayerSession
    /// object is returned that contains the response returned from the server.</returns>
    /// <remarks>Calls GET /handles/{handleId}/session</remarks>
    inline pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> get_current_session_by_handle(
        _In_ const string_t& handleId
        );

    /// <summary>
    /// Retrieve a list of sessions with various filters
    /// </summary>
    /// <param name="getSessionRequest">A session request object that sends a query for the session </param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, a collection of
    /// SessionStates objects are returned where each contains metadata about one session.</returns>
    /// <remarks>Calls V102 GET /serviceconfigs/{scid}/sessions or /serviceconfigs/{scid}/sessiontemplates/{session-template-name}/sessions</remarks>
    inline pplx::task<xbox_live_result<std::vector<multiplayer_session_states>>> get_sessions(
        _In_ multiplayer_get_sessions_request getSessionsRequest
        );

    /// <summary>
    /// Sets the passed session as the user's current activity, which will be displayed in Xbox 
    /// dashboard user experiences (e.g. friends and gamercard) as associated with the currently 
    /// running title.  If the session is joinable, it may also be displayed as such in those 
    /// user experiences.
    /// </summary>
    /// <param name="sessionReference">A multiplayer_session_reference for the session of the activity</param>
    /// <returns>The async object for notifying when the operation is completed.</returns>
    inline pplx::task<xbox_live_result<void>> set_activity(_In_ multiplayer_session_reference sessionReference);

    /// <summary>
    /// The access rights the caller has to the origin session are extended to the target session.
    /// For example, in a title with a lobby session and a game session, the title could put a transfer handle 
    /// linking the lobby to the game inside the lobby session. Users invited to the lobby can use the handle to join the game session as well.
    /// The transfer handle is deleted once the target session is deleted.
    /// </summary>
    /// <param name="targetSessionReference">Target multiplayer_session_reference for the session you want to extend the access rights for</param>
    /// <param name="originSessionReference">Origin multiplayer_session_reference for the session that grants access to the target session</param>
    /// <returns>The async object for notifying when the operation is completed. This contains the transfer handle ID string.</returns>
    inline pplx::task<xbox_live_result<string_t>> set_transfer_handle(
        _In_ multiplayer_session_reference targetSessionReference,
        _In_ multiplayer_session_reference originSessionReference
        );

    /// <summary>
    /// Creates a search handle associated with the session. The visibility of the session is dependent on its search handle. 
    /// While you can create an searchable session, it is not queryable and visible to others unless you have the associated search handle committed as well. 
    /// </summary>
    /// <param name="searchHandleRequest">A search handle request object for the associated session</param>
    /// <returns>The async object for notifying when the operation is completed.</returns>
    inline pplx::task<xbox_live_result<void>> set_search_handle(
        _In_ multiplayer_search_handle_request searchHandleRequest
        );

    /// <summary>
    /// Clears the user's current activity session for the specified serviceConfigurationId
    /// </summary>
    /// <param name="serviceConfigurationId">A string containing the serviceConfigurationId in which to clear activity.</param>
    /// <returns>The async object for notifying when the operation is completed.</returns>
    inline pplx::task<xbox_live_result<void>> clear_activity(_In_ const string_t& serviceConfigurationId);

    /// <summary>
    /// Clears the search handle that is associated with the session.
    /// </summary>
    /// <param name="handleId">The handleId associated with the session to clear.</param>
    /// <returns>The async object for notifying when the operation is completed.</returns>
    inline pplx::task<xbox_live_result<void>> clear_search_handle(_In_ const string_t& handleId);

    /// <summary>
    /// Invites the specified users to a session.  This will result in a notification being shown to
    /// each invited user using standard invite text.  If a user accepts that notification the title will be activated.
    /// </summary>
    /// <param name="sessionReference">A multiplayer_session_reference object representing the session the target users will be invited to.</param>
    /// <param name="xboxUserIds">The list of xbox user IDs who will be invited.</param>
    /// <param name="titleId">The ID of the title that the invited user will activate in order to join the session.</param>
    /// <returns>The async object for notifying when the operation is completed.  This contains a vectorview of handle ID strings corresponding to the invites that have been sent.</returns>
    inline pplx::task<xbox_live_result<std::vector<string_t>>> send_invites(
        _In_ multiplayer_session_reference sessionReference,
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ uint32_t titleId
        );

    /// <summary>
    /// Invites the specified users to a session.  This will result in a notification being shown to
    /// each invited user.  If a user accepts that notification the title will be activated.
    /// </summary>
    /// <param name="sessionReference">A multiplayer_session_reference object representing the session the target users will be invited to.</param>
    /// <param name="xboxUserIds">The list of xbox user IDs who will be invited.</param>
    /// <param name="titleId">The ID of the title that the invited user will activate in order to join the session.</param>
    /// <param name="contextStringId">The custom context string ID.  This string ID is defined 
    /// during Xbox Live ingestion to identify the invitation text that is additional to the standard 
    /// invitation text. The ID string must be prefixed with "///".  Pass an empty string if 
    /// you don't want a custom string added to the invite.</param>
    /// <param name="customActivationContext">The activation context string. A game defined string that is passed to the invited game client and interpreted as desired. (Optional)</param>
    /// <returns>The async object for notifying when the operation is completed.  This contains a vectorview of handle ID strings corresponding to the invites that have been sent.</returns>
    inline pplx::task<xbox_live_result<std::vector<string_t>>> send_invites(
        _In_ multiplayer_session_reference sessionReference,
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ uint32_t titleId,
        _In_ const string_t& contextStringId,
        _In_ const string_t& customActivationContext
        );

    /// <summary>
    /// Queries for the current activity for a socialgroup of users associated with a particular
    /// "owner" user.  There is no paging or continuation, and the multiplayer service will
    /// limit the number of items returned to 100.
    /// </summary>
    /// <param name="serviceConfigurationId">The scid within which to query for activities.</param>
    /// <param name="socialGroupOwnerXboxUserId">The person whose social group will be used for the query.</param>
    /// <param name="socialGroup">The social group to use in order to get the list of users. (e.g. "friends" or "favorites")</param>
    /// <returns>The async object for notifying when the operation is completed.  This contains a vectorview of MultiplayerActivityDetails objects, containing the details of the activities of the targeted users.</returns>
    inline pplx::task<xbox_live_result<std::vector<multiplayer_activity_details>>> get_activities_for_social_group(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& socialGroupOwnerXboxUserId,
        _In_ const string_t& socialGroup
        );

    /// <summary>
    /// Queries for the current activity for a set of users specified by xuid.  
    /// There is no paging or continuation, and the multiplayer service will limit the 
    /// number of items returned to 100.
    /// </summary>
    /// <param name="serviceConfigurationId">The scid within which to query for activities.</param>
    /// <param name="xboxUserIds">The list of user ids to find activities for.</param>
    /// <returns>The async object for notifying when the operation is completed.  This contains a vectorview of MultiplayerActivityDetails objects, containing the details of the activities of the targeted users.</returns>
    inline pplx::task<xbox_live_result<std::vector<multiplayer_activity_details>>> get_activities_for_users(
        _In_ const string_t& serviceConfigurationId,
        _In_ const std::vector<string_t>& xboxUserIds
        );

    /// <summary>
    /// Queries for the all search handles that references the searchable sessions given the specific query.
    /// There is no paging or continuation, and the multiplayer service will limit the number of items returned to 100.
    /// Call get_search_handles(multiplayer_query_search_handle_request) instead.
    /// </summary>
    /// <param name="serviceConfigurationId">The scid within which to query for search handles.</param>
    /// <param name="sessionTemplateName">The name of the template to query for search handles.</param>
    /// <param name="orderBy">This specifies the attribute to sort the search handles by.  Pass empty string to default to ordering by 'Timestamp asc'. (Optional)</param>
    /// <param name="orderAscending">Pass true to order ascending, false to order descending</param>
    /// <param name="searchFilter">The query string to get the search handles for. (Optional)
    /// The search query.
    /// The query syntax is an OData like syntax with only the following operators supported EQ, NE, GE, GT, LE and LT along with the logical operators of AND and OR.
    ///
    /// Example 1:
    /// To search for search handles for a specific XboxUserId use
    ///     "MemberXuids/any(d:d eq '12345678')" or "OwnerXuids/any(d:d eq '12345678')"
    ///
    /// Example 2:
    /// To search for search handles for a title defined string metadata use
    ///     "Strings/stringMetadataType eq 'value'"
    ///
    /// Example 3:
    /// To search for search handles for a title defined numbers metadata AND a tag type value use
    ///     "Numbers/numberMetadataType eq 53 AND Tags/tagType eq 'value'"
    /// </param>
    /// <returns>The async object for notifying when the operation is completed.  This contains a vectorview of multiplayer_search_handle_details objects, containing the details of the search handles.</returns>
    inline pplx::task<xbox_live_result<std::vector<multiplayer_search_handle_details>>> get_search_handles(
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& sessionTemplateName,
        _In_ const string_t& orderBy,
        _In_ bool orderAscending,
        _In_ const string_t& searchFilter
        );

    /// <summary>
    /// Queries for the all search handles that references the searchable sessions given the specific query.
    /// There is no paging or continuation, and the multiplayer service will limit the number of items returned to 100.
    /// </summary>
    /// <param name="searchHandleRequest"> A search handle request object that queries for the all search handles.</param>
    /// <returns>The async object for notifying when the operation is completed.  This contains a vectorview of multiplayer_search_handle_details objects, containing the details of the search handles.</returns>
    inline pplx::task<xbox_live_result<std::vector<multiplayer_search_handle_details>>> get_search_handles(
        _In_ const multiplayer_query_search_handle_request& searchHandleRequest
        );

    /// <summary>
    /// Starts multiplayerservice connectivity via RTA, for two purposes:
    /// 1. subscriptions to changes on specific sessions, using the MultiplayerSession object.
    /// 2. automatic removal of members from sessions when the collection underlying this multiplayer subscription is broken.
    /// This method does not actually make the connection, but enables the connection, and helps track its lifetime.
    /// This method will fail if called twice, unless the multiplayerconnection has been lost or stopped in the interim.  
    /// This can be detected by listening for the MultiplayerSubscriptionsLost event.
    /// </summary>
    inline std::error_code enable_multiplayer_subscriptions();

    /// <summary>
    /// Stops multiplayerservice connectivity via RTA.
    /// When stopping multiplayer is complete, a MultiplayerSubscriptionsLost event will fire.
    /// It is not necessary to wait for this event to fire, unless you intend to call EnableMultiplayerSubscriptions() to re-start it,
    /// in which case you must wait for the stop to complete.
    /// </summary>
    inline void disable_multiplayer_subscriptions();

    /// <summary>
    /// Indicates whether multiplayer subscriptions are currently enabled.  
    /// </summary>
    inline bool subscriptions_enabled();

    /// <summary>
    /// Registers an event handler for notifications when a multiplayer session changes.
    /// Event handlers receive a multiplayer_session_change_event_args&amp; object.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    /// <remarks>
    /// Session subscriptions are created using the multiplayer_session object within the multiplayer namespace.
    /// </remarks>
    inline function_context add_multiplayer_session_changed_handler(_In_ std::function<void(const multiplayer_session_change_event_args&)> handler);

    /// <summary>
    /// Unregisters an event handler for multiplayer session change notifications.
    /// </summary>
    /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
    /// <param name="context">The callback function that receives notifications.</param>
    inline void remove_multiplayer_session_changed_handler(_In_ function_context context);

    /// <summary>
    /// Registers an event handler for notifications when a multiplayer subscription is lost.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    /// <remarks>
    /// Session subscriptions are created using the multiplayer_session object within the multiplayer namespace.
    /// </remarks>
    inline function_context add_multiplayer_subscription_lost_handler(_In_ std::function<void()> handler);

    /// <summary>
    /// Unregisters an event handler for multiplayer subscription lost notifications.
    /// </summary>
    /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
    /// <param name="handler">The callback function that receives notifications.</param>
    inline void remove_multiplayer_subscription_lost_handler(_In_ function_context context);

    /// <summary>
    /// Registers an event handler for notifications when a multiplayer connection id is changed.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    /// <remarks>
    /// Connection id maps the multiplayer service to rta.
    /// </remarks>
    inline function_context add_multiplayer_connection_id_changed_handler(_In_ std::function<void()> handler);

    /// <summary>
    /// Unregisters an event handler for multiplayer connection id changed notifications.
    /// </summary>
    /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
    /// <param name="handler">The callback function that receives notifications.</param>
    inline void remove_multiplayer_connection_id_changed_handler(_In_ function_context context);

    inline multiplayer_service(const multiplayer_service& other);
    inline multiplayer_service& operator=(multiplayer_service other);
    inline ~multiplayer_service();

private:
    inline multiplayer_service(_In_ XblContextHandle contextHandle);

    XblContextHandle m_handle;
    struct HandlerContext;

    friend xbox_live_context;
};
}}}

#include "impl/multiplayer.hpp"

#endif