// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "title_storage_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

const char CONTENT_TYPE_HEADER_VALUE[] = "application/octet-stream";
const char E_TAG_HEADER_NAME[] = "ETag";
const char IF_MATCH_HEADER_NAME[] = "If-Match";
const char IF_NONE_HEADER_NAME[] = "If-None-Match";
const char E_TAG_INVALID_VALUE[] = "InvalidETagValue";
const char RANGE_HEADER_NAME[] = "Range";
 
TitleStorageService::TitleStorageService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings
) :
    m_user{ std::move(user) },
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings))
{
}

HRESULT 
TitleStorageService::GetQuota(
    _In_ xsapi_internal_string scid,
    _In_ XblTitleStorageType storageType,
    _In_ AsyncContext<Result<XblTitleStorageQuota>> async
)
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(scid);

    auto subpath = TitleStorageQuotaSubpath(storageType, scid, m_user.Xuid());
    RETURN_HR_INVALIDARGUMENT_IF(!Succeeded(subpath));

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("titlestorage", subpath.Payload()),
        xbox_live_api::get_quota
    );

    RETURN_HR_IF_FAILED(hr);

    hr = httpCall->Perform(
        AsyncContext<HttpResult>{
            async.Queue(),
            [async](HttpResult httpResult)
        {
            HRESULT hr = httpResult.Hresult();
            if (SUCCEEDED(hr))
            {
                hr = httpResult.Payload()->Result();
                if (SUCCEEDED(hr))
                {
                    async.Complete(DeserializeTitleStorageQuota(httpResult.Payload()->GetResponseBodyJson()));
                }
            }

            if (!SUCCEEDED(hr))
            {
                async.Complete(hr);
            }
        }});

    return hr;
}

Result<XblTitleStorageQuota>
TitleStorageService::DeserializeTitleStorageQuota(
    _In_ const JsonValue& json
    )
{
    XblTitleStorageQuota returnObject {};
    if (json.IsNull())
    {
        return returnObject;
    }

    HRESULT errc = S_OK;

    if (json.IsObject() && json.HasMember("quotaInfo"))
    {
        const JsonValue& quotaInfoJson = json["quotaInfo"];
        if (!quotaInfoJson.IsNull())
        {
            uint64_t usedBytes = 0;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(quotaInfoJson, "usedBytes", usedBytes));
            returnObject.usedBytes = (size_t)usedBytes;

            uint64_t quotaBytes = 0;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonUInt64(quotaInfoJson, "quotaBytes", quotaBytes));
            returnObject.quotaBytes = (size_t)quotaBytes;
        }
    }

    if (FAILED(errc))
    {
        return E_FAIL;
    }
    
    return returnObject;
}

HRESULT 
TitleStorageService::GetBlobMetadata(
    _In_ xsapi_internal_string scid,
    _In_ XblTitleStorageType storageType,
    _In_ xsapi_internal_string blobPath,
    _In_ uint64_t xboxUserId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ xsapi_internal_string continuationToken,
    _In_ AsyncContext<Result<std::shared_ptr<XblTitleStorageBlobMetadataResult>>> async
)
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(scid);

    uint64_t xuid = xboxUserId;
    if (xuid == 0 && (storageType == XblTitleStorageType::TrustedPlatformStorage || storageType == XblTitleStorageType::Universal))
    {
        xuid = m_user.Xuid();
    }

    auto subpath = TitleStorageBlobMetadataSubpath(
        storageType,
        scid,
        xuid,
        blobPath,
        skipItems,
        maxItems,
        continuationToken
    );

    RETURN_HR_INVALIDARGUMENT_IF(!Succeeded(subpath));

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("titlestorage", subpath.Payload()),
        xbox_live_api::get_blob_metadata
    );

    RETURN_HR_IF_FAILED(hr);

    hr = httpCall->Perform(
        AsyncContext<HttpResult>{
        async.Queue(),
            [
                sharedThis{ shared_from_this() }, 
                scid, 
                xuid, 
                storageType, 
                blobPath, 
                async
            ]
        (HttpResult httpResult)
        {
            HRESULT hr = httpResult.Hresult();
            if (SUCCEEDED(hr))
            {
                hr = httpResult.Payload()->Result();
                if (SUCCEEDED(hr))
                {
                    auto result = XblTitleStorageBlobMetadataResult::Deserialize(httpResult.Payload()->GetResponseBodyJson());
                    if (Succeeded(result))
                    {
                        result.Payload()->Initialize(sharedThis, scid, xuid, storageType, blobPath);
                    }
                    async.Complete(result);
                }
            }

            if (!SUCCEEDED(hr))
            {
                async.Complete(hr);
            }
        }});

    return hr;
}

