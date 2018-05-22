// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/title_storage.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "utils.h"

using namespace xbox::services::system;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

static const string_t CONTENT_TYPE_HEADER_VALUE =_T("application/octet-stream");
static const string_t E_TAG_HEADER_NAME = _T("ETag");
static const string_t IF_MATCH_HEADER_NAME = _T("If-Match");
static const string_t IF_NONE_HEADER_NAME = _T("If-None-Match");
static const string_t E_TAG_INVALID_VALUE = _T("InvalidETagValue");
static const string_t RANGE_HEADER_NAME = _T("Range");
 
const uint32_t title_storage_service::MIN_UPLOAD_BLOCK_SIZE = 1024;
const uint32_t title_storage_service::MAX_UPLOAD_BLOCK_SIZE = 4 * 1024 * 1024;
const uint32_t title_storage_service::DEFAULT_UPLOAD_BLOCK_SIZE = 256 * 1024;
const uint32_t title_storage_service::MIN_DOWNLOAD_BLOCK_SIZE = 1024;
const uint32_t title_storage_service::DEFAULT_DOWNLOAD_BLOCK_SIZE = 1024 * 1024;

title_storage_service::title_storage_service(
    _In_ std::shared_ptr<user_context> userContext,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<title_storage_quota>>
title_storage_service::get_quota(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, title_storage_quota, "Service configuration id is empty");
    string_t xboxUserId = utils::string_t_from_internal_string(m_userContext->xbox_user_id());
    xbox_live_result<string_t> subpathAndQueryResult = title_storage_quota_subpath(
        storageType,
        serviceConfigurationId,
        xboxUserId,
        string_t(),
        string_t()
        );
    
    RETURN_TASK_CPP_IF_ERR(subpathAndQueryResult, title_storage_quota);

    string_t subpathAndQuery = subpathAndQueryResult.payload();
    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("titlestorage"), m_appConfig),
        subpathAndQuery,
        xbox_live_api::get_quota
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([httpCall, serviceConfigurationId, storageType, xboxUserId](std::shared_ptr<http_call_response> response)
    {
        auto result = title_storage_quota::_Deserialize(
            response->response_body_json()
            );

        auto& titleStorageQuota = result.payload();
        titleStorageQuota._Initialize(
            serviceConfigurationId,
            storageType,
            xboxUserId,
            string_t(),
            string_t()
            );

        return utils::generate_xbox_live_result<title_storage_quota>(
            result, 
            response
            );
    });

    return utils::create_exception_free_task<title_storage_quota>(
        task
        );
}

pplx::task<xbox_live_result<title_storage_quota>>
title_storage_service::get_quota_for_session_storage(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& multiplayerSessionTemplateName,
    _In_ const string_t& multiplayerSessionName
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, title_storage_quota, "Service configuration id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(multiplayerSessionTemplateName, title_storage_quota, "Multiplayer session template name is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(multiplayerSessionName, title_storage_quota, "Multiplayer session name is empty");

    xbox_live_result<string_t> subpathAndQueryResult = title_storage_quota_subpath(
        title_storage_type::session_storage,
        serviceConfigurationId,
        string_t(),
        multiplayerSessionTemplateName,
        multiplayerSessionName
        );

    RETURN_TASK_CPP_IF_ERR(subpathAndQueryResult, title_storage_quota);

    string_t subpathAndQuery = subpathAndQueryResult.payload();
    
    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("titlestorage"), m_appConfig),
        subpathAndQuery,
        xbox_live_api::get_quota_for_session_storage
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([serviceConfigurationId, multiplayerSessionTemplateName, multiplayerSessionName](std::shared_ptr<http_call_response> response)
    {
        auto result = title_storage_quota::_Deserialize(
            response->response_body_json()
            );

        auto& titleStorageQuota = result.payload();
        titleStorageQuota._Initialize(
            serviceConfigurationId,
            title_storage_type::session_storage,
            string_t(),
            multiplayerSessionTemplateName,
            multiplayerSessionName
            );

        return utils::generate_xbox_live_result<title_storage_quota>(
            result, 
            response
            );
    });

    return utils::create_exception_free_task<title_storage_quota>(
        task
        );
}


