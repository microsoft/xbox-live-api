#pragma once
#include "httpClient/httpClient.h"
#include "shared_macros.h"
#include "xbox_live_context_settings_internal.h"

const char CONTENT_TYPE_HEADER[] = "Content-Type";
const char ACCEPT_LANGUAGE_HEADER[] = "Accept-Language";
const char CONTRACT_VERSION_HEADER[] = "x-xbl-contract-version";
const char USER_AGENT_HEADER[] = "User-Agent";

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

enum class xbox_live_api
{
    unspecified,
    add_user_to_club,
    add_club_role,
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
    create_club,
    create_match_ticket,
    delete_blob,
    delete_club,
    delete_match_ticket,
    download_blob,
    events_upload,
    get_achievement,
    get_achievements,
    get_activities_for_social_group,
    get_activities_for_users,
    get_avoid_or_mute_list,
    get_blob_metadata,
    get_broadcasts,
    get_catalog_item_details,
    get_club,
    get_club_batch,
    get_clubs_owned,
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
    delete_search_handle,
    get_session_host_allocation_status,
    get_sessions,
    get_single_user_statistics,
    get_social_graph,
    get_social_relationships,
    get_stats_value_document,
    get_ticket_status,
    get_teams,
    get_team_details,
    get_user_profiles,
    get_user_profiles_for_social_group,
    get_users_club_associations,
    recommend_clubs,
    register_team,
    remove_user_from_club,
    remove_club_role,
    rename_club,
    search_clubs,
    send_invites,
    set_activity,
    set_presence_helper,
    set_search_handle,
    set_transfer_handle,
    set_user_presence_within_club,
    submit_batch_reputation_feedback,
    submit_reputation_feedback,
    subscribe_to_notifications,
    suggest_clubs,
    update_achievement,
    patch_stats_value_document,
    post_stats_value_document,
    upload_blob,
    verify_strings,
    write_session_using_subpath,
    xbox_one_pins_add_item,
    xbox_one_pins_contains_item,
    xbox_one_pins_remove_item,
    post_recent_players,
    get_activity_batch,
    delete_activity,
    mpa_send_invites
};

using HttpHeaders = xsapi_internal_map<xsapi_internal_string, xsapi_internal_string>;
using HttpResult = Result<std::shared_ptr<class HttpCall>>;

// RAII wrapper around HCHttpCall. No Xbox Live specific logic.
class HttpCall : public RefCounter, public std::enable_shared_from_this<HttpCall>
{
public:
    HttpCall() = default;
    virtual ~HttpCall();

    HRESULT Init(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& fullUrl
    );

    virtual HRESULT SetHeader(
        _In_ const xsapi_internal_string& key,
        _In_ const xsapi_internal_string& value,
        _In_ bool allowTracing = true
    );

    virtual HRESULT SetTracing(bool traceCall);
    virtual HRESULT SetRequestBody(const xsapi_internal_vector<uint8_t>& bytes);
    virtual HRESULT SetRequestBody(_In_reads_bytes_(requestBodySize) const uint8_t* requestBodyBytes, _In_ uint32_t requestBodySize);
    virtual HRESULT SetRequestBody(const xsapi_internal_string& bodyString);
    virtual HRESULT SetRequestBody(const JsonValue& bodyJson);
    virtual HRESULT SetRequestBody(_In_z_ const char* requestBodyString);
    virtual HRESULT SetRetryAllowed(bool retryAllowed);
    virtual HRESULT SetRetryCacheId(uint32_t retryAfterCacheId);
    virtual HRESULT SetRetryDelay(uint32_t retryDelayInSeconds);
    virtual HRESULT SetTimeout(uint32_t timeoutInSeconds);
    virtual HRESULT SetTimeoutWindow(uint32_t timeoutWindowInSeconds);

    virtual HRESULT Perform(
        AsyncContext<HttpResult> async,
        bool forceRefresh = false
    );


    virtual HRESULT GetRequestUrl(const char** url) const;
    virtual HRESULT GetErrorMessage(const char** errorMessage) const;
    virtual HRESULT Result() const;
    virtual uint32_t HttpStatus() const;

