// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>
#include "pplx/pplxtasks.h"
#include "xsapi-cpp/types.h"
#include "xsapi-c/multiplayer_manager_c.h"

typedef void* context_t;

namespace xbox { namespace services { 
    /// <summary>
    /// Contains classes related to multiplayer.
    /// </summary>
    namespace multiplayer { 
        /// <summary>
        /// Contains classes and enumerations for more easily managing multiplayer
        /// scenarios.
        /// </summary>
        namespace manager {

/// <summary>
/// Defines values used to indicate who can join your lobby.
/// </summary>
enum class joinability
{
    /// <summary>
    /// Joinability not set or no lobby exists yet.
    /// </summary>
    none,

    /// <summary>
    /// Default value. The lobby is joinable by users who are followed by an existing member of the session.
    /// </summary>
    joinable_by_friends,

    /// <summary>
    /// The lobby is joinable only via an invite.
    /// </summary>
    invite_only,

    /// <summary>
    /// This option will close the lobby only when a game is in progress. All other times,
    /// it will keep the lobby open for invite_only so invitees can join when no game is in progress.
    /// </summary>
    disable_while_game_in_progress,

    /// <summary>
    /// This option will close the lobby immediately.
    /// </summary>
    closed
};

/// <summary>
/// Defines values used to indicate status for the matchmaking stages.
/// </summary>
enum match_status
{
    /// <summary>
    /// Indicates no matchmaking search has been started.
    /// </summary>
    none,

    /// <summary>
    /// Indicates that a match ticket was submitted for matchmaking.
    /// </summary>
    submitting_match_ticket,

    /// <summary>
    /// Indicates that matchmaking is still searching.
    /// </summary>
    searching,

    /// <summary>
    /// Indicates that matchmaking search has found a match.
    /// </summary>
    found,

    /// <summary> 
    /// Joining initialization stage.
    /// Matchmaking creates the game session and adds users to it.
    /// The client has up to the joining timeout to join the session during this phase.
    /// </summary>
    joining,

    /// <summary> 
    /// Waiting for remote clients to join the game session.
    /// The client has up to the joining timeout to join the session during this phase.
    /// </summary>
    waiting_for_remote_clients_to_join,

    /// <summary>
    /// Measuring initialization stage.
    /// Stage where QoS measurement happens.
    /// The client has up to the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    measuring,

    /// <summary>
    /// Uploading QoS measurement results to the service.
    /// The client has up to the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    uploading_qos_measurements,

    /// <summary>
    /// Waiting for remote clients to upload QoS measurement results to the service.
    /// The client has up to the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    waiting_for_remote_clients_to_upload_qos,

    /// <summary>
    /// Evaluating initialization stage.
    /// If auto evaluate is true, then this stage is skipped.  
    /// Otherwise the title will do its own evaluation.
    /// </summary>
    evaluating,

    /// <summary>
    /// Match was found and QoS measurement was successful.
    /// </summary>
    completed,

    /// <summary>
    /// If the match that was found was not successful and is resubmitting.
    /// </summary>
    resubmitting,

    /// <summary>
    /// Indicates that matchmaking search has expired.
    /// </summary>
    expired,

    /// <summary>
    /// Indicates that matchmaking is in the process of canceling the search.
    /// </summary>
    canceling,

    /// <summary>
    /// Indicates that matchmaking search has been canceled.
    /// </summary>
    canceled,

    /// <summary>
    /// Failed initialization stage.
    /// The initialization failed.
    /// </summary>
    failed,
};

/// <summary>
/// Defines values used to indicate event types for a multiplayer lobby or game.
/// </summary>
enum class multiplayer_event_type
{
    /// <summary>
    /// Indicates the user was added.
    /// </summary>
    user_added,
    
    /// <summary>
    /// Indicates the user was removed.
    /// </summary>
    user_removed,
    
    /// <summary>
    /// Indicates a new member has joined the session.
    /// The event_args object should be cast to a
    /// member_joined_event_args object for more information.
    /// </summary>
    member_joined,

    /// <summary>
    /// Indicates a member has left the session.
    /// The event_args object should be cast to a
    /// member_left_event_args object for more information.
    /// </summary>
    member_left,

    /// <summary>
    /// Indicates a member property has changed.
    /// The event_args object should be cast to a
    /// member_property_changed_event_args object for more information.
    /// </summary>
    member_property_changed,

    /// <summary>
    /// Indicates that the set_local_member_properties() or delete_local_member_properties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write local member properties by calling the set_local_member_properties() operation.
    /// </summary>
    local_member_property_write_completed,

    /// <summary>
    /// Indicates that the set_local_member_connection_address() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write local member properties by calling the set_local_member_connection_address() operation.
    /// </summary>
    local_member_connection_address_write_completed,

