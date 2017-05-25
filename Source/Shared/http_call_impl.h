// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/system.h"
#include "http_call_response.h"
#include "http_client.h"
#include "system_internal.h"

#if XSAPI_SERVER || UNIT_TEST_SYSTEM || XSAPI_U
#include "signature_policy.h"
#include "Ecdsa.h"
#endif

#include "xsapi/http_call.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

enum class xbox_live_api
{
    unspecified,
    allocate_cluster,
    allocate_cluster_inline,
    allocate_session_host,
    browse_catalog_bundles_helper,
    browse_catalog_helper,
    check_multiple_permissions_with_multiple_target_users,
    check_permission_with_target_user,
    clear_activity,
    clear_search_handle,
    consume_inventory_item,
    create_match_ticket,
    delete_blob,
    delete_match_ticket,
    download_blob,
    get_achievement,
    get_achievements,
    get_activities_for_social_group,
    get_activities_for_users,
    get_avoid_or_mute_list,
    get_blob_metadata,
    get_broadcasts,
    get_catalog_item_details,
    get_configuration,
    get_current_session,
    get_current_session_by_handle,
    get_game_clips,
    get_game_server_metadata,
    get_hopper_statistics,
    get_inventory_item,
    get_inventory_items,
    get_leaderboard_for_social_group_internal,
    get_leaderboard_internal,
    get_match_ticket_details,
    get_multiple_user_statistics_for_multiple_service_configurations,
    get_presence,
    get_presence_for_multiple_users,
    get_presence_for_social_group,
    get_quality_of_service_servers,
    get_quota,
    get_quota_for_session_storage,
    get_search_handles,
    get_session_host_allocation_status,
    get_sessions,
    get_single_user_statistics,
    get_social_graph,
    get_social_relationships,
    get_stats_value_document,
    get_ticket_status,
    get_tournaments,
    get_tournament_details,
    get_teams,
    get_team_details,
    get_user_profiles,
    get_user_profiles_for_social_group,
    register_team,
    send_invites,
    set_activity,
    set_presence_helper,
    set_search_handle,
    set_transfer_handle,
    submit_batch_reputation_feedback,
    submit_reputation_feedback,
    subscribe_to_notifications,
    update_achievement,
    update_stats_value_document,
    upload_blob,
    verify_strings,
    write_session_using_subpath,
    xbox_one_pins_add_item,
    xbox_one_pins_contains_item,
    xbox_one_pins_remove_item
};

struct http_call_data
{
    http_call_data(
        _In_ const std::shared_ptr<xbox_live_context_settings>& _xboxLiveContextSettings,
        _In_ const string_t& _httpMethod,
        _In_ const string_t& _serverName,
        _In_ const web::uri& _pathQueryFragment,
        _In_ xbox_live_api _xboxLiveApi
        ) :
        xboxLiveContextSettings(_xboxLiveContextSettings),
        httpMethod(_httpMethod),
        serverName(_serverName),
        pathQueryFragment(_pathQueryFragment),
        xboxLiveApi(_xboxLiveApi),
        hasPerformedRetryOn401(false),
        retryAllowed(true),
        iterationNumber(0),
        httpCallResponseBodyType(http_call_response_body_type::json_body),
        longHttpCall(false),
        httpTimeout(std::chrono::seconds(DEFAULT_HTTP_TIMEOUT_SECONDS)),
        contentTypeHeaderValue(_T("application/json; charset=utf-8")),
        xboxContractVersionHeaderValue(_T("1")),
        addDefaultHeaders(true)
    {
        delayBeforeRetry = xboxLiveContextSettings->http_retry_delay();
    }

    std::chrono::milliseconds delayBeforeRetry;
    chrono_clock_t::time_point firstCallStartTime;
    bool hasPerformedRetryOn401;
    bool retryAllowed;
    uint32_t iterationNumber;

    bool longHttpCall;
    std::chrono::seconds httpTimeout;
    std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings;
    std::shared_ptr<user_context> userContext;