pplx::task<xbox_live_result<title_storage_blob_metadata_result>>
title_storage_service::get_blob_metadata(
    _In_ string_t serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ string_t blobPath,
    _In_ string_t xboxUserId,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, title_storage_blob_metadata_result, "Service configuration id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(storageType == title_storage_type::global_storage && !xboxUserId.empty(), title_storage_blob_metadata_result, "Global storage type with empty xbox user id");

    if ((storageType == title_storage_type::trusted_platform_storage || storageType == title_storage_type::json_storage) && xboxUserId.empty())
    {
        xboxUserId = utils::string_t_from_internal_string(m_userContext->xbox_user_id());
    }

    return internal_get_blob_metadata(
        std::move(serviceConfigurationId),
        storageType,
        std::move(blobPath),
        std::move(xboxUserId),
        string_t(),
        string_t(),
        skipItems,
        maxItems,
        string_t()
        );
}

pplx::task<xbox_live_result<title_storage_blob_metadata_result>>
title_storage_service::get_blob_metadata_for_session_storage(
    _In_ string_t serviceConfigurationId,
    _In_ string_t blobPath,
    _In_ string_t multiplayerSessionTemplateName,
    _In_ string_t multiplayerSessionName,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, title_storage_blob_metadata_result, "Service configuration id is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(multiplayerSessionTemplateName, title_storage_blob_metadata_result, "Multiplayer session template name is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(multiplayerSessionName, title_storage_blob_metadata_result, "Multiplayer session name is empty");

    return internal_get_blob_metadata(
        std::move(serviceConfigurationId),
        title_storage_type::session_storage,
        std::move(blobPath),
        string_t(),
        std::move(multiplayerSessionTemplateName),
        std::move(multiplayerSessionName),
        skipItems,
        maxItems,
        string_t()
        );
}

pplx::task<xbox_live_result<void>>
title_storage_service::delete_blob(
    _In_ const title_storage_blob_metadata& blobMetadata,
    _In_ bool deleteOnlyIfEtagMatches
    )
{
    title_storage_e_tag_match_condition eTagMatchCondition = deleteOnlyIfEtagMatches ?
        title_storage_e_tag_match_condition::if_match :
        title_storage_e_tag_match_condition::not_used;

    xbox_live_result<string_t> subpathAndQueryResult = title_storage_download_blob_subpath(
        blobMetadata,
        string_t()
        );

    RETURN_TASK_CPP_IF_ERR(subpathAndQueryResult, void);

    string_t subpathAndQuery = subpathAndQueryResult.payload();

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("DELETE"),
        utils::create_xboxlive_endpoint(_T("titlestorage"), m_appConfig),
        subpathAndQuery,
        xbox_live_api::delete_blob
        );

    httpCall->set_content_type_header_value(CONTENT_TYPE_HEADER_VALUE);

    set_e_tag_header(
        httpCall,
        blobMetadata.e_tag(),
        eTagMatchCondition
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        auto errorStatus = response->err_code();
        return xbox_live_result<void>(errorStatus, "Invalid HTTP received on delete");
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

pplx::task<xbox_live_result<title_storage_blob_metadata_result>>
title_storage_service::internal_get_blob_metadata(
    _In_ const string_t& serviceConfigurationId,
    _In_ title_storage_type storageType,
    _In_ const string_t& blobPath,
    _In_ const string_t& xboxUserId,
    _In_ const string_t& multiplayerSessionTemplateName,
    _In_ const string_t& multiplayerSessionName,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken
    )
{
    xbox_live_result<string_t> subpathAndQueryResult = title_storage_blob_metadata_subpath(
        storageType,
        serviceConfigurationId,
        xboxUserId,
        multiplayerSessionTemplateName,
        multiplayerSessionName,
        blobPath,
        skipItems,
        maxItems,
        continuationToken
        );

    RETURN_TASK_CPP_IF_ERR(subpathAndQueryResult, title_storage_blob_metadata_result);

    string_t subpathAndQuery = subpathAndQueryResult.payload();

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("titlestorage"), m_appConfig),
        subpathAndQuery,
        xbox_live_api::get_blob_metadata
        );

    auto userContextShared = m_userContext;
    auto xboxLiveContextSettingsShared = m_xboxLiveContextSettings;
    auto appConfigShared = m_appConfig;

    auto task =  httpCall->get_response_with_auth(m_userContext)
    .then([httpCall, 
        userContextShared, 
        appConfigShared, 
        xboxLiveContextSettingsShared, 
        xboxUserId, 
        serviceConfigurationId, 
        storageType, 
        multiplayerSessionTemplateName, 
        multiplayerSessionName, 
        blobPath]
        (std::shared_ptr<http_call_response> response)
    {
        auto result = title_storage_blob_metadata_result::_Deserialize(
            response->response_body_json()
            );

        auto& titleStorageBlobMetadataResult = result.payload();

        titleStorageBlobMetadataResult._Initialize(
            userContextShared,
            xboxLiveContextSettingsShared,
            appConfigShared,
            serviceConfigurationId,
            storageType,
            blobPath,
            xboxUserId,
            multiplayerSessionTemplateName,
            multiplayerSessionName
            );

        return utils::generate_xbox_live_result<title_storage_blob_metadata_result>(
            result,
            response
            );
    });

    return utils::create_exception_free_task<title_storage_blob_metadata_result>(
        task
        );
}