HRESULT
TitleStorageService::DeleteBlob(
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _In_ bool deleteOnlyIfEtagMatches,
    _In_ AsyncContext<HRESULT> async
    )
{
    Result<xsapi_internal_string> subpath = TitleStorageDownloadBlobSubpath(blobMetadata, "");

    RETURN_HR_INVALIDARGUMENT_IF(!Succeeded(subpath));

    XblTitleStorageETagMatchCondition etagMatchCondition = deleteOnlyIfEtagMatches ?
        XblTitleStorageETagMatchCondition::IfMatch :
        XblTitleStorageETagMatchCondition::NotUsed;

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "DELETE",
        XblHttpCall::BuildUrl("titlestorage", subpath.Payload()),
        xbox_live_api::delete_blob
    );

    RETURN_HR_IF_FAILED(hr);
    RETURN_HR_IF_FAILED(httpCall->SetHeader(CONTENT_TYPE_HEADER, CONTENT_TYPE_HEADER_VALUE));

    hr = SetEtagHeader(
        httpCall,
        blobMetadata.eTag,
        etagMatchCondition
        );

    RETURN_HR_IF_FAILED(hr);

    hr = httpCall->Perform(
        AsyncContext<HttpResult>{
        async.Queue(),
            [async](HttpResult httpResult)
        {
            HRESULT hr = httpResult.Hresult();
            if (SUCCEEDED(hr))
            {
                hr = httpResult.Payload()->Result();
            }

            async.Complete(hr);
        }});

    return hr;
}

HRESULT
TitleStorageService::DownloadBlob(
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _In_ uint8_t* blobBuffer,
    _In_ size_t blobBufferSize,
    _In_ XblTitleStorageETagMatchCondition etagMatchCondition,
    _In_ xsapi_internal_string selectQuery,
    _In_ size_t preferredDownloadBlockSize,
    _In_ AsyncContext<Result<XblTitleStorageBlobMetadata>> async
    )
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(blobBuffer);
    RETURN_HR_INVALIDARGUMENT_IF(blobBufferSize < blobMetadata.length);

    Result<xsapi_internal_string> subpath = TitleStorageDownloadBlobSubpath(blobMetadata, selectQuery);

    RETURN_HR_INVALIDARGUMENT_IF(!Succeeded(subpath));

    if (preferredDownloadBlockSize == 0)
    {
        preferredDownloadBlockSize = XBL_TITLE_STORAGE_DEFAULT_DOWNLOAD_BLOCK_SIZE;
    }
    else
    {
        preferredDownloadBlockSize = preferredDownloadBlockSize < XBL_TITLE_STORAGE_MIN_DOWNLOAD_BLOCK_SIZE ? XBL_TITLE_STORAGE_MIN_DOWNLOAD_BLOCK_SIZE : preferredDownloadBlockSize;
    }

    auto args = MakeShared<BlobArgs>();
    args->blobMetadata = { blobMetadata };
    args->downloadBlobBuffer = blobBuffer;
    args->blobBufferSize = blobBufferSize;
    args->etagMatchCondition = etagMatchCondition;
    args->selectQuery = selectQuery;
    args->preferredBlockSize = preferredDownloadBlockSize;
    args->startByte = 0;
    args->async = std::move(async);

    return DownloadBlobHelper(args);
}

