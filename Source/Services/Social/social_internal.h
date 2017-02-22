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
        _In_ string_t sessionName,
        _In_ string_t reasonMessage,
        _In_ string_t evidenceResourceId
        );

    web::json::value serialize_feedback_request();
    static web::json::value serialize_batch_feedback_request(_In_ const std::vector< reputation_feedback_item >& feedbackItems, _Out_ std::error_code& err);
    static const xbox_live_result<string_t> convert_reputation_feedback_type_to_string(reputation_feedback_type feedbackType);

private:
    reputation_feedback_type m_feedbackType;
    string_t m_sessionName;
    string_t m_reasonMessage;
    string_t m_evidenceResourceId;
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
        _In_ const string_t& xboxUserId
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

    pplx::task<xbox_live_result<xbox_social_relationship_result>> get_social_relationships(
        _In_ xbox_social_relationship_filter socialRelationshipFilter,
        _In_ uint32_t startIndex,
        _In_ uint32_t maxItems
        );

    pplx::task<xbox_live_result<xbox_social_relationship_result>> get_social_relationships(
        _In_ const string_t& xboxUserId,
        _In_ xbox_social_relationship_filter filter,
        _In_ unsigned int startIndex,
        _In_ unsigned int maxItems
        );

private:
    void social_relationship_changed(_In_ social_relationship_change_event_args eventArgs);
    static string_t pathandquery_social_subpath(
        _In_ const string_t& ownerId,
        _In_ bool includeViewFilter,
        _In_ const string_t& view,
        _In_ uint64_t startIndex,
        _In_ uint64_t maxItems
        );

    static const string_t xbox_social_relationship_filter_to_string(
        _In_ xbox_social_relationship_filter xboxSocialRelationshipFilter
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    std::unordered_map<uint32_t, std::function<void(const social_relationship_change_event_args&)>> m_socialRelationshipChangeHandler;
    function_context m_socialRelationshipChangeHandlerCounter;
    xbox::services::system::xbox_live_mutex m_socialRelationshipChangeHandlerLock;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_realTimeActivityService;
};

}}}