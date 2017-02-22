// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/title_storage.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN

title_storage_blob_result::title_storage_blob_result()
{
}

title_storage_blob_result::title_storage_blob_result(
    _In_ std::shared_ptr<std::vector<unsigned char>> blobBuffer,
    _In_ title_storage_blob_metadata blobMetadata
    ) :
    m_blobBuffer(blobBuffer),
    m_blobMetadata(std::move(blobMetadata))
{
}

std::shared_ptr<std::vector<unsigned char>> const
title_storage_blob_result::blob_buffer() const
{
    return m_blobBuffer;
}

const title_storage_blob_metadata& 
title_storage_blob_result::blob_metadata() const
{
    return m_blobMetadata;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END