// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "system_internal.h"
#include "xsapi/multiplayer.h"
namespace xbox { namespace services { namespace tournaments {
    class tournament_team_result;
}}}

namespace xbox { namespace services { namespace multiplayer {

class multiplayer_invite_handle_post_request
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    multiplayer_invite_handle_post_request();

    /// <summary>
    /// Internal function
    /// </summary>
    multiplayer_invite_handle_post_request(
        _In_ multiplayer_session_reference sessionReference,
        _In_ uint32_t version,
        _In_ string_t invitedXboxUserId,
        _In_ uint32_t titleId,
        _In_ string_t contextString,
        _In_ string_t customActivationContext
        );

    /// <summary>
    /// Internal function
    /// </summary>
    const multiplayer_session_reference& session_reference() const;

    /// <summary>
    /// Internal function
    /// </summary>
    uint32_t version() const;

    /// <summary>
    /// Internal function
    /// </summary>
    const string_t& invited_xbox_user_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    uint32_t title_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    const string_t& context_string() const;

    /// <summary>
    /// Internal function
    /// </summary>
    const string_t& custom_activation_context() const;

    /// <summary>
    /// Internal function
    /// </summary>
    web::json::value serialize() const;

private:
    multiplayer_session_reference m_sessionReference;
    uint32_t m_version;
    string_t m_invitedXboxUserId;
    uint32_t m_titleId;
    string_t m_contextString;
    string_t m_customActivationContext;
};

class multiplayer_subscription : public xbox::services::real_time_activity::real_time_activity_subscription
{
public:
    multiplayer_subscription(
        _In_ const std::function<void(const multiplayer_session_change_event_args&)>& multiplayerSessionChangeHandler,
        _In_ const std::function<void()>& multiplayerSubscriptionLostHandler,
        _In_ const std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)>& subscriptionErrorHandler
        );

    void _Set_state(_In_ xbox::services::real_time_activity::real_time_activity_subscription_state newState) override;
    const string_t& rta_connection_id() const;
    pplx::task_completion_event<xbox_live_result<string_t>> task;

protected:
    void on_subscription_created(_In_ uint32_t id, _In_ const web::json::value& data) override;
    void on_event_received(_In_ const web::json::value& data) override;

private:
    string_t m_mpConnectionId;
    std::function<void(const multiplayer_session_change_event_args&)> m_multiplayerSessionChangeHandler;
    std::function<void()> m_multiplayerSubscriptionLostHandler;
};


class multiplayer_activity_query_post_request
{
public:
    multiplayer_activity_query_post_request();
    multiplayer_activity_query_post_request(_In_ string_t scid, _In_ std::vector<string_t> xuids);
    multiplayer_activity_query_post_request(_In_ string_t scid, _In_ string_t socialGroup, _In_ string_t socialGroupXuid);

    const string_t& scid() const;
    const std::vector<string_t>& xuids() const;
    const string_t& social_group() const;
    const string_t& social_group_xuid() const;

    web::json::value serialize();
private:
    string_t m_scid;
    std::vector<string_t> m_xuids;
    string_t m_socialGroupXuid;
    string_t m_socialGroup;
};

class multiplayer_session_member_request
{
public:
    multiplayer_session_member_request();

    multiplayer_session_member_request(
        _In_ bool writeRequest,
        _In_ string_t memberId,
        _In_ string_t xboxUserId,
        _In_ web::json::value memberConstants,
        _In_ bool addInitializePropertyToRequest,
        _In_ bool initializeRequested
        );

    bool write_request() const;
    bool write_constants() const;
    const string_t& member_id() const;
    const web::json::value& custom_constants() const;

    const web::json::value& custom_properties() const;
    void set_custom_properties(_In_ web::json::value customProperties);
    std::error_code set_custom_properties_property(
        _In_ string_t name,
        _In_ web::json::value customProperty
        );

    const string_t& xbox_user_id() const;

    bool is_active() const;
    void set_is_active(_In_ bool isActive);

    bool write_is_active() const;
    void set_write_is_active(_In_ bool writeIsActive);

    const string_t& service_device_address_base64() const;
    void set_secure_device_address_base64(_In_ string_t deviceAddress);

