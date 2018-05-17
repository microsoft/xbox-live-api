// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "xsapi-c/social_c.h"
#if !XDK_API
#include "system_c.h"
#endif
#include "social_internal.h"
#include "xbox_live_context_internal_c.h"
#include "xbox_live_context_impl.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::social;

size_t calculate_social_relationship_result_size(
    std::shared_ptr<xbox_social_relationship_result_internal> internalResult
    )
{
    size_t requiredSize = sizeof(XblSocialRelationshipResult);

    for (const auto& relationship : internalResult->items())
    {
        requiredSize += sizeof(XblSocialRelationship);
        for (const auto& socialNetwork : relationship->social_networks())
        {
            requiredSize += sizeof(UTF8CSTR);
            requiredSize += (socialNetwork.length() + 1);
        }
    }
    return requiredSize;
}

void copy_social_relationship_result(
    _In_ std::shared_ptr<xbox_social_relationship_result_internal> internal,
    _In_ size_t bufferSize,
    _Out_ XblSocialRelationshipResult* buffer
    )
{
    buffer_allocator allocator(buffer, bufferSize);

    auto result = allocator.alloc<XblSocialRelationshipResult>();
    result->itemsCount = (uint32_t)internal->items().size();
    result->totalCount = internal->total_count();
    result->hasNext = internal->has_next();
    result->filter = static_cast<XblSocialRelationshipFilter>(internal->filter());
    result->continuationSkip = internal->continuation_skip();
    result->items = allocator.alloc_array<XblSocialRelationship>((uint32_t)internal->items().size());

    // populate the relationships
    uint32_t i = 0;
    for (const auto& relationship : internal->items())
    {
        result->items[i].isFavorite = relationship->is_favorite();
        result->items[i].isFollowingCaller = relationship->is_following_caller();
        result->items[i].socialNetworksCount = (uint32_t)relationship->social_networks().size();
        result->items[i].xboxUserId = utils::internal_string_to_uint64(relationship->xbox_user_id());
        result->items[i].socialNetworks = allocator.alloc_array<UTF8CSTR>(result->items[i].socialNetworksCount);

        for (uint32_t j = 0; j < result->items[i].socialNetworksCount; ++j)
        {
            auto& socialNetwork = relationship->social_networks()[j];
            auto length = socialNetwork.length() + 1;
            result->items[i].socialNetworks[j] = (UTF8CSTR)allocator.alloc(length);
            memcpy((void*)result->items[i].socialNetworks[j], socialNetwork.data(), length);
        }
        i++;
    }
}

STDAPI XblGetSocialRelationshipsHelper(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XblSocialRelationshipFilter socialRelationshipFilter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems
    )
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || async == nullptr);

    struct Context
    {
        xbl_context_handle xboxLiveContext;
        uint64_t xuid;
        XblSocialRelationshipFilter filter;
        uint32_t startIndex;
        uint32_t maxItems;
        std::shared_ptr<xbox_social_relationship_result_internal> result;
    };

    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context
    {
        xboxLiveContext,
        xboxUserId,
        socialRelationshipFilter,
        startIndex,
        maxItems
    };

    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
        case AsyncOp_DoWork:
            context->xboxLiveContext->contextImpl->social_service_impl()->get_social_relationships(
                utils::uint64_to_internal_string(context->xuid),
                static_cast<xbox_social_relationship_filter>(context->filter),
                context->startIndex,
                context->maxItems,
                data->async->queue,
                [data, context](xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>> result)
            {
                context->result = result.payload();
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                CompleteAsync(data->async, hr, calculate_social_relationship_result_size(context->result));
            });
            return E_PENDING;

        case AsyncOp_GetResult:
            copy_social_relationship_result(context->result, data->bufferSize, static_cast<XblSocialRelationshipResult*>(data->buffer));
            break;

        case AsyncOp_Cleanup:
            context->~Context();
            xsapi_memory::mem_free(context);
            break;
        }
        return S_OK;
    });

    if (FAILED(hr))
    {
        return hr;
    }
    return ScheduleAsync(async, 0);
}