pplx::task<xbox_live_result<title_storage_blob_result>>
title_storage_service::download_blob(
    _In_ title_storage_blob_metadata blobMetadata,
    _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
    _In_ title_storage_e_tag_match_condition etagMatchCondition,
    _In_ string_t selectQuery
    )
{
    return download_blob(
        std::move(blobMetadata),
        blobBuffer,
        etagMatchCondition,
        std::move(selectQuery),
        DEFAULT_DOWNLOAD_BLOCK_SIZE
        );
}

pplx::task<xbox_live_result<title_storage_blob_result>>
title_storage_service::download_blob(
    _In_ title_storage_blob_metadata blobMetadata,
    _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
    _In_ title_storage_e_tag_match_condition etagMatchCondition,
    _In_ string_t selectQuery,
    _In_ uint32_t preferredDownloadBlockSize
    )
{
    if(blobBuffer == nullptr)
    {
        return pplx::task_from_result(xbox_live_result<title_storage_blob_result>(xbox_live_error_code::invalid_argument, "Null blobBuffer Argument")); 
    }
    preferredDownloadBlockSize = preferredDownloadBlockSize < MIN_DOWNLOAD_BLOCK_SIZE ? MIN_DOWNLOAD_BLOCK_SIZE : preferredDownloadBlockSize;

    auto sharedXboxLiveContextSettings = m_xboxLiveContextSettings;
    auto sharedUserContext = m_userContext;
    auto sharedAppConfig = m_appConfig;
    auto task = pplx::create_task([sharedXboxLiveContextSettings, sharedUserContext, sharedAppConfig, blobMetadata, blobBuffer, etagMatchCondition, selectQuery, preferredDownloadBlockSize]()
    {
        title_storage_blob_metadata resultBlobMetadata(
            blobMetadata
            );

        bool isBinaryData = (blobMetadata.blob_type() == title_storage_blob_type::binary);
        bool isDownloading = true;
        uint32_t startByte = 0;
        
        xbox_live_result<string_t> subpathAndQueryResult = title_storage_download_blob_subpath(
            blobMetadata,
            selectQuery
            );

        if(subpathAndQueryResult.err()) return xbox_live_result<title_storage_blob_result>(subpathAndQueryResult.err(), subpathAndQueryResult.err_message());

        string_t subpathAndQuery = subpathAndQueryResult.payload();
        blobBuffer->clear();
        while (isDownloading)
        {
            std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
                sharedXboxLiveContextSettings,
                _T("GET"),
                utils::create_xboxlive_endpoint(_T("titlestorage"), sharedAppConfig),
                subpathAndQuery,
                xbox_live_api::download_blob
                );

            httpCall->set_content_type_header_value(CONTENT_TYPE_HEADER_VALUE);
            httpCall->set_long_http_call(true);

            set_e_tag_header(
                httpCall,
                blobMetadata.e_tag(),
                etagMatchCondition
                );

            if (isBinaryData)
            {
                set_range_header(
                    httpCall,
                    startByte,
                    startByte + preferredDownloadBlockSize - 1
                    );
            }

            std::error_code errc = xbox_live_error_code::no_error;
            httpCall->get_response_with_auth(sharedUserContext, http_call_response_body_type::vector_body)
            .then([&errc, blobBuffer, &startByte, isBinaryData, preferredDownloadBlockSize, &isDownloading, &resultBlobMetadata](std::shared_ptr<http_call_response> response)
            {
                errc = response->err_code();
                if (!response->err_code())
                {
                    auto responseVector = response->response_body_vector();
                    size_t responseByteLength = responseVector.size();
                    blobBuffer->resize(blobBuffer->size() + responseByteLength);
                    if (responseByteLength > 0)
                    {
                        memcpy(&(blobBuffer->at(startByte)), &responseVector[0], responseByteLength);
                    }

                    startByte += static_cast<uint32_t>(responseByteLength);

                    if (!isBinaryData || 
                        responseByteLength < preferredDownloadBlockSize || 
                        startByte == resultBlobMetadata.length())
                    {
                        isDownloading = false;
                        resultBlobMetadata._Set_e_tag_and_length(
                            response->e_tag(),
                            responseByteLength
                            );
                    }
                }
            }).wait();

            if (errc)
            {
                return xbox_live_result<title_storage_blob_result>(errc, "Download failed");
            }
        }

        return xbox_live_result<title_storage_blob_result>(
            title_storage_blob_result(
                blobBuffer,
                resultBlobMetadata
                ),
                xbox_live_error_code::no_error
                );
    });

    return utils::create_exception_free_task<title_storage_blob_result>(
        task
        );
}

