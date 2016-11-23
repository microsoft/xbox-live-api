//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/title_storage.h"
#include "TitleStorageBlobMetadata_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

/// <summary>
/// Blob data returned from the cloud. 
/// </summary>
public ref class TitleStorageBlobResult sealed
{
public:
    /// <summary>
    /// The contents of the title storage blob.
    /// </summary>
    property Windows::Storage::Streams::IBuffer^ BlobBuffer { Windows::Storage::Streams::IBuffer^ get(); }

    /// <summary>
    /// Updated TitleStorageBlobMetadata object following an upload or download.
    /// </summary>
    property TitleStorageBlobMetadata^ BlobMetadata { TitleStorageBlobMetadata^ get(); };

internal:
    TitleStorageBlobResult(_In_ xbox::services::title_storage::title_storage_blob_result cppObj);

private:
    xbox::services::title_storage::title_storage_blob_result m_cppObj;
    Windows::Storage::Streams::IBuffer^ m_blobBuffer;
    TitleStorageBlobMetadata^ m_blobMetadata;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END