    void set_role_info(_In_ const std::unordered_map<string_t, string_t>& roles);

    bool write_secure_device_address_base64() const;
    void set_write_secure_device_address_base64(_In_ bool shouldWrite);

    bool does_initialized_requested_exist() const;
    bool initialize_requested() const;
    
    const web::json::value& member_measurements_json() const;
    void set_member_measurements_json(_In_ web::json::value memberMeasurements);

    bool write_member_measurements_json() const;
    void set_write_member_measurements_json(_In_ bool shouldWrite);

    const web::json::value& member_server_measurements_json() const;
    void set_member_server_measurements_json(_In_ web::json::value json);

    bool write_member_server_measurements_json() const;
    void set_write_member_server_measurements_json(_In_ bool shouldWrite);

    std::vector<uint32_t> members_in_group() const;
    void set_members_in_group(_In_ std::vector<uint32_t> membersInGroup);

    bool write_members_in_group() const;
    void set_write_members_in_group(_In_ bool writeMembersInGroup);

    const std::vector<string_t>& groups() const;
    void set_groups(_In_ std::vector<string_t> groups);

    const std::vector<string_t>& encounters() const;
    void set_encounters(_In_ std::vector<string_t> encounters);

    bool write_subscribed_change_types() const;
    void set_write_subscribed_change_types(_In_ bool shouldWrite);

    multiplayer_session_change_types subscribed_change_types() const;
    void set_subscribed_change_types(_In_ multiplayer_session_change_types changeTypes);

    const string_t& subscription_id() const;
    void set_subscription_id(_In_ string_t subscriptionId);

    const string_t& rta_connection_id() const;
    void set_rta_connection_id(_In_ string_t connectionId);

    const std::unordered_map<string_t, xbox::services::tournaments::tournament_team_result>& results() const;
    void set_result(_In_ const string_t& team, _In_ const xbox::services::tournaments::tournament_team_result& result);
    void set_results(_In_ const std::unordered_map<string_t, xbox::services::tournaments::tournament_team_result>& results);

    web::json::value serialize();
private:
    static std::vector<string_t> get_vector_view_for_change_types(_In_ multiplayer_session_change_types changeTypes);

    bool m_writeRequest;
    bool m_writeConstants;
    string_t m_memberId;
    web::json::value m_customConstants;
    web::json::value m_customProperties;
    string_t m_xboxUserId;
    bool m_writeIsActive;
    bool m_isActive;
    std::unordered_map<string_t, string_t> m_roles;
    bool m_writeRoleInfo;
    string_t m_serviceDeviceAddressBase64;
    bool m_writeSecureDeviceAddressBase64;
    bool m_addInitializePropertyToRequest;
    bool m_initializeRequested;
    bool m_writeMemberMeasurementsJson;
    web::json::value m_memberMeasurementsJson;
    bool m_writeMemberServerMeasurementsJson;
    web::json::value m_memberServerMeasurementsJson;
    bool m_writeMembersInGroup;
    std::vector<uint32_t> m_membersInGroup;
    std::vector<string_t> m_groups;
    std::vector<string_t> m_encounters;
    bool m_writeSubscribedChangeType;
    multiplayer_session_change_types m_changeTypes;
    string_t m_subscriptionId;
    string_t m_rtaConnectionId;
    std::unordered_map<string_t, xbox::services::tournaments::tournament_team_result> m_results;
};

/// <summary>
/// Represents a reference to member in a multiplayer session.
/// </summary>
class multiplayer_session_request
{
public:
    multiplayer_session_request();
    multiplayer_session_request(
        _In_opt_ std::shared_ptr<multiplayer_session_constants> multiplayerSessionConstants
        );

    std::shared_ptr<multiplayer_session_request> create_deep_copy();

    std::shared_ptr<multiplayer_session_member_request> add_member_request(
        _In_ bool writeRequest,
        _In_ bool isMe,
        _In_ string_t xboxUserId,
        _In_ web::json::value memberConstants,
        _In_ bool addInitializePropertyToRequest,
        _In_ bool initializedRequested
        );

    void leave_session();
    void set_leave_session(_In_ bool leaveSession);

    const multiplayer_session_reference& session_reference() const;
    void set_session_reference(_In_ multiplayer_session_reference sessionReference);