HRESULT TitleStorageService::DownloadBlobHelper(
    _In_ std::shared_ptr<BlobArgs> args
    )
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(args->downloadBlobBuffer);
    RETURN_HR_INVALIDARGUMENT_IF(args->blobBufferSize < args->blobMetadata.length);

    Result<xsapi_internal_string> subpath = TitleStorageDownloadBlobSubpath(args->blobMetadata, args->selectQuery);

    RETURN_HR_INVALIDARGUMENT_IF(!Succeeded(subpath));

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "GET",
        XblHttpCall::BuildUrl("titlestorage", subpath.Payload()),
        xbox_live_api::download_blob
    );
    RETURN_HR_IF_FAILED(hr);
    
    RETURN_HR_IF_FAILED(httpCall->SetHeader(CONTENT_TYPE_HEADER, CONTENT_TYPE_HEADER_VALUE));
    httpCall->SetLongHttpCall(true);

    hr = SetEtagHeader(
        httpCall,
        args->blobMetadata.eTag,
        args->etagMatchCondition
    );
    RETURN_HR_IF_FAILED(hr);

    if (args->blobMetadata.blobType == XblTitleStorageBlobType::Binary)
    {
        // Partial download is only used for binary blob types
        hr = SetRangeHeader(
            httpCall,
            args->startByte,
            args->startByte + args->preferredBlockSize - 1
        );
        RETURN_HR_IF_FAILED(hr);
    }

    return httpCall->Perform(
        AsyncContext<HttpResult>{
            args->async.Queue(),
            [
                args,
                sharedThis{ shared_from_this() }
            ](HttpResult httpResult)
            {
                HRESULT hr = httpResult.Hresult();
                if (SUCCEEDED(hr))
                {
                    hr = httpResult.Payload()->Result();

                    if (SUCCEEDED(hr))
                    {
                        auto responseBody = httpResult.Payload()->GetResponseBodyBytes();
                        if (args->startByte + responseBody.size() > args->blobBufferSize)
                        {
                            args->async.Complete(E_NOT_SUFFICIENT_BUFFER);
                            return;
                        }

                        memcpy(args->downloadBlobBuffer + args->startByte, responseBody.data(), responseBody.size());
                        
                        args->startByte += responseBody.size();

                        // Check if there is more data to load
                        // If not binary blob type then the service has returned the entire payload.
                        // If binary blob type then check if the service returned less data than requested or
                        // if we've loaded all the data defined by the blob metadata length property.
                        if (args->blobMetadata.blobType != XblTitleStorageBlobType::Binary ||
                            responseBody.size() < args->preferredBlockSize ||
                            args->startByte == args->blobMetadata.length)
                        {
                            auto etag = httpResult.Payload()->GetResponseHeader(ETAG_HEADER);
                            utils::strcpy(args->blobMetadata.eTag, etag.length() + 1, etag.c_str());
                            args->blobMetadata.length = args->startByte;
                            args->async.Complete(std::move(args->blobMetadata));
                        }
                        else
                        {
                            hr = sharedThis->DownloadBlobHelper(args);
                        }
                    }
                }

                if (!SUCCEEDED(hr))
                {
                    args->async.Complete(hr);
                }
            }});
}

HRESULT
TitleStorageService::UploadBlob(
    _In_ XblTitleStorageBlobMetadata blobMetadata,
    _In_ const uint8_t* blobBuffer,
    _In_ size_t blobBufferSize,
    _In_ XblTitleStorageETagMatchCondition etagMatchCondition,
    _In_ size_t preferredUploadBlockSize,
    _In_ AsyncContext<Result<XblTitleStorageBlobMetadata>> async
    )
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(blobBuffer);
    RETURN_HR_INVALIDARGUMENT_IF(blobBufferSize == 0);

    if (preferredUploadBlockSize == 0)
    {
        preferredUploadBlockSize = XBL_TITLE_STORAGE_DEFAULT_UPLOAD_BLOCK_SIZE;
    }
    else
    {
        preferredUploadBlockSize = preferredUploadBlockSize < XBL_TITLE_STORAGE_MIN_UPLOAD_BLOCK_SIZE ? XBL_TITLE_STORAGE_MIN_UPLOAD_BLOCK_SIZE : preferredUploadBlockSize;
        preferredUploadBlockSize = preferredUploadBlockSize > XBL_TITLE_STORAGE_MAX_UPLOAD_BLOCK_SIZE ? XBL_TITLE_STORAGE_MAX_UPLOAD_BLOCK_SIZE : preferredUploadBlockSize;
    }

    auto args = MakeShared<BlobArgs>();
    args->blobMetadata = { blobMetadata };
    args->uploadBlobBuffer = blobBuffer;
    args->blobBufferSize = blobBufferSize;
    args->etagMatchCondition = etagMatchCondition;
    args->preferredBlockSize = preferredUploadBlockSize;
    args->startByte = 0;
    args->async = async;

    if (args->blobMetadata.xboxUserId == 0)
    {
        args->blobMetadata.xboxUserId = m_user.Xuid();
    }

    return UploadBlobHelper(args, "");
}

