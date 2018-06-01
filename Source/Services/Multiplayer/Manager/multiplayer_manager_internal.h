// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>
#include <set>
#include "pplx/pplxtasks.h"
#include "xsapi/multiplayer.h"
#include "xsapi/multiplayer_manager.h"
#include "xsapi/social.h"
#include "xsapi/real_time_activity.h"
#include "system_internal.h"
#include "user_context.h"
#include "xbox_live_context_impl.h"

namespace xbox { namespace services { 
    class xbox_live_context_impl;
}}

namespace xbox { namespace services { namespace multiplayer { namespace manager {

class multiplayer_match_client;
class multiplayer_client_manager;
class multiplayer_local_user_manager;
class multiplayer_lobby_client;

enum class multiplayer_local_user_lobby_state
{
    unknown,

    add,

    join,

    in_session,

    leave,

    remove
};

enum class multiplayer_local_user_game_state
{
    unknown,

    pending_join,

    join,

    in_session,

    leave
};

enum class pending_request_type
{
    synchronized_changes,

    non_synchronized_changes
};

class multiplayer_client_pending_request 
{
public:
    multiplayer_client_pending_request();

    pending_request_type request_type() const;

    context_t context();

    uint32_t identifier() const;

    // Local user properties
    std::shared_ptr<multiplayer_local_user> local_user();
    void set_local_user(_In_ std::shared_ptr<multiplayer_local_user> user);

    multiplayer_local_user_lobby_state lobby_state();
    void set_lobby_state(_In_ multiplayer_local_user_lobby_state userState);

    const string_t& lobby_handle_id() const;
    void set_lobby_handle_id(_In_ const string_t& handleId);

    const multiplayer_session_reference& team_session_reference() const;
    void set_team_session_reference(_In_ const multiplayer_session_reference& sessionRef);

    const string_t& local_user_connection_address() const;
    void set_local_user_connection_address(
        _In_ std::shared_ptr<multiplayer_local_user> localUser,
        _In_ string_t connectionAddress, 
        _In_opt_ context_t context
        );

    const std::map<string_t, web::json::value>& local_user_properties() const;
    void set_local_user_properties(
        _In_ std::shared_ptr<multiplayer_local_user> localUser, 
        _In_ string_t name, 
        _In_ web::json::value valueJson, 
        _In_opt_ context_t context
        );

    // Session non-synchronized properties
    multiplayer::manager::joinability joinability();
    void set_joinability(_In_ xbox::services::multiplayer::manager::joinability value, _In_opt_ context_t context);

    const std::map<string_t, web::json::value>& session_properties() const;
    void set_session_properties(_In_ string_t name, _In_ web::json::value valueJson, _In_opt_ context_t context);

    // Session synchronized properties
    const string_t& synchronized_host_device_token() const;
    void set_synchronized_host_device_token(_In_ const string_t& hostDeviceToken, _In_opt_ context_t context);

    const std::map<string_t, web::json::value>& synchronized_session_properties() const;
    void set_synchronized_session_properties(_In_ string_t name, _In_ web::json::value valueJson, _In_opt_ context_t context);

    void append_pending_changes(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> sessionToCommit, 
        _In_ std::shared_ptr<multiplayer_local_user> localUser, 
        _In_ bool isGameInProgress = false
        );

private:

    context_t m_context;
    pending_request_type m_requestType;
    uint32_t m_identifier;

    // Local user properties
    std::shared_ptr<multiplayer_local_user> m_localUser;
    multiplayer_local_user_lobby_state m_localUserLobbyState;
    std::map<string_t, web::json::value> m_localUserProperties;
    string_t m_localUserConnectionAddress;
    string_t m_lobbyHandleId;   // Only used while joining a friend's lobby
    xbox::services::multiplayer::multiplayer_session_reference m_teamSessionRef;   // Only used for Tournament MPM integration support.

    // Session non-synchronized properties
    std::map<string_t, web::json::value> m_sessionProperties;
    xbox::services::multiplayer::manager::joinability m_joinability;

    // Session synchronized properties
    string_t m_synchronizedHostDeviceToken;
    std::map<string_t, web::json::value> m_synchronizedSessionProperties;
};


class multiplayer_session_writer : public std::enable_shared_from_this<multiplayer_session_writer>
{
public:
    multiplayer_session_writer();

    multiplayer_session_writer(
        _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
        );

    const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session() const;
    void update_session(_In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& updatedSession);

    uint64_t tap_change_number() const;
    void set_tap_change_number(_In_ uint64_t changeNumber);

    bool is_tap_received() const;
    void set_tap_received(_In_ bool bReceived);

    bool is_write_in_progress() const;
    void set_write_in_progress(_In_ bool writeInProgress);

    void on_session_changed(
        _In_ const xbox::services::multiplayer::multiplayer_session_change_event_args& args
        );

    pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> commit_synchronized_changes(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> sessionToCommit
        );

    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> leave_remote_session(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session
        );

    pplx::task<xbox_live_result<std::vector<multiplayer_event>>> commit_pending_changes(
        _In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue,
        _In_ multiplayer_session_type sessionType,
        _In_ bool isGameInProgress = false
        );

    pplx::task<xbox_live_result<std::vector<multiplayer_event>>> commit_pending_synchronized_changes(
        _In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue,
        _In_ multiplayer_session_type sessionType
        );

    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> write_session(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContext,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session,
        _In_ xbox::services::multiplayer::multiplayer_session_write_mode mode,
        _In_ bool updateLatest = true
        );

    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> write_session_by_handle(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContext,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session,
        _In_ xbox::services::multiplayer::multiplayer_session_write_mode mode,
        _In_ const string_t& handleId,
        _In_ bool updateLatest = true
        );

    void on_session_updated(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& updatedSession
        );

    function_context add_multiplayer_session_updated_handler(
        _In_ std::function<void(const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& )> handler
        );

    void on_resync_message_received();

    std::shared_ptr<xbox_live_context_impl> get_primary_context();

    std::vector<multiplayer_event> handle_events(
        _In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue,
        _In_ const std::error_code& errorCode,
        _In_ const std::string& errorMessage,
        _In_ multiplayer_session_type sessionType
        );

private:
    void destroy();

    void resync();

    xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> write_session_helper(
        _In_ xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> sessionResult,
        _In_ bool updateLatest
        );

     pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> get_current_session_helper(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContext,
        _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionReference
        );

    // resync
    bool m_isTaskInProgress;
    function_context m_handleResyncEventCounter;
    xbox::services::system::xbox_live_mutex m_resyncLock;

    xbox::services::system::xbox_live_mutex m_stateLock;
    function_context m_sessionUpdateEventHandlerCounter;
    std::unordered_map<uint32_t, std::function<void(const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>)>> m_sessionUpdateEventHandler;

    std::mutex m_synchronizeWriteWithTapLock;
    uint64_t m_tapChangeNumber;
    bool m_isTapReceived;
    uint64_t m_numOfWritesInProgress;
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> m_session;
    std::shared_ptr<multiplayer_local_user_manager> m_multiplayerLocalUserManager;
};

class multiplayer_game_client : public std::enable_shared_from_this<multiplayer_game_client>
{
public:
    multiplayer_game_client();

    ~multiplayer_game_client();

    multiplayer_game_client(
        _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
        );

    void deep_copy_if_updated(_In_ const multiplayer_game_client& other);

    void initialize();

    void set_game_session_template(_In_ const string_t& sessionTemplateName);

    const std::shared_ptr<multiplayer_session_writer>& session_writer() const;

    const std::shared_ptr<multiplayer_game_session>& game() const;

    void update_game(_In_ const std::shared_ptr<multiplayer_game_session>& multiplayerGame);

    std::vector<multiplayer_event> do_work();
    bool is_request_in_progress();
    bool is_pending_game_changes();

    void clear_pending_queue();
    void add_to_pending_queue(_In_ std::shared_ptr<multiplayer_client_pending_request> pendingRequest);
    void add_to_processing_queue(_In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue);
    void remove_from_processing_queue(_In_ uint32_t identifier);

    const std::vector<multiplayer_event>& multiplayer_event_queue();
    std::vector<std::shared_ptr<multiplayer_client_pending_request>> get_processing_queue();

    void update_game_session(_In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session);

    void update_objects(
        const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& updatedSession,
        const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& lobbySession
        );

    const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session() const;

    void update_session(_In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session);

    void remove_stale_users_from_remote_session();

    void set_local_member_properties_to_remote_session(
        _In_ const std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_local_user>& localUser,
        _In_ const std::map<string_t, web::json::value>& localUsersMap,
        _In_ const string_t& localUserConnectionAddress
        );

    xbox_live_result<void> join_game_helper(
        _In_ const string_t& sessionName
        );

    void leave_remote_session(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session,
        _In_ bool stopAdvertisingGameSession,
        _In_ bool triggerCompletionEvent
        );

    xbox_live_result<void> join_game_from_lobby_helper();

    pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> join_game_by_session_reference(
        _In_ const xbox::services::multiplayer::multiplayer_session_reference& gameSessionRef
        );

    pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> join_game_by_handle(
        _In_ const string_t& handleId,
        _In_ bool createGameIfFailedToJoin
        );

    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> join_game_for_all_local_members(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& gameSession,
        _In_ const string_t& handleId = string_t(),
        _In_ bool createGameIfFailedToJoin = false
        );

    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> join_game_for_all_local_members_helper(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session,
        _In_ bool writeMemberPropertiesFromLobby,
        _In_ const string_t& handleId
        );

private:

    std::shared_ptr<multiplayer_lobby_client> lobby_client();
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> lobby_session();

