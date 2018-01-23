// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "system_internal.h"

namespace xbox { namespace services { namespace social {

/// <summary>
/// Used to provide feedback within a game session.
/// </summary>
class reputation_feedback_request
{
    // Example:
    //{
    //    "evidenceId":"992399", 
    //    "feedbackType" : "PositiveHighQualityUGC", 
    //    "sessionName" : "Game", 
    //    "textReason" : "Great level"
    //}

public:
    /// <summary>
    /// Sends a feedback report for a game session. 
    /// </summary>
    /// <param name="feedbackType">The type of report being made.</param>
    /// <param name="sessionName">The name of the session in which the reported action occurred.</param>
    /// <param name="reasonMessage">The user supplied reason the report is being made.</param>
    /// <param name="evidenceResourceId">An identifier for the resource that shows evidance supporting the report.</param>
    reputation_feedback_request(
        _In_ reputation_feedback_type feedbackType,
        _In_ xsapi_internal_string sessionName,
        _In_ xsapi_internal_string reasonMessage,
        _In_ xsapi_internal_string evidenceResourceId
        );

    web::json::value serialize_feedback_request();
    static web::json::value serialize_batch_feedback_request(_In_ const xsapi_internal_vector< reputation_feedback_item >& feedbackItems, _Out_ std::error_code& err);
    static const xbox_live_result<xsapi_internal_string> convert_reputation_feedback_type_to_string(reputation_feedback_type feedbackType);

private:
    reputation_feedback_type m_feedbackType;
    xsapi_internal_string m_sessionName;
    xsapi_internal_string m_reasonMessage;
    xsapi_internal_string m_evidenceResourceId;
};

class social_service_impl : public std::enable_shared_from_this<social_service_impl>
{
public:
    social_service_impl(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService
        );

    xbox_live_result<std::shared_ptr<social_relationship_change_subscription>> subscribe_to_social_relationship_change(
        _In_ const xsapi_internal_string& xboxUserId
        );

    xbox_live_result<void> unsubscribe_from_social_relationship_change(
        _In_ std::shared_ptr<social_relationship_change_subscription> subscription
        );

    function_context add_social_relationship_changed_handler(
        _In_ std::function<void(social_relationship_change_event_args)> handler
        );

    void remove_social_relationship_changed_handler(
        _In_ function_context context
        );

    xbox_live_result<void> get_social_relationships(
        _In_ xbox_social_relationship_filter socialRelationshipFilter,
        _In_ uint32_t startIndex,
        _In_ uint32_t maxItems,
        _In_ uint64_t taskGroupId,
        _In_ xbox_live_callback<xbox_live_result<xbox_social_relationship_result>> callback
        );

    xbox_live_result<void> get_social_relationships(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ xbox_social_relationship_filter filter,
        _In_ uint32_t startIndex,
        _In_ uint32_t maxItems,
        _In_ uint64_t taskGroupId,
        _In_ xbox_live_callback<xbox_live_result<xbox_social_relationship_result>> callback
        );

private:
    void social_relationship_changed(_In_ social_relationship_change_event_args eventArgs);
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
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    xsapi_internal_unordered_map<uint32_t, std::function<void(const social_relationship_change_event_args&)>> m_socialRelationshipChangeHandler;
    function_context m_socialRelationshipChangeHandlerCounter;
    xbox::services::system::xbox_live_mutex m_socialRelationshipChangeHandlerLock;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_realTimeActivityService;
};

}}}