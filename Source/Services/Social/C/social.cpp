// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "xsapi-c/social_c.h"
#include "xsapi-c/system_c.h"
#include "social_internal.h"
#include "social_helpers.h"
#include "xbox_live_context_internal_c.h"
#include "xbox_live_context_impl.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsHelper(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER socialRelationshipFilter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    )
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr);

    auto result = xboxLiveContext->contextImpl->social_service_impl()->get_social_relationships(
        xboxUserId, static_cast<xbox_social_relationship_filter>(socialRelationshipFilter), startIndex, maxItems, taskGroupId,
        [callback, callbackContext](xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>> result)
    {
        XBL_RESULT cResult = utils::create_xbl_result(result.err());
        if (!result.err())
        {
            xbl_xbox_social_relationship_result_wrapper wrapper(result.payload());
            callback(cResult, wrapper.xbl_xbox_social_relationship_result(), callbackContext);
        }
        else
        {
            callback(cResult, nullptr, callbackContext);
        }
    });

    return utils::create_xbl_result(result.err());
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationships(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        xboxLiveContext,
        xboxLiveContext->xboxUserId.data(),
        XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER_ALL,
        0,
        0,
        taskGroupId,
        callbackContext,
        callback
        );
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsWithFilter(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER socialRelationshipFilter,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        xboxLiveContext,
        xboxLiveContext->user->xboxUserId,
        socialRelationshipFilter,
        0,
        0,
        taskGroupId,
        callbackContext,
        callback
        );
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsForUser(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        xboxLiveContext,
        xboxUserId,
        XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER_ALL,
        0,
        0,
        taskGroupId,
        callbackContext,
        callback
        );
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGetSocialRelationshipsEx(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER socialRelationshipFilter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        xboxLiveContext,
        xboxLiveContext->user->xboxUserId,
        socialRelationshipFilter,
        startIndex,
        maxItems,
        taskGroupId,
        callbackContext,
        callback
        );
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSocialRelationshipResultGetNext(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ CONST XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT *socialRelationshipResult,
    _In_ uint32_t maxItems,
    _In_ uint64_t taskGroupId,
    _In_opt_ void* callbackContext,
    _In_ XBL_GET_SOCIAL_RELATIONSHIPS_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
{
    return XblGetSocialRelationshipsHelper(
        xboxLiveContext,
        xboxLiveContext->user->xboxUserId,
        socialRelationshipResult->filter,
        socialRelationshipResult->continuationSkip,
        maxItems,
        taskGroupId,
        callbackContext,
        callback
        );
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubscribeToSocialRelationshipChange(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _Out_ XBL_SOCIAL_RELATIONSHIP_CHANGE_SUBSCRIPTION *subscriptionHandle
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || xboxUserId == nullptr || subscriptionHandle == nullptr);

    auto result = xboxLiveContext->contextImpl->social_service_impl()->subscribe_to_social_relationship_change(xboxUserId);
    if (!result.err())
    {
        *subscriptionHandle = static_cast<void*>(result.payload().get());
    }
    return utils::create_xbl_result(result.err());
}
CATCH_RETURN()

XBL_API XBL_RESULT XBL_CALLING_CONV
XblUnsubscribeFromSocialRelationshipChange(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ XBL_SOCIAL_RELATIONSHIP_CHANGE_SUBSCRIPTION subscriptionHandle
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr);

    return utils::create_xbl_result(
        xboxLiveContext->contextImpl->social_service_impl()->unsubscribe_from_social_relationship_change(
            std::shared_ptr<social_relationship_change_subscription_internal>(static_cast<social_relationship_change_subscription_internal*>(subscriptionHandle))
            ).err()
        );
}
CATCH_RETURN()

XBL_API FUNCTION_CONTEXT XBL_CALLING_CONV
XblAddSocialRelationshipChangedHandler(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ XBL_SOCIAL_RELATIONSHIP_CHANGED_HANDLER handler,
    _In_ void *handlerContext
    ) XBL_NOEXCEPT
try
{
    return xboxLiveContext->contextImpl->social_service_impl()->add_social_relationship_changed_handler(
        [handler, handlerContext](std::shared_ptr<social_relationship_change_event_args_internal> eventArgs)
    {
        auto wrapper = xbl_social_relationship_change_event_args_wrapper(eventArgs);
        handler(wrapper.xbl_social_relationship_change_event_args(), handlerContext);
    });
}
CATCH_RETURN_WITH(-1)

XBL_API void XBL_CALLING_CONV
XblRemoveSocialRelationshipChangedHandler(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ FUNCTION_CONTEXT handlerFunctionContext
    ) XBL_NOEXCEPT
try
{
    xboxLiveContext->contextImpl->social_service_impl()->remove_social_relationship_changed_handler(handlerFunctionContext);
}
CATCH_RETURN_WITH(;)

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubmitReputationFeedback(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ PCSTR xboxUserId,
    _In_ XBL_REPUTATION_FEEDBACK_TYPE reputationFeedbackType,
    _In_opt_ PCSTR sessionName,
    _In_opt_ PCSTR reasonMessage,
    _In_opt_ PCSTR evidenceResourceId,
    _In_ uint64_t taskGroupId,
    _In_opt_ void *callbackContext,
    _In_ XBL_SUBMIT_REPUTATION_FEEDBACK_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || xboxUserId == nullptr);

    xboxLiveContext->contextImpl->reputation_service_impl()->submit_reputation_feedback(
        xboxUserId,
        static_cast<reputation_feedback_type>(reputationFeedbackType),
        taskGroupId,
        [callback, callbackContext](xbox_live_result<void> result)
        {
            callback(utils::create_xbl_result(result.err()), callbackContext);
        },
        sessionName == nullptr ? "" : sessionName,
        reasonMessage == nullptr ? "" : reasonMessage,
        evidenceResourceId == nullptr ? "" : evidenceResourceId
        );

    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblSubmitBatchReputationFeedback(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContext,
    _In_ XBL_REPUTATION_FEEDBACK_ITEM *feedbackItems,
    _In_ uint32_t feedbackItemsCount,
    _In_ uint64_t taskGroupId,
    _In_opt_ void *callbackContext,
    _In_ XBL_SUBMIT_REPUTATION_FEEDBACK_COMPLETION_ROUTINE callback
    ) XBL_NOEXCEPT
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || feedbackItems == nullptr);

    xsapi_internal_vector<reputation_feedback_item_internal> internalItems(feedbackItemsCount);
    for (uint32_t i = 0; i < feedbackItemsCount; ++i)
    {
        internalItems[i] = reputation_feedback_item_internal(
            feedbackItems[i].xboxUserId,
            static_cast<reputation_feedback_type>(feedbackItems[i].feedbackType),
            xbox::services::multiplayer::multiplayer_session_reference(), // TODO
            feedbackItems[i].reasonMessage == nullptr ? "" : feedbackItems[i].reasonMessage,
            feedbackItems[i].evidenceResourceId == nullptr ? "" : feedbackItems[i].evidenceResourceId
            );
    }

    xboxLiveContext->contextImpl->reputation_service_impl()->submit_batch_reputation_feedback(
        internalItems,
        taskGroupId,
        [callback, callbackContext](xbox_live_result<void> result)
    {
        callback(utils::create_xbl_result(result.err()), callbackContext);
    });

    return XBL_RESULT_OK;
}