STDAPI XblSocialGetSocialRelationships(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XblSocialRelationshipFilter socialRelationshipFilter
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        async,
        xboxLiveContext,
        xboxUserId,
        socialRelationshipFilter,
        0,
        0
        );
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultGetNext(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ CONST XblSocialRelationshipResult *socialRelationshipResult,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT
{
    return XblGetSocialRelationshipsHelper(
        async,
        xboxLiveContext,
        xboxLiveContext->xboxUserId,
        socialRelationshipResult->filter,
        socialRelationshipResult->continuationSkip,
        maxItems
        );
}

STDAPI XblSocialGetSocialRelationshipsResult(
    _In_ AsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_opt_(bufferSize, *bufferUsed) XblSocialRelationshipResult* buffer,
    _Out_opt_ size_t* bufferUsed
    ) XBL_NOEXCEPT
{
    return GetAsyncResult(async, nullptr, bufferSize, buffer, bufferUsed);
}

STDAPI XblSocialRelationshipResultGetNextResult(
    _In_ AsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_opt_(bufferSize, *bufferUsed) XblSocialRelationshipResult* buffer,
    _Out_opt_ size_t* bufferUsed
    ) XBL_NOEXCEPT
{
    return GetAsyncResult(async, nullptr, bufferSize, buffer, bufferUsed);
}

STDAPI XblSocialSubscribeToSocialRelationshipChange(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _Out_ xbl_social_relationship_change_subscription_handle* subscriptionHandle
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || subscriptionHandle == nullptr);

    auto result = xboxLiveContext->contextImpl->social_service_impl()->subscribe_to_social_relationship_change(utils::uint64_to_internal_string(xboxUserId));
    if (!result.err())
    {
        *subscriptionHandle = static_cast<void*>(result.payload().get());
    }
    else
    {
        *subscriptionHandle = nullptr;
    }
    return utils::convert_xbox_live_error_code_to_hresult(result.err());
}
CATCH_RETURN()

STDAPI XblSocialUnsubscribeFromSocialRelationshipChange(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ xbl_social_relationship_change_subscription_handle subscriptionHandle
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF_NULL(xboxLiveContext);

    return utils::convert_xbox_live_error_code_to_hresult(
        xboxLiveContext->contextImpl->social_service_impl()->unsubscribe_from_social_relationship_change(
            std::shared_ptr<social_relationship_change_subscription_internal>(static_cast<social_relationship_change_subscription_internal*>(subscriptionHandle))
            ).err()
        );
}
CATCH_RETURN()

STDAPI_(function_context)
XblSocialAddSocialRelationshipChangedHandler(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblSocialRelationshipChangedHandler handler,
    _In_ void* handlerContext
    ) XBL_NOEXCEPT
try
{
    return xboxLiveContext->contextImpl->social_service_impl()->add_social_relationship_changed_handler(
        [handler, handlerContext](std::shared_ptr<social_relationship_change_event_args_internal> eventArgs)
    {
        const auto& xuids = eventArgs->xbox_user_ids();

        XblSocialRelationshipChangeEventArgs* args = static_cast<XblSocialRelationshipChangeEventArgs*>(xsapi_memory::mem_alloc(
            FIELD_OFFSET(XblSocialRelationshipChangeEventArgs, xboxUserIds[xuids.size()])));
        if (args == nullptr) return;

        args->callerXboxUserId = atoi(eventArgs->caller_xbox_user_id().data());
        args->socialNotification = static_cast<XblSocialNotificationType>(eventArgs->social_notification());
        args->xboxUserIdsCount = (uint32_t)xuids.size();

        for (uint32_t i = 0; i < args->xboxUserIdsCount; ++i)
        {
            args->xboxUserIds[i] = atoi(xuids[i].data());
        }
        handler(args, handlerContext);
        xsapi_memory::mem_free(args);
    });
}
CATCH_RETURN_WITH(-1)

