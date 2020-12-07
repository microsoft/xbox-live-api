// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_internal.h"
#include "xbox_live_context_internal.h"
#include "real_time_activity_subscription.h"

using namespace xbox::services;
using namespace xbox::services::social;

STDAPI XblSocialGetSocialRelationshipsAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ uint64_t xuid,
    _In_ XblSocialRelationshipFilter filter,
    _In_ size_t startIndex,
    _In_ size_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    VERIFY_XBL_INITIALIZED();
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContextHandle == nullptr || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext{ xboxLiveContextHandle->shared_from_this() },
            xuid,
            filter,
            startIndex,
            maxItems,
            socialRelationshipResult = std::shared_ptr<XblSocialRelationshipResult>{ nullptr }
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xboxLiveContext->SocialService()->GetSocialRelationships(xuid, filter, startIndex, maxItems, {
                data->async->queue,
                [
                    &socialRelationshipResult,
                    asyncBlock{ data->async }
                ]
            (Result<std::shared_ptr<XblSocialRelationshipResult>> result)
            {
                if (Succeeded(result))
                {
                    socialRelationshipResult = result.ExtractPayload();
                }
                XAsyncComplete(asyncBlock, result.Hresult(), sizeof(XblSocialRelationshipResultHandle));
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto resultHandle = static_cast<XblSocialRelationshipResultHandle*>(data->buffer);
            socialRelationshipResult->AddRef();
            *resultHandle = socialRelationshipResult.get();
            return S_OK;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}
CATCH_RETURN()

STDAPI XblSocialGetSocialRelationshipsResult(
    _In_ XAsyncBlock* async,
    _Out_ XblSocialRelationshipResultHandle* handle
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblSocialRelationshipResultHandle), handle, nullptr);
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultGetRelationships(
    _In_ XblSocialRelationshipResultHandle resultHandle,
    _Out_ const XblSocialRelationship** relationships,
    _Out_ size_t* relationshipsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(resultHandle == nullptr || relationships == nullptr || relationshipsCount == nullptr);

    *relationships = resultHandle->SocialRelationships().data();
    *relationshipsCount = resultHandle->SocialRelationships().size();

    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultHasNext(
    _In_ XblSocialRelationshipResultHandle resultHandle,
    _Out_ bool* hasNext
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(resultHandle == nullptr || hasNext == nullptr);
    *hasNext = resultHandle->HasNext();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultGetTotalCount(
    _In_ XblSocialRelationshipResultHandle resultHandle,
    _Out_ size_t* totalCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(resultHandle == nullptr || totalCount == nullptr);
    *totalCount = resultHandle->TotalCount();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultGetNextAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblSocialRelationshipResultHandle resultHandle,
    _In_ size_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || resultHandle == nullptr || async == nullptr);
    return XblSocialGetSocialRelationshipsAsync(xboxLiveContext, xboxLiveContext->Xuid(), resultHandle->Filter, resultHandle->ContinuationSkip, maxItems, async);
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultGetNextResult(
    _In_ XAsyncBlock* async,
    _Out_ XblSocialRelationshipResultHandle* handle
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblSocialRelationshipResultHandle), handle, nullptr);
}
CATCH_RETURN()

STDAPI XblSocialRelationshipResultDuplicateHandle(
    _In_ XblSocialRelationshipResultHandle handle,
    _Out_ XblSocialRelationshipResultHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || duplicatedHandle == nullptr);
    handle->AddRef();
    *duplicatedHandle = handle;
    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblSocialRelationshipResultCloseHandle(
    _In_ XblSocialRelationshipResultHandle handle
) XBL_NOEXCEPT
try
{
    if (handle)
    {
        handle->DecRef();
    }
}
CATCH_RETURN_WITH(;)

STDAPI XblSocialSubscribeToSocialRelationshipChange(
    _In_ XblContextHandle xboxLiveContext,
    _In_ uint64_t xuid,
    _Out_ XblRealTimeActivitySubscriptionHandle* subscriptionHandle
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(xboxLiveContext);
    UNREFERENCED_PARAMETER(xuid);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(subscriptionHandle);
    *subscriptionHandle = Make<XblRealTimeActivitySubscription>();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSocialUnsubscribeFromSocialRelationshipChange(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(xboxLiveContext);
    RETURN_HR_INVALIDARGUMENT_IF_NULL(subscriptionHandle);
    Delete(subscriptionHandle);
    return S_OK;
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblSocialAddSocialRelationshipChangedHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblSocialRelationshipChangedHandler handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    // TODO really should be returning HRESULT E_INVALIDARG here
    if (xboxLiveContext == nullptr || handler == nullptr)
    {
        return XblFunctionContext{ 0 };
    }

    return xboxLiveContext->SocialService()->AddSocialRelationshipChangedHandler(
        [
            handler,
            context
        ]
    (const XblSocialRelationshipChangeEventArgs& args)
    {
        try
        {
            handler(&args, context);
        }
        catch (...)
        {
            LOGS_ERROR << __FUNCTION__ << ": exception in client handler!";
        }
    });

}
CATCH_RETURN()

STDAPI XblSocialRemoveSocialRelationshipChangedHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext handlerFunctionContext
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(xboxLiveContext);
    xboxLiveContext->SocialService()->RemoveSocialRelationshipChangedHandler(handlerFunctionContext);
    return S_OK;
}
CATCH_RETURN()

HRESULT SubmitReputationFeedback(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ ReputationFeedbackRequest r,
    _In_ XAsyncBlock* async
)
{
    assert(async);
    return RunAsync(async, __FUNCTION__,
        [
            xboxLiveContext{ xboxLiveContextHandle->shared_from_this() },
            request{ std::move(r) }
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xboxLiveContext->ReputationService()->SubmitFeedback(request, data->async));
            return E_PENDING;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}

STDAPI XblSocialSubmitReputationFeedbackAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ uint64_t xuid,
    _In_ XblReputationFeedbackType feedbackType,
    _In_opt_ const XblMultiplayerSessionReference* sessionReference,
    _In_z_ const char* reasonMessage,
    _In_opt_z_ const char* evidenceResourceId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContextHandle == nullptr || reasonMessage == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return SubmitReputationFeedback(
        xboxLiveContextHandle,
        ReputationFeedbackRequest{ xuid, feedbackType, sessionReference, reasonMessage, evidenceResourceId },
        async
    );
}
CATCH_RETURN()

STDAPI XblSocialSubmitBatchReputationFeedbackAsync(
    _In_ XblContextHandle xboxLiveContextHandle,
    _In_ const XblReputationFeedbackItem* feedbackItems,
    _In_ size_t feedbackItemsCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContextHandle == nullptr || feedbackItems == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    return SubmitReputationFeedback(
        xboxLiveContextHandle,
        ReputationFeedbackRequest{ feedbackItems, feedbackItemsCount },
        async
    );
}
CATCH_RETURN()