pplx::task<xbox_live_result<title_storage_blob_metadata>>
title_storage_service::upload_blob(
    _In_ title_storage_blob_metadata blobMetadata,
    _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
    _In_ title_storage_e_tag_match_condition etagMatchCondition,
    _In_ uint32_t preferredUploadBlockSize
    )
{ 
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(blobBuffer == nullptr, title_storage_blob_metadata, "Blob buffer is null");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(blobBuffer->empty(), title_storage_blob_metadata, "Blob buffer is empty");
    
    preferredUploadBlockSize = preferredUploadBlockSize < MIN_UPLOAD_BLOCK_SIZE ? MIN_UPLOAD_BLOCK_SIZE : preferredUploadBlockSize;
    preferredUploadBlockSize = preferredUploadBlockSize > MAX_UPLOAD_BLOCK_SIZE ? MAX_UPLOAD_BLOCK_SIZE : preferredUploadBlockSize;

    auto sharedXboxLiveContextSettings = m_xboxLiveContextSettings;
    auto sharedUserContext = m_userContext;
    auto appConfig = m_appConfig;

    auto task = pplx::create_task([sharedXboxLiveContextSettings, sharedUserContext, appConfig, blobMetadata, blobBuffer, preferredUploadBlockSize, etagMatchCondition]()
    {
        title_storage_blob_metadata resultBlobMetadata(
            blobMetadata
            );

        bool isBinaryData = resultBlobMetadata.blob_type() == title_storage_blob_type::binary;
        bool isFinalBlock = false;

        std::vector<uint8_t> dataChunk(0);
        auto blobBufferSize = static_cast<uint32_t>(blobBuffer->size());
        if (!isBinaryData)
        {
            dataChunk.resize(blobBufferSize);
        }

        size_t start = 0;
        size_t count = 0;
        string_t continuationToken;

        while (start < blobBufferSize)
        {
            if (isBinaryData)
            {
                count = blobBufferSize - start;

                if (count > preferredUploadBlockSize)
                {
                    count = preferredUploadBlockSize;
                }
                dataChunk.resize(count);

                memcpy(&dataChunk[0], &(blobBuffer->at(start)), count);
                
                start += count;
                isFinalBlock = start == blobBufferSize;
            }
            else
            {
                dataChunk = *blobBuffer;
                count = static_cast<uint32_t>(dataChunk.size());
                start = static_cast<uint32_t>(dataChunk.size());
                isFinalBlock = true;
            }

            xbox_live_result<string_t> subpathAndQueryResult = title_storage_upload_blob_subpath(
                resultBlobMetadata,
                continuationToken,
                isFinalBlock
                );

            if (subpathAndQueryResult.err()) return xbox_live_result<title_storage_blob_metadata>(subpathAndQueryResult.err(), subpathAndQueryResult.err_message());
            string_t subpathAndQuery = subpathAndQueryResult.payload();

            std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
                sharedXboxLiveContextSettings,
                _T("PUT"),
                utils::create_xboxlive_endpoint(_T("titlestorage"), appConfig),
                subpathAndQuery,
                xbox_live_api::upload_blob
                );

            httpCall->set_content_type_header_value(CONTENT_TYPE_HEADER_VALUE);
            httpCall->set_long_http_call(true);

            set_e_tag_header(
                httpCall,
                resultBlobMetadata.e_tag(),
                etagMatchCondition
                );

            httpCall->set_request_body(dataChunk);

            std::error_code errc = xbox_live_error_code::no_error;
            httpCall->get_response_with_auth(sharedUserContext)
            .then([&errc, isFinalBlock, &continuationToken, &resultBlobMetadata, blobBufferSize](std::shared_ptr<http_call_response> response)
            {
                errc = response->err_code();
                auto responseJson = response->response_body_json();
                continuationToken = _T("");
                if (!errc && !responseJson.is_null())
                {
                    continuationToken = utils::extract_json_string(responseJson, _T("continuationToken"));
                }

                if (!errc && isFinalBlock)
                {
                    resultBlobMetadata._Set_e_tag_and_length(
                        response->e_tag(),
                        blobBufferSize
                        );
                }
            }).wait();

            if (errc)
            {
                return xbox_live_result<title_storage_blob_metadata>(errc, "Upload failed");
            }
        }

        return xbox_live_result<title_storage_blob_metadata>(resultBlobMetadata, xbox_live_error_code::no_error, "");
    });

    return utils::create_exception_free_task<title_storage_blob_metadata>(
        task
        );
}