    std::shared_ptr<multiplayer_game_session> convert_to_multiplayer_game(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& sessionToConvert,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& lobbySession
        );

    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> join_helper(
        _In_ std::shared_ptr<multiplayer_local_user> localUser,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session,
        _In_ bool writeMemberPropertiesFromLobby,
        _In_ const string_t& handleId
        );

    void add_multiplayer_event_helper(
        _In_ const std::error_code& errorCode,
        _In_ const std::string& errorMessage,
        _In_ multiplayer_event_type eventType,
        _In_ std::shared_ptr<multiplayer_event_args> eventArgs,
        _In_opt_ context_t context = nullptr
        );

    mutable std::mutex m_clientRequestLock;
    std::atomic<bool> m_pendingCommitInProgress;
    string_t m_gameSessionTemplateName;
    uint64_t m_updateNumber;
    std::shared_ptr<multiplayer_session_writer> m_sessionWriter;
    std::vector<multiplayer_event> m_multiplayerEventQueue;
    std::shared_ptr<multiplayer_game_session> m_multiplayerGame;
    std::shared_ptr<multiplayer_local_user_manager> m_multiplayerLocalUserManager;
    std::queue<std::shared_ptr<multiplayer_client_pending_request>> m_pendingRequestQueue;
    std::vector<std::shared_ptr<multiplayer_client_pending_request>> m_processingQueue;
};

class multiplayer_lobby_client : public std::enable_shared_from_this<multiplayer_lobby_client>
{
public:
    multiplayer_lobby_client();

    ~multiplayer_lobby_client();

    multiplayer_lobby_client(
        _In_ string_t lobbySessionTemplateName,
        _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
        );

    void deep_copy_if_updated(_In_ const multiplayer_lobby_client& other);

    void initialize();

    const std::shared_ptr<multiplayer_session_writer>& session_writer() const;

    const std::shared_ptr<multiplayer_lobby_session>& lobby() const;

    void clear_pending_queue();
    void add_to_pending_queue(_In_ std::shared_ptr<multiplayer_client_pending_request> pendingRequest);
    void add_to_processing_queue(_In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue);
    void remove_from_processing_queue(_In_ uint32_t identifier);

    xbox_live_result<void> add_local_user(
        _In_ xbox_live_user_t user,
        _In_ multiplayer_local_user_lobby_state userState,
        _In_ const string_t& handleId = string_t(),
        _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionRef = xbox::services::multiplayer::multiplayer_session_reference()
        );

    void add_local_users(_In_ std::vector<xbox_live_user_t> user, _In_ const string_t& handleId);
    void add_local_users(_In_ std::vector<xbox_live_user_t> user, _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionRef);

    xbox_live_result<void> remove_local_user(_In_ xbox_live_user_t user);
    void remove_all_local_users();

    xbox_live_result<void> set_local_member_properties(
        _In_ xbox_live_user_t user,
        _In_ string_t name,
        _In_ web::json::value valueJson,
        _In_opt_ context_t context
        );

    xbox_live_result<void> delete_local_member_properties(
        _In_ xbox_live_user_t user,
        _In_ const string_t& name,
        _In_opt_ context_t context
        );

    xbox_live_result<void> set_local_member_connection_address(
        _In_ xbox_live_user_t user,
        _In_ string_t address,
        _In_opt_ context_t context
        );

    std::vector<multiplayer_event> do_work();
    bool is_pending_lobby_changes();
    bool is_request_in_progress();

    pplx::task<xbox_live_result<std::vector<multiplayer_event>>> commit_pending_lobby_changes(
        _In_ std::vector<string_t> xuidsInOrder,
        _In_ bool joinByHandleId,
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef = xbox::services::multiplayer::multiplayer_session_reference()
        );

    pplx::task<xbox_live_result<std::vector<multiplayer_event>>> commit_lobby_changes(
        _In_ std::vector<string_t> xuidsInOrder,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> lobbySessionToCommit
        );

    xbox_live_result<void> create_game_from_lobby();

    xbox::services::multiplayer::manager::joinability joinability();

    xbox_live_result<void> set_joinability(
        _In_ xbox::services::multiplayer::manager::joinability value,
        _In_opt_ context_t context
        );

    const std::vector<multiplayer_event>& multiplayer_event_queue();
    std::vector<std::shared_ptr<multiplayer_client_pending_request>> get_processing_queue();

    void update_lobby_session(_In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& updatedSession);

    void update_objects(
        const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& updatedSession,
        const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& gameSession
        );

    const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session() const;

    void update_session(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& updatedSession
        );

    void leave_remote_session(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session
        );

    void stop_advertising_game_session(
        _In_ xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>> result
        );

    void advertise_game_session();
    void clear_game_session_from_lobby();

    bool is_transfer_handle_state(_In_ const string_t& state);
    string_t get_transfer_handle();

