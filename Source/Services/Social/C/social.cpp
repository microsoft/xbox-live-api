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

struct xbl_social_relationship_result
{
    xbl_social_relationship_result(std::shared_ptr<xbox_social_relationship_result_internal> _internalResult)
        : internalResult(std::move(_internalResult)),
        refCount(1)
    {
        auto& internalItems = internalResult->items();
        items = xsapi_internal_vector<XblSocialRelationship>(internalItems.size());

        for (uint32_t i = 0; i < items.size(); ++i)
        {
            items[i].xboxUserId = utils::internal_string_to_uint64(internalItems[i]->xbox_user_id());
            items[i].isFavorite = internalItems[i]->is_favorite();
            items[i].isFollowingCaller = internalItems[i]->is_following_caller();
            items[i].socialNetworksCount = static_cast<uint32_t>(internalItems[i]->social_networks().size());

            items[i].socialNetworks = (const char**)xsapi_memory::mem_alloc(sizeof(const char*) * items[i].socialNetworksCount);
            for (uint32_t j = 0; j < items[i].socialNetworksCount; ++j)
            {
                items[i].socialNetworks[j] = internalItems[i]->social_networks()[j].data();
            }
        }
    }

    virtual ~xbl_social_relationship_result()
    {
        for (auto& item : items)
        {
            xsapi_memory::mem_free(item.socialNetworks);
        }
    }

    xsapi_internal_vector<XblSocialRelationship> items;
    std::shared_ptr<xbox_social_relationship_result_internal> internalResult;
    std::atomic<int> refCount;
};

STDAPI XblGetSocialRelationshipsHelper(
    _Inout_ AsyncBlock* async,
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
        xbl_social_relationship_result_handle resultHandle;
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
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                if (SUCCEEDED(hr))
                {
                    context->resultHandle = new (xsapi_memory::mem_alloc(sizeof(xbl_social_relationship_result))) xbl_social_relationship_result(result.payload());
                }
                CompleteAsync(data->async, hr, sizeof(xbl_social_relationship_result_handle));
            });
            return E_PENDING;

        case AsyncOp_GetResult:
            memcpy(data->buffer, &context->resultHandle, sizeof(xbl_social_relationship_result_handle));
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

STDAPI XblSocialGetSocialRelationshipsAsync(
    _Inout_ AsyncBlock* async,
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

STDAPI XblSocialRelationshipResultGetRelationships(
    _In_ xbl_social_relationship_result_handle resultHandle,
    _Out_ XblSocialRelationship** relationships,
    _Out_ uint32_t* relationshipsCount
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(resultHandle == nullptr || relationships == nullptr || relationshipsCount == nullptr);
    verify_global_init();

    *relationshipsCount = static_cast<uint32_t>(resultHandle->items.size());
    *relationships = resultHandle->items.data();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultHasNext(
    _In_ xbl_social_relationship_result_handle resultHandle,
    _Out_ bool* hasNext
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(resultHandle == nullptr || hasNext == nullptr);
    verify_global_init();
    *hasNext = resultHandle->internalResult->has_next();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultGetNextAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ xbl_social_relationship_result_handle resultHandle,
    _In_ uint32_t maxItems
    ) XBL_NOEXCEPT
try
{
    return XblGetSocialRelationshipsHelper(
        async,
        xboxLiveContext,
        xboxLiveContext->xboxUserId,
        static_cast<XblSocialRelationshipFilter>(resultHandle->internalResult->filter()),
        resultHandle->internalResult->continuation_skip(),
        maxItems
        );
}
CATCH_RETURN()

STDAPI XblSocialGetSocialRelationshipsResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_social_relationship_result_handle* handle
    ) XBL_NOEXCEPT
try
{
    return GetAsyncResult(async, nullptr, sizeof(xbl_social_relationship_result_handle), handle, nullptr);
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultGetNextResult(
    _Inout_ AsyncBlock* async,
    _Out_ xbl_social_relationship_result_handle* handle
    ) XBL_NOEXCEPT
try
{
    return GetAsyncResult(async, nullptr, sizeof(xbl_social_relationship_result_handle), handle, nullptr);
}
CATCH_RETURN()

STDAPI_(xbl_social_relationship_result_handle) XblSocialRelationshipResultDuplicateHandle(
    _In_ xbl_social_relationship_result_handle handle
    ) XBL_NOEXCEPT
try
{
    if (handle == nullptr)
    {
        return nullptr;
    }

    handle->refCount++;
    return handle;
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(void) XblSocialRelationshipResultCloseHandle(
    _In_ xbl_social_relationship_result_handle handle
    ) XBL_NOEXCEPT
try
{
    int refCount = --handle->refCount;
    if (refCount <= 0)
    {
        assert(refCount == 0);
        handle->~xbl_social_relationship_result();
        xsapi_memory::mem_free(handle);
    }
}
CATCH_RETURN_WITH(;)

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

STDAPI XblSocialSubmitReputationFeedbackAsync(
    _Inout_ AsyncBlock* async,
    _In_ xbl_context_handle xboxLiveContext,
    _In_ uint64_t xboxUserId,
    _In_ XblReputationFeedbackType reputationFeedbackType,
    _In_opt_z_ const char* sessionName,
    _In_opt_z_ const char* reasonMessage,
    _In_opt_z_ const char* evidenceResourceId
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
    _Inout_ AsyncBlock* async,
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
