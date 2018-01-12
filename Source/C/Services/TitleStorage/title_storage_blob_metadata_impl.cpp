// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/title_storage_c.h"
#include "title_storage_blob_metadata_impl.h"

using namespace xbox::services;

XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL::XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL(
    XSAPI_TITLE_STORAGE_BLOB_METADATA* pMetadata,
    const xbox::services::title_storage::title_storage_blob_metadata& cppObj
    )
    : m_pMetadata(pMetadata)
{
    update(cppObj);
}

void XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL::update(const xbox::services::title_storage::title_storage_blob_metadata& cppObj)
{
    m_cppObject = cppObj;

    m_blobPath = utils::utf8_from_utf16(cppObj.blob_path());
    m_pMetadata->blobPath = m_blobPath.data();

    m_pMetadata->blobType = static_cast<XSAPI_TITLE_STORAGE_BLOB_TYPE>(cppObj.blob_type());

    m_pMetadata->storageType = static_cast<XSAPI_TITLE_STORAGE_TYPE>(cppObj.storage_type());

    m_displayName = utils::utf8_from_utf16(cppObj.display_name());
    m_pMetadata->displayName = m_displayName.data();

    m_eTag = utils::utf8_from_utf16(cppObj.e_tag());
    m_pMetadata->ETag = m_eTag.data();

    m_pMetadata->clientTimestamp = utils::time_t_from_datetime(cppObj.client_timestamp());

    m_pMetadata->length = cppObj.length();

    m_serviceConfigurationId = utils::utf8_from_utf16(cppObj.service_configuration_id());
    m_pMetadata->serviceConfigurationId = m_serviceConfigurationId.data();

    m_xboxUserId = utils::utf8_from_utf16(cppObj.xbox_user_id());
    m_pMetadata->xboxUserId = m_xboxUserId.data();
}

const xbox::services::title_storage::title_storage_blob_metadata& XSAPI_TITLE_STORAGE_BLOB_METADATA_IMPL::cppObject() const
{
    return m_cppObject;
}