    void handle_game_session_ready_event(
        _In_ const  std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& lobbySession
    );

    static const string_t c_transferHandlePropertyName;
    static const string_t c_joinabilityPropertyName;

    void remove_stale_xbox_live_context_from_map();
    std::map<string_t, std::shared_ptr<multiplayer_local_user>> get_local_user_map();
    std::shared_ptr<xbox_live_context_impl> get_primary_context();

private:
    std::shared_ptr<multiplayer_game_client> game_client();
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> game_session();

    void update_lobby(_In_ std::shared_ptr<multiplayer_lobby_session> multiplayerLobby);
    void update_local_lobby_members(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& lobbySession,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& gameSession
        );

    bool is_pending_lobby_local_user_changes();

    bool should_update_host_token(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_local_user> localUser,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session
        );

    void user_state_changed(
        _In_ const std::error_code& errorCode,
        _In_ const std::string& errorMessage,
        _In_ multiplayer_local_user_lobby_state localUserLobbyState,
        _In_ string_t xboxUserId
        );

    void handle_lobby_change_events(
        _In_ const std::error_code& errorCode,
        _In_ const std::string& errorMessage,
        _In_ std::shared_ptr<multiplayer_local_user> localUser,
        _In_ const std::vector<std::shared_ptr<multiplayer_client_pending_request>>& processingQueue
        );

    void handle_join_lobby_completed(
        _In_ const std::error_code& errorCode,
        _In_ const std::string& errorMessage
        );

    void join_lobby_completed(
        _In_ const std::error_code& errorCode,
        _In_ const std::string& errorMessage,
        _In_ const string_t& invitedXboxUserId
        );

    void add_to_multiplayer_event_queue();

    void add_multiplayer_event_helper(
        _In_ const std::error_code& errorCode,
        _In_ const std::string& errorMessage,
        _In_ multiplayer_event_type eventType,
        _In_ std::shared_ptr<multiplayer_event_args> eventArgs,
        _In_opt_ context_t context = nullptr
        );

    xbox_live_result<std::vector<multiplayer_event>> commit_lobby_changes_helper(
        _In_ std::vector<string_t> xuids,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> lobbySessionToCommit
        );

    std::shared_ptr<multiplayer_lobby_session> convert_to_multiplayer_lobby(
        _In_ const  std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& sessionToConvert,
        _In_ const  std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& gameSession
        );

    string_t m_lobbySessionTemplateName;
    std::atomic<bool> m_pendingCommitInProgress;

    uint64_t m_updateNumber;
    xbox::services::multiplayer::manager::joinability m_joinability;
    mutable std::mutex m_clientRequestLock;
    std::queue<std::shared_ptr<multiplayer_client_pending_request>> m_pendingRequestQueue;
    std::vector<multiplayer_event> m_multiplayerEventQueue;
    std::shared_ptr<multiplayer_session_writer> m_sessionWriter;
    std::shared_ptr<multiplayer_lobby_session> m_multiplayerLobby;
    std::vector<std::shared_ptr<multiplayer_member>> m_localLobbyMembers;
    std::shared_ptr<multiplayer_local_user_manager> m_multiplayerLocalUserManager;
    std::vector<std::shared_ptr<multiplayer_client_pending_request>> m_processingQueue;
};

class multiplayer_client_pending_reader : public std::enable_shared_from_this<multiplayer_client_pending_reader>
{
public:
    multiplayer_client_pending_reader();

    ~multiplayer_client_pending_reader();

    multiplayer_client_pending_reader(
        _In_ string_t lobbySessionTemplateName,
        _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
        );

    void deep_copy_if_updated(_In_ const multiplayer_client_pending_reader& other);
    bool is_update_avaialable(_In_ const multiplayer_client_pending_reader& other);

    void do_work();
    void process_match_events();

    std::shared_ptr<multiplayer_lobby_client> lobby_client();
    std::shared_ptr<multiplayer_game_client> game_client();
    std::shared_ptr<multiplayer_match_client> match_client();

    std::vector<multiplayer_event> multiplayer_event_queue() const;

    void clear_multiplayer_event_queue();
    void add_to_multiplayer_event_queue(_In_ multiplayer_event multiplayerEvent);
    void add_to_multiplayer_event_queue(_In_ std::vector<multiplayer_event> multiplayerEventQueue);

    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> get_session(_In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef);
    void update_session(_In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef, _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session);

    bool is_lobby(_In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef);
    bool is_game(_In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef);
    bool is_match(_In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef);

    xbox_live_result<void> find_match(
        _In_ const string_t& hopperName,
        _In_ const web::json::value& attributes,
        _In_ const std::chrono::seconds& timeout
        );

    void set_auto_fill_members_during_matchmaking(_In_ bool autoFillMembers);

