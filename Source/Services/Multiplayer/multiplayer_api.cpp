// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/multiplayer_c.h"
#include "multiplayer_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;

STDAPI XblMultiplayerSearchHandleDuplicateHandle(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ XblMultiplayerSearchHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || duplicatedHandle == nullptr);

    handle->AddRef();
    *duplicatedHandle = handle;

    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblMultiplayerSearchHandleCloseHandle(
    _In_ XblMultiplayerSearchHandle handle
) XBL_NOEXCEPT
try
{
    if (handle)
    {
        handle->DecRef();
    }
}
CATCH_RETURN_WITH(;)

STDAPI XblMultiplayerSearchHandleGetSessionReference(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ XblMultiplayerSessionReference* sessionRef
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || sessionRef == nullptr);
    *sessionRef = handle->SessionReference();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetId(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const char** id
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || id == nullptr);
    *id = handle->HandleId().data();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetSessionOwnerXuids(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const uint64_t** xuids,
    _Out_ size_t* xuidsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || xuids == nullptr || xuidsCount == nullptr);

    *xuids = handle->SessionOwnerXuids().data();
    *xuidsCount = handle->SessionOwnerXuids().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetTags(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const XblMultiplayerSessionTag** tags,
    _Out_ size_t* tagsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || tags == nullptr || tagsCount == nullptr);

    *tags = handle->Tags().data();
    *tagsCount = handle->Tags().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetStringAttributes(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const XblMultiplayerSessionStringAttribute** attributes,
    _Out_ size_t* attributesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || attributes == nullptr || attributesCount == nullptr);

    *attributes = handle->StringAttributes().data();
    *attributesCount = handle->StringAttributes().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetNumberAttributes(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const XblMultiplayerSessionNumberAttribute** attributes,
    _Out_ size_t* attributesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || attributes == nullptr || attributesCount == nullptr);

    *attributes = handle->NumberAttributes().data();
    *attributesCount = handle->NumberAttributes().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetVisibility(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ XblMultiplayerSessionVisibility* visibility
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || visibility == nullptr);
    *visibility = handle->Visibility();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetJoinRestriction(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ XblMultiplayerSessionRestriction* joinRestriction
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || joinRestriction == nullptr);
    *joinRestriction = handle->JoinRestriction();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetSessionClosed(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ bool* closed
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || closed == nullptr);
    *closed = handle->Closed();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetMemberCounts(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_opt_ size_t* maxMembers,
    _Out_opt_ size_t* currentMembers
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr);

    if (maxMembers)
    {
        *maxMembers = handle->MaxMembersCount();
    }
    if (currentMembers)
    {
        *currentMembers = handle->MembersCount();
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetCreationTime(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ time_t* creationTime
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || creationTime == nullptr);
    *creationTime = utils::time_t_from_datetime(handle->HandleCreationTime());
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerSearchHandleGetCustomSessionPropertiesJson(
    _In_ XblMultiplayerSearchHandle handle,
    _Out_ const char** customPropertiesJson
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || customPropertiesJson == nullptr);
    auto& customPropertiesString{ handle->CustomSessionPropertiesJson() };
    if (customPropertiesString.empty())
    {
        *customPropertiesJson = nullptr;
    }
    else
    {
        *customPropertiesJson = customPropertiesString.data();
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI XblMultiplayerCreateSearchHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ const XblMultiplayerSessionReference* sessionRef,
    _In_reads_opt_(tagsCount) const XblMultiplayerSessionTag* tags,
    _In_ size_t tagsCount,
    _In_reads_opt_(numberAttributesCount) const XblMultiplayerSessionNumberAttribute* numberAttributes,
    _In_ size_t numberAttributesCount,
    _In_reads_opt_(stringAttributesCount) const XblMultiplayerSessionStringAttribute* stringAttributes,
    _In_ size_t stringAttributesCount,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContext == nullptr || sessionRef == nullptr || async == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF(tagsCount > 0 && tags == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF(numberAttributesCount > 0 && numberAttributes == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF(stringAttributesCount > 0 && stringAttributes == nullptr);

    //ensure all attributes are properly terminated, otherwise we'll write a property 
    //longer than is allowed into the json, which will crash eventually on deserialization
    if (tagsCount > 0)
    {
        for (size_t i = 0; i < tagsCount; i++)
        {
            RETURN_HR_INVALIDARGUMENT_IF(utils::EnsureLessThanMaxLength(tags[i].value, XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH) == false);
        }
    }
    if (numberAttributesCount > 0)
    {
        for (size_t i = 0; i < numberAttributesCount; i++)
        {
            RETURN_HR_INVALIDARGUMENT_IF(utils::EnsureLessThanMaxLength(numberAttributes[i].name, XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH) == false);
        }
    }
    if (stringAttributesCount > 0)
    {
        for (size_t i = 0; i < stringAttributesCount; i++)
        {
            RETURN_HR_INVALIDARGUMENT_IF(utils::EnsureLessThanMaxLength(stringAttributes[i].name, XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH) == false);
            RETURN_HR_INVALIDARGUMENT_IF(utils::EnsureLessThanMaxLength(stringAttributes[i].value, XBL_MULTIPLAYER_SEARCH_HANDLE_MAX_FIELD_LENGTH) == false);
        }
    }

    MultiplayerSearchHandleRequest request{ *sessionRef };
    if (tagsCount > 0)
    {
        request.SetTags(xsapi_internal_vector<XblMultiplayerSessionTag>{ tags, tags + tagsCount });
    }
    if (numberAttributesCount > 0)
    {
        request.SetNumberAttributes(xsapi_internal_vector<XblMultiplayerSessionNumberAttribute>{ numberAttributes, numberAttributes + numberAttributesCount });
    }
    if (stringAttributesCount > 0)
    {
        request.SetStringAttributes(xsapi_internal_vector<XblMultiplayerSessionStringAttribute>{ stringAttributes, stringAttributes + stringAttributesCount });
    }

    return RunAsync(async, __FUNCTION__,
        [
            context{ xblContext->shared_from_this() },
            request,
            resultHandle = std::shared_ptr<XblMultiplayerSearchHandleDetails>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(context->MultiplayerService()->CreateSearchHandle(
                request,
                AsyncContext<Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>>> { data->async->queue,
                [
                    &resultHandle,
                    data
                ]
            (Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>> result)
            {
                if (Succeeded(result))
                {
                    resultHandle = result.ExtractPayload();
                }
                XAsyncComplete(data->async, result.Hresult(), sizeof(XblMultiplayerSearchHandle));
            }
            }));

            return E_PENDING;
        }

        case XAsyncOp::GetResult:
        {
            auto handlePtr = static_cast<XblMultiplayerSearchHandle*>(data->buffer);
            resultHandle->AddRef();
            *handlePtr = resultHandle.get();

            return S_OK;
        }

        default: return S_OK;
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerCreateSearchHandleResult(
    _In_ XAsyncBlock* async,
    _Out_opt_ XblMultiplayerSearchHandle* handle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);

    XblMultiplayerSearchHandle handleCopy{ nullptr };
    auto hr = XAsyncGetResult(async, nullptr, sizeof(XblMultiplayerSearchHandle), &handleCopy, nullptr);
    if (SUCCEEDED(hr))
    {
        if (handle != nullptr)
        {
            *handle = handleCopy;
        }
        else
        {
            XblMultiplayerSearchHandleCloseHandle(handleCopy);
        }
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerDeleteSearchHandleAsync(
    _In_ XblContextHandle xblContext,
    _In_ const char* handleId,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContext == nullptr || handleId == nullptr || async == nullptr);

    return RunAsync(async, __FUNCTION__,
        [
            context{ xblContext->shared_from_this() },
            handleId = xsapi_internal_string{ handleId }
        ]
    (XAsyncOp op, const XAsyncProviderData* data)
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(context->MultiplayerService()->DeleteSearchHandle(handleId, data->async));
            return E_PENDING;
        }
        default: return S_OK;
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerGetSearchHandlesAsync(
    _In_ XblContextHandle xblContext,
    _In_z_ const char* scid,
    _In_z_ const char* sessionTemplateName,
    _In_opt_z_ const char* orderByAttribute,
    _In_ bool orderAscending,
    _In_opt_z_ const char* searchFilter,
    _In_opt_z_ const char* socialGroup,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xblContext == nullptr || scid == nullptr || sessionTemplateName == nullptr || async == nullptr);

    MultiplayerQuerySearchHandleRequest request{ scid, sessionTemplateName };
    if (orderByAttribute)
    {
        request.SetOrderBy(orderByAttribute);
        request.SetOrderAscending(orderAscending);
    }
    if (searchFilter)
    {
        request.SetSearchFilter(searchFilter);
    }
    if (socialGroup)
    {
        request.SetSocialGroup(socialGroup);
    }

    return RunAsync(async, __FUNCTION__,
        [
            context{ xblContext->shared_from_this() },
            request,
            searchHandles = xsapi_internal_vector<std::shared_ptr<XblMultiplayerSearchHandleDetails>>{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(context->MultiplayerService()->GetSearchHandles(
                request,
                AsyncContext<Result<Vector<std::shared_ptr<XblMultiplayerSearchHandleDetails>>>>{ data->async->queue,
                [
                    &searchHandles,
                    data
                ]
            (Result<Vector<std::shared_ptr<XblMultiplayerSearchHandleDetails>>> result)
            {
                if (Succeeded(result))
                {
                    searchHandles = result.ExtractPayload();
                }
                XAsyncComplete(data->async, result.Hresult(), searchHandles.size() * sizeof(XblMultiplayerSearchHandle));
            }
            }));

            return E_PENDING;
        }

        case XAsyncOp::GetResult:
        {
            auto handlesPtr = static_cast<XblMultiplayerSearchHandle*>(data->buffer);
            for (const auto& searchHandle : searchHandles)
            {
                searchHandle->AddRef();
                *handlesPtr++ = searchHandle.get();
            }
            return S_OK;
        }

        default: return S_OK;
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerGetSearchHandlesResultCount(
    _In_ XAsyncBlock* async,
    _Out_ size_t* searchHandleCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(async == nullptr || searchHandleCount == nullptr);

    size_t bufferSize{ 0 };
    auto hr = XAsyncGetResultSize(async, &bufferSize);

    if (SUCCEEDED(hr))
    {
        *searchHandleCount = bufferSize / sizeof(XblMultiplayerSearchHandle);
    }
    return hr;
}
CATCH_RETURN()

STDAPI XblMultiplayerGetSearchHandlesResult(
    _In_ XAsyncBlock* async,
    _Out_writes_(searchHandlesCount) XblMultiplayerSearchHandle* searchHandles,
    _In_ size_t searchHandlesCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_IF(searchHandlesCount == 0, S_OK);
    RETURN_HR_INVALIDARGUMENT_IF(async == nullptr || searchHandles == nullptr);
    return XAsyncGetResult(async, nullptr, searchHandlesCount * sizeof(XblMultiplayerSearchHandle), searchHandles, nullptr);
}
CATCH_RETURN()

STDAPI XblMultiplayerSetTransferHandleAsync(
    _In_ XblContextHandle xblContextHandle,
    _In_ XblMultiplayerSessionReference targetSessionReference,
    _In_ XblMultiplayerSessionReference originSessionReference,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(async);

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xblContextHandle->shared_from_this() },
            targetSessionReference,
            originSessionReference,
            result = XblMultiplayerSessionHandleId{}
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        switch (op)
        {
        case XAsyncOp::DoWork:
        {
            RETURN_HR_IF_FAILED(xblContext->MultiplayerService()->SetTransferHandle(
                targetSessionReference,
                originSessionReference,
                AsyncContext<Result<String>>{ data->async->queue,
                [
                    async{ data->async },
                    &result
                ]
            (Result<String> internalResult)
            {
                auto hr = internalResult.Hresult();
                if (SUCCEEDED(hr))
                {
                    utils::strcpy(result.value, sizeof(result.value), internalResult.Payload().data());
                }
                XAsyncComplete(async, hr, sizeof(XblMultiplayerSessionHandleId));
            }
            }));

            return E_PENDING;
        }
        case XAsyncOp::GetResult:
        {
            auto resultPtr = static_cast<XblMultiplayerSessionHandleId*>(data->buffer);
            *resultPtr = result;
            return S_OK;
        }
        default: return S_OK;
        }
    });
}
CATCH_RETURN()

STDAPI XblMultiplayerSetTransferHandleResult(
    _In_ XAsyncBlock* async,
    _Out_ XblMultiplayerSessionHandleId* handleId
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblMultiplayerSessionHandleId), handleId, nullptr);
}
CATCH_RETURN()
