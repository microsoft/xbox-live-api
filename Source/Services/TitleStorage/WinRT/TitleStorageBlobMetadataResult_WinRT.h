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
/// Metadata about blob data returned from the cloud.
/// </summary>
public ref class TitleStorageBlobMetadataResult sealed
{
public:
    /// <summary>
    /// The collection of blob metadata objects returned by a service metadata request.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<TitleStorageBlobMetadata^>^ Items { Windows::Foundation::Collections::IVectorView<TitleStorageBlobMetadata^>^ get(); }

    /// <summary>
    /// Gets the next page of blob metadata results.
    /// </summary>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to request all items.</param>
    /// <returns>A TitleStorageBlobMetadataResult object containing the next page of TitleStorageBlobMetadata objects.</returns>
    Windows::Foundation::IAsyncOperation<TitleStorageBlobMetadataResult^>^ GetNextAsync(
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Indicates if there is additional data to retrieve from a GetNextAsync call
    /// </summary>
    DEFINE_PROP_GET_OBJ(HasNext, has_next, bool);

internal:
    TitleStorageBlobMetadataResult(_In_ xbox::services::title_storage::title_storage_blob_metadata_result cppObj);

private:
    xbox::services::title_storage::title_storage_blob_metadata_result m_cppObj;
    Windows::Foundation::Collections::IVectorView<TitleStorageBlobMetadata^>^ m_items;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END