    xbox_live_result<void> set_joinability(
        _In_ xbox::services::multiplayer::manager::joinability value,
        _In_opt_ context_t context
        );

    xbox_live_result<void> set_properties(
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef,
        _In_ string_t name,
        _In_ web::json::value valueJson,
        _In_opt_ context_t context
        );

    xbox_live_result<void> set_synchronized_properties(
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef,
        _In_ string_t name,
        _In_ web::json::value valueJson,
        _In_opt_ context_t context
        );

    xbox_live_result<void> set_synchronized_host(
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef,
        _In_ const string_t& hostDeviceToken,
        _In_opt_ context_t context
        );

    std::shared_ptr<multiplayer_member> convert_to_game_member(_In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member> member);

private:
    void add_to_pending_queue(
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef,
        _In_ std::shared_ptr<multiplayer_client_pending_request> pendingRequest
        );

    static bool is_local(_In_ const string_t& xboxUserId, _In_ const std::map<string_t, std::shared_ptr<multiplayer_local_user>>& xboxLiveContextMap);

    bool m_autoFillMembers;
    std::vector<multiplayer_event> m_multiplayerEventQueue;
    mutable std::mutex m_clientRequestLock;
    std::shared_ptr<multiplayer_lobby_client> m_lobbyClient;
    std::shared_ptr<multiplayer_game_client> m_gameClient;
    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_match_client> m_matchClient;
    std::shared_ptr<multiplayer_local_user_manager> m_multiplayerLocalUserManager;
};

class multiplayer_local_user
{
public:

    multiplayer_local_user(
        _In_ xbox_live_user_t user,
        _In_ string_t xboxUserId,
        _In_ bool isPrimary
        );

    ~multiplayer_local_user();

    string_t xbox_user_id() const;

    std::shared_ptr<xbox::services::xbox_live_context_impl> context() const;

    const string_t& lobby_handle_id() const;
    void set_lobby_handle_id(_In_ const string_t& handleId);

    multiplayer_local_user_lobby_state lobby_state() const;
    void set_lobby_state(_In_ multiplayer_local_user_lobby_state userState);

    multiplayer_local_user_game_state game_state() const;
    void set_game_state(_In_ multiplayer_local_user_game_state userState);

    string_t connection_address() const;
    void set_connection_address(_In_ string_t address);

    bool is_primary_xbox_live_context() const;
    void set_is_primary_xbox_live_context(_In_ bool isPrimary);

    void set_write_changes_to_service(_In_ bool value);
    bool write_changes_to_service() const;

    function_context rta_state_changed_context() const;
    void set_rta_state_changed_context(_In_ function_context functionContext);

    function_context session_changed_context() const;
    void set_session_changed_context(_In_ function_context functionContext);

    function_context rta_resync_context() const;
    void set_rta_resync_context(_In_ function_context functionContext);

    function_context subscription_lost_context() const;
    void set_subscription_lost_context(_In_ function_context functionContext);

private:
    function_context m_rtaStateChangedContext;
    function_context m_sessionChangedContext;
    function_context m_subscriptionLostContext;
    function_context m_rtaResyncContext;
    bool m_writeChangesToService;
    bool m_writeConnectionAddress;
    string_t m_xboxUserId;
    string_t m_connectionAddress;
    string_t m_lobbyHandleId;
    multiplayer_local_user_lobby_state m_lobbyState;
    multiplayer_local_user_game_state m_gameState;
    bool m_isPrimaryXboxLiveContext;
    std::shared_ptr<xbox::services::xbox_live_context_impl> m_xboxLiveContextImpl;
};

class multiplayer_local_user_manager : public std::enable_shared_from_this<multiplayer_local_user_manager>
{
public:
    multiplayer_local_user_manager();

    ~multiplayer_local_user_manager();

    xbox::services::system::xbox_live_mutex m_lock;
    std::shared_ptr<xbox_live_context_impl> get_primary_context();

    void change_all_local_user_lobby_state(_In_ multiplayer_local_user_lobby_state state);
    void change_all_local_user_game_state(_In_ multiplayer_local_user_game_state state);
    bool is_local_user_game_state(_In_ multiplayer_local_user_game_state state);

    std::map<string_t, std::shared_ptr<multiplayer_local_user>> get_local_user_map();
    std::shared_ptr<xbox::services::xbox_live_context_impl> get_context(_In_ const string_t& xboxUserId);

    std::shared_ptr<multiplayer_local_user> get_local_user(_In_ const string_t& xboxUserId);
    std::shared_ptr<multiplayer_local_user> get_local_user_helper(_In_ const string_t& xboxUserId);

    std::shared_ptr<multiplayer_local_user> get_local_user(
        _In_ xbox_live_user_t user
        );

    std::shared_ptr<multiplayer_local_user> get_local_user_helper(
        _In_ xbox_live_user_t user
        );