void
title_storage_service::set_e_tag_header(
    _In_ std::shared_ptr<http_call> httpCall,
    _In_ string_t etag,
    _In_ title_storage_e_tag_match_condition eTagMatchCondition
    )
{
    if (eTagMatchCondition == title_storage_e_tag_match_condition::not_used)
    {
        httpCall->set_custom_header(E_TAG_HEADER_NAME, E_TAG_INVALID_VALUE);
        httpCall->set_custom_header(IF_NONE_HEADER_NAME, E_TAG_INVALID_VALUE);
    }
    else
    {
        string_t etagValue = etag.empty() ?
            IF_NONE_HEADER_NAME :
            etag;

        httpCall->set_custom_header(E_TAG_HEADER_NAME, etagValue);

        if (eTagMatchCondition == title_storage_e_tag_match_condition::if_match)
        {
            httpCall->set_custom_header(IF_MATCH_HEADER_NAME, etagValue);
        }
        else
        {
            httpCall->set_custom_header(IF_NONE_HEADER_NAME, etagValue);
        }
    }
}

void
title_storage_service::set_range_header(
    _In_ std::shared_ptr<http_call> httpCall,
    _In_ uint32_t startByte,
    _In_ uint32_t endByte
    )
{
    stringstream_t byteRange;
    byteRange << _T("bytes=");
    byteRange << startByte;
    byteRange << _T("-");
    byteRange << endByte;

    httpCall->set_custom_header(
        RANGE_HEADER_NAME,
        byteRange.str()
        );
}

