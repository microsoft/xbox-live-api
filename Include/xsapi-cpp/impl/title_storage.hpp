// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

inline title_storage_quota::title_storage_quota(
    string_t scid, 
    title_storage_type storageType, 
    uint64_t xuid, 
    uint64_t usedByted, 
    uint64_t quotaBytes
)
    :
    m_serviceConfigurationId{ scid },
    m_storageType{ storageType },
    m_xboxUserId{ xuid },
    m_usedBytes{ usedByted },
    m_quotaBytes{ quotaBytes }
{ }

inline const string_t& title_storage_quota::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

inline title_storage_type title_storage_quota::storage_type() const
{
    return m_storageType;
}

inline string_t title_storage_quota::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_xboxUserId);
}

inline uint64_t title_storage_quota::used_bytes() const
{
    return m_usedBytes;
}

inline uint64_t title_storage_quota::quota_bytes() const
{
    return m_quotaBytes;
}

inline title_storage_blob_metadata::title_storage_blob_metadata(
    XblTitleStorageBlobMetadata blobMetadata
)
    :
    m_blobMetadata{blobMetadata}
{ }

inline title_storage_blob_metadata::title_storage_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ title_storage_blob_type blobType,
    _In_ string_t xboxUserId
)
    :
    title_storage_blob_metadata(
        serviceConfigurationId,
        storageType,
        blobPath,
        blobType,
        xboxUserId,
        _T(""),
        _T("")
    )
{ }

inline title_storage_blob_metadata::title_storage_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ title_storage_blob_type blobType,
    _In_ string_t xboxUserId,
    _In_ string_t displayName,
    _In_ string_t eTag
)
    :
    title_storage_blob_metadata(
        serviceConfigurationId,
        storageType,
        blobPath,
        blobType,
        xboxUserId,
        _T(""),
        _T(""),
        utility::datetime()
    )
{ }

inline title_storage_blob_metadata::title_storage_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ title_storage_blob_type blobType,
    _In_ string_t xboxUserId,
    _In_ string_t displayName,
    _In_ string_t eTag,
    _In_ utility::datetime clientTimestamp
)
{
    Utils::Utf8FromCharT(serviceConfigurationId.c_str(), m_blobMetadata.serviceConfigurationId, sizeof(m_blobMetadata.serviceConfigurationId));
    m_blobMetadata.storageType = static_cast<XblTitleStorageType>(storageType);
    Utils::Utf8FromCharT(blobPath.c_str(), m_blobMetadata.blobPath, sizeof(m_blobMetadata.blobPath));
    m_blobMetadata.blobType = static_cast<XblTitleStorageBlobType>(blobType);
    m_blobMetadata.xboxUserId = Utils::Uint64FromStringT(xboxUserId);
    Utils::Utf8FromCharT(displayName.c_str(), m_blobMetadata.displayName, sizeof(m_blobMetadata.displayName));
    Utils::Utf8FromCharT(eTag.c_str(), m_blobMetadata.eTag, sizeof(m_blobMetadata.eTag));
    m_blobMetadata.clientTimestamp = Utils::TimeTFromDatetime(clientTimestamp);
}

inline string_t title_storage_blob_metadata::blob_path() const
{
    return Utils::StringTFromUtf8(m_blobMetadata.blobPath);
}

inline title_storage_blob_type title_storage_blob_metadata::blob_type() const
{
    return static_cast<title_storage_blob_type>(m_blobMetadata.blobType);
}

inline title_storage_type title_storage_blob_metadata::storage_type() const
{
    return static_cast<title_storage_type>(m_blobMetadata.storageType);
}

inline string_t title_storage_blob_metadata::display_name() const
{
    return Utils::StringTFromUtf8(m_blobMetadata.displayName);
}

inline string_t title_storage_blob_metadata::e_tag() const
{
    return Utils::StringTFromUtf8(m_blobMetadata.eTag);
}

inline utility::datetime title_storage_blob_metadata::client_timestamp() const
{
    return Utils::DatetimeFromTimeT(m_blobMetadata.clientTimestamp);
}

inline void title_storage_blob_metadata::set_client_timestamp(_In_ utility::datetime value)
{
    m_blobMetadata.clientTimestamp = Utils::TimeTFromDatetime(value);
}

inline uint64_t title_storage_blob_metadata::length() const
{
    return m_blobMetadata.length;
}

inline string_t title_storage_blob_metadata::service_configuration_id() const
{
    return Utils::StringTFromUtf8(m_blobMetadata.serviceConfigurationId);
}

inline string_t title_storage_blob_metadata::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_blobMetadata.xboxUserId);
}

inline title_storage_blob_metadata_result::title_storage_blob_metadata_result(
    XblTitleStorageBlobMetadataResultHandle handle
)
{
    XblTitleStorageBlobMetadataResultDuplicateHandle(handle, &m_handle);

    size_t itemsCount;
    const XblTitleStorageBlobMetadata* items;
    XblTitleStorageBlobMetadataResultGetItems(m_handle, &items, &itemsCount);
    
    for (size_t i = 0; i < itemsCount; i++)
    {
        m_items.push_back(title_storage_blob_metadata(items[i]));
    }
}

