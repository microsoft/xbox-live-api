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