    const std::shared_ptr<multiplayer_local_user>& add_user_to_xbox_live_context_to_map(_In_ xbox_live_user_t user);
    void remove_stale_local_users_from_map();

    void activate_multiplayer_events(_In_ const std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_local_user>& localuser);
    void deactivate_multiplayer_events(_In_ const std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_local_user>& localUser);

    function_context add_multiplayer_session_changed_handler(
        _In_ std::function<void(const xbox::services::multiplayer::multiplayer_session_change_event_args&)> handler
        );
    void remove_multiplayer_session_changed_handler(_In_ function_context context);

    function_context add_multiplayer_subscription_lost_handler(_In_ std::function<void()> handler);
    void remove_multiplayer_subscription_lost_handler(_In_ function_context context);

    function_context add_rta_resync_handler(_In_ std::function<void()> handler);
    void remove_rta_resync_handler(_In_ function_context context);

private:
    void on_subscriptions_lost(_In_ const string_t& xboxUserId);

    void on_session_changed(
        _In_ const xbox::services::multiplayer::multiplayer_session_change_event_args& args
        );

    void on_connection_state_changed(
        _In_ const string_t& xboxUserId,
        _In_ xbox::services::real_time_activity::real_time_activity_connection_state state
        );

    void on_resync_message_received();

    xbox::services::system::xbox_live_mutex m_subscriptionLock;
    function_context m_sessionChangeEventHandlerCounter;
    function_context m_multiplayerSubscriptionLostEventHandlerCounter;
    function_context m_rtaResyncEventHandlerCounter;
    std::unordered_map<uint32_t, std::function<void(const xbox::services::multiplayer::multiplayer_session_change_event_args&)>> m_sessionChangeEventHandler;
    std::unordered_map<uint32_t, std::function<void()>> m_multiplayerSubscriptionLostEventHandler;
    std::unordered_map<uint32_t, std::function<void()>> m_rtaResyncEventHandler;

    std::map<string_t, std::shared_ptr<multiplayer_local_user>> m_localUserRequestMap;
    std::shared_ptr<xbox_live_context_impl> m_primaryXboxLiveContext;
};

class multiplayer_client_manager : public std::enable_shared_from_this<multiplayer_client_manager>
{
public:
    multiplayer_client_manager(const multiplayer_client_manager& other);

    multiplayer_client_manager& deep_copy(_In_ const multiplayer_client_manager& other);

    multiplayer_client_manager(
        _In_ string_t lobbySessionTemplateName
        );

    void initialize();
    void shutdown();

    void register_local_user_manager_events();
    bool is_update_avaialable();
    bool is_request_in_progress();

    std::shared_ptr<multiplayer_local_user_manager> local_user_manager();

    std::shared_ptr<xbox_live_context_impl> get_primary_context();

    std::shared_ptr<multiplayer_lobby_client> lobby_client() const;
    std::shared_ptr<multiplayer_client_pending_reader> latest_pending_read() const;
    std::shared_ptr<multiplayer_client_pending_reader> last_pending_read() const;

    xbox_live_result<void> join_lobby_by_handle(
        _In_ const string_t& handleId,
        _In_ std::vector<xbox_live_user_t> users
        );

    xbox_live_result<void> join_lobby_by_session_reference(
        _In_ const multiplayer_session_reference& sessionRef,
        _In_ std::vector<xbox_live_user_t> users
        );
#if UWP_API || TV_API || UNIT_TEST_SERVICES
    xbox_live_result<void> join_lobby(
        _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
        _In_ std::vector<xbox_live_user_t> users
        );

    xbox_live_result<void> join_lobby( _In_ Windows::Foundation::Uri^ url, _In_ std::vector<xbox_live_user_t> users );
#endif
    xbox_live_result<void> join_game_from_lobby(_In_ const string_t& sessionTemplateName);

    xbox_live_result<void> join_game(
        _In_ const string_t& sessionName,
        _In_ const string_t& sessionTemplateName,
        _In_ const std::vector<string_t>& xboxUserIds
        );

    xbox_live_result<void> leave_game();

    std::vector<multiplayer_event> do_work();

    pplx::task<xbox_live_result<std::vector<xbox::services::multiplayer::multiplayer_activity_details>>> get_activities_for_social_group(
        _In_ xbox_live_user_t user,
        _In_ const string_t& socialGroup
        );

    xbox_live_result<void> invite_friends(
        _In_ xbox_live_user_t user,
        _In_ const string_t& contextStringId,
        _In_ const string_t& customActivationContext
        );

    xbox_live_result<void> invite_users(
        _In_ xbox_live_user_t user,
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const string_t& contextStringId,
        _In_ const string_t& customActivationContext
        );

    xbox_live_result<void> set_properties(
        _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionRef,
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson,
        _In_opt_ context_t context
        );

    xbox_live_result<void> set_joinability(
        _In_ xbox::services::multiplayer::manager::joinability value,
        _In_opt_ context_t context
        );

