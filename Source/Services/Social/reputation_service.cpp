// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "user_context.h"
#include "utils.h"
#include "xbox_system_factory.h"
#include "social_internal.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

reputation_service::reputation_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<void>> 
reputation_service::submit_batch_reputation_feedback(
    _In_ const std::vector< reputation_feedback_item >& feedbackItems
    )
{
    for (auto& feedbackItem : feedbackItems)
    {
        RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(feedbackItem.xbox_user_id(), void, "Xbox user id is empty");
        RETURN_TASK_CPP_INVALIDARGUMENT_IF(
            feedbackItem.feedback_type() < reputation_feedback_type::fair_play_kills_teammates ||
            feedbackItem.feedback_type() > reputation_feedback_type::fair_play_leaderboard_cheater,
            void,
            "Reputation feedback type is out of range"
            );
    }
    
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("reputation"), m_appConfig),
        _T("/users/batchtitlefeedback"),
        xbox_live_api::submit_batch_reputation_feedback
        );

    httpCall->set_retry_allowed(false);
    httpCall->set_xbox_contract_version_header_value(_T("101"));

    std::error_code err;
    web::json::value request = reputation_feedback_request::serialize_batch_feedback_request(utils::internal_vector_from_std_vector(feedbackItems), err);
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(err, void, "Invalid reputation_feedback_item");
    httpCall->set_request_body(request.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext, http_call_response_body_type::string_body)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<void>(
        task
        );
}


pplx::task<xbox_live_result<void>> 
reputation_service::submit_reputation_feedback(
    _In_ const string_t& xboxUserId,
    _In_ reputation_feedback_type reputationFeedbackType,
    _In_ const string_t& sessionName,
    _In_ const string_t& reasonMessage,
    _In_ const string_t& evidenceResourceId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(xboxUserId, void, "Xbox user id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(
        reputationFeedbackType < reputation_feedback_type::fair_play_kills_teammates ||
        reputationFeedbackType > reputation_feedback_type::fair_play_leaderboard_cheater,
        void,
        "Reputation feedback type is out of range"
        );
    
    string_t pathAndQuery = reputation_feedback_subpath(xboxUserId);
    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("reputation"), m_appConfig),
        pathAndQuery,
        xbox_live_api::submit_reputation_feedback
        );

    httpCall->set_retry_allowed(false);
    httpCall->set_xbox_contract_version_header_value(_T("100"));

    reputation_feedback_request reputationFeedbackRequest(
        reputationFeedbackType,
        utils::internal_string_from_external_string(sessionName),
        utils::internal_string_from_external_string(reasonMessage),
        utils::internal_string_from_external_string(evidenceResourceId)
        );

    web::json::value request = reputationFeedbackRequest.serialize_feedback_request();
    httpCall->set_request_body(request.serialize());

    auto task = httpCall->get_response_with_auth(m_userContext, http_call_response_body_type::string_body)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

string_t
reputation_service::reputation_feedback_subpath(
    _In_ const string_t& xboxUserId
)
{
    stringstream_t source;
    source << _T("/users/xuid(");
    source << xboxUserId;
    source << _T(")/feedback");

    return source.str();
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END