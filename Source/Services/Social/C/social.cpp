// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "xsapi-c/social_c.h"
#include "xsapi-c/system_c.h"
#include "social_internal.h"
#include "xbox_live_context_impl_c.h"
#include "social_helpers.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsHelper(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_ XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER socialRelationshipFilter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems,
    _In_ uint64_t taskGroupId,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext
    )
{
    RETURN_C_INVALIDARGUMENT_IF(pContext == nullptr);

    auto result = pContext->pImpl->cppObject()->social_service_impl()->get_social_relationships(
        xboxUserId, static_cast<xbox_social_relationship_filter>(socialRelationshipFilter), startIndex, maxItems, taskGroupId,
        [completionRoutine, completionRoutineContext](xbox_live_result<xbox_social_relationship_result> result)
    {
        XBL_RESULT cResult = utils::create_xbl_result(result.err());
        if (!result.err())
        {
            xbl_xbox_social_relationship_result_wrapper wrapper(result.payload());
            completionRoutine(cResult, wrapper.xbl_xbox_social_relationship_result(), completionRoutineContext);
        }
        else
        {
            completionRoutine(cResult, nullptr, completionRoutineContext);
        }
    });

    return utils::create_xbl_result(result.err());
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationships(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ uint64_t taskGroupId,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        pContext, 
        pContext->pUser->xboxUserId, 
        XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER_ALL,
        0,
        0,
        taskGroupId,
        completionRoutine,
        completionRoutineContext
        );
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsWithFilter(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER socialRelationshipFilter,
    _In_ uint64_t taskGroupId,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        pContext,
        pContext->pUser->xboxUserId,
        socialRelationshipFilter,
        0,
        0,
        taskGroupId,
        completionRoutine,
        completionRoutineContext
        );
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsForUser(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_ uint64_t taskGroupId,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        pContext,
        xboxUserId,
        XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER_ALL,
        0,
        0,
        taskGroupId,
        completionRoutine,
        completionRoutineContext
        );
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsEx(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER socialRelationshipFilter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems,
    _In_ uint64_t taskGroupId,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        pContext,
        pContext->pUser->xboxUserId,
        socialRelationshipFilter,
        startIndex,
        maxItems,
        taskGroupId,
        completionRoutine,
        completionRoutineContext
        );
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubscribeToSocialRelationshipChange(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _Out_ XBL_SOCIAL_RELATIONSHIP_CHANGE_SUBSCRIPTION *subscriptionHandle
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(pContext == nullptr || xboxUserId == nullptr || subscriptionHandle == nullptr);

    auto result = pContext->pImpl->cppObject()->social_service_impl()->subscribe_to_social_relationship_change(xboxUserId);
    if (!result.err())
    {
        *subscriptionHandle = static_cast<void*>(result.payload().get());
    }
    return utils::create_xbl_result(result.err());
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblUnsubscribeFromSocialRelationshipChange(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ XBL_SOCIAL_RELATIONSHIP_CHANGE_SUBSCRIPTION subscriptionHandle
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(pContext == nullptr);

    return utils::create_xbl_result(
        pContext->pImpl->cppObject()->social_service_impl()->unsubscribe_from_social_relationship_change(
            std::shared_ptr<social_relationship_change_subscription>(static_cast<social_relationship_change_subscription*>(subscriptionHandle))
            ).err()
        );
}
CATCH_RETURN()

XBL_API FUNCTION_CONTEXT XBL_CALLING_CONV
XblAddSocialRelationshipChangedHandler(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ XBL_SOCIAL_RELATIONSHIP_CHANGED_HANDLER handler,
    _In_ void *handlerContext
    ) XBL_NOEXCEPT
try
{
    return pContext->pImpl->cppObject()->social_service_impl()->add_social_relationship_changed_handler(
        [handler, handlerContext](social_relationship_change_event_args eventArgs)
    {
        auto wrapper = xbl_social_relationship_change_event_args_wrapper(eventArgs);
        handler(wrapper.xbl_social_relationship_change_event_args(), handlerContext);
    });
}
CATCH_RETURN_WITH(-1)

XBL_API void XBL_CALLING_CONV
XblRemoveSocialRelationshipChangedHandler(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ FUNCTION_CONTEXT handlerContext
    ) XBL_NOEXCEPT
try
{
    pContext->pImpl->cppObject()->social_service_impl()->remove_social_relationship_changed_handler(handlerContext);
}
CATCH_RETURN_WITH(;)

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubmitReputationFeedback(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR xboxUserId,
    _In_ XBL_REPUTATION_FEEDBACK_TYPE reputationFeedbackType,
    _In_opt_ PCSTR sessionName,
    _In_opt_ PCSTR reasonMessage,
    _In_opt_ PCSTR evidenceResourceId,
    _In_ uint64_t taskGroupId,
    _In_ XBL_SUBMIT_REPUTATION_FEEDBACK_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void *completionRoutineContext
    ) XBL_NOEXCEPT
{
    RETURN_C_INVALIDARGUMENT_IF(pContext == nullptr || xboxUserId == nullptr);

    pContext->pImpl->cppObject()->reputation_service_impl()->submit_reputation_feedback(
        xboxUserId,
        static_cast<reputation_feedback_type>(reputationFeedbackType),
        taskGroupId,
        [completionRoutine, completionRoutineContext](xbox_live_result<void> result)
        {
            completionRoutine(utils::create_xbl_result(result.err()), completionRoutineContext);
        },
        sessionName == nullptr ? "" : sessionName,
        reasonMessage == nullptr ? "" : reasonMessage,
        evidenceResourceId == nullptr ? "" : evidenceResourceId
        );

    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubmitBatchReputationFeedback(
    _In_ XBL_XBOX_LIVE_CONTEXT* pContext,
    _In_ XBL_REPUTATION_FEEDBACK_ITEM *feedbackItems,
    _In_ uint32_t feedbackItemsCount,
    _In_ uint64_t taskGroupId,
    _In_ XBL_SUBMIT_REPUTATION_FEEDBACK_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void *completionRoutineContext
    ) XBL_NOEXCEPT
{
    RETURN_C_INVALIDARGUMENT_IF(pContext == nullptr || feedbackItems == nullptr);

    xsapi_internal_vector<reputation_feedback_item> internalItems(feedbackItemsCount);
    for (uint32_t i = 0; i < feedbackItemsCount; ++i)
    {
        internalItems[i] = reputation_feedback_item(
            feedbackItems[i].xboxUserId,
            static_cast<reputation_feedback_type>(feedbackItems[i].feedbackType),
            xbox::services::multiplayer::multiplayer_session_reference(), // TODO
            feedbackItems[i].reasonMessage == nullptr ? "" : feedbackItems[i].reasonMessage,
            feedbackItems[i].evidenceResourceId == nullptr ? "" : feedbackItems[i].evidenceResourceId
            );
    }

    pContext->pImpl->cppObject()->reputation_service_impl()->submit_batch_reputation_feedback(
        internalItems,
        taskGroupId,
        [completionRoutine, completionRoutineContext](xbox_live_result<void> result)
    {
        completionRoutine(utils::create_xbl_result(result.err()), completionRoutineContext);
    });

    return XBL_RESULT_OK;
}