HRESULT 
TitleStorageService::UploadBlobHelper(
    _In_ std::shared_ptr<BlobArgs> args,
    _In_ const xsapi_internal_string& continuationToken
    )
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(args->uploadBlobBuffer);
    RETURN_HR_INVALIDARGUMENT_IF(args->blobBufferSize == 0);

    bool isBinary = args->blobMetadata.blobType == XblTitleStorageBlobType::Binary;

    bool finalBlock = true;

    size_t dataChunkSize = isBinary ? args->blobBufferSize - args->startByte : args->blobBufferSize;
    if (isBinary)
    {
        if (dataChunkSize > args->preferredBlockSize)
        {
            dataChunkSize = args->preferredBlockSize;
        }

        size_t endByteOfChunk = args->startByte + dataChunkSize;
        finalBlock = (endByteOfChunk == args->blobBufferSize);
    }

    Result<xsapi_internal_string> subpath = TitleStorageUploadBlobSubpath(args->blobMetadata, continuationToken, finalBlock);
    RETURN_HR_INVALIDARGUMENT_IF(!Succeeded(subpath));

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_xboxLiveContextSettings,
        "PUT",
        XblHttpCall::BuildUrl("titlestorage", subpath.Payload()),
        xbox_live_api::upload_blob
    );

    RETURN_HR_IF_FAILED(httpCall->SetHeader(CONTENT_TYPE_HEADER, CONTENT_TYPE_HEADER_VALUE));
    httpCall->SetLongHttpCall(true);

    if (isBinary)
    {
        xsapi_internal_vector<uint8_t> dataChunk(dataChunkSize);
        memcpy(dataChunk.data(), args->uploadBlobBuffer + args->startByte, dataChunkSize);
        RETURN_HR_IF_FAILED(httpCall->SetRequestBody(std::move(dataChunk)));
        
        args->startByte += dataChunkSize; // Now move the start byte forward
    }
    else 
    {
        RETURN_HR_IF_FAILED(httpCall->SetRequestBody((const char*)std::move(args->uploadBlobBuffer)));
    }

    hr = SetEtagHeader(
        httpCall,
        args->blobMetadata.eTag,
        args->etagMatchCondition
    );
    RETURN_HR_IF_FAILED(hr);
    
    return httpCall->Perform(
        AsyncContext<HttpResult>{
        args->async.Queue(),
            [
                args,
                finalBlock,
                sharedThis{ shared_from_this() }
            ](HttpResult httpResult)
        {
            HRESULT hr = httpResult.Hresult();
            if (SUCCEEDED(hr))
            {
                hr = httpResult.Payload()->Result();

                if (SUCCEEDED(hr))
                {
                    auto etag = httpResult.Payload()->GetResponseHeader(ETAG_HEADER);
                    utils::strcpy(args->blobMetadata.eTag, etag.length() + 1, etag.c_str());

                    if (finalBlock)
                    {
                        args->async.Complete(std::move(args->blobMetadata));
                    }
                    else
                    {
                        auto responseBody = httpResult.Payload()->GetResponseBodyJson();
                        xsapi_internal_string continuationToken;
                        JsonUtils::ExtractJsonString(responseBody, "continuationToken", continuationToken);
                        hr = sharedThis->UploadBlobHelper(args, continuationToken);
                    }
                }
            }

            if (!SUCCEEDED(hr))
            {
                args->async.Complete(hr);
            }
        }});
}