title_storage_blob_metadata_result::title_storage_blob_metadata_result(
    _In_ const title_storage_blob_metadata_result& other
)
{
    XblTitleStorageBlobMetadataResultDuplicateHandle(other.m_handle, &m_handle);

    size_t itemsCount;
    const XblTitleStorageBlobMetadata* items;
    XblTitleStorageBlobMetadataResultGetItems(m_handle, &items, &itemsCount);

    for (size_t i = 0; i < itemsCount; i++)
    {
        m_items.push_back(title_storage_blob_metadata(items[i]));
    }
}

title_storage_blob_metadata_result& title_storage_blob_metadata_result::operator=(
    title_storage_blob_metadata_result other
    )
{
    std::swap(m_handle, other.m_handle);
    std::swap(m_items, other.m_items);
    return *this;
}

inline title_storage_blob_metadata_result::~title_storage_blob_metadata_result()
{
    if (m_handle)
    {
        XblTitleStorageBlobMetadataResultCloseHandle(m_handle);
    }
}

inline const std::vector<title_storage_blob_metadata>& title_storage_blob_metadata_result::items() const
{
    return m_items;
}

inline pplx::task<xbox_live_result<title_storage_blob_metadata_result>> title_storage_blob_metadata_result::get_next(
    _In_ uint32_t maxItems
) const
{
    auto asyncWrapper = new AsyncWrapper<title_storage_blob_metadata_result>(
        [](XAsyncBlock* async, title_storage_blob_metadata_result& result)
    {
        XblTitleStorageBlobMetadataResultHandle handle;
        auto hr = XblTitleStorageBlobMetadataResultGetNextResult(async, &handle);
        if (SUCCEEDED(hr))
        {
            // title_storage_blob_metadata_result owns the lifetime of the handle
            result = title_storage_blob_metadata_result(handle);
            XblTitleStorageBlobMetadataResultCloseHandle(handle);
        }
        return hr;
    });

    auto hr = XblTitleStorageBlobMetadataResultGetNextAsync(
        m_handle,
        maxItems,
        &asyncWrapper->async
    );
    return asyncWrapper->Task(hr);
}

inline bool title_storage_blob_metadata_result::has_next() const
{
    bool hasNext = false;
    XblTitleStorageBlobMetadataResultHasNext(m_handle, &hasNext);
    return hasNext;
}

inline title_storage_blob_result::title_storage_blob_result(
    std::shared_ptr<std::vector<unsigned char>> blobBuffer, title_storage_blob_metadata blobMetadata
)
    :
    m_blobBuffer{ std::move(blobBuffer) },
    m_blobMetadata{ std::move(blobMetadata) }
{ }

inline std::shared_ptr<std::vector<unsigned char>> const title_storage_blob_result::blob_buffer() const
{
    return m_blobBuffer;
}

inline const title_storage_blob_metadata& title_storage_blob_result::blob_metadata() const
{
    return m_blobMetadata;
}

inline title_storage_service::title_storage_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

inline title_storage_service::title_storage_service(const title_storage_service& other)
{
    XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
}

inline title_storage_service& title_storage_service::operator=(title_storage_service other)
{
    std::swap(m_xblContext, other.m_xblContext);
    return *this;
}

inline title_storage_service::~title_storage_service()
{
    XblContextCloseHandle(m_xblContext);
}

inline pplx::task<xbox_live_result<title_storage_quota>> title_storage_service::get_quota(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType
)
{
    uint64_t xuid{ 0 };
    XblContextGetXboxUserId(m_xblContext, &xuid);

    auto asyncWrapper = new AsyncWrapper<title_storage_quota>(
        [serviceConfigurationId, storageType, xuid](XAsyncBlock* async, title_storage_quota& result)
    {
        size_t usedBytes;
        size_t quotaBytes;
        auto hr = XblTitleStorageGetQuotaResult(async, &usedBytes, &quotaBytes);
        if (SUCCEEDED(hr))
        {
            result = title_storage_quota(serviceConfigurationId, storageType, xuid, static_cast<uint64_t>(usedBytes), static_cast<uint64_t>(quotaBytes));
        }
        return hr;
    });
    
    char scid[XBL_SCID_LENGTH];
    Utils::Utf8FromCharT(serviceConfigurationId.c_str(), scid, sizeof(scid));
    auto hr = XblTitleStorageGetQuotaAsync(
        m_xblContext, 
        scid, 
        static_cast<XblTitleStorageType>(storageType), 
        &asyncWrapper->async
    );
    return asyncWrapper->Task(hr);
}