    xbox_live_result<void> set_synchronized_host(
        _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionRef,
        _In_ const string_t& hostDeviceToken,
        _In_opt_ context_t context
        );

    xbox_live_result<void> set_synchronized_properties(
        _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionRef,
        _In_ const string_t& name,
        _In_ const web::json::value& valueJson,
        _In_opt_ context_t context
        );

    std::shared_ptr<multiplayer_match_client> match_client();

    xbox_live_result<void> find_match(
        _In_ const string_t& hopperName,
        _In_ const web::json::value& attributes,
        _In_ const std::chrono::seconds& timeout
        );

    void set_auto_fill_members_during_matchmaking(_In_ bool autoFillMembers);

    void on_session_changed(
        _In_ const xbox::services::multiplayer::multiplayer_session_change_event_args& args
    );

    std::vector<multiplayer_event> event_queue() const;
    void clear_event_queue();

    void on_multiplayer_subscriptions_lost();

private:

    void destroy();

    xbox::services::multiplayer::multiplayer_service& get_multiplayer_service(
        _In_ xbox_live_user_t user
        );

    std::map<string_t, std::shared_ptr<multiplayer_local_user>> get_xbox_live_context_map();

    bool is_pending_lobby_local_user_changes();
    void on_resync_message_received();

    bool should_update_host_token(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_local_user> localUser,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session
        );

    void update_match_session(_In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session);
    void add_to_multiplayer_event_queue(_In_ multiplayer_event multiplayerEvent);

    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> get_current_session_helper(
        _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContext,
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionReference
        );

    multiplayer_session_type get_session_type(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session
        );

    void process_events(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> oldSession,
        _In_ multiplayer_session_type sessionType
        );

    void handle_member_list_changed(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> oldSession,
        _In_ multiplayer_session_type sessionType
        );

    void handle_member_properties_changed(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> oldSession,
        _In_ multiplayer_session_type sessionType
        );

    void handle_session_properties_changed(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> oldSession,
        _In_ multiplayer_session_type sessionType
        );

    void handle_host_changed(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession,
        _In_ multiplayer_session_type sessionType
        );

    void handle_tournament_properties_changed(
        _In_ std::shared_ptr<multiplayer_session> currentSession,
        _In_ std::shared_ptr<multiplayer_session> oldSession,
        _In_ multiplayer_session_type sessionType
    );

    void handle_match_status_changed(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession
        );

    void handle_initialization_state_changed(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession
        );

    void synchronized_write_completed(
        _In_ std::error_code errorCode,
        _In_ std::string errorMessage,
        _In_ multiplayer_event_type eventType,
        _In_ multiplayer_session_type sessionType
        );

    void add_multiplayer_event(
        _In_ multiplayer_event_type eventType,
        _In_ multiplayer_session_type sessionType,
        _In_ std::error_code errorCode = xbox::services::xbox_live_error_code::no_error,
        _In_ std::string errorMessage = std::string()
        );

    void add_multiplayer_event_helper(
        _In_ multiplayer_event_type eventType,
        _In_ multiplayer_session_type sessionType,
        _In_ std::error_code errorCode = xbox::services::xbox_live_error_code::no_error,
        _In_ std::string errorMessage = std::string()
        );

    mutable std::mutex m_clientRequestLock;
    std::mutex m_synchronizeWriteWithTapLock;
    std::atomic<bool> m_subscriptionsLostFired;

    bool m_autoFillMembers;
    string_t m_lobbySessionTemplateName;
    function_context m_sessionChangedContext;
    function_context m_subscriptionLostContext;
    function_context m_rtaResyncContext;

    pplx::task<void> m_pendingGameCommitTask;
    std::vector<multiplayer_event> m_multiplayerEventQueue;
    xbox::services::multiplayer::multiplayer_service m_clientManagerMultiplayerService;
    std::shared_ptr<xbox_live_context_impl> m_primaryXboxLiveContext;
    std::shared_ptr<multiplayer_local_user_manager> m_multiplayerLocalUserManager;
    std::shared_ptr<multiplayer_client_pending_reader> m_lastPendingRead;
    std::shared_ptr<multiplayer_client_pending_reader> m_latestPendingRead;
};

class multiplayer_match_client : public std::enable_shared_from_this<multiplayer_match_client>
{
public:
    multiplayer_match_client(
        _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
        );

    std::vector<multiplayer_event> do_work();
    const std::vector<multiplayer_event>& multiplayer_event_queue();
    void deep_copy_if_updated(_In_ const multiplayer_match_client& other);

    xbox::services::multiplayer::manager::match_status match_status() const;
    void set_match_status(_In_ xbox::services::multiplayer::manager::match_status status);

    std::chrono::seconds estimated_match_wait_time() const;

