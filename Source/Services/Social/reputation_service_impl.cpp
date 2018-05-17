// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "social_internal.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

reputation_service_impl::reputation_service_impl(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

xbox_live_result<void> reputation_service_impl::submit_reputation_feedback(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ reputation_feedback_type reputationFeedbackType,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<void>> callback,
    _In_ const xsapi_internal_string& sessionName,
    _In_ const xsapi_internal_string& reasonMessage,
    _In_ const xsapi_internal_string& evidenceResourceId
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "Xbox user id is empty");
    RETURN_CPP_INVALIDARGUMENT_IF(
        reputationFeedbackType < reputation_feedback_type::fair_play_kills_teammates ||
        reputationFeedbackType > reputation_feedback_type::fair_play_leaderboard_cheater,
        void,
        "Reputation feedback type is out of range"
        );

    xsapi_internal_string pathAndQuery = reputation_feedback_subpath(xboxUserId);
    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "POST",
        utils::create_xboxlive_endpoint("reputation", m_appConfig),
        web::uri(utils::string_t_from_internal_string(pathAndQuery)),
        xbox_live_api::submit_reputation_feedback
        );

    httpCall->set_retry_allowed(false);
    httpCall->set_xbox_contract_version_header_value(_T("100"));

    reputation_feedback_request reputationFeedbackRequest(
        reputationFeedbackType,
        sessionName,
        reasonMessage,
        evidenceResourceId
        );

    web::json::value request = reputationFeedbackRequest.serialize_feedback_request();
    httpCall->set_request_body(utils::internal_string_from_string_t(request.serialize()));

    httpCall->get_response_with_auth(
        m_userContext, 
        http_call_response_body_type::string_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            callback(xbox_live_result<void>(response->err_code(), std::string(response->err_message().data())));
        }
    );

    return xbox_live_result<void>();
}

xbox_live_result<void> reputation_service_impl::submit_batch_reputation_feedback(
    _In_ const xsapi_internal_vector<reputation_feedback_item_internal>& feedbackItems,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<void>> callback
    )
{
    for (auto& feedbackItem : feedbackItems)
    {
        RETURN_CPP_INVALIDARGUMENT_IF(feedbackItem.xbox_user_id().empty(), void, "Xbox user id is empty");
        RETURN_CPP_INVALIDARGUMENT_IF(
            feedbackItem.feedback_type() < reputation_feedback_type::fair_play_kills_teammates ||
            feedbackItem.feedback_type() > reputation_feedback_type::fair_play_leaderboard_cheater,
            void,
            "Reputation feedback type is out of range"
        );
    }

    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "POST",
        utils::create_xboxlive_endpoint("reputation", m_appConfig),
        _T("/users/batchtitlefeedback"),
        xbox_live_api::submit_batch_reputation_feedback
        );

    httpCall->set_retry_allowed(false);
    httpCall->set_xbox_contract_version_header_value(_T("101"));

    std::error_code err;
    web::json::value request = reputation_feedback_request::serialize_batch_feedback_request(feedbackItems, err);
    RETURN_CPP_INVALIDARGUMENT_IF(err, void, "Invalid reputation_feedback_item");
    httpCall->set_request_body(utils::internal_string_from_string_t(request.serialize()));

    httpCall->get_response_with_auth(
        m_userContext, 
        http_call_response_body_type::string_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            callback(xbox_live_result<void>(response->err_code(), std::string(response->err_message().data())));
        }
    );

    return xbox_live_result<void>();
}


xsapi_internal_string reputation_service_impl::reputation_feedback_subpath(
    _In_ const xsapi_internal_string& xboxUserId
    )
{
    xsapi_internal_stringstream source;
    source << "/users/xuid(";
    source << xboxUserId;
    source << ")/feedback";

    return source.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END