    std::shared_ptr<multiplayer_session_constants> session_constants() const;
    void set_session_constants(_In_ std::shared_ptr<multiplayer_session_constants> sessionConstants);

    const std::vector<string_t>& session_properties_keywords() const;
    void set_session_properties_keywords(_In_ std::vector<string_t> keywords);

    const std::vector<uint32_t>& session_properties_turns() const;
    void set_session_properties_turns(_In_ std::vector<uint32_t> sessionProperties);

    multiplayer_session_restriction join_restriction() const;
    void set_join_restriction(_In_ multiplayer_session_restriction joinRestriction);

    multiplayer_session_restriction read_restriction() const;
    void set_read_restriction(_In_ multiplayer_session_restriction readRestriction);

    const web::json::value& session_properties_custom_properties() const;
    void set_session_custom_properties(_In_ web::json::value sessionCustomProperties);

    bool write_matchmaking_client_result() const;
    void set_write_matchmaking_client_result(_In_ bool writeMatchmakingClientResult);

    bool write_matchmaking_session_constants() const;
    void set_write_matchmaking_session_constants(_In_ bool matchmakingSessionConstant);

    const web::json::value& session_properties_target_sessions_constants() const;
    void set_session_properties_target_sessions_constants(_In_ web::json::value targetSessionsConstants);

    bool write_initialization_status() const;
    void set_write_initialization_status(_In_ bool status);

    bool initialization_succeeded() const;
    void set_initialization_succeeded(_In_ bool succeeded);

    bool write_host_device_token() const;
    void set_write_host_device_token(_In_ bool writeToken);

    const string_t& host_device_token() const;
    void set_host_device_token(_In_ string_t deviceToken);

    bool write_matchmaking_server_connection_path() const;
    void set_write_matchmaking_server_connection_path(_In_ bool writePath);

    const string_t& matchmaking_server_connection_path() const;
    void set_matchmaking_server_connection_path(_In_ string_t serverPath);

    bool write_matchmaking_resubmit() const;
    void set_write_matchmaking_resubmit(_In_ bool writeMatchmakingResubmit);

    bool matchmaking_match_resubmit() const;
    void set_matchmaking_match_resubmit(_In_ bool matchmakingResubmit);

    bool write_server_connection_string_candidates() const;
    void set_write_server_connection_string_candidates(_In_ bool writeCandidates);

    const std::vector<string_t>& server_connection_string_candidates() const;
    void set_server_connection_string_candidates(_In_ std::vector<string_t> candidates);

    const std::vector<std::shared_ptr<multiplayer_session_member_request>>& members() const;
    void set_members(_In_ std::vector<std::shared_ptr<multiplayer_session_member_request>> members);

    web::json::value servers() const;
    void set_servers(_In_ web::json::value servers);

    bool write_closed() const;
    void set_write_closed(_In_ bool writeClosed);

    bool closed() const;
    void set_closed(_In_ bool closed);

    bool write_locked() const;
    void set_write_locked(_In_ bool writeLocked);
    bool locked() const;
    void set_locked(_In_ bool locked);

    bool write_allocate_cloud_compute() const;
    void set_write_allocate_cloud_compute(_In_ bool writeAllocateCloudCompute);

    bool allocate_cloud_compute() const;
    void set_allocate_cloud_compute(_In_ bool allocateCloudCompute);

    void set_mutable_role_settings(_In_ const std::unordered_map<string_t, multiplayer_role_type>& roleTypes);

    web::json::value create_properties_json();
    web::json::value create_matchmaking_json();
    web::json::value create_role_types_json();
    web::json::value serialize();

private:
    void deep_copy_from(
        _In_ const multiplayer_session_request& other
        );

