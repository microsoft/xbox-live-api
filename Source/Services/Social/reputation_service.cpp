// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "social_internal.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

reputation_service::reputation_service(
    _In_ std::shared_ptr<reputation_service_impl> serviceImpl
    ) :
    m_serviceImpl(std::move(serviceImpl))
{
}

pplx::task<xbox_live_result<void>> 
reputation_service::submit_batch_reputation_feedback(
    _In_ const std::vector< reputation_feedback_item >& feedbackItems
    )
{
    task_completion_event<xbox_live_result<void>> tce;

    auto result = m_serviceImpl->submit_batch_reputation_feedback(
        utils::internal_vector_from_std_vector<reputation_feedback_item_internal, reputation_feedback_item>(feedbackItems),
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<void> result) 
        {
            tce.set(result); 
        });

    if (result.err())
    {
        return pplx::task_from_result(result);
    }
    return pplx::task<xbox_live_result<void>>(tce);
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
    task_completion_event<xbox_live_result<void>> tce;

    auto result = m_serviceImpl->submit_reputation_feedback(
        utils::internal_string_from_string_t(xboxUserId),
        reputationFeedbackType,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<void> result) 
        {
            tce.set(result); 
        },
        utils::internal_string_from_string_t(sessionName),
        utils::internal_string_from_string_t(reasonMessage),
        utils::internal_string_from_string_t(evidenceResourceId)
        );

    if (result.err())
    {
        return pplx::task_from_result(result);
    }
    return pplx::task<xbox_live_result<void>>(tce);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END