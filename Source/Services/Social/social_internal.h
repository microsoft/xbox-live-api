// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social.h"
#include "system_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

class xbox_social_relationship_internal
{
public:
    xbox_social_relationship_internal(
        _In_ xsapi_internal_string xboxUserId,
        _In_ bool isFavorite,
        _In_ bool isFollowingCaller,
        _In_ xsapi_internal_vector<xsapi_internal_string> socialNetworks
        );

    const xsapi_internal_string& xbox_user_id() const;

    bool is_favorite() const;

    bool is_following_caller() const;

    const xsapi_internal_vector<xsapi_internal_string>& social_networks() const;

    static xbox_live_result<std::shared_ptr<xbox_social_relationship_internal>> deserialize(_In_ const web::json::value& json);
private:
    xsapi_internal_string m_xboxUserId;
    bool m_isFavorite;
    bool m_isFollowingCaller;
    xsapi_internal_vector<xsapi_internal_string> m_socialNetworks;
};

class xbox_social_relationship_result_internal
{
public:
    xbox_social_relationship_result_internal(
        _In_ xsapi_internal_vector<std::shared_ptr<xbox_social_relationship_internal>> socialRelationships,
        _In_ uint32_t totalCount
        );

    const xsapi_internal_vector<std::shared_ptr<xbox_social_relationship_internal>>& items() const;

    uint32_t total_count() const;

    bool has_next() const;

    xbox_live_result<void> get_next(
        _In_ uint32_t maxItems,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>> callback
        );

    void init_next_page_info(
        _In_ std::shared_ptr<social_service_impl> socialImpl,
        _In_ xbox_social_relationship_filter filter,
        _In_ uint32_t continuationSkip
        );

    xbox_social_relationship_filter filter() const;

    uint32_t continuation_skip() const;

    static xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>> deserialize(_In_ const web::json::value& json);

private:
    std::shared_ptr<social_service_impl> m_socialImpl;

    xsapi_internal_vector<std::shared_ptr<xbox_social_relationship_internal>> m_socialRelationships;
    uint32_t m_totalCount;
    xbox_social_relationship_filter m_filter;
    uint32_t m_continuationSkip;
};

class social_relationship_change_event_args_internal
{
public:
    social_relationship_change_event_args_internal();

    social_relationship_change_event_args_internal(
        _In_ xsapi_internal_string callerXboxUserId,
        _In_ social_notification_type notificationType,
        _In_ xsapi_internal_vector<xsapi_internal_string> xboxUserIds
        );

    const xsapi_internal_string& caller_xbox_user_id() const;

    social_notification_type social_notification() const;

    const xsapi_internal_vector<xsapi_internal_string>& xbox_user_ids() const;

private:
    xsapi_internal_string m_callerXboxUserId;
    social_notification_type m_notificationType;
    xsapi_internal_vector<xsapi_internal_string> m_xboxUserIds;
};

class social_relationship_change_subscription_internal : public xbox::services::real_time_activity::real_time_activity_subscription
{
public:
    social_relationship_change_subscription_internal(
        _In_ xsapi_internal_string xboxUserId,
        _In_ xbox_live_callback<std::shared_ptr<social_relationship_change_event_args_internal>> handler,
        _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler // TODO change to xbox_live_callback
        );

    const xsapi_internal_string& xbox_user_id() const;

protected:
    void on_subscription_created(_In_ uint32_t id, _In_ const web::json::value& data) override;

    void on_event_received(_In_ const web::json::value& data) override;

private:
    social_notification_type convert_string_type_to_notification_type(
        _In_ const xsapi_internal_string& notificationType
        ) const;

    xsapi_internal_string m_xboxUserId;
    xbox_live_callback<std::shared_ptr<social_relationship_change_event_args_internal>> m_handler;
};

class social_service_impl : public std::enable_shared_from_this<social_service_impl>
{
public:
    social_service_impl(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig,
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService
        );

    xbox_live_result<std::shared_ptr<social_relationship_change_subscription_internal>> subscribe_to_social_relationship_change(
        _In_ const xsapi_internal_string& xboxUserId
        );

    xbox_live_result<void> unsubscribe_from_social_relationship_change(
        _In_ std::shared_ptr<social_relationship_change_subscription_internal> subscription
        );

    function_context add_social_relationship_changed_handler(
        _In_ xbox_live_callback<std::shared_ptr<social_relationship_change_event_args_internal>> handler
        );

    void remove_social_relationship_changed_handler(
        _In_ function_context context
        );