    xbox_live_api xboxLiveApi;
    string_t httpMethod;
    string_t serverName;
    web::uri pathQueryFragment;
    string_t xboxContractVersionHeaderValue;
    string_t contentTypeHeaderValue;
    std::unordered_map<string_t, string_t> customHeaderMap;

    web::http::http_request request;
    http_call_response_body_type httpCallResponseBodyType;
    http_call_request_message requestBody;
    bool addDefaultHeaders;
};

struct http_retry_after_api_state
{
    http_retry_after_api_state()
    {
        errCode = std::make_error_code(xbox_live_error_code::no_error);
    }

    http_retry_after_api_state(
        _In_ const chrono_clock_t::time_point& _retryAfterTime,
        _In_ const std::error_code& _errCode,
        _In_ const std::string& _errMessage
        ) :
        retryAfterTime(_retryAfterTime),
        errCode(_errCode),
        errMessage(_errMessage)
    {
    }

    chrono_clock_t::time_point retryAfterTime;
    std::error_code errCode;
    std::string errMessage;
};

class http_call_internal : public http_call
{
public:

    /// <summary>
    /// Send the request without authentication and get the response of a specific type
    /// </summary>
    virtual pplx::task< std::shared_ptr<http_call_response> > get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ const web::http::http_request& httpRequest
        ) = 0;

    virtual web::http::http_request get_default_request() = 0;

    virtual const http_call_request_message& request_body() const = 0;

#if XSAPI_SERVER || UNIT_TEST_SYSTEM || XSAPI_U
    /// <summary>
    /// Sign the request and get the response. Used for auth services.
    /// </summary>
    virtual pplx::task<std::shared_ptr<http_call_response>> get_response(
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::system::ecdsa> proofKey,
        _In_ const XBOX_LIVE_NAMESPACE::system::signature_policy& signaturePolicy,
        _In_ http_call_response_body_type httpCallResponseBodyType
        ) = 0;
#endif
};

class http_retry_after_manager
{
public:
    static std::shared_ptr<http_retry_after_manager> get_http_retry_after_manager_singleton();

    void set_state(
        _In_ xbox_live_api xboxLiveApi,
        _In_ const http_retry_after_api_state& state
        );

    void clear_state(
        _In_ xbox_live_api xboxLiveApi
        );

    http_retry_after_api_state get_state(
        _In_ xbox_live_api xboxLiveApi
        );

private:
	XBOX_LIVE_NAMESPACE::system::xbox_live_mutex m_lock;
    std::unordered_map<uint32_t, http_retry_after_api_state> m_apiStateMap;
};

class http_call_impl : public http_call_internal, public std::enable_shared_from_this<http_call_impl>
{
public:
    http_call_impl();

    http_call_impl(
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const string_t& httpMethod,
        _In_ const string_t& serverName,
        _In_ const web::uri& pathQueryFragment,
        _In_ xbox_live_api xboxLiveApi
        );

#if XSAPI_SERVER || UNIT_TEST_SYSTEM || XSAPI_U
    pplx::task<std::shared_ptr<http_call_response>> get_response(
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::system::ecdsa> proofKey,
        _In_ const XBOX_LIVE_NAMESPACE::system::signature_policy& signaturePolicy,
        _In_ http_call_response_body_type httpCallResponseBodyType
         ) override;
#endif