xbox_live_result<string_t>
title_storage_service::title_storage_quota_subpath(
    _In_ title_storage_type storageType,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& xboxUserId,
    _In_ const string_t& multiplayerSessionTemplateName,
    _In_ const string_t& multiplayerSessionName
    )
{
    stringstream_t path;
    switch (storageType)
    {
        case title_storage_type::trusted_platform_storage:
            path << _T("/trustedplatform/users/xuid(");
            path << xboxUserId;
            path << _T(")/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::json_storage:
            path << _T("/json/users/xuid(");
            path << xboxUserId;
            path << _T(")/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::global_storage:
            path << _T("/global/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::session_storage:
            path << _T("/sessions/");
            path << multiplayerSessionTemplateName;
            path << _T("~");
            path << multiplayerSessionName;
            path << _T("/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::untrusted_platform_storage:
            path << _T("/untrustedplatform/users/xuid(");
            path << xboxUserId;
            path << _T(")/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::universal:
            path << _T("/universalplatform/users/xuid(");
            path << xboxUserId;
            path << _T(")/scids/");
            path << serviceConfigurationId;
            break;

        default:
            return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Invalid storage type");
    }

    return xbox_live_result<string_t>(path.str());
}

xbox_live_result<string_t>
title_storage_service::title_storage_blob_metadata_subpath(
    _In_ title_storage_type storageType,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& xboxUserId,
    _In_ const string_t& multiplayerSessionTemplateName,
    _In_ const string_t& multiplayerSessionName,
    _In_ const string_t& blobPath,
    _In_ uint32_t skipItems,
    _In_ uint32_t maxItems,
    _In_ const string_t& continuationToken
    )
{
    stringstream_t path;
    switch (storageType)
    {
        case title_storage_type::trusted_platform_storage:
            path << _T("/trustedplatform/users/xuid(");
            path << xboxUserId;
            path << _T(")/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::json_storage:
            path << _T("/json/users/xuid(");
            path << xboxUserId;
            path << _T(")/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::global_storage:
            path << _T("/global/scids/");
            path << serviceConfigurationId; 
            break;

        case title_storage_type::session_storage:
            path << _T("/sessions/");
            path << multiplayerSessionTemplateName;
            path << _T("~");
            path << multiplayerSessionName;
            path << _T("/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::untrusted_platform_storage:
            path << _T("/untrustedplatform/users/xuid(");
            path << xboxUserId;
            path << _T(")/scids/");
            path << serviceConfigurationId;
            break;

        case title_storage_type::universal:
            path << _T("/universalplatform/users/xuid(");
            path << xboxUserId;
            path << _T(")/scids/");
            path << serviceConfigurationId;
            break;

        default:
            return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Invalid storage type");
    }

    path << _T("/data");

    if (!blobPath.empty())
    {
        path << _T("/");
        path << web::uri::encode_uri(blobPath, web::uri::components::query);
    }

    web::uri_builder params;
    utils::append_paging_info(
        params,
        skipItems,
        maxItems,
        continuationToken
        );


    string_t paramPath = params.query();
    if (paramPath.size() > 1)
    {
        path << "?" << paramPath;
    }

    return path.str();
}

xbox_live_result<string_t>
title_storage_service::title_storage_download_blob_subpath(
    _In_ const title_storage_blob_metadata& blobMetadata,
    _In_ const string_t& selectQuery
    )
{
    stringstream_t path;
    xbox_live_result<string_t> titleStorageBlobToString;
    switch (blobMetadata.storage_type())
    {
        case title_storage_type::trusted_platform_storage:
            titleStorageBlobToString = title_storage_blob_metadata::_Convert_title_storage_blob_type_to_string(blobMetadata.blob_type());
            if (titleStorageBlobToString.err())
            {
                return xbox_live_result<string_t>(titleStorageBlobToString.err(), titleStorageBlobToString.err_message());
            }

            path << _T("/trustedplatform/users/xuid(");
            path << blobMetadata.xbox_user_id();
            path << _T(")/scids/");
            path << blobMetadata.service_configuration_id();
            path << _T("/data/");
            path << blobMetadata.blob_path();
            path << _T(",");
            path << titleStorageBlobToString.payload();
            break;

        case title_storage_type::json_storage:
            path << _T("/json/users/xuid(");
            path << blobMetadata.xbox_user_id();
            path << _T(")/scids/");
            path << blobMetadata.service_configuration_id();
            path << _T("/data/");
            path << blobMetadata.blob_path();
            path << _T(",json");
            break;

        case title_storage_type::global_storage:
            titleStorageBlobToString = title_storage_blob_metadata::_Convert_title_storage_blob_type_to_string(blobMetadata.blob_type());
            if (titleStorageBlobToString.err())
            {
                return xbox_live_result<string_t>(titleStorageBlobToString.err(), titleStorageBlobToString.err_message());
            }

            path << _T("/global/scids/");
            path << blobMetadata.service_configuration_id();
            path << _T("/data/");
            path << blobMetadata.blob_path();
            path << _T(",");
            path << titleStorageBlobToString.payload();
            break;

        case title_storage_type::session_storage:
            titleStorageBlobToString = title_storage_blob_metadata::_Convert_title_storage_blob_type_to_string(blobMetadata.blob_type());
            if (titleStorageBlobToString.err())
            {
                return xbox_live_result<string_t>(titleStorageBlobToString.err(), titleStorageBlobToString.err_message());
            }

            path << _T("/sessions/");
#pragma warning(suppress: 4996)
            path << blobMetadata.multiplayer_session_template_name();
            path << _T("~");
#pragma warning(suppress: 4996)
            path << blobMetadata.multiplayer_session_name();
            path << _T("/scids/");
            path << blobMetadata.service_configuration_id();
            path << _T("/data/");
            path << blobMetadata.blob_path();
            path << _T(",");
            path << titleStorageBlobToString.payload();
            break;

        case title_storage_type::untrusted_platform_storage:
            titleStorageBlobToString = title_storage_blob_metadata::_Convert_title_storage_blob_type_to_string(blobMetadata.blob_type());
            if (titleStorageBlobToString.err())
            {
                return xbox_live_result<string_t>(titleStorageBlobToString.err(), titleStorageBlobToString.err_message());
            }

            path << _T("/untrustedplatform/users/xuid(");
            path << blobMetadata.xbox_user_id();
            path << _T(")/scids/");
            path << blobMetadata.service_configuration_id();
            path << _T("/data/");
            path << blobMetadata.blob_path();
            path << _T(",");
            path << titleStorageBlobToString.payload();
            break;

        case title_storage_type::universal:
            titleStorageBlobToString = title_storage_blob_metadata::_Convert_title_storage_blob_type_to_string(blobMetadata.blob_type());
            if (titleStorageBlobToString.err())
            {
                return xbox_live_result<string_t>(titleStorageBlobToString.err(), titleStorageBlobToString.err_message());
            }

            path << _T("/universalplatform/users/xuid(");
            path << blobMetadata.xbox_user_id();
            path << _T(")/scids/");
            path << blobMetadata.service_configuration_id();
            path << _T("/data/");
            path << blobMetadata.blob_path();
            path << _T(",");
            path << titleStorageBlobToString.payload();
            break;

        default:
            return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "Invalid storage type");
    }

    std::vector<string_t> params;
    if (!selectQuery.empty())
    {
        if (blobMetadata.blob_type() == title_storage_blob_type::config)
        {
            stringstream_t param;
            param << _T("customSelector=");
            param << web::uri::encode_uri(selectQuery.c_str());
            params.push_back(param.str());
        }
        else if (blobMetadata.blob_type() == title_storage_blob_type::json)
        {
            stringstream_t param;
            param << _T("select=");
            param << web::uri::encode_uri(selectQuery.c_str());
            params.push_back(param.str());
        }
    }

    path << utils::get_query_from_params(params);

    return xbox_live_result<string_t>(path.str());
}

xbox_live_result<string_t>
title_storage_service::title_storage_upload_blob_subpath(
    _In_ const title_storage_blob_metadata& blobMetadata,
    _In_ const string_t& continuationToken,
    _In_ bool finalBlock
    )
{
    stringstream_t source;
    xbox_live_result<string_t> titleStorageDlSubpath = title_storage_download_blob_subpath(
        blobMetadata,
        _T("")
        );

    if (titleStorageDlSubpath.err()) return titleStorageDlSubpath;

    source << titleStorageDlSubpath.payload();

    std::vector<string_t> params;

    if (blobMetadata.client_timestamp().to_interval() != 0)
    {
        string_t clientTimeStamp = blobMetadata.client_timestamp().to_string();
        stringstream_t param;
        param << _T("clientFileTime=");
        param << web::uri::encode_uri(clientTimeStamp);
        params.push_back(param.str());
    }

    if (!blobMetadata.display_name().empty())
    {
        stringstream_t param;
        param << _T("displayName=");
        param << web::uri::encode_uri(blobMetadata.display_name());
        params.push_back(param.str());
    }

    if (!continuationToken.empty())
    {
        stringstream_t param;
        param << _T("continuationToken=");
        param << web::uri::encode_uri(continuationToken);
        params.push_back(param.str());
    }

    if (blobMetadata.blob_type() == title_storage_blob_type::binary)
    {
        string_t param = finalBlock ? _T("finalBlock=true") : _T("finalBlock=false");
        params.push_back(param);
    }

    source << utils::get_query_from_params(params);

    return xbox_live_result<string_t>(source.str());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END