    /// <summary>
    /// Indicates a session (lobby or game) property has changed.
    /// The event_args object should be cast to a
    /// session_property_changed_event_args object for more information.
    /// </summary>
    session_property_changed,

    /// <summary>
    /// Indicates that the set_synchronized_properties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write synchronized properties by calling the set_properties() operation.
    /// </summary>
    session_property_write_completed,

    /// <summary>
    /// Indicates that the set_synchronized_properties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write synchronized properties by calling the set_synchronized_properties() operation.
    /// </summary>
    session_synchronized_property_write_completed,

    /// <summary>
    /// Indicates host has changed.
    /// The event_args object should be cast to a
    /// host_changed_event_args object for more information.
    /// </summary>
    host_changed,

    /// <summary>
    /// Indicates that the set_synchronized_host() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write synchronized host by calling the set_synchronized_host() operation.
    /// </summary>
    synchronized_host_write_completed,

    /// <summary>
    /// Indicates that the joinability value has changed.
    /// A game can be change the state by calling the set_joinability() operation.
    /// </summary>
    joinability_state_changed,

    /// <summary>
    /// Fired when a match has been found, and the client has joined the target game session.
    /// When this event occurs, title should provide qos measurement results (via set_quality_of_service_measurements) between itself and a list of remote clients.
    /// Note: If your title does not require QoS (based on the session template), this event will not be triggered.
    /// </summary>
    perform_qos_measurements,

    /// <summary>
    /// Indicates that the find_match() operation has completed.
    /// The event_args object should be cast to a
    /// find_match_completed_event_args object for more information.
    /// </summary>
    find_match_completed,

    /// <summary>
    /// Indicates that the join_game() operation has completed. Once the join succeeds, 
    /// the member is now part of the game session, and can use data in the session 
    /// to connect to other game members.
    /// </summary>
    join_game_completed,

    /// <summary>
    /// Indicates that the leave_game() operation has completed. After receiving this event,
    /// the game session object will be set to null. You can join another game by calling 
    /// join_game() or join_game_from_lobby().
    /// </summary>
    leave_game_completed,

    /// <summary>
    /// Indicates that the join_lobby() operation has completed. Once the join succeeds, 
    /// the member is now part of the lobby session, and can use data in the session 
    /// to connect to other lobby members. The event_args object should be cast to a
    /// join_lobby_completed_event_args object for more information.
    /// </summary>
    join_lobby_completed,

    /// <summary>
    /// Fired when the title's connection to MPSD using the real-time activity service is lost. 
    /// When this event occurs, the title should shut down the multiplayer.
    /// </summary>
    client_disconnected_from_multiplayer_service,

    /// <summary>
    /// Indicates that the invite API operation has been completed.
    /// </summary>
    invite_sent
};

/// <summary>
/// Defines values used to indicate types for multiplayer sessions.
/// </summary>
enum class multiplayer_session_type
{
    /// <summary>
    /// The session type is unknown.
    /// </summary>
    unknown,

    /// <summary>
    /// Indicates multiplayer lobby session.
    /// </summary>
    lobby_session,

    /// <summary>
    /// Indicates multiplayer game session.
    /// </summary>
    game_session,

    /// <summary>
    /// Indicates multiplayer match session.
    /// </summary>
    match_session
};

/// <summary>
/// Represents a reference to a member in a multiplayer game.
/// </summary>
class multiplayer_member
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_member(_In_ const XblMultiplayerManagerMember& internalMember) : m_internalMember(internalMember) {}

    /// <summary>
    /// Id for the member.
    /// </summary>
    inline uint32_t member_id() const;

    /// <summary> 
    /// Only applicable if you are using Team rules with Smart Match. 
    /// Initial team assignment suggested by Smart Match. 
    /// </summary> 
    inline string_t initial_team() const;

    /// <summary>
    /// Xbox User ID of the member.
    /// </summary>
    inline string_t xbox_user_id() const;

    /// <summary>
    /// The Gamertag of the member. This is only to be used for debugging purposes as this gamertag may be out of date.
    /// It is recommended you use social manager or the profile service to get this information.
    /// </summary>
    inline string_t debug_gamertag() const;

    /// <summary>
    /// Indicates if this member is playing on the local device.
    /// </summary>
    inline bool is_local() const;

    /// <summary>
    /// Indicates if this member is part of the lobby.
    /// </summary>
    inline bool is_in_lobby() const;

    /// <summary>
    /// Indicates if this member is part of the game.
    /// </summary>
    inline bool is_in_game() const;

    /// <summary>
    /// The status of this member.
    /// </summary>
    inline xbox::services::multiplayer::multiplayer_session_member_status status() const;

    /// <summary>
    /// The address used for network connection.
    /// This can be used for secure socket connection.
    /// </summary>
    inline string_t connection_address() const;

    /// <summary>
    /// JSON value that specify the custom properties of the member.
    /// </summary>
    inline web::json::value properties() const;