    xbox_live_result<void> get_social_relationships(
        _In_ xbox_social_relationship_filter socialRelationshipFilter,
        _In_ uint32_t startIndex,
        _In_ uint32_t maxItems,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>> callback
        );

    xbox_live_result<void> get_social_relationships(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ xbox_social_relationship_filter filter,
        _In_ uint32_t startIndex,
        _In_ uint32_t maxItems,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>> callback
        );

private:
    void social_relationship_changed(_In_ std::shared_ptr<social_relationship_change_event_args_internal> eventArgs);

    static xsapi_internal_string pathandquery_social_subpath(
        _In_ const xsapi_internal_string& ownerId,
        _In_ bool includeViewFilter,
        _In_ const xsapi_internal_string& view,
        _In_ uint64_t startIndex,
        _In_ uint64_t maxItems
        );

    static const xsapi_internal_string xbox_social_relationship_filter_to_string(
        _In_ xbox_social_relationship_filter xboxSocialRelationshipFilter
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config_internal> m_appConfig;
    xsapi_internal_unordered_map<uint32_t, xbox_live_callback<std::shared_ptr<social_relationship_change_event_args_internal>>> m_socialRelationshipChangeHandler;
    function_context m_socialRelationshipChangeHandlerCounter;
    xbox::services::system::xbox_live_mutex m_socialRelationshipChangeHandlerLock;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_realTimeActivityService;
};

class reputation_feedback_item_internal
{
public:
    reputation_feedback_item_internal() {}

    reputation_feedback_item_internal(
        _In_ const reputation_feedback_item& legacyCppObj
        );

    reputation_feedback_item_internal(
        _In_ xsapi_internal_string xboxUserId,
        _In_ reputation_feedback_type reputationFeedbackType,
        _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef = xbox::services::multiplayer::multiplayer_session_reference(),
        _In_ xsapi_internal_string reasonMessage = xsapi_internal_string(),
        _In_ xsapi_internal_string evidenceResourceId = xsapi_internal_string()
        );

    const xsapi_internal_string& xbox_user_id() const;

    reputation_feedback_type feedback_type() const;

    const xbox::services::multiplayer::multiplayer_session_reference& session_reference() const;

    const xsapi_internal_string& reason_message() const;

    const xsapi_internal_string& evidence_resource_id() const;

private:
    xsapi_internal_string m_xboxUserId;
    reputation_feedback_type m_reputationFeedbackType;
    xbox::services::multiplayer::multiplayer_session_reference m_sessionRef;
    xsapi_internal_string m_reasonMessage;
    xsapi_internal_string m_evidenceResourceId;
};

class reputation_feedback_request
{
public:
    reputation_feedback_request(
        _In_ reputation_feedback_type feedbackType,
        _In_ xsapi_internal_string sessionName,
        _In_ xsapi_internal_string reasonMessage,
        _In_ xsapi_internal_string evidenceResourceId
        );

    web::json::value serialize_feedback_request();

    static web::json::value serialize_batch_feedback_request(
        _In_ const xsapi_internal_vector<reputation_feedback_item_internal>& feedbackItems,
        _Out_ std::error_code& err
        );

    static const xbox_live_result<xsapi_internal_string> convert_reputation_feedback_type_to_string(
        reputation_feedback_type feedbackType
        );

private:
    reputation_feedback_type m_feedbackType;
    xsapi_internal_string m_sessionName;
    xsapi_internal_string m_reasonMessage;
    xsapi_internal_string m_evidenceResourceId;
};

class reputation_service_impl : public std::enable_shared_from_this<reputation_service_impl>
{
public:
    reputation_service_impl(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig
        );

    _XSAPIIMP xbox_live_result<void> submit_reputation_feedback(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ reputation_feedback_type reputationFeedbackType,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<void>> callback,
        _In_ const xsapi_internal_string& sessionName = xsapi_internal_string(),
        _In_ const xsapi_internal_string& reasonMessage = xsapi_internal_string(),
        _In_ const xsapi_internal_string& evidenceResourceId = xsapi_internal_string()
        );

    _XSAPIIMP xbox_live_result<void> submit_batch_reputation_feedback(
        _In_ const xsapi_internal_vector<reputation_feedback_item_internal>& feedbackItems,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<void>> callback
        );

private:
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config_internal> m_appConfig;

    xsapi_internal_string reputation_feedback_subpath(
        _In_ const xsapi_internal_string& xboxUserId
        );
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END