HRESULT
TitleStorageService::SetEtagHeader(
    _In_ std::shared_ptr<XblHttpCall> httpCall,
    _In_ xsapi_internal_string etag,
    _In_ XblTitleStorageETagMatchCondition eTagMatchCondition
    )
{
    if (eTagMatchCondition == XblTitleStorageETagMatchCondition::NotUsed)
    {
        RETURN_HR_IF_FAILED(httpCall->SetHeader(E_TAG_HEADER_NAME, E_TAG_INVALID_VALUE));
        RETURN_HR_IF_FAILED(httpCall->SetHeader(IF_NONE_HEADER_NAME, E_TAG_INVALID_VALUE));
    }
    else
    {
        xsapi_internal_string etagValue = etag.empty() ?
            IF_NONE_HEADER_NAME :
            etag;

        RETURN_HR_IF_FAILED(httpCall->SetHeader(E_TAG_HEADER_NAME, etagValue));

        if (eTagMatchCondition == XblTitleStorageETagMatchCondition::IfMatch)
        {
            RETURN_HR_IF_FAILED(httpCall->SetHeader(IF_MATCH_HEADER_NAME, etagValue));
        }
        else
        {
            RETURN_HR_IF_FAILED(httpCall->SetHeader(IF_NONE_HEADER_NAME, etagValue));
        }
    }
    return S_OK;
}

HRESULT
TitleStorageService::SetRangeHeader(
    _In_ std::shared_ptr<XblHttpCall> httpCall,
    _In_ size_t startByte,
    _In_ size_t endByte
    )
{
    xsapi_internal_stringstream byteRange;
    byteRange << "bytes=";
    byteRange << startByte;
    byteRange << "-";
    byteRange << endByte;

    RETURN_HR_IF_FAILED(httpCall->SetHeader(RANGE_HEADER_NAME, byteRange.str()));
    return S_OK;
}

Result<xsapi_internal_string>
TitleStorageService::TitleStorageQuotaSubpath(
    _In_ XblTitleStorageType storageType,
    _In_ const xsapi_internal_string& serviceConfigurationId,
    _In_ uint64_t xboxUserId
)
{
    xsapi_internal_stringstream path;
    switch (storageType)
    {
        case XblTitleStorageType::TrustedPlatformStorage:
            path << "/trustedplatform/users/xuid(";
            path << xboxUserId;
            path << ")/scids/";
            path << serviceConfigurationId;
            break;
            
        case XblTitleStorageType::GlobalStorage:
            path << "/global/scids/";
            path << serviceConfigurationId;
            break;

        case XblTitleStorageType::Universal:
            path << "/universalplatform/users/xuid(";
            path << xboxUserId;
            path << ")/scids/";
            path << serviceConfigurationId;
            break;

        default:
            return Result<xsapi_internal_string>(E_INVALIDARG);
    }

    return path.str();
}

Result<xsapi_internal_string>
TitleStorageService::TitleStorageBlobMetadataSubpath(
    _In_ XblTitleStorageType storageType,
    _In_ const xsapi_internal_string& serviceConfigurationId,
    _In_ uint64_t xboxUserId,
    _In_ const xsapi_internal_string& blobPath,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const xsapi_internal_string& continuationToken
    )
{
    xsapi_internal_stringstream path;
    switch (storageType)
    {
        case XblTitleStorageType::TrustedPlatformStorage:
            path << "/trustedplatform/users/xuid(";
            path << xboxUserId;
            path << ")/scids/";
            path << serviceConfigurationId;
            break;
            
        case XblTitleStorageType::GlobalStorage:
            path << "/global/scids/";
            path << serviceConfigurationId; 
            break;
            
        case XblTitleStorageType::Universal:
            path << "/universalplatform/users/xuid(";
            path << xboxUserId;
            path << ")/scids/";
            path << serviceConfigurationId;
            break;

        default:
            return Result<xsapi_internal_string>(E_INVALIDARG);
    }

    path << "/data";

    if (!blobPath.empty())
    {
        path << "/";
        path << utils::encode_uri(blobPath, xbox::services::uri::components::query);
    }

    xbox::services::uri_builder params;
    utils::append_paging_info(
        params,
        skipItems,
        maxItems,
        continuationToken
        );

    xsapi_internal_string paramPath = params.query();
    if (paramPath.size() > 1)
    {
        path << "?" << paramPath;
    }

    return path.str();
}

