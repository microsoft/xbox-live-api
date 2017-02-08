//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "TitleStorageService_WinRT.h"

using namespace xbox::services::title_storage;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

TitleStorageService::TitleStorageService(
    _In_ xbox::services::title_storage::title_storage_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

Windows::Foundation::IAsyncOperation<TitleStorageQuota^>^ 
TitleStorageService::GetQuotaAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ TitleStorageType storageType
    )
{
    auto task = m_cppObj.get_quota(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        static_cast<title_storage_type>(storageType)
        )
    .then([](xbox_live_result<title_storage_quota> titleStorageQuota)
    {
        THROW_IF_ERR(titleStorageQuota);
        return ref new TitleStorageQuota(titleStorageQuota.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TitleStorageQuota^>^
TitleStorageService::GetQuotaForSessionStorageAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ multiplayerSessionTemplateName,
    _In_ Platform::String^ multiplayerSessionName
    )
{
#pragma warning(suppress: 4996)
    auto task = m_cppObj.get_quota_for_session_storage(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(multiplayerSessionTemplateName),
        STRING_T_FROM_PLATFORM_STRING(multiplayerSessionName)
        )
    .then([](xbox_live_result<title_storage_quota> titleStorageQuota)
    {
        THROW_IF_ERR(titleStorageQuota);
        return ref new TitleStorageQuota(titleStorageQuota.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TitleStorageBlobMetadataResult^>^
TitleStorageService::GetBlobMetadataAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ TitleStorageType storageType,
    _In_opt_ Platform::String^ blobPath,
    _In_opt_ Platform::String^ xboxUserId,
    _In_ uint32 skipItems,
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_blob_metadata(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        static_cast<title_storage_type>(storageType),
        STRING_T_FROM_PLATFORM_STRING(blobPath),
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        skipItems,
        maxItems
        )
    .then([](xbox_live_result<title_storage_blob_metadata_result> blobMetadataResult)
    {
        THROW_IF_ERR(blobMetadataResult);
        return ref new TitleStorageBlobMetadataResult(blobMetadataResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TitleStorageBlobMetadataResult^>^
TitleStorageService::GetBlobMetadataForSessionStorageAsync(
    _In_ Platform::String^ serviceConfigurationId,
    _In_opt_ Platform::String^ blobPath,
    _In_ Platform::String^ multiplayerSessionTemplateName,
    _In_ Platform::String^ multiplayerSessionName,
    _In_ uint32 skipItems,
    _In_ uint32 maxItems
    )
{
#pragma warning(suppress: 4996)
    auto task = m_cppObj.get_blob_metadata_for_session_storage(
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(blobPath),
        STRING_T_FROM_PLATFORM_STRING(multiplayerSessionTemplateName),
        STRING_T_FROM_PLATFORM_STRING(multiplayerSessionName),
        skipItems,
        maxItems
        )
    .then([](xbox_live_result<title_storage_blob_metadata_result> blobMetadataResult)
    {
        THROW_IF_ERR(blobMetadataResult);
        return ref new TitleStorageBlobMetadataResult(blobMetadataResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncAction^
TitleStorageService::DeleteBlobAsync(
    _In_ TitleStorageBlobMetadata^ blobMetadata,
    _In_ bool deleteOnlyIfETagMatches
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(blobMetadata);

    auto task = m_cppObj.delete_blob(
        blobMetadata->GetCppObj(),
        deleteOnlyIfETagMatches
        )
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TitleStorageBlobResult^>^
TitleStorageService::DownloadBlobAsync(
    _In_ TitleStorageBlobMetadata^ blobMetadata,
    _In_ Windows::Storage::Streams::IBuffer^ blobBuffer,
    _In_ TitleStorageETagMatchCondition etagMatchCondition,
    _In_opt_ Platform::String^ selectQuery
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(blobMetadata);
    THROW_INVALIDARGUMENT_IF_NULL(blobBuffer);

    std::shared_ptr<std::vector<unsigned char>> nativeBlobBuffer = std::make_shared<std::vector<unsigned char>>(blobBuffer->Length);

    auto task = m_cppObj.download_blob(
        blobMetadata->GetCppObj(),
        nativeBlobBuffer,
        static_cast<title_storage_e_tag_match_condition>(etagMatchCondition),
        STRING_T_FROM_PLATFORM_STRING(selectQuery),
        title_storage_service::DEFAULT_DOWNLOAD_BLOCK_SIZE
        )
    .then([](xbox_live_result<title_storage_blob_result> blobResult)
    {
        THROW_IF_ERR(blobResult);
        return ref new TitleStorageBlobResult(blobResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TitleStorageBlobResult^>^
TitleStorageService::DownloadBlobAsync(
    _In_ TitleStorageBlobMetadata^ blobMetadata,
    _In_ Windows::Storage::Streams::IBuffer^ blobBuffer,
    _In_ TitleStorageETagMatchCondition etagMatchCondition,
    _In_opt_ Platform::String^ selectQuery,
    _In_ uint32 preferredDownloadBlockSize
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(blobBuffer);
    THROW_INVALIDARGUMENT_IF_NULL(blobMetadata);

    std::shared_ptr<std::vector<unsigned char>> nativeBlobBuffer = std::make_shared<std::vector<unsigned char>>(blobBuffer->Length);

    auto task = m_cppObj.download_blob(
        blobMetadata->GetCppObj(),
        nativeBlobBuffer,
        static_cast<title_storage_e_tag_match_condition>(etagMatchCondition),
        STRING_T_FROM_PLATFORM_STRING(selectQuery),
        preferredDownloadBlockSize
        )
    .then([](xbox_live_result<title_storage_blob_result> blobResult)
    {
        THROW_IF_ERR(blobResult);
        return ref new TitleStorageBlobResult(blobResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TitleStorageBlobMetadata^>^
TitleStorageService::UploadBlobAsync(
    _In_ TitleStorageBlobMetadata^ blobMetadata,
    _In_ Windows::Storage::Streams::IBuffer^ blobBuffer,
    _In_ TitleStorageETagMatchCondition etagMatchCondition,
    _In_ uint32 preferredUploadBlockSize
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(blobBuffer);
    THROW_INVALIDARGUMENT_IF_NULL(blobMetadata);

    auto reader = Windows::Storage::Streams::DataReader::FromBuffer(blobBuffer);
    std::shared_ptr<std::vector<unsigned char>> nativeBlobBuffer = std::make_shared<std::vector<unsigned char>>(reader->UnconsumedBufferLength);

    auto nativeBlobBufferSize = nativeBlobBuffer->size();
    THROW_INVALIDARGUMENT_IF(nativeBlobBufferSize > UINT32_MAX);
    reader->ReadBytes(Platform::ArrayReference<unsigned char>(&(nativeBlobBuffer->at(0)), static_cast<uint32>(nativeBlobBufferSize)));

    auto task = m_cppObj.upload_blob(
        blobMetadata->GetCppObj(),
        nativeBlobBuffer,
        static_cast<title_storage_e_tag_match_condition>(etagMatchCondition),
        preferredUploadBlockSize
        )
    .then([](xbox_live_result<title_storage_blob_metadata> blobMetadata)
    {
        THROW_IF_ERR(blobMetadata);
        return ref new TitleStorageBlobMetadata(blobMetadata.payload());
    });

    return ASYNC_FROM_TASK(task);

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END