    multiplayer_session_reference m_sessionReference;
    std::unordered_map<string_t, multiplayer_role_type> m_roleTypes;
    std::shared_ptr<multiplayer_session_constants> m_sessionConstants;
    multiplayer_session_member_request m_memberRequest;
    std::vector<string_t> m_sessionPropertiesKeywords;
    std::vector<uint32_t> m_sessionPropertiesTurns;
    multiplayer_session_restriction m_joinRestriction;
    multiplayer_session_restriction m_readRestriction;
    web::json::value m_sessionPropertiesCustomProperties;
    bool m_writeMatchmakingClientResult;
    bool m_writeMatchmakingSessionConstants;
    web::json::value m_sessionPropertiesTargetSessionsConstants;
    bool m_writeInitializationStatus;
    bool m_initializationSucceeded;
    bool m_writeHostDeviceToken;
    string_t m_hostDeviceToken;
    bool m_writeMatchmakingServerConnectionPath;
    string_t m_matchmakingServerConnectionPath;
    bool m_writeMatchmakingResubmit;
    bool m_matchmakingMatchResubmit;
    bool m_writeServerConnectionStringCandidates;
    std::vector<string_t> m_serverConnectionStringCandidates;
    std::vector<std::shared_ptr<multiplayer_session_member_request>> m_members;
    web::json::value m_servers;
    uint32_t m_memberRequestIndex;
    bool m_bLeaveSession;
    xbox::services::system::xbox_live_mutex m_lock;
    bool m_writeClosed;
    bool m_closed;
    bool m_writeLocked;
    bool m_locked;
    bool m_writeAllocateCloudCompute;
    bool m_allocateCloudCompute;
};

class multiplayer_invite
{
public:
    multiplayer_invite();

    const string_t& handle_id() const;

    static xbox_live_result<multiplayer_invite> deserialize(_In_ const web::json::value& inputJson);

private:
    string_t m_handleId;
};

class multiplayer_activity_handle_post_request
{
public:
    multiplayer_activity_handle_post_request();
    multiplayer_activity_handle_post_request(_In_ multiplayer_session_reference sessionReference, _In_ uint32_t version);

    const multiplayer_session_reference& session_reference() const;
    uint32_t version() const;

    web::json::value serialize() const;
private:
    multiplayer_session_reference m_sessionReference;
    uint32_t m_version;
};

class multiplayer_transfer_handle_post_request
{
public:
    multiplayer_transfer_handle_post_request();
    multiplayer_transfer_handle_post_request(
        _In_ multiplayer_session_reference targetSessionReference,
        _In_ multiplayer_session_reference originSessionReference,
        _In_ uint32_t version
        );

    const multiplayer_session_reference& origin_session_reference() const;
    const multiplayer_session_reference& target_session_reference() const;
    uint32_t version() const;

    web::json::value serialize() const;
private:
    multiplayer_session_reference m_originSessionReference;
    multiplayer_session_reference m_targetSessionReference;
    uint32_t m_version;
};

class multiplayer_service_impl : public std::enable_shared_from_this<multiplayer_service_impl>
{
public:
    multiplayer_service_impl(
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService
        );

    void multiplayer_session_changed(_In_ const multiplayer_session_change_event_args& eventArgs);
    void multiplayer_subscription_lost();

    pplx::task<xbox_live_result<string_t>> ensure_multiplayer_subscription();

    std::error_code enable_multiplayer_subscriptions();

    void disable_multiplayer_subscriptions();

    bool subscriptions_enabled();

    function_context add_multiplayer_session_changed_handler(_In_ std::function<void(const multiplayer_session_change_event_args&)> handler);

    void remove_multiplayer_session_changed_handler(_In_ function_context context);

    function_context add_multiplayer_subscription_lost_handler(_In_ std::function<void()> handler);

    void remove_multiplayer_subscription_lost_handler(_In_ function_context context);
    
    ~multiplayer_service_impl();

private:
    std::shared_ptr<real_time_activity::real_time_activity_service> m_realTimeActivityService;
    std::shared_ptr<multiplayer_subscription> m_subscription;
    std::unordered_map<uint32_t, std::function<void(const multiplayer_session_change_event_args&)>> m_sessionChangeEventHandler;
    std::unordered_map<uint32_t, std::function<void()>> m_multiplayerSubscriptionLostEventHandler;

    function_context m_sessionChangeEventHandlerCounter;
    function_context m_multiplayerSubscriptionLostEventHandlerCounter;

    bool m_subscriptionEnabled;
    xbox::services::system::xbox_live_mutex m_subscriptionEnabledLock;
    xbox::services::system::xbox_live_mutex m_subscriptionLock;
    function_context m_multiplayerJoinabilityChangeCounter;
};

}}}