    pplx::task< std::shared_ptr<http_call_response> > get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType
        ) override;

    pplx::task<std::shared_ptr<http_call_response>> get_response(
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ const web::http::http_request& httpRequest
        ) override;

    pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ const std::shared_ptr<XBOX_LIVE_NAMESPACE::user_context>& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ bool allUsersAuthRequired = false
        ) override;

    pplx::task<std::shared_ptr<http_call_response>> get_response_with_auth(
        _In_ http_call_response_body_type httpCallResponseBodyType
        ) override;

    pplx::task<std::shared_ptr<http_call_response>> _Internal_get_response_with_auth(
        _In_ const std::shared_ptr<user_context>& userContext,
        _In_ http_call_response_body_type httpCallResponseBodyType,
        _In_ bool allUsersAuthRequired
        ) override;

    const string_t& server_name() const override;
    const web::uri& path_query_fragment() const override;
    const string_t& http_method() const override;

    void set_add_default_headers(bool value) override;
    bool add_default_headers() const override;


    void set_long_http_call(_In_ bool value) override;
    bool long_http_call() const override;

    void set_retry_allowed(_In_ bool value) override;
    bool retry_allowed() const override; 

    void set_request_body(_In_ const string_t& value) override;
    void set_request_body(_In_ const web::json::value& value) override;
    void set_request_body(_In_ const std::vector<uint8_t>& value) override;
    const http_call_request_message& request_body() const override;

    void set_content_type_header_value(_In_ const string_t& value) override;
    const string_t& content_type_header_value() const override;

    void set_xbox_contract_version_header_value(_In_ const string_t& value) override;
    const string_t& xbox_contract_version_header_value() const override;

    void set_custom_header(
        _In_ const string_t& headerName,
        _In_ const string_t& headerValue
        ) override;

    web::http::http_request get_default_request() override;

private:
    NO_COPY_AND_ASSIGN(http_call_impl);

    std::shared_ptr<http_call_data> m_httpCallData;

    static bool should_retry(
        _In_ const std::shared_ptr<http_call_response>& httpCallResponse,
        _In_ const std::shared_ptr<http_call_data>& httpCallData,
        _In_ xbox_live_error_code httpNetworkError
        );

    static bool handle_unauthorized_error(
        _In_ const std::shared_ptr<http_call_data>& httpCallData
        );

    static pplx::task<std::shared_ptr<http_call_response>> internal_get_response(
        _In_ const std::shared_ptr<http_call_data>& httpCallData
        );

    static void set_user_agent(
        _In_ const std::shared_ptr<http_call_data>& httpCallData
        );

    static web::http::client::http_client_config get_config(
        _In_ const std::shared_ptr<http_call_data>& httpCallData
        );

    static void handle_response_error(
        _In_ const std::shared_ptr<http_call_response>& httpCallResponse,
        _In_ xbox_live_error_code errFromException,
        _In_ const std::string& errMessage,
        _In_ const web::http::http_response& response
        );

    static std::shared_ptr<http_call_response> get_http_call_response(
        _In_ const std::shared_ptr<http_call_data>& httpCallData,
        _In_ const web::http::http_response& response
        );

    static XBOX_LIVE_NAMESPACE::xbox_live_error_code get_xbox_live_error_code_from_http_status(
        _In_ const web::http::status_code& statusCode
        );

    static pplx::task<std::shared_ptr<http_call_response>> handle_json_body_response(
        _In_ web::http::http_response httpResponse,
        _In_ std::shared_ptr<http_call_response> httpCallResponse
        );

    static pplx::task<std::shared_ptr<http_call_response>> handle_string_body_response(
        _In_ web::http::http_response httpResponse,
        _In_ std::shared_ptr<http_call_response> httpCallResponse
        );

    static pplx::task<std::shared_ptr<http_call_response>> handle_vector_body_response(
        _In_ web::http::http_response httpResponse,
        _In_ std::shared_ptr<http_call_response> httpCallResponse
        );

    static bool should_fast_fail(
        _In_ const http_retry_after_api_state& apiState,
        _In_ const std::shared_ptr<http_call_data>& httpCallData,
        _In_ const chrono_clock_t::time_point& currentTime
        );

    static pplx::task<std::shared_ptr<http_call_response>> handle_fast_fail(
        _In_ const http_retry_after_api_state& apiState,
        _In_ const std::shared_ptr<http_call_data>& httpCallData
        );

    static void set_http_timeout(
        _In_ const std::shared_ptr<http_call_data>& httpCallData,
        _In_ const chrono_clock_t::time_point& currentTime
        );
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
