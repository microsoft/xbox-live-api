// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/title_storage_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

class TitleStorageService;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END

struct XblTitleStorageQuota
{
    size_t usedBytes;
    size_t quotaBytes;
};

struct XblTitleStorageBlobMetadataResult : public xbox::services::RefCounter, public std::enable_shared_from_this<XblTitleStorageBlobMetadataResult>
{
public:
    XblTitleStorageBlobMetadataResult() = default;

    void Initialize(_In_ std::shared_ptr<xbox::services::title_storage::TitleStorageService> titleStorageService, _In_ xsapi_internal_string scid, _In_ uint64_t xuid, _In_ XblTitleStorageType storageType, _In_ xsapi_internal_string blobPath);

    const xsapi_internal_vector<XblTitleStorageBlobMetadata>& Items() const;

    bool HasNext() const;

    HRESULT GetNext(_In_ uint32_t maxItems, _In_ xbox::services::AsyncContext<xbox::services::Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>>> async);

    static xbox::services::Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>> Deserialize(_In_ const JsonValue& json);
    static xbox::services::Result<XblTitleStorageBlobMetadata> DeserializeXblTitleStorageBlobMetadata(_In_ const JsonValue& json);

    static XblTitleStorageBlobType ConvertStringToTitleStorageBlobType(_In_ const xsapi_internal_string& value);

protected:
    // RefCounter
    std::shared_ptr<xbox::services::RefCounter> GetSharedThis() override;

private:
    std::shared_ptr<xbox::services::title_storage::TitleStorageService> m_titleStorageService;
    xsapi_internal_string m_scid;
    uint64_t m_xuid{};
    XblTitleStorageType m_storageType{};
    xsapi_internal_string m_blobPath;

    xsapi_internal_vector<XblTitleStorageBlobMetadata> m_items;
    xsapi_internal_string m_continuationToken;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

class TitleStorageService : public std::enable_shared_from_this<TitleStorageService>
{
public:
    TitleStorageService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings
    );

    HRESULT GetQuota(
        _In_ xsapi_internal_string scid,
        _In_ XblTitleStorageType storageType,
        _In_ AsyncContext<Result<XblTitleStorageQuota>> async
    );

    HRESULT GetBlobMetadata(
        _In_ xsapi_internal_string scid,
        _In_ XblTitleStorageType storageType,
        _In_ xsapi_internal_string blobPath,
        _In_ uint64_t xboxUserId,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ xsapi_internal_string continuationToken,
        _In_ AsyncContext<Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>>> async
    );

    HRESULT DeleteBlob(
        _In_ XblTitleStorageBlobMetadata blobMetadata,
        _In_ bool deleteOnlyIfEtagMatches,
        _In_ AsyncContext<HRESULT> async
    );

    HRESULT DownloadBlob(
        _In_ XblTitleStorageBlobMetadata blobMetadata,
        _In_ uint8_t* blobBuffer,
        _In_ size_t blobBufferSize,
        _In_ XblTitleStorageETagMatchCondition etagMatchCondition,
        _In_ xsapi_internal_string selectQuery,
        _In_ size_t preferredDownloadBlockSize,
        _In_ AsyncContext<Result<XblTitleStorageBlobMetadata>> async
    );

    HRESULT UploadBlob(
        _In_ XblTitleStorageBlobMetadata blobMetadata,
        _In_ const uint8_t* blobBuffer,
        _In_ size_t blobBufferSize,
        _In_ XblTitleStorageETagMatchCondition etagMatchCondition,
        _In_ size_t preferredUploadBlockSize,
        _In_ AsyncContext<Result<XblTitleStorageBlobMetadata>> async
    );
private:

    struct BlobArgs
    {
        XblTitleStorageBlobMetadata blobMetadata{};
        uint8_t* downloadBlobBuffer{ nullptr };
        const uint8_t* uploadBlobBuffer{ nullptr };
        size_t blobBufferSize{ 0 };
        XblTitleStorageETagMatchCondition etagMatchCondition{};
        xsapi_internal_string selectQuery;
        size_t preferredBlockSize{ 0 };
        size_t startByte{ 0 };
        AsyncContext<Result<XblTitleStorageBlobMetadata>> async;
    };

    HRESULT DownloadBlobHelper(
        _In_ std::shared_ptr<BlobArgs> downloadBlobArgs
    );

    HRESULT UploadBlobHelper(
        _In_ std::shared_ptr<BlobArgs> uploadBlobArgs,
        _In_ const xsapi_internal_string& continuationToken
    );

    static Result<xsapi_internal_string> TitleStorageQuotaSubpath(
        _In_ XblTitleStorageType storageType,
        _In_ const xsapi_internal_string& serviceConfigurationId,
        _In_ uint64_t xboxUserId
    );

    static Result<xsapi_internal_string> TitleStorageBlobMetadataSubpath(
            _In_ XblTitleStorageType storageType,
            _In_ const xsapi_internal_string& serviceConfigurationId,
            _In_ uint64_t xboxUserId,
            _In_ const xsapi_internal_string& blobPath,
            _In_ uint32_t skipItems,
            _In_ uint32_t maxItems,
            _In_ const xsapi_internal_string& continuationToken
    );

    static Result<xsapi_internal_string> TitleStorageDownloadBlobSubpath(
        _In_ const XblTitleStorageBlobMetadata& blobMetadata,
        _In_ const xsapi_internal_string& selectQuery
    );

    static Result<xsapi_internal_string> TitleStorageUploadBlobSubpath(
        _In_ const XblTitleStorageBlobMetadata& blobMetadata,
        _In_ const xsapi_internal_string& continuationToken,
        _In_ bool finalBlock
    );

    HRESULT SetEtagHeader(
        _In_ std::shared_ptr<XblHttpCall> httpCall,
        _In_ xsapi_internal_string etag,
        _In_ XblTitleStorageETagMatchCondition eTagMatchCondition
    );

    HRESULT SetRangeHeader(
        _In_ std::shared_ptr<XblHttpCall> httpCall,
        _In_ size_t startByte,
        _In_ size_t endByte
    );

    // Deserialize Helpers
    static Result<XblTitleStorageQuota> DeserializeTitleStorageQuota(
        _In_ const JsonValue& json
    );

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END
