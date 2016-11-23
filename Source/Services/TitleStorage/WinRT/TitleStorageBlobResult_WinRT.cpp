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
#include "TitleStorageBlobResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

TitleStorageBlobResult::TitleStorageBlobResult(
    _In_ xbox::services::title_storage::title_storage_blob_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    auto nativeBlobBuffer = m_cppObj.blob_buffer();
    auto writer = ref new Windows::Storage::Streams::DataWriter();

    auto nativeBlobBufferSize = nativeBlobBuffer->size();
    if(nativeBlobBufferSize <= UINT32_MAX)
    {
        writer->WriteBytes(Platform::ArrayReference<unsigned char>(&(nativeBlobBuffer->at(0)), static_cast<uint32>(nativeBlobBufferSize)));
    }
    else
    {
        throw ref new Platform::OutOfBoundsException(L"Stream size is too large");
    }

    m_blobBuffer = writer->DetachBuffer();
    m_blobMetadata = ref new TitleStorageBlobMetadata(m_cppObj.blob_metadata());
}

Windows::Storage::Streams::IBuffer^
TitleStorageBlobResult::BlobBuffer::get()
{
    return m_blobBuffer;
}

TitleStorageBlobMetadata^
TitleStorageBlobResult::BlobMetadata::get()
{
    return m_blobMetadata;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END