    /// <summary>
    /// Determines whether the member is on the same device.
    /// </summary>
    inline bool is_member_on_same_device(_In_ std::shared_ptr<multiplayer_member> member) const;

private:
    const XblMultiplayerManagerMember m_internalMember;
    friend class multiplayer_lobby_session;
    friend class multiplayer_game_session;
};

/// <summary>
/// Represents a multiplayer lobby. This is also where you would manage members that are local to this device.
/// There are 2 game objects when using a multiplayer manager.  One represents the lobby_session() which is where friends you invite will join.  
/// Another is the game_session() which contains people that your lobby has been matched with.
/// </summary>
class multiplayer_lobby_session
{
public:
    /// <summary>
    /// A unique ID to the session used to query trace logs for entries that relate to the session.
    /// </summary>
    inline string_t correlation_id() const;

    /// <summary>
    /// Object containing identifying information for the session.
    /// </summary>
    inline xbox::services::multiplayer::multiplayer_session_reference session_reference() const;

    /// <summary>
    /// A collection of members that are local to this device.
    /// </summary>
    inline std::vector<std::shared_ptr<multiplayer_member>> local_members() const;

    /// <summary>
    /// A collection of members that are in the lobby. When a friend accepts a game invite, 
    /// members will be added to the lobby.
    /// </summary>
    inline std::vector<std::shared_ptr<multiplayer_member>> members() const;

    /// <summary>
    /// Returns the host member for the lobby.
    /// The host is defined as the user with the lowest index on the host device.
    /// </summary>
    inline std::shared_ptr<multiplayer_member> host() const;

    /// <summary>
    /// JSON string that specify the custom properties for the game.  These can be changed anytime.
    /// </summary>
    inline web::json::value properties() const;

    /// <summary>
    /// A set of constants associated with this session. These can only be set through the session template.
    /// </summary>
    inline const std::shared_ptr<xbox::services::multiplayer::multiplayer_session_constants> session_constants() const;

    /// <summary>
    /// Changes are batched and written to the service on the next do_work(). All session properties and members
    /// contain updated response returned from the server upon calling do_work().
    /// Hosts a new lobby when the first user is added. For all other users, they will be added to the existing lobby
    /// as secondary users. This API will also advertise the lobby for friends to join.
    /// You can send invites, set lobby properties, access lobby members via lobby() only once you've added the local user.
    /// While joining a lobby via an invite, or a handleId, you can skip adding the local user to avoid creating a lobby and 
    /// instead pass in the list of users in the join_lobby() API.
    /// </summary>
    /// <param name="user">The associated system User.</param>
    inline xbox_live_result<void> add_local_user(
        _In_ xbox_live_user_t user
    );

    /// <summary>
    /// Changes are batched and written to the service on the next do_work(). All session properties and members
    /// contain updated response returned from the server upon calling do_work().
    /// Removes the local user from the lobby and game session.
    /// After this method is called, if no local users are active, title will not be able to perform any further multiplayer operations.
    /// You can join another game or re-add the local user.
    /// </summary>
    /// <param name="user">The associated system User.</param>
    inline xbox_live_result<void> remove_local_user(
        _In_ xbox_live_user_t user
    );

    /// <summary>
    /// Set a custom property on the local member to the specified JSON string
    /// Changes are batched and written to the service on the next do_work(). All session properties and members
    /// contain updated response returned from the server upon calling do_work().
    /// The result is delivered via multiplayer_event callback of type local_member_property_write_completed through do_work().
    /// </summary>
    /// <param name="user">The associated system User you want to set the property for.</param>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_local_member_properties(
        _In_ xbox_live_user_t user,
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson,
        _In_opt_ context_t context = nullptr
    );

    /// <summary>
    /// Delete a custom property on the local member
    /// Changes are batched and written to the service on the next do_work(). All session properties and members
    /// contain updated response returned from the server upon calling do_work().
    /// The result is delivered via multiplayer_event callback of type local_member_property_write_completed through do_work().
    /// </summary>
    /// <param name="user">The associated system User you want to delete the property for.</param>
    /// <param name="name">The name of the property to delete</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> delete_local_member_properties(
        _In_ xbox_live_user_t user,
        _In_ const string_t& name,
        _In_opt_ context_t context = nullptr
    );

    /// <summary>
    /// Set connection address for the local member. The address can be used for network and secure socket connection.
    /// Changes are batched and written to the service on the next do_work(). All session properties and members
    /// contain updated response returned from the server upon calling do_work().
    /// The result is delivered via multiplayer_event callback of type local_member_connection_address_write_completed through do_work().
    /// </summary>
    /// <param name="user">The associated system User you want to set the property for.</param>
    /// <param name="connectionAddress">The network connection address to set.</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_local_member_connection_address(
        _In_ xbox_live_user_t user,
        _In_ const string_t& connectionAddress,
        _In_opt_ context_t context = nullptr
    );