STDAPI_(void) XblSocialRemoveSocialRelationshipChangedHandler(
    _In_ xbl_context_handle xboxLiveContext,
    _In_ function_context handlerFunctionContext
    ) XBL_NOEXCEPT
try
{
    xboxLiveContext->contextImpl->social_service_impl()->remove_social_relationship_changed_handler(handlerFunctionContext);
}
CATCH_RETURN_WITH(;)

STDAPI XblSocialSubmitReputationFeedback(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XblReputationFeedbackType reputationFeedbackType,
    _In_opt_ UTF8CSTR sessionName,
    _In_opt_ UTF8CSTR reasonMessage,
    _In_opt_ UTF8CSTR evidenceResourceId
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || async == nullptr);

    struct Context
    {
        xbl_context_handle xboxLiveContext;
        uint64_t xuid;
        XblReputationFeedbackType reputationFeedbackType;
        xsapi_internal_string sessionName;
        xsapi_internal_string reasonMessage;
        xsapi_internal_string evidenceResourceId;
    };

    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context
    {
        xboxLiveContext,
        xboxUserId,
        reputationFeedbackType,
        sessionName == nullptr ? "" : sessionName,
        reasonMessage == nullptr ? "" : reasonMessage,
        evidenceResourceId == nullptr ? "" : evidenceResourceId
    };
    
    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);

        switch (op)
        {
        case AsyncOp_DoWork:
            context->xboxLiveContext->contextImpl->reputation_service_impl()->submit_reputation_feedback(
                utils::uint64_to_internal_string(context->xuid),
                static_cast<reputation_feedback_type>(context->reputationFeedbackType),
                data->async->queue,
                [data](xbox_live_result<void> result)
                {
                    CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(result.err()), 0);
                },
                context->sessionName,
                context->reasonMessage,
                context->evidenceResourceId
                );
            return E_PENDING;

        case AsyncOp_Cleanup:
            context->~Context();
            xsapi_memory::mem_free(context);
            break;
        }
        return S_OK;
    });

    if (FAILED(hr))
    {
        return hr;
    }
    return ScheduleAsync(async, 0);
}
CATCH_RETURN()

STDAPI XblSocialSubmitBatchReputationFeedback(
    _In_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ XblReputationFeedbackItem* feedbackItems,
    _In_ uint32_t feedbackItemsCount
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || feedbackItems == nullptr || async == nullptr);
    
    struct Context
    {
        xbl_context_handle xboxLiveContext;
        xsapi_internal_vector<reputation_feedback_item_internal> feedbackItems;
    };

    auto context = new (xsapi_memory::mem_alloc(sizeof(Context))) Context { xboxLiveContext };

    for (uint32_t i = 0; i < feedbackItemsCount; ++i)
    {
        context->feedbackItems.push_back(reputation_feedback_item_internal(
            utils::uint64_to_internal_string(feedbackItems[i].xboxUserId),
            static_cast<reputation_feedback_type>(feedbackItems[i].feedbackType),
            xbox::services::multiplayer::multiplayer_session_reference(), // TODO
            feedbackItems[i].reasonMessage == nullptr ? "" : feedbackItems[i].reasonMessage,
            feedbackItems[i].evidenceResourceId == nullptr ? "" : feedbackItems[i].evidenceResourceId
            ));
    }

    auto hr = BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        auto context = static_cast<Context*>(data->context);
        switch (op) 
        {
        case AsyncOp_DoWork:
            context->xboxLiveContext->contextImpl->reputation_service_impl()->submit_batch_reputation_feedback(
                context->feedbackItems,
                data->async->queue,
                [data](xbox_live_result<void> result)
                {
                    CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(result.err()), 0);
                });
            return E_PENDING;

        case AsyncOp_Cleanup:
            context->~Context();
            xsapi_memory::mem_free(context);
            return S_OK;
        default: 
            return S_OK;
        }
    });

    if (FAILED(hr))
    {
        return hr;
    }
    return ScheduleAsync(async, 0);
}
CATCH_RETURN()