Result<xsapi_internal_string>
TitleStorageService::TitleStorageDownloadBlobSubpath(
    _In_ const XblTitleStorageBlobMetadata& blobMetadata,
    _In_ const xsapi_internal_string& selectQuery
    )
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(blobMetadata.serviceConfigurationId);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(blobMetadata.blobPath);

    xsapi_internal_stringstream path;

    xsapi_internal_string titleStorageBlobToString;
    switch (blobMetadata.blobType)
    {
    case XblTitleStorageBlobType::Binary: titleStorageBlobToString = "binary"; break;
    case XblTitleStorageBlobType::Json: titleStorageBlobToString = "json"; break;
    case XblTitleStorageBlobType::Config: titleStorageBlobToString = "config"; break;
    default: return Result<xsapi_internal_string>(E_INVALIDARG);
    }

    switch (blobMetadata.storageType)
    {
        case XblTitleStorageType::TrustedPlatformStorage:
            path << "/trustedplatform/users/xuid(";
            path << blobMetadata.xboxUserId;
            path << ")/scids/";
            break;
            
        case XblTitleStorageType::GlobalStorage:
            path << "/global/scids/";
            break;
            
        case XblTitleStorageType::Universal:
            path << "/universalplatform/users/xuid(";
            path << blobMetadata.xboxUserId;
            path << ")/scids/";
            break;

        default:
            return Result<xsapi_internal_string>(E_INVALIDARG);
    }
    
    path << blobMetadata.serviceConfigurationId;
    path << "/data/";
    path << blobMetadata.blobPath;
    path << ",";
    path << titleStorageBlobToString;

    xsapi_internal_vector<xsapi_internal_string> params;
    if (!selectQuery.empty())
    {
        if (blobMetadata.blobType == XblTitleStorageBlobType::Config)
        {
            xsapi_internal_stringstream param;
            param << "customSelector=";
            param << utils::encode_uri(selectQuery.c_str());
            params.push_back(param.str());
        }
        else if (blobMetadata.blobType == XblTitleStorageBlobType::Json)
        {
            xsapi_internal_stringstream param;
            param << "select=";
            param << utils::encode_uri(selectQuery.c_str());
            params.push_back(param.str());
        }
    }

    path << utils::get_query_from_params(params);

    return Result<xsapi_internal_string>(path.str());
}

Result<xsapi_internal_string>
TitleStorageService::TitleStorageUploadBlobSubpath(
    _In_ const XblTitleStorageBlobMetadata& blobMetadata,
    _In_ const xsapi_internal_string& continuationToken,
    _In_ bool finalBlock
    )
{
    xsapi_internal_stringstream source;
    Result<xsapi_internal_string> titleStorageDlSubpath = TitleStorageDownloadBlobSubpath(
        blobMetadata,
        ""
        );

    if (!Succeeded(titleStorageDlSubpath)) return titleStorageDlSubpath;

    source << titleStorageDlSubpath.Payload();

    xsapi_internal_vector<xsapi_internal_string> params;

    if (blobMetadata.clientTimestamp != 0)
    {
        // Format: Tue, 13 Aug 2019 09:01:23 GMT
        char clientTimeStamp[37]{};

#if HC_PLATFORM_IS_MICROSOFT
        struct tm timeinfo{};
        if (gmtime_s(&timeinfo, &blobMetadata.clientTimestamp) == 0)
        {
            strftime(clientTimeStamp, 32, "%a, %d %b %Y %H:%M:%S GMT", &timeinfo);
        }
#else
        struct tm* timeinfo{ nullptr };
        timeinfo = gmtime(&blobMetadata.clientTimestamp);
        strftime(clientTimeStamp, 32, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
#endif

        xsapi_internal_stringstream param;
        param << "clientFileTime=";
        param << utils::encode_uri(clientTimeStamp);
        params.push_back(param.str());
    }

    if (strlen(blobMetadata.displayName) > 0)
    {
        xsapi_internal_stringstream param;
        param << "displayName=";
        param << utils::encode_uri(blobMetadata.displayName);
        params.push_back(param.str());
    }

    if (!continuationToken.empty())
    {
        xsapi_internal_stringstream param;
        param << "continuationToken=";
        param << utils::encode_uri(continuationToken);
        params.push_back(param.str());
    }

    if (blobMetadata.blobType == XblTitleStorageBlobType::Binary)
    {
        xsapi_internal_string param = finalBlock ? "finalBlock=true" : "finalBlock=false";
        params.push_back(param);
    }

    source << utils::get_query_from_params(params);

    return Result<xsapi_internal_string>(source.str());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END