    /// <summary>
    /// Whether or not the Xbox User ID is the host.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user</param>
    inline bool is_host(
        _In_ const string_t& xboxUserId
    );

    /// <summary>
    /// Set a custom game property to the specified JSON string.
    /// Changes are batched and written to the service on the next do_work(). All session properties and members
    /// contain updated response returned from the server upon calling do_work().
    /// The result is delivered via multiplayer_event callback of type session_property_write_completed through do_work().
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_properties(
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson,
        _In_opt_ context_t context = nullptr
    );

    /// <summary>
    /// Sets a custom property to the specified JSON string using multiplayer_session_write_mode::synchronized_update.
    /// Use this method to resolve any conflicts between devices while trying to set properties to a shared portion that other 
    /// devices can also modify. It ensures that updates to the session are atomic. If writing to non-sharable properties, use set_properties() instead.
    /// The service may reject your request if a race condition occurred (due to a conflict) resulting in error_code 
    /// http_status_412_precondition_failed (HTTP status 412). To resolve this, evaluate the need to write again and re-submit if needed.
    /// The result is delivered via multiplayer_event callback of type session_synchronized_property_write_completed through do_work().
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_synchronized_properties(
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson,
        _In_opt_ context_t context = nullptr
    );

    /// <summary>
    /// Sets the host for the game using multiplayer_session_write_mode::synchronized_update. Use this method to resolve
    /// any conflicts between devices trying to set the host at the same time. It ensures that updates to the session are atomic. 
    /// The service may reject your request if a race condition occurred(due to a conflict) resulting in error_code
    /// http_status_412_precondition_failed (HTTP status 412). To resolve this, evaluate the need to write again and re-submit if needed.
    /// The result is delivered via multiplayer_event callback of type synchronized_host_write_completed through do_work().
    /// </summary>
    /// <param name="gameHost">The host member.</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_synchronized_host(
        _In_ std::shared_ptr<multiplayer_member> gameHost,
        _In_opt_ context_t context = nullptr
    );

#if HC_PLATFORM_IS_MICROSOFT
    /// <summary>
    /// Displays the invite UI and allows the user to select people from the user's people list and invite them to join the user's party
    /// If a user accepts that notification the title will be activated.
    /// </summary>
    /// <param name="user">The associated system User.</param>
    /// <param name="contextStringId">The custom context string ID.  This string ID is defined 
    /// during Xbox Live ingestion to identify the invitation text that is additional to the standard 
    /// invitation text. The ID string must be prefixed with "///". (Optional)</param>
    /// <param name="customActivationContext">The activation context string. A game defined string that is passed to the invited game client and interpreted as desired. (Optional)</param>
    inline xbox_live_result<void> invite_friends(
        _In_ xbox_live_user_t user,
        _In_ const string_t& contextStringId = string_t(),
        _In_ const string_t& customActivationContext = string_t()
    );
#endif

    /// <summary>
    /// Invites the specified users to a game.  This will result in a notification being shown to
    /// each invited user using standard invite text.  If a user accepts that notification the title will be activated.
    /// </summary>
    /// <param name="user">The associated system User.</param>
    /// <param name="xboxUserIds">The list of xbox user IDs who will be invited.</param>
    /// <param name="contextStringId">The custom context string ID.  This string ID is defined 
    /// during Xbox Live ingestion to identify the invitation text that is additional to the standard 
    /// invitation text. The ID string must be prefixed with "///". (Optional)</param>
    /// <param name="customActivationContext">The activation context string. A game defined string that is passed to the invited game client and interpreted as desired. (Optional)</param>
    /// <returns>The async object for notifying when the operation is completed.</returns>
    inline xbox_live_result<void> invite_users(
        _In_ xbox_live_user_t user,
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const string_t& contextStringId = string_t(),
        _In_ const string_t& customActivationContext = string_t()
    );

private:
    mutable std::shared_ptr<xbox::services::multiplayer::multiplayer_session_constants> m_sessionConstants;
};

/// <summary>
/// Represents a multiplayer game.
/// There are 2 game objects when using a multiplayer manager.  One represents the lobby_session() which is where friends you invite will join.  
/// Another is the game_session() which contains people that your lobby has been matched with.
/// </summary>
class multiplayer_game_session
{
public:
    /// <summary>
    /// A unique ID to the session used to query trace logs for entries that relate to the session.
    /// </summary>
    inline string_t correlation_id() const;

    /// <summary>
    /// Object containing identifying information for the session.
    /// </summary>
    inline xbox::services::multiplayer::multiplayer_session_reference session_reference() const;

    /// <summary>
    /// A collection of members that are in the game. When a friend accepts a game invite, 
    /// members will be added to the lobby and the game session members list.
    /// </summary>
    inline std::vector<std::shared_ptr<multiplayer_member>> members() const;

