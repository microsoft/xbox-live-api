// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "title_storage_internal.h"

using namespace xbox::services;
using namespace xbox::services::title_storage;

void XblTitleStorageBlobMetadataResult::Initialize(
    _In_ std::shared_ptr<TitleStorageService> titleStorageService,
    _In_ xsapi_internal_string scid,
    _In_ uint64_t xuid,
    _In_ XblTitleStorageType storageType,
    _In_ xsapi_internal_string blobPath
)
{
    m_titleStorageService = titleStorageService;
    m_scid = scid;
    m_xuid = xuid;
    m_storageType = storageType;
    m_blobPath = blobPath;

    for (auto& item : m_items)
    {
        utils::strcpy(item.serviceConfigurationId, XBL_SCID_LENGTH, m_scid.c_str());
        item.xboxUserId = m_xuid;
        item.storageType = m_storageType;
    }
}

const xsapi_internal_vector<XblTitleStorageBlobMetadata>&
XblTitleStorageBlobMetadataResult::Items() const
{
    return m_items;
}

bool XblTitleStorageBlobMetadataResult::HasNext() const
{
    return !m_continuationToken.empty();
}

HRESULT XblTitleStorageBlobMetadataResult::GetNext(
    _In_ uint32_t maxItems, 
    _In_ AsyncContext<Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>>> async
)
{
    return m_titleStorageService->GetBlobMetadata(
        m_scid,
        m_storageType,
        m_blobPath,
        m_xuid,
        0, // use continuationToken, ignore skipItems.
        maxItems,
        m_continuationToken,
        async
    );
}

std::shared_ptr<xbox::services::RefCounter>
XblTitleStorageBlobMetadataResult::GetSharedThis()
{
    return shared_from_this();
}

Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>> 
XblTitleStorageBlobMetadataResult::Deserialize(_In_ const JsonValue& json)
{
    if (json.IsNull())
    {
        return Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>>(nullptr);
    }

    auto titleStorageBlobMetadataResult = MakeShared<XblTitleStorageBlobMetadataResult>();

    if (json.IsObject() && json.HasMember("blobs"))
    {
        const JsonValue& blobs = json["blobs"];
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVectorInPlace<XblTitleStorageBlobMetadata>(
            DeserializeXblTitleStorageBlobMetadata,
            blobs,
            titleStorageBlobMetadataResult->m_items
            ));
    }
    else
    {
        //required
        titleStorageBlobMetadataResult->m_items = xsapi_internal_vector<XblTitleStorageBlobMetadata>();
        return WEB_E_INVALID_JSON_STRING;
    }


    if (json.IsObject() && json.HasMember("pagingInfo"))
    {
        const JsonValue& pagingInfoJson = json["pagingInfo"];
        if (!pagingInfoJson.IsNull())
        {
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(
                pagingInfoJson,
                "continuationToken",
                titleStorageBlobMetadataResult->m_continuationToken
            ));
        }
    }

    return Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>>(titleStorageBlobMetadataResult, S_OK);
}

XblTitleStorageBlobType
XblTitleStorageBlobMetadataResult::ConvertStringToTitleStorageBlobType(
    _In_ const xsapi_internal_string& value
)
{
    if (utils::str_icmp_internal(value, "binary") == 0)
    {
        return XblTitleStorageBlobType::Binary;
    }
    else if (utils::str_icmp_internal(value, "json") == 0)
    {
        return XblTitleStorageBlobType::Json;
    }
    else if (utils::str_icmp_internal(value, "config") == 0)
    {
        return XblTitleStorageBlobType::Config;
    }

    return XblTitleStorageBlobType::Unknown;
}

HRESULT
XblTitleStorageBlobMetadataResult::DeserializeXblTitleStorageBlobMetadata(
    _In_ const JsonValue& json,
    _Out_ XblTitleStorageBlobMetadata& metadata
)
{
    // Filling the caller's storage means a partial write would survive an early exit. memset rather
    // than 'metadata = XblTitleStorageBlobMetadata{}' is deliberate: the latter can materialize a
    // ~1.2KB temporary and defeat the point of the out parameter.
    memset(&metadata, 0, sizeof(metadata));

    if (json.IsNull())
    {
        return S_OK;
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTimeT(json, "clientFileTime", metadata.clientTimestamp));

    xsapi_internal_string displayName;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "displayName", displayName));
    utils::strcpy(metadata.displayName, sizeof(metadata.displayName), displayName.c_str());

    xsapi_internal_string etag;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "etag", etag));
    if (etag.size() >= sizeof(metadata.eTag))
    {
        return WEB_E_INVALID_JSON_STRING;
    }
    utils::strcpy(metadata.eTag, sizeof(metadata.eTag), etag.c_str());

    uint64_t size = 0;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(json, "size", size));
    metadata.length = static_cast<size_t>(size);

    xsapi_internal_string fileName;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "fileName", fileName));
    if (!fileName.empty())
    {
        auto nPos = fileName.find(',');
        if (nPos == std::string::npos)
        {
            return WEB_E_INVALID_JSON_STRING;
        }

        xsapi_internal_string smartBlobType = fileName.substr(nPos + 1);
        metadata.blobType = ConvertStringToTitleStorageBlobType(smartBlobType);
        fileName.resize(nPos);
        if (fileName.size() >= sizeof(metadata.blobPath))
        {
            return WEB_E_INVALID_JSON_STRING;
        }
        utils::strcpy(metadata.blobPath, sizeof(metadata.blobPath), fileName.c_str());
    }

    return S_OK;
}