inline pplx::task<xbox_live_result<title_storage_blob_metadata_result>> title_storage_service::get_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ string_t xboxUserId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
)
{
    auto asyncWrapper = new AsyncWrapper<title_storage_blob_metadata_result>(
        [](XAsyncBlock* async, title_storage_blob_metadata_result& result)
    {
        XblTitleStorageBlobMetadataResultHandle handle;
        auto hr = XblTitleStorageGetBlobMetadataResult(async, &handle);
        if (SUCCEEDED(hr))
        {
            // title_storage_blob_metadata_result owns the lifetime of the handle
            result = title_storage_blob_metadata_result(handle);
            XblTitleStorageBlobMetadataResultCloseHandle(handle);
        }
        return hr;
    });

    char scid[XBL_SCID_LENGTH] = { 0 };
    Utils::Utf8FromCharT(serviceConfigurationId.c_str(), scid, sizeof(scid));

    char blobPathUTF8[XBL_TITLE_STORAGE_BLOB_PATH_MAX_LENGTH] = { 0 };
    Utils::Utf8FromCharT(blobPath.c_str(), blobPathUTF8, sizeof(blobPathUTF8));

    auto hr = XblTitleStorageGetBlobMetadataAsync(
        m_xblContext, 
        scid, 
        static_cast<XblTitleStorageType>(storageType), 
        blobPathUTF8, 
        Utils::Uint64FromStringT(xboxUserId), 
        skipItems, 
        maxItems, 
        &asyncWrapper->async
    );
    return asyncWrapper->Task(hr);
}

inline pplx::task<xbox_live_result<void>> title_storage_service::delete_blob(
    _In_ const title_storage_blob_metadata& blobMetadata,
    _In_ bool deleteOnlyIfEtagMatches
)
{
    auto asyncWrapper = new AsyncWrapper<void>();
    auto hr = XblTitleStorageDeleteBlobAsync(
        m_xblContext,
        blobMetadata.m_blobMetadata,
        deleteOnlyIfEtagMatches,
        &asyncWrapper->async
    );
    return asyncWrapper->Task(hr);
}

inline pplx::task<xbox_live_result<title_storage_blob_result>> title_storage_service::download_blob(
    _In_ title_storage_blob_metadata blobMetadata,
    _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
    _In_ title_storage_e_tag_match_condition etagMatchCondition,
    _In_ string_t selectQuery
)
{
    return download_blob(
        blobMetadata,
        blobBuffer,
        etagMatchCondition,
        selectQuery,
        XBL_TITLE_STORAGE_DEFAULT_DOWNLOAD_BLOCK_SIZE
    );
}

inline pplx::task<xbox_live_result<title_storage_blob_result>> title_storage_service::download_blob(
    _In_ title_storage_blob_metadata blobMetadata,
    _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
    _In_ title_storage_e_tag_match_condition etagMatchCondition,
    _In_ string_t selectQuery,
    _In_ uint32_t preferredDownloadBlockSize
)
{
    auto asyncWrapper = new AsyncWrapper<title_storage_blob_result>(
        [blobBuffer](XAsyncBlock* async, title_storage_blob_result& result)
    {
        XblTitleStorageBlobMetadata blobMetadata;
        auto hr = XblTitleStorageDownloadBlobResult(async, &blobMetadata);
        if (SUCCEEDED(hr))
        {
            result = title_storage_blob_result(blobBuffer, blobMetadata);
        }
        return hr;
    });


    auto hr = XblTitleStorageDownloadBlobAsync(
        m_xblContext,
        blobMetadata.m_blobMetadata,
        static_cast<uint8_t*>(blobBuffer->data()),
        blobBuffer->size(),
        static_cast<XblTitleStorageETagMatchCondition>(etagMatchCondition),
        Utils::StringFromStringT(selectQuery).c_str(),
        static_cast<size_t>(preferredDownloadBlockSize),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

inline pplx::task<xbox_live_result<title_storage_blob_metadata>> title_storage_service::upload_blob(
    _In_ title_storage_blob_metadata blobMetadata,
    _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
    _In_ title_storage_e_tag_match_condition etagMatchCondition,
    _In_ uint32_t preferredUploadBlockSize
)
{
    auto asyncWrapper = new AsyncWrapper<title_storage_blob_metadata>(
        [](XAsyncBlock* async, title_storage_blob_metadata& result)
    {
        XblTitleStorageBlobMetadata blobMetadata;
        auto hr = XblTitleStorageUploadBlobResult(async, &blobMetadata);
        if (SUCCEEDED(hr))
        {
            result = title_storage_blob_metadata(blobMetadata);
        }
        return hr;
    });


    auto hr = XblTitleStorageUploadBlobAsync(
        m_xblContext,
        blobMetadata.m_blobMetadata,
        static_cast<uint8_t*>(blobBuffer->data()),
        blobBuffer->size(),
        static_cast<XblTitleStorageETagMatchCondition>(etagMatchCondition),
        static_cast<size_t>(preferredUploadBlockSize),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END