    /// <summary>
    /// Returns the host member for the game.
    /// The host is defined as the user with the lowest index on the host device.
    /// </summary>
    inline std::shared_ptr<multiplayer_member> host() const;

    /// <summary>
    /// JSON string that specify the custom properties for the game.  These can be changed anytime.
    /// </summary>
    inline web::json::value properties() const;

    /// <summary>
    /// A set of constants associated with this session. These can only be set through the session template.
    /// </summary>
    inline const std::shared_ptr<xbox::services::multiplayer::multiplayer_session_constants> session_constants() const;

    /// <summary>
    /// Whether or not the Xbox User ID is the host.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the user</param>
    inline bool is_host(
        _In_ const string_t& xboxUserId
        );

    /// <summary>
    /// Set a custom game property to the specified JSON string.
    /// Changes are batched and written to the service on the next do_work(). All session properties and members
    /// contain updated response returned from the server upon calling do_work().
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_properties(
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson,
        _In_opt_ context_t context = nullptr
        );

    /// <summary>
    /// Sets a custom property to the specified JSON string using multiplayer_session_write_mode::synchronized_update.
    /// Use this method to resolve any conflicts between devices while trying to set properties to a shared portion that other 
    /// devices can also modify. It ensures that updates to the session are atomic. If writing to non-sharable properties, use set_properties() instead.
    /// The service may reject your request if a race condition occurred (due to a conflict) resulting in error_code 
    /// http_status_412_precondition_failed (HTTP status 412). To resolve this, evaluate the need to write again and re-submit if needed.
    /// The result is delivered via multiplayer_event callback of type synchronized_property_write_completed through do_work().
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_synchronized_properties(
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson,
        _In_opt_ context_t context = nullptr
        );

    /// <summary>
    /// Sets the host for the game using multiplayer_session_write_mode::synchronized_update. Use this method to resolve
    /// any conflicts between devices trying to set the host at the same time. It ensures that updates to the session are atomic. 
    /// The service may reject your request if a race condition occurred(due to a conflict) resulting in error_code
    /// http_status_412_precondition_failed (HTTP status 412). To resolve this, evaluate the need to write again and re-submit if needed.
    /// The result is delivered via multiplayer_event callback of type synchronized_host_write_completed through do_work().
    /// </summary>
    /// <param name="gameHost">The host member.</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_synchronized_host(
        _In_ std::shared_ptr<multiplayer_member> gameHost,
        _In_opt_ context_t context = nullptr
        );

private:
    mutable std::shared_ptr<xbox::services::multiplayer::multiplayer_session_constants> m_sessionConstants;
};

class multiplayer_event_args
{
public:
    inline multiplayer_event_args(XblMultiplayerEventArgsHandle argsHandle);
    inline virtual ~multiplayer_event_args();

protected:
    inline string_t GetXuid() const;
    inline std::vector<std::shared_ptr<multiplayer_member>> GetMembers() const;
    inline std::shared_ptr<multiplayer_member> GetMember() const;
    inline web::json::value GetPropertiesJson() const;

    XblMultiplayerEventArgsHandle m_argsHandle;
};

/// <summary>
/// Notifies the title when a new user was added.
/// </summary>
class user_added_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    user_added_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// Xbox User ID of the member that that was added.
    /// </summary>
    string_t xbox_user_id() const
    {
        return GetXuid();
    }
};

/// <summary>
/// Notifies the title when a user was removed.
/// </summary>
class user_removed_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    user_removed_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// Xbox User ID of the member that that was removed.
    /// </summary>
    string_t xbox_user_id() const
    {
        return GetXuid();
    }
};

/// <summary>
/// Notifies the title when a new game member joins the game. 
/// </summary>
class member_joined_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    member_joined_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// A list of members that joined the game. 
    /// </summary>
    std::vector<std::shared_ptr<multiplayer_member>> members()
    {
        return GetMembers();
    }
};

/// <summary>
/// Notifies the title when an existing game member leaves the game. 
/// </summary>
class member_left_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    member_left_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// A list of members that left the game. 
    /// </summary>
    std::vector<std::shared_ptr<multiplayer_member>> members()
    {
        return GetMembers();
    }
};

/// <summary>
/// Notifies the title when a new host member has been set. 
/// </summary>
class host_changed_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    host_changed_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// The new host member. If an existing host leaves, the host_member() will be nullptr.
    /// </summary>
    std::shared_ptr<multiplayer_member> host_member()
    {
        return GetMember();
    }
};

/// <summary>
/// Notifies the title when a game member property has been added or modified. 
/// </summary>
class member_property_changed_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    member_property_changed_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// The member whose property changed.
    /// </summary>
    std::shared_ptr<multiplayer_member> member()
    {
        return GetMember();
    }

    /// <summary>
    /// The JSON of the property that changed.
    /// </summary>
    web::json::value properties()
    {
        return GetPropertiesJson();
    }
};

