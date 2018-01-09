// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "taskargs.h"
#include "xsapi/title_storage_c.h"

struct title_storage_taskargs
{
    XSAPI_XBOX_LIVE_CONTEXT* pXboxLiveContext;
};

struct get_quota_taskargs : public title_storage_taskargs, public taskargs_with_payload<XSAPI_TITLE_STORAGE_QUOTA>
{
    std::string serviceConfigurationId; 
    XSAPI_TITLE_STORAGE_TYPE storageType;
    std::string xboxUserId;
};

struct get_blob_metadata_taskargs : public title_storage_taskargs, public taskargs_with_payload<XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT>
{
    string_t serviceConfigurationId;
    XSAPI_TITLE_STORAGE_TYPE storageType;
    string_t blobPath;
    string_t xboxUserId;
    uint32_t skipItems;
    uint32_t maxItems; 
};

struct blob_metadata_result_get_next_taskargs : public taskargs_with_payload<XSAPI_TITLE_STORAGE_BLOB_METADATA_RESULT>
{
    uint32_t maxItems;
};

struct delete_blob_taskargs : public title_storage_taskargs, public taskargs
{
    const XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata;
    bool deleteOnlyIfEtagMatches;
};

struct download_blob_taskargs : public title_storage_taskargs, public taskargs_with_payload<XSAPI_TITLE_STORAGE_BLOB_RESULT>
{
    const XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata;
    PBYTE blobBuffer;
    uint32_t cbBlobBuffer;
    XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION etagMatchCondition;
    string_t selectQuery;
    uint32_t preferredDownloadBlockSize;
};

struct upload_blob_taskargs : public title_storage_taskargs, public taskargs_with_payload<const XSAPI_TITLE_STORAGE_BLOB_METADATA*>
{
    const XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata;
    std::shared_ptr<std::vector<unsigned char>> blobBuffer;
    XSAPI_TITLE_STORAGE_E_TAG_MATCH_CONDITION etagMatchCondition;
    uint32_t preferredUploadBlockSize;
};