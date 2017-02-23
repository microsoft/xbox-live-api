// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TitleStorageBlobMetadataResult_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::title_storage;
NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

TitleStorageBlobMetadataResult::TitleStorageBlobMetadataResult(
    _In_ xbox::services::title_storage::title_storage_blob_metadata_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_items = Xbox::Services::System::UtilsWinRT::CreatePlatformVectorFromStdVectorObj<TitleStorageBlobMetadata, title_storage_blob_metadata>(
        m_cppObj.items()
        )->GetView();
}

Windows::Foundation::Collections::IVectorView<TitleStorageBlobMetadata^>^
TitleStorageBlobMetadataResult::Items::get()
{
    return m_items;
}

Windows::Foundation::IAsyncOperation<TitleStorageBlobMetadataResult^>^
TitleStorageBlobMetadataResult::GetNextAsync(
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_next(maxItems)
    .then([](xbox::services::xbox_live_result<title_storage_blob_metadata_result> metadataResult)
    {
        THROW_IF_ERR(metadataResult);
        return ref new TitleStorageBlobMetadataResult(metadataResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END