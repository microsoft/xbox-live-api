// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/title_storage_c.h"
#include "title_storage_internal.h"
#include "xbox_live_context_internal.h"

using namespace xbox::services;
using namespace xbox::services::title_storage;

STDAPI XblTitleStorageBlobMetadataResultGetItems(
    _In_ XblTitleStorageBlobMetadataResultHandle resultHandle,
    _Out_ const XblTitleStorageBlobMetadata** items,
    _Out_ size_t* itemsCount
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(resultHandle == nullptr || items == nullptr || itemsCount == nullptr);
    VERIFY_XBL_INITIALIZED();

    *items = resultHandle->Items().data();
    *itemsCount = resultHandle->Items().size();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblTitleStorageBlobMetadataResultHasNext(
    _In_ XblTitleStorageBlobMetadataResultHandle resultHandle,
    _Out_ bool* hasNext
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(resultHandle == nullptr || hasNext == nullptr);
    VERIFY_XBL_INITIALIZED();

    *hasNext = resultHandle->HasNext();
    
    return S_OK;
}
CATCH_RETURN()

STDAPI XblTitleStorageBlobMetadataResultGetNextAsync(
    _In_ XblTitleStorageBlobMetadataResultHandle resultHandle,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(resultHandle == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    std::shared_ptr<XblTitleStorageBlobMetadataResult> cResult{ nullptr };

    return RunAsync(async, __FUNCTION__,
        [
            resultHandle,
            maxItems,
            async,
            cResult
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            HRESULT hr = resultHandle->GetNext(
                maxItems,
                AsyncContext<Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>>>
                (async->queue, [async, &cResult](Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>> result)
            {
                if (Succeeded(result))
                {
                    cResult = result.ExtractPayload();
                }

                XAsyncComplete(async, result.Hresult(), sizeof(XblTitleStorageBlobMetadataResultHandle));
            }
            ));

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            auto resultHandlePtr = static_cast<XblTitleStorageBlobMetadataResultHandle*>(data->buffer);
            *resultHandlePtr = cResult.get();
            cResult->AddRef();
        }
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblTitleStorageBlobMetadataResultGetNextResult(
    _In_ XAsyncBlock* async,
    _Out_ XblTitleStorageBlobMetadataResultHandle* result
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(result == nullptr);
    VERIFY_XBL_INITIALIZED();

    return XAsyncGetResult(async, nullptr, sizeof(XblTitleStorageBlobMetadataResultHandle), result, nullptr);
}
CATCH_RETURN()

STDAPI XblTitleStorageBlobMetadataResultDuplicateHandle(
    _In_ XblTitleStorageBlobMetadataResultHandle handle,
    _Out_ XblTitleStorageBlobMetadataResultHandle* duplicatedHandle
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(handle == nullptr || duplicatedHandle == nullptr);
    VERIFY_XBL_INITIALIZED();

    handle->AddRef();
    *duplicatedHandle = handle;

    return S_OK;
}
CATCH_RETURN()

STDAPI_(void) XblTitleStorageBlobMetadataResultCloseHandle(
    _In_ XblTitleStorageBlobMetadataResultHandle handle
) XBL_NOEXCEPT
try
{
    if (handle)
    {
        handle->DecRef();
    }
}
CATCH_RETURN_WITH(;)

STDAPI XblTitleStorageGetQuotaAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_z_ const char* serviceConfigurationId,
    _In_ XblTitleStorageType storageType,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || serviceConfigurationId == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    XblTitleStorageQuota cResult;

    
    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() },
            scid{ xsapi_internal_string(serviceConfigurationId) },
            storageType,
            async,
            cResult
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            HRESULT hr = xblContext->TitleStorageService()->GetQuota(
                scid, 
                storageType, 
                AsyncContext<Result<XblTitleStorageQuota>>
                (async->queue, [async, &cResult](Result<XblTitleStorageQuota> result) 
                {
                    if (Succeeded(result))
                    {
                        cResult = result.ExtractPayload();
                    }
                    
                    XAsyncComplete(async, result.Hresult(), sizeof(XblTitleStorageQuota));
                }
            ));

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            auto resultHandle = static_cast<XblTitleStorageQuota*>(data->buffer);
            *resultHandle = std::move(cResult);
        }
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblTitleStorageGetQuotaResult(
    _In_ XAsyncBlock* async,
    _Out_ size_t* usedBytes,
    _Out_ size_t* quotaBytes
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(async == nullptr || usedBytes == nullptr || quotaBytes == nullptr);
    VERIFY_XBL_INITIALIZED();

    XblTitleStorageQuota quota{};
    HRESULT hr = XAsyncGetResult(async, nullptr, sizeof(XblTitleStorageQuota), &quota, nullptr);

    if (SUCCEEDED(hr))
    {
        *usedBytes = quota.usedBytes;
        *quotaBytes = quota.quotaBytes;
    }

    return hr;
}
CATCH_RETURN()

STDAPI XblTitleStorageGetBlobMetadataAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_z_ const char* serviceConfigurationId,
    _In_ XblTitleStorageType storageType,
    _In_z_ const char* blobPath,
    _In_ uint64_t xuid,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || serviceConfigurationId == nullptr || async == nullptr);
    VERIFY_XBL_INITIALIZED();

    std::shared_ptr<XblTitleStorageBlobMetadataResult> cResult;

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() },
            scid{ xsapi_internal_string(serviceConfigurationId) },
            storageType,
            blobPathStr{ xsapi_internal_string(blobPath) },
            xuid,
            skipItems,
            maxItems,
            async,
            cResult
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            HRESULT hr = xblContext->TitleStorageService()->GetBlobMetadata(
                scid, 
                storageType,
                blobPathStr,
                xuid,
                skipItems,
                maxItems,
                "",
                AsyncContext<Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>>>
                (async->queue, [async, &cResult](Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>> result)
                {
                    if (Succeeded(result))
                    {
                        cResult = result.ExtractPayload();
                    }

                    XAsyncComplete(async, result.Hresult(), sizeof(XblTitleStorageBlobMetadataResultHandle));
                }
            ));

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            auto resultHandle = static_cast<XblTitleStorageBlobMetadataResultHandle*>(data->buffer);
            *resultHandle = cResult.get();
            cResult->AddRef();
        }
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblTitleStorageGetBlobMetadataResult(
    _In_ XAsyncBlock* async,
    _Out_ XblTitleStorageBlobMetadataResultHandle* result
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblTitleStorageBlobMetadataResultHandle), result, nullptr);
}
CATCH_RETURN()

STDAPI XblTitleStorageDeleteBlobAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _In_ bool deleteOnlyIfEtagMatches,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || async == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF(blobMetadata.blobType == XblTitleStorageBlobType::Unknown);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(blobMetadata.serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(blobMetadata.blobPath);
    VERIFY_XBL_INITIALIZED();

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() },
            blobMetadata,
            deleteOnlyIfEtagMatches,
            async
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        UNREFERENCED_PARAMETER(data);

        if (op == XAsyncOp::DoWork)
        {
            HRESULT hr = xblContext->TitleStorageService()->DeleteBlob(
                blobMetadata,
                deleteOnlyIfEtagMatches,
                AsyncContext<HRESULT>(async)
            );

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblTitleStorageDownloadBlobAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _Out_writes_(blobBufferCount) uint8_t* blobBuffer,
    _In_ size_t blobBufferCount,
    _In_ XblTitleStorageETagMatchCondition etagMatchCondition,
    _In_opt_z_ const char* selectQuery,
    _In_ size_t preferredDownloadBlockSize,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || blobBuffer == nullptr || async == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF(blobMetadata.blobType == XblTitleStorageBlobType::Unknown);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(blobMetadata.serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(blobMetadata.blobPath);

    VERIFY_XBL_INITIALIZED();

    XblTitleStorageBlobMetadata cResult;

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() },
            blobMetadata,
            blobBuffer,
            blobBufferCount,
            etagMatchCondition,
            selectQueryStr = selectQuery ? String{ selectQuery } : String{},
            preferredDownloadBlockSize,
            async,
            cResult
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            HRESULT hr = xblContext->TitleStorageService()->DownloadBlob(
                blobMetadata,
                blobBuffer,
                blobBufferCount,
                etagMatchCondition,
                selectQueryStr,
                preferredDownloadBlockSize,
                AsyncContext<Result<XblTitleStorageBlobMetadata>>
                (async->queue, [async, &cResult](Result<XblTitleStorageBlobMetadata> result)
            {
                if (Succeeded(result))
                {
                    cResult = result.ExtractPayload();
                }

                XAsyncComplete(async, result.Hresult(), sizeof(XblTitleStorageBlobMetadata));
            }
            ));

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            std::memcpy(data->buffer, &cResult, sizeof(XblTitleStorageBlobMetadata));
        }
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblTitleStorageDownloadBlobResult(
    _In_ XAsyncBlock* async,
    _Out_ XblTitleStorageBlobMetadata* blobMetadata
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblTitleStorageBlobMetadata), blobMetadata, nullptr);
}
CATCH_RETURN()

STDAPI XblTitleStorageUploadBlobAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _In_ const uint8_t* blobBuffer,
    _In_ size_t blobBufferCount,
    _In_ XblTitleStorageETagMatchCondition etagMatchCondition,
    _In_ size_t preferredDownloadBlockSize,
    _In_ XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(xboxLiveContext == nullptr || blobBuffer == nullptr || async == nullptr);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(blobMetadata.serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(blobMetadata.blobPath);
    VERIFY_XBL_INITIALIZED();

    XblTitleStorageBlobMetadata cResult;

    return RunAsync(async, __FUNCTION__,
        [
            xblContext{ xboxLiveContext->shared_from_this() },
            blobMetadata,
            blobBuffer,
            blobBufferCount,
            etagMatchCondition,
            preferredDownloadBlockSize,
            async,
            cResult
        ]
    (XAsyncOp op, const XAsyncProviderData* data) mutable
    {
        if (op == XAsyncOp::DoWork)
        {
            HRESULT hr = xblContext->TitleStorageService()->UploadBlob(
                blobMetadata,
                blobBuffer,
                blobBufferCount,
                etagMatchCondition,
                preferredDownloadBlockSize,
                AsyncContext<Result<XblTitleStorageBlobMetadata>>
                (async->queue, [async, &cResult](Result<XblTitleStorageBlobMetadata> result)
            {
                if (Succeeded(result))
                {
                    cResult = result.ExtractPayload();
                }

                XAsyncComplete(async, result.Hresult(), sizeof(XblTitleStorageBlobMetadata));
            }
            ));

            return SUCCEEDED(hr) ? E_PENDING : hr;
        }
        else if (op == XAsyncOp::GetResult)
        {
            std::memcpy(data->buffer, &cResult, sizeof(XblTitleStorageBlobMetadata));
        }
        return S_OK;
    });
}
CATCH_RETURN()

STDAPI XblTitleStorageUploadBlobResult(
    _In_ XAsyncBlock* async,
    _Out_ XblTitleStorageBlobMetadata* blobMetadata
) XBL_NOEXCEPT
try
{
    return XAsyncGetResult(async, nullptr, sizeof(XblTitleStorageBlobMetadata), blobMetadata, nullptr);
}
CATCH_RETURN()