    virtual xsapi_internal_string GetResponseHeader(const xsapi_internal_string& key) const;
    virtual HttpHeaders GetResponseHeaders() const;
    virtual xsapi_internal_vector<uint8_t> GetResponseBodyBytes() const;
    virtual HRESULT GetResponseBodyBytesSize(_Out_ size_t* bufferSize);
    virtual HRESULT GetResponseBodyBytes(_In_ size_t bufferSize, _Out_writes_bytes_to_(bufferSize, *bufferUsed) uint8_t* buffer, _Out_opt_ size_t* bufferUsed);
    virtual xsapi_internal_string GetResponseBodyString() const;
    virtual HRESULT GetResponseString(_Out_ const char** responseString);
    virtual JsonDocument GetResponseBodyJson() const;
    virtual HRESULT GetNetworkErrorCode(_Out_ HRESULT* networkErrorCode, _Out_ uint32_t* platformNetworkErrorCode);
    virtual HRESULT GetPlatformNetworkErrorMessage(_Out_ const char** platformNetworkErrorMessage);
    virtual HRESULT ResponseGetHeader(_In_z_ const char* headerName, _Out_ const char** headerValue);
    virtual HRESULT ResponseGetNumHeaders(_Out_ uint32_t* numHeaders);
    virtual HRESULT ResponseGetHeaderAtIndex(_In_ uint32_t headerIndex, _Out_ const char** headerName, _Out_ const char** headerValue);

private:
    HttpCall(const HttpCall&) = delete;
    HttpCall& operator=(HttpCall) = delete;
    static void CALLBACK CompletionCallback(_In_ XAsyncBlock* async);
    std::shared_ptr<RefCounter> GetSharedThis() override;
    static HRESULT ConvertHttpStatusToHRESULT(_In_ uint32_t httpStatusCode);
    HRESULT CopyHttpCallHandle();

    XAsyncBlock m_asyncBlock{};
    AsyncContext<HttpResult> m_asyncContext;
    bool m_performAlreadyCalled{ false };

    enum class Step
    {
        Uninitialized,
        Pending,
        Running,
        Done
    } m_step{ Step::Uninitialized };

protected:
    HCCallHandle m_callHandle{ nullptr };
    HRESULT ResetAndCopyForRetry();
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END


using namespace xbox::services;

// Xbox Live specific http call wrapper that gets and attaches the Xbox Live token,
// signs the request, and adds Xbox Live specific headers automatically.
// Handles 401 token refresh logic, and 429 throttling logic
struct XblHttpCall : public HttpCall
{
public:
    XblHttpCall(User&& user);

    virtual HRESULT Init(
        _In_ std::shared_ptr<XboxLiveContextSettings> contextSettings,
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& fullUrl,
        _In_ xbox_live_api xboxLiveApi
    );

    HRESULT SetHeader(
        _In_ const xsapi_internal_string& key,
        _In_ const xsapi_internal_string& value,
        _In_ bool allowTracing = true
    ) override;

    // Override the UserAgent that was specified in the contextSettings
    HRESULT SetUserAgent(_In_ HttpCallAgent userAgent);

    void SetLongHttpCall(_In_ bool longHttpCall);
    HRESULT SetXblServiceContractVersion(uint32_t contractVersion);

    HRESULT SetRequestBody(const xsapi_internal_vector<uint8_t>& bytes) override;
    HRESULT SetRequestBody(_In_reads_bytes_(requestBodySize) const uint8_t* requestBodyBytes, _In_ uint32_t requestBodySize) override;
    HRESULT SetRequestBody(const xsapi_internal_string& bodyString) override;
    HRESULT SetRequestBody(const JsonValue& bodyJson) override;
    HRESULT SetRequestBody(_In_z_ const char* requestBodyString) override;

    void SetAuthRetryAllowed(bool authRetryAllowed);
    HRESULT Perform(
        AsyncContext<HttpResult> async,
        bool forceRefresh = false
    ) override;

    static xsapi_internal_string BuildUrl(
        xsapi_internal_string&& serviceName,
        const xsapi_internal_string& pathQueryFragment
    );

private:
    XblHttpCall(const XblHttpCall&) = delete;
    XblHttpCall& operator=(XblHttpCall) = delete;

    void IntermediateHttpCallCompleteCallback(HttpResult result);
    HRESULT HandleAuthError(_In_ std::shared_ptr<class HttpCall> httpCall, _Out_ bool& wasHandled);
    void HandleThrottleError(_In_ std::shared_ptr<class HttpCall> httpCall);
    HRESULT CalcHttpTimeout();

    User m_user;
    xsapi_internal_vector<uint8_t> m_requestBody;
    HttpHeaders m_requestHeaders;
    xsapi_internal_string m_httpMethod;
    xsapi_internal_string m_fullUrl;
    bool m_longHttpCall{ false };
    uint32_t m_longHttpTimeout{ 0 };
    uint32_t m_httpTimeoutWindowInSeconds{ 0 };
    chrono_clock_t::time_point m_firstCallStartTime;
    chrono_clock_t::time_point m_requestStartTime;
    uint32_t m_iterationNumber{ 0 };
    AsyncContext<HttpResult> m_asyncContext;
    bool m_authRetryExplicitlyAllowed{ false };
    bool m_hasPerformedRetryOn401{ false };
};