    xbox_live_result<void> find_match(
        _In_ const string_t& hopperName,
        _In_ const web::json::value& attributes,
        _In_ const std::chrono::seconds& timeout,
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session,
        _In_ bool preserveSession = false
        );

    xbox_live_result<void> find_match(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session,
        _In_ bool preserveSession
        );

    void cancel_match();

    void update_session(_In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession);
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session();

    void handle_match_status_changed(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> matchSession
        );

    void handle_initialization_state_changed(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> matchSession
        );

    void on_session_changed(
        _In_ const xbox::services::multiplayer::multiplayer_session_change_event_args& args
        );

    void set_quality_of_service_measurements(
        _In_ std::shared_ptr<std::vector<xbox::services::multiplayer::multiplayer_quality_of_service_measurements>> measurements
        );

    void resubmit_matchmaking(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session
        );

    void handle_find_match_completed(
        _In_ std::error_code errorCode,
        _In_ std::string errorMessage
        );

    void disable_next_timer(bool value);

    bool m_disableNextTimer;

private:
    void check_next_timer();
    void process_ticket_response();
    void handle_session_joined();
    void get_latest_session();

    void handle_qos_measurements();

    void handle_match_found(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> currentSession
        );

    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>>
    join_session_helper(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session
        );

    xbox::services::system::xbox_live_mutex m_lock;
    xbox::services::system::xbox_live_mutex m_getSessionLock;
    utility::datetime m_nextTimerToFetchSession;
    string_t m_hopperName;
    web::json::value m_attributes;
    std::chrono::seconds m_timeout;
    bool m_preservingMatchmakingSession;
    std::atomic<xbox::services::multiplayer::manager::match_status> m_matchStatus;
    mutable std::mutex m_multiplayerEventQueueLock;
    std::vector<multiplayer_event> m_multiplayerEventQueue;
    xbox::services::matchmaking::create_match_ticket_response m_matchTicketResponse;
    xbox::services::multiplayer::multiplayer_session_reference m_matchTicketSessionRef;
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> m_matchSession;
    std::shared_ptr<multiplayer_local_user_manager> m_multiplayerLocalUserManager;

    pplx::task<void> m_getSessionTask;
    pplx::task<xbox_live_result<std::shared_ptr<xbox::services::multiplayer::multiplayer_session>>> m_joinTargetSessionTask;
};

class multiplayer_manager_utils
{
public:

    static bool do_sessions_match(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session1,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session2
        );

    static bool do_session_references_match(
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef1,
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef2
        )
    {
        return  utils::str_icmp(sessionRef1.service_configuration_id(), sessionRef2.service_configuration_id()) == 0 &&
                utils::str_icmp(sessionRef1.session_template_name(), sessionRef2.session_template_name()) == 0 &&
                utils::str_icmp(sessionRef1.session_name(), sessionRef2.session_name()) == 0;
    }

    static bool is_multiplayer_session_change_type(
        _In_ xbox::services::multiplayer::multiplayer_session_change_types diffType,
        _In_ xbox::services::multiplayer::multiplayer_session_change_types value
        )
    {
        return (diffType & value) == value;
    }

    static bool is_player_in_session(
        _In_ const string_t& xboxUserId,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session
        );

    static std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member> get_player_in_session(
        _In_ const string_t& xboxUserId,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session
        );

    static bool is_host(_In_ const string_t& xboxUserId,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session
        );

    static std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member> host_member(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session
        );

    static string_t get_local_user_xbox_user_id(
        _In_ xbox_live_user_t user
    );

    static bool has_session_property_changed(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& currentSession,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& oldSession,
        _In_ const string_t& propertyName
        );

    static bool is_local(
        _In_ const string_t& xboxUserId,
        _In_ const std::map<string_t, std::shared_ptr<multiplayer_local_user>>& xboxLiveContextMap
        );

    static std::shared_ptr<multiplayer_member> convert_to_game_member(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member>& member,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& lobbySession,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& gameSession,
        _In_ const std::map<string_t, std::shared_ptr<multiplayer_local_user>>& xboxLiveContextMap
        );

    static std::shared_ptr<multiplayer_member> convert_to_game_member(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member>& member,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& lobbySession,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& gameSession,
        _In_ bool isLocal
        );

    static bool compare_sessions(
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session1,
        _In_ const std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& session2
        );

    static void set_joinability(
        _In_ xbox::services::multiplayer::manager::joinability value,
        _In_ std::shared_ptr<multiplayer_session> sessionToCommit,
        _In_ bool isGameInProgress
        );

    static xbox::services::multiplayer::manager::joinability get_joinability(
        _In_ std::shared_ptr<multiplayer_session_properties> sessionProperties
        );

    static xbox::services::multiplayer::manager::joinability convert_string_to_joinability(
        _In_ const string_t& value
        );

    static string_t convert_joinability_to_string(_In_ xbox::services::multiplayer::manager::joinability value);
};

}}}}