/// <summary>
/// Notifies the title when a session property has been added or modified.
/// </summary>
class session_property_changed_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    session_property_changed_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// The JSON of the property that changed.
    /// </summary>
    web::json::value properties()
    {
        return GetPropertiesJson();
    }
};

/// <summary>
/// Notifies the title when the client joins a lobby. Once join succeeds, the member is now 
/// part of the lobby session, and can use data in the session to connect to other lobby members.
/// To join a friend's lobby, call join_lobby(handleId) operation using the handleId you back get from
/// multiplayer_service::get_activities_for_social_group. If the user accepts an invite or joined a friends' game via the shell, 
/// the title will get protocol activated, in which case you should call join_lobby(IProtocolActivatedEventArgs^).
/// 
/// For scenarios where the local user has not been added, you can pass the local user object part of the join_lobby API.
/// If the invited user is not added via add_local_user() or through join_lobby, then join_lobby() will fail and 
/// provide the invited_xbox_user_id() that the invite was sent for as part of the join_lobby_completed_event_args().
/// </summary>
class join_lobby_completed_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    join_lobby_completed_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// Invited Xbox User ID of the member that the invite was sent for.
    /// </summary>
    string_t invited_xbox_user_id() const
    {
        return GetXuid();
    }
};

/// <summary>
/// Contains information for an event that indicates when a multiplayer match is found.
/// </summary>
class find_match_completed_event_args : public multiplayer_event_args
{
public:
    find_match_completed_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}

    /// <summary>
    /// Provides the current matchmaking status.
    /// </summary>
    inline xbox::services::multiplayer::manager::match_status match_status() const;

    /// <summary>
    /// The cause of why the initialization failed, or multiplayer_measurement_failure::None if there was no failure.
    /// Set when transitioning out of the "joining" or "measuring" stage if this member doesn't pass.
    /// </summary>
    inline xbox::services::multiplayer::multiplayer_measurement_failure initialization_failure_cause() const;
};

/// <summary>
/// Notifies the title when it should provide qos measurement results between itself and a list of remote clients.
/// </summary>
class perform_qos_measurements_event_args : public multiplayer_event_args
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    perform_qos_measurements_event_args(XblMultiplayerEventArgsHandle handle) : multiplayer_event_args(handle) {}
    
    /// <summary>
    /// A map of connection addresses and device tokens to run qos on.
    /// </summary>
    inline std::map<string_t, string_t> connection_address_to_device_tokens() const;
};

/// <summary>
/// Base class for all event arg classes. Based on the multiplayer_event_type, you would need to cast the args
/// to the appropriate class. Note that the event will only be valid until the next call to multiplayer_manager::do_work().
/// </summary>
class multiplayer_event
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    inline multiplayer_event(_In_ const XblMultiplayerEvent* internalEvent);

    /// <summary>
    /// The error code indicating the result of the operation.
    /// </summary>
    inline std::error_code err() const;

    /// <summary>
    /// Returns call specific debug information if join fails
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    inline std::string err_message() const;

    /// <summary>
    /// A pointer to the application-defined data passed into the initiating method.
    /// </summary>
    context_t inline context();

    /// <summary>
    /// Type of the event triggered.
    /// </summary>
    inline multiplayer_event_type event_type() const;

    /// <summary>
    /// You need to cast this to one of the event arg classes to retrieve the data for that particular event.
    /// </summary>
    inline std::shared_ptr<multiplayer_event_args> event_args();

    /// <summary>
    /// The multiplayer session type this event was triggered for. Depending upon the session type,
    /// you can then retrieve the latest lobby or game session from the multiplayer_manager class.
    /// </summary>
    inline multiplayer_session_type session_type() const;

private:
    const XblMultiplayerEvent* m_internalEvent;
    std::shared_ptr<multiplayer_event_args> m_eventArgs;
};

/// <summary>
/// APIs for matchmaking, player roster and multiplayer session management.
/// </summary>
class multiplayer_manager
{
public:

    /// <summary>
    /// Gets the multiplayer_manager singleton instance
    /// </summary>
    inline static std::shared_ptr<multiplayer_manager> get_singleton_instance();

    /// <summary>
    /// Initializes the object.
    /// </summary>
    /// <param name="lobbySessionTemplateName">The name of the template for the lobby session to be based on.</param>
    inline void initialize(
        _In_ const string_t& lobbySessionTemplateName
        );

    /// <summary>
    /// Ensures proper game state updates are maintained between the title and the Xbox Live Multiplayer Service.
    /// To ensure best performance, do_work() must be called frequently, such as once per frame. 
    /// Title needs to be thread safe when calling do_work() since this is when the state will change.
    /// For example, if you looping through the list of members() on a different thread than your calling do_work() on, 
    /// it may change when do_work() is called.
    /// </summary>
    /// <returns>A list of all callback events for the game to handle. Empty if no events are triggered during this update.</returns>
    inline std::vector<multiplayer_event> do_work();

