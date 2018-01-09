// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/title_storage_c.h"
#include "title_storage_taskargs.h"
#include "xbox_live_context_impl.h"
#include "title_storage_blob_metadata_impl.h"

using namespace xbox::services::title_storage;

HC_RESULT get_quota_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    auto args = reinterpret_cast<get_quota_taskargs*>(context);
    auto titleStorageService = args->pXboxLiveContext->pImpl->cppObject()->title_storage_service();

    auto result = titleStorageService.get_quota(
        utils_c::to_utf16string(args->serviceConfigurationId),
        static_cast<title_storage_type>(args->storageType))
        .get();

    args->copy_xbox_live_result(result);

    args->completionRoutinePayload.serviceConfigurationId = args->serviceConfigurationId.data();
    args->completionRoutinePayload.storageType = args->storageType;

    if (!result.err())
    {
        XSAPI_TITLE_STORAGE_QUOTA& quota = args->completionRoutinePayload;

        args->xboxUserId = utils_c::to_utf8string(result.payload().xbox_user_id());
        quota.xboxUserId = args->xboxUserId.data();

        quota.usedBytes = result.payload().used_bytes();
        quota.quotaBytes = result.payload().quota_bytes();
    }

    return HCTaskSetCompleted(taskHandle);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
TitleStorageGetQuota(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR serviceConfigurationId,
    _In_ XSAPI_TITLE_STORAGE_TYPE storageType,
    _In_ XSAPI_GET_QUOTA_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto args = new get_quota_taskargs();
    args->pXboxLiveContext = pContext;
    args->serviceConfigurationId = serviceConfigurationId;
    args->storageType = storageType;

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            get_quota_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine_with_payload<get_quota_taskargs, XSAPI_GET_QUOTA_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()

HC_RESULT get_blob_metadata_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    auto args = reinterpret_cast<get_blob_metadata_taskargs*>(context);
    auto titleStorageService = args->pXboxLiveContext->pImpl->cppObject()->title_storage_service();

    auto result = titleStorageService.get_blob_metadata(
        args->serviceConfigurationId,
        static_cast<title_storage_type>(args->storageType),
        args->blobPath,
        args->xboxUserId,
        args->skipItems,
        args->maxItems)
        .get();

    args->copy_xbox_live_result(result);

    if (!result.err())
    {
        auto& metadataResultImpl = get_xsapi_singleton_c()->m_titleStorageState->m_blobMetadataResultImpl;

        metadataResultImpl.update(result.payload());

        args->completionRoutinePayload.itemCount = metadataResultImpl.item_count();
        args->completionRoutinePayload.hasNext = metadataResultImpl.cppObject().has_next();
        args->completionRoutinePayload.items = metadataResultImpl.first_item();
    }
    return HCTaskSetCompleted(taskHandle);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
TitleStorageGetBlobMetadata(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ PCSTR serviceConfigurationId,
    _In_ XSAPI_TITLE_STORAGE_TYPE storageType,
    _In_opt_ PCSTR blobPath,
    _In_opt_ PCSTR xboxUserId,
    _In_opt_ uint32_t skipItems,
    _In_opt_ uint32_t maxItems,
    _In_ XSAPI_GET_BLOB_METADATA_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto args = new get_blob_metadata_taskargs();
    args->pXboxLiveContext = pContext;
    args->serviceConfigurationId = utils_c::to_utf16string(serviceConfigurationId);
    args->storageType = storageType;
    args->skipItems = skipItems;
    args->maxItems = maxItems;

    if (blobPath != nullptr)
    {
        args->blobPath = utils_c::to_utf16string(blobPath);
    }

    if (xboxUserId != nullptr)
    {
        args->xboxUserId = utils_c::to_utf16string(xboxUserId);
    }

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            get_blob_metadata_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine_with_payload<get_blob_metadata_taskargs, XSAPI_GET_BLOB_METADATA_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()

HC_RESULT blob_metadata_result_get_next_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    auto args = reinterpret_cast<blob_metadata_result_get_next_taskargs*>(context);

    auto& metadataResultImpl = get_xsapi_singleton_c()->m_titleStorageState->m_blobMetadataResultImpl;
    auto result = metadataResultImpl.cppObject().get_next(args->maxItems).get();

    args->copy_xbox_live_result(result);

    if (!result.err())
    {
        metadataResultImpl.update(result.payload());

        args->completionRoutinePayload.itemCount = metadataResultImpl.item_count();
        args->completionRoutinePayload.hasNext = metadataResultImpl.cppObject().has_next();
        args->completionRoutinePayload.items = metadataResultImpl.first_item();
    }
    return HCTaskSetCompleted(taskHandle);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
TitleStorageBlobMetadataResultGetNext(
    _In_ XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT metadataResult,
    _In_ uint32_t maxItems,
    _In_ XSAPI_GET_BLOB_METADATA_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    auto singleton = get_xsapi_singleton_c();
    {
        std::lock_guard<std::recursive_mutex> lock(singleton->m_titleStorageState->m_lock);
        if (!singleton->m_titleStorageState->m_blobMetadataResultImpl.cppObject().has_next())
        {
            return XSAPI_RESULT_E_GENERIC_ERROR;
        }
    }
    auto args = new blob_metadata_result_get_next_taskargs();
    args->maxItems = maxItems;

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            blob_metadata_result_get_next_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine_with_payload<blob_metadata_result_get_next_taskargs, XSAPI_GET_BLOB_METADATA_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
TitleStorageCreateBlobMetadata(
    _In_ PCSTR serviceConfigurationId,
    _In_ XSAPI_TITLE_STORAGE_TYPE storageType,
    _In_ PCSTR blobPath,
    _In_ XSAPI_TITLE_STORAGE_BLOB_TYPE blobType,
    _In_opt_ PCSTR xboxUserId,
    _In_opt_ PCSTR displayName,
    _In_opt_ PCSTR etag,
    _In_opt_ time_t* pClientTimeStamp,
    _Out_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA** ppMetadata
    ) XBL_NOEXCEPT
try
{
    auto singleton = get_xsapi_singleton_c();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_titleStorageState->m_lock);

    auto pMetadata = new XSAPI_TITLE_STORAGE_BLOB_METADATA();

    title_storage_blob_metadata cppMetadata = title_storage_blob_metadata(
        utils_c::to_utf16string(serviceConfigurationId),
        static_cast<title_storage_type>(storageType),
        utils_c::to_utf16string(blobPath),
        static_cast<title_storage_blob_type>(blobType),
        xboxUserId == nullptr ? string_t() : utils_c::to_utf16string(xboxUserId),
        displayName == nullptr ? string_t() : utils_c::to_utf16string(displayName),
        etag == nullptr ? string_t() : utils_c::to_utf16string(etag),
        pClientTimeStamp == nullptr ? utility::datetime() : utils_c::datetime_from_time_t(pClientTimeStamp));

    pMetadata->pImpl = new XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL(pMetadata, cppMetadata);
    singleton->m_titleStorageState->m_blobMetadata.insert(pMetadata);

    *ppMetadata = pMetadata;

    return XSAPI_RESULT_OK;
}
CATCH_RETURN()

XBL_API XSAPI_RESULT XBL_CALLING_CONV
TitleStorageReleaseBlobMetadata(
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata
    ) XBL_NOEXCEPT
try
{
    auto singleton = get_xsapi_singleton_c();
    std::lock_guard<std::recursive_mutex> lock(singleton->m_titleStorageState->m_lock);

    size_t erasedItems = singleton->m_titleStorageState->m_blobMetadata.erase(pMetadata);
    if (erasedItems > 0 && pMetadata->pImpl != nullptr)
    {
        delete pMetadata->pImpl;
    }
    return XSAPI_RESULT_OK;
}
CATCH_RETURN()

HC_RESULT delete_blob_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
    )
{
    auto args = reinterpret_cast<delete_blob_taskargs*>(context);
    auto titleStorageService = args->pXboxLiveContext->pImpl->cppObject()->title_storage_service();

    auto result = titleStorageService.delete_blob(args->pMetadata->pImpl->cppObject(), args->deleteOnlyIfEtagMatches).get();

    args->copy_xbox_live_result(result);

    return HCTaskSetCompleted(taskHandle);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
TitleStorageDeleteBlob(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata,
    _In_ bool deleteOnlyIfEtagMatches,
    _In_ XSAPI_DELETE_BLOB_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    if (pContext == nullptr || pMetadata == nullptr || pMetadata->pImpl == nullptr)
    {
        return XSAPI_RESULT_E_HC_INVALIDARG;
    }

    auto args = new delete_blob_taskargs();
    args->pXboxLiveContext = pContext;
    args->pMetadata = pMetadata;
    args->deleteOnlyIfEtagMatches = deleteOnlyIfEtagMatches;

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            delete_blob_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine<delete_blob_taskargs, XSAPI_DELETE_BLOB_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()

HC_RESULT download_blob_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
)
{
    auto args = reinterpret_cast<download_blob_taskargs*>(context);
    auto titleStorageService = args->pXboxLiveContext->pImpl->cppObject()->title_storage_service();

    auto blobBufferSharedPtr = std::make_shared<std::vector<unsigned char>>(args->cbBlobBuffer);

    auto result = titleStorageService.download_blob(
        args->pMetadata->pImpl->cppObject(),
        blobBufferSharedPtr,
        static_cast<title_storage_e_tag_match_condition>(args->etagMatchCondition),
        args->selectQuery,
        args->preferredDownloadBlockSize)
        .get();

    args->copy_xbox_live_result(result);

    if (!result.err())
    {
        args->pMetadata->pImpl->update(result.payload().blob_metadata());
        args->completionRoutinePayload.pMetadata = args->pMetadata;

        memcpy(args->blobBuffer, blobBufferSharedPtr->data(), args->cbBlobBuffer);
        args->completionRoutinePayload.blobBuffer = args->blobBuffer;

        args->completionRoutinePayload.cbBlobBuffer = (uint32_t)blobBufferSharedPtr->size();
    }
    return HCTaskSetCompleted(taskHandle);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
TitleStorageDownloadBlob(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata,
    _In_ PBYTE blobBuffer,
    _In_ uint32_t cbBlobBuffer,
    _In_ XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION etagMatchCondition,
    _In_opt_ PCSTR selectQuery,
    _In_opt_ uint32_t* preferredDownloadBlockSize,
    _In_ XSAPI_DOWNLOAD_BLOB_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    if (pContext == nullptr || pMetadata == nullptr || pMetadata->pImpl == nullptr)
    {
        return XSAPI_RESULT_E_HC_INVALIDARG;
    }

    auto args = new download_blob_taskargs();
    args->pMetadata = pMetadata;
    args->pXboxLiveContext = pContext;
    args->blobBuffer = blobBuffer;
    args->cbBlobBuffer = cbBlobBuffer;
    args->etagMatchCondition = etagMatchCondition;
    args->selectQuery = selectQuery == nullptr ? string_t() : utils_c::to_utf16string(selectQuery);
    args->preferredDownloadBlockSize = preferredDownloadBlockSize == nullptr ? title_storage_service::DEFAULT_DOWNLOAD_BLOCK_SIZE : *preferredDownloadBlockSize;

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            download_blob_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine_with_payload<download_blob_taskargs, XSAPI_DOWNLOAD_BLOB_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()

HC_RESULT upload_blob_execute(
    _In_opt_ void* context,
    _In_ HC_TASK_HANDLE taskHandle
)
{
    auto args = reinterpret_cast<upload_blob_taskargs*>(context);
    auto titleStorageService = args->pXboxLiveContext->pImpl->cppObject()->title_storage_service();

    auto result = titleStorageService.upload_blob(
        args->pMetadata->pImpl->cppObject(),
        args->blobBuffer,
        static_cast<title_storage_e_tag_match_condition>(args->etagMatchCondition),
        args->preferredUploadBlockSize)
        .get();

    args->copy_xbox_live_result(result);

    if (!result.err())
    {
        args->pMetadata->pImpl->update(result.payload());
        args->completionRoutinePayload = args->pMetadata;
    }
    return HCTaskSetCompleted(taskHandle);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
TitleStorageUploadBlob(
    _In_ XSAPI_XBOX_LIVE_CONTEXT* pContext,
    _In_ CONST XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata,
    _In_ PBYTE blobBuffer,
    _In_ uint32_t cbBlobBuffer,
    _In_ XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION etagMatchCondition,
    _In_opt_ uint32_t* preferredUploadBlockSize,
    _In_ XSAPI_UPLOAD_BLOB_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    if (pContext == nullptr || pMetadata == nullptr || pMetadata->pImpl == nullptr)
    {
        return XSAPI_RESULT_E_HC_INVALIDARG;
    }

    auto args = new upload_blob_taskargs();
    args->pMetadata = pMetadata;
    args->pXboxLiveContext = pContext;
    args->blobBuffer = std::make_shared<std::vector<unsigned char>>(blobBuffer, blobBuffer + cbBlobBuffer);
    args->etagMatchCondition = etagMatchCondition;
    args->preferredUploadBlockSize = preferredUploadBlockSize == nullptr ? title_storage_service::DEFAULT_UPLOAD_BLOCK_SIZE : *preferredUploadBlockSize;

    return utils_c::xsapi_result_from_hc_result(
        HCTaskCreate(
            HC_SUBSYSTEM_ID_XSAPI,
            taskGroupId,
            upload_blob_execute,
            static_cast<void*>(args),
            utils_c::execute_completion_routine_with_payload<upload_blob_taskargs, XSAPI_UPLOAD_BLOB_COMPLETION_ROUTINE>,
            static_cast<void*>(args),
            static_cast<void*>(completionRoutine),
            completionRoutineContext,
            nullptr
        ));
}
CATCH_RETURN()