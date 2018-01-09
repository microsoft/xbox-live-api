// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/title_storage_c.h"

struct XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL
{
public:
    XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL(XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata, const xbox::services::title_storage::title_storage_blob_metadata& cppObj);

    void update(const xbox::services::title_storage::title_storage_blob_metadata& cppObj);
    const xbox::services::title_storage::title_storage_blob_metadata& cppObject() const;

private:
    std::string m_blobPath;
    std::string m_displayName;
    std::string m_eTag;
    std::string m_serviceConfigurationId;
    std::string m_xboxUserId;

    XSAPI_TITLE_STORAGE_BLOB_METADATA* m_pMetadata;
    xbox::services::title_storage::title_storage_blob_metadata m_cppObject;
};