    /// <summary>
    /// Represents a lobby session used for managing members that are local to this device and your invited friends.
    /// When a member accepts a game invite, they will be added to the lobby and the game session (if it exists).
    /// Users found via matchmaking will not be added in the lobby. 
    /// </summary>
    inline std::shared_ptr<multiplayer_lobby_session> lobby_session() const;

    /// <summary>
    /// Represents a game session used for your active gameplay. When a member accepts an invite, 
    /// they will be added to the lobby and the game session (if there is room).
    /// You can call leave_game() to leave the game session.
    /// </summary>
    inline std::shared_ptr<multiplayer_game_session> game_session() const;

    /// <summary>
    /// Joins a game given a session handle id. A handle is a service-side pointer to a session.
    /// The handleId is a GUID identifier of the handle.  Callers will usually get the handleId from 
    /// another member's multiplayer_activity_details. Optionally, if you haven't added the local users through
    /// lobby_session()::add_local_user(), you can pass in the list of users via the join_lobby() API.
    /// The result is delivered via multiplayer_event callback of type join_lobby_completed through do_work().
    /// After joining, you can set the host via set_synchronized_host if one doesn't exist.
    /// </summary>
    /// <param name="handleId">A multiplayer handle id, which uniquely identifies the game session you want to join.</param>
    /// <param name="user">The system User joining the lobby.</param>
    inline xbox_live_result<void> join_lobby(
        _In_ const string_t& handleId,
        _In_ xbox_live_user_t user
        );

#if (HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP)
    /// <summary>
    /// Joins a game via the specified IProtocolActivatedEventArgs.
    /// The IProtocolActivatedEventArgs provides arguments for protocol activation. If the user accepts an invite or 
    /// joined a friends game via a shell UI, the title will get a protocol activation. The result is delivered via 
    /// join_lobby_completed_event_args() callback through do_work(). 
    /// Optionally, if you haven't added the local users through lobby_session()::add_local_user(), you can pass in 
    /// the list of users via the join_lobby() API. If the invited user is not added either via 
    /// lobby_session()::add_local_user() or through join_lobby(), then join_lobby() will fail and provide the 
    /// invited_xbox_user_id() that the invite was sent for as part of the join_lobby_completed_event_args()
    /// After joining, you can set the host via set_synchronized_host if one doesn't exist.
    /// </summary>
    /// <param name="eventArgs">The IProtocolActivatedEventArgs when the title is protocol activated.</param>
    /// <param name="user">The system User joining the lobby.</param>
    inline xbox_live_result<void> join_lobby(
        _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
        _In_ xbox_live_user_t user
        );
#endif

#if HC_PLATFORM == HC_PLATFORM_XDK
    /// <summary>
    /// Joins a game given a session handle id. A handle is a service-side pointer to a game session.
    /// The handleId is a GUID identifier of the handle.  Callers will usually get the handleId from 
    /// another member's multiplayer_activity_details. Optionally, if you haven't added the local users through
    /// lobby_session()::add_local_user(), you can pass in the list of users via the join_lobby() API.
    /// The result is delivered via multiplayer_event callback of type join_lobby_completed through do_work().
    /// After joining, you can set the host via set_synchronized_host if one doesn't exist.
    /// </summary>
    /// <param name="handleId">A multiplayer handle id, which uniquely identifies the game session you want to join.</param>
    /// <param name="users">List of system Users joining the lobby.</param>
    inline xbox_live_result<void> join_lobby(
        _In_ const string_t& handleId,
        _In_ std::vector<Windows::Xbox::System::User^> users
        );

    /// <summary>
    /// Joins a game via the specified IProtocolActivatedEventArgs.
    /// The IProtocolActivatedEventArgs provides arguments for protocol activation. If the user accepts an invite or 
    /// joined a friends game via a shell UI, the title will get a protocol activation. The result is delivered via 
    /// join_lobby_completed_event_args() callback through do_work(). 
    /// Optionally, if you haven't added the local users through lobby_session()::add_local_user(), you can pass in 
    /// the list of users via the join_lobby() API. If the invited user is not added either via 
    /// lobby_session()::add_local_user() or through join_lobby(), then join_lobby() will fail and provide the 
    /// invited_xbox_user_id() that the invite was sent for as part of the join_lobby_completed_event_args()
    /// After joining, you can set the host via set_synchronized_host if one doesn't exist.
    /// </summary>
    /// <param name="eventArgs">The IProtocolActivatedEventArgs when the title is protocol activated.</param>
    /// <param name="users">List of system Users joining the lobby.</param>
    inline xbox_live_result<void> join_lobby(
        _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
        _In_ std::vector<Windows::Xbox::System::User^> users
        );

    /// <summary>
    /// Send invites to your party to join your game.
    /// </summary>
    inline void invite_party_to_game();

#endif

    /// <summary>
    /// Join the lobby's game session if one exists and if there is room. If the session doesn't exist, it creates a new game session
    /// with the existing lobby members. The result is delivered via multiplayer_event callback of 
    /// type join_game_completed through do_work(). This does not migrate existing lobby session properties over to the game session. 
    /// After joining, you can set the properties or the host via game_session()::set_synchronized APIs.
    /// </summary>
    /// <param name="sessionTemplateName">The name of the template for the game session to be based on.</param>
    inline xbox_live_result<void> join_game_from_lobby(
        _In_ const string_t& sessionTemplateName
        );

    /// <summary>
    /// Joins a game given a globally unique session name. Callers can get the unique session name
    /// as a result of the title's third party matchmaking. Call this on all clients that needs to join this game.
    /// The result is delivered via multiplayer_event callback of type join_game_completed through do_work().
    /// After joining, you can set the properties or the host via game_session()::set_synchronized APIs.
    /// </summary>
    /// <param name="sessionName">A unique name for the session.</param>
    /// <param name="sessionTemplateName">The name of the template for the game session to be based on.</param>
    /// <param name="xboxUserIds">The list of xbox user IDs you want to be part of the game.</param>
    inline xbox_live_result<void> join_game(
        _In_ const string_t& sessionName,
        _In_ const string_t& sessionTemplateName,
        _In_ const std::vector<string_t>& xboxUserIds = std::vector<string_t>()
        );

    /// <summary>
    /// Leaving the game will put you back into the lobby. This will remove all local users from the game.
    /// The result is delivered via multiplayer_event callback of type leave_game_completed through do_work().
    /// </summary>
    inline xbox_live_result<void> leave_game();

    /// <summary>
    /// Sends a matchmaking request to the server.  When a match is found, the manager will join
    /// the game and notify the title via find_match_completed_event().
    /// </summary>
    /// <param name="hopperName">The name of the hopper.</param>
    /// <param name="attributes">The ticket attributes for the match. (Optional)</param>
    /// <param name="timeout">The maximum time to wait for members to join the match. (Optional)</param>
    inline xbox_live_result<void> find_match(
        _In_ const string_t& hopperName,
        _In_ const web::json::value& attributes = web::json::value(),
        _In_ const std::chrono::seconds& timeout = std::chrono::seconds(60)
        );

    /// <summary>
    /// Cancels the match request on the server, if one exists.
    /// </summary>
    inline void cancel_match();

    /// <summary>
    /// Provides the current status of matchmaking. 'None' if no matchmaking is in progress.
    /// </summary>
    inline xbox::services::multiplayer::manager::match_status match_status() const;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other members.
    /// Only applies after find_match() has been called.
    /// </summary>
    inline std::chrono::seconds estimated_match_wait_time() const;

    /// <summary>
    /// Indicates whether the game should auto fill open slots during gameplay.
    /// </summary>
    inline bool auto_fill_members_during_matchmaking() const;

    /// <summary>
    /// If set to true, it finds members via matchmaking to fill open slots during gameplay.
    /// This can be changed anytime.
    /// </summary>
    /// <param name="autoFillMembers">True to search for members during matchmaking if the game has open slots.</param>
    inline void set_auto_fill_members_during_matchmaking(
        _In_ bool autoFillMembers
        );

    /// <summary>
    /// Sets a collection of multiplayer_quality_of_service_measurements between itself and a list of remote clients.
    /// This is only used when the title is manually managing QoS.
    /// </summary>
    inline void set_quality_of_service_measurements(
        _In_ std::shared_ptr<std::vector<xbox::services::multiplayer::multiplayer_quality_of_service_measurements>> measurements
        );

    /// <summary>
    /// Indicates who can join your game via the lobby.
    /// </summary>
    inline joinability joinability() const;

    /// <summary>
    /// Restricts who can join the game. Defaults to "joinable_by_friends", meaning only local users and users who are followed 
    /// by an existing member of the lobby can join without an invite.
    /// Changes are batched and written to the service on the next do_work(). All session properties and members
    /// contain updated response returned from the server upon calling do_work().
    /// The result is delivered via multiplayer_event callback of type joinability_state_changed through do_work().
    /// </summary>
    /// <param name="value">The joinability value you want to set.</param>
    /// <param name="context">The application-defined data to correlate the multiplayer_event to the initiating call. (Optional)</param>
    inline xbox_live_result<void> set_joinability(
        _In_ xbox::services::multiplayer::manager::joinability value,
        _In_opt_ context_t context = nullptr
        );

private:
    multiplayer_manager() {}
    multiplayer_manager(multiplayer_manager const&) = delete;
    void operator=(multiplayer_manager const&) = delete;

    mutable std::shared_ptr<multiplayer_lobby_session> m_lobbySession;
};

}}}}

#include "impl/multiplayer_manager.hpp"
