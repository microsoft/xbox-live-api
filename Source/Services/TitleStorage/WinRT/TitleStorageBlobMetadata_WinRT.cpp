// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TitleStorageBlobMetadata_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::title_storage;
NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

TitleStorageBlobMetadata::TitleStorageBlobMetadata(
    _In_ title_storage_blob_metadata cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

TitleStorageBlobMetadata::TitleStorageBlobMetadata(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ TitleStorageType storageType,
    _In_ Platform::String^ blobPath,
    _In_ TitleStorageBlobType blobType,
    _In_opt_ Platform::String^ xboxUserId
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj = title_storage_blob_metadata(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            static_cast<title_storage_type>(storageType),
            STRING_T_FROM_PLATFORM_STRING(blobPath),
            static_cast<title_storage_blob_type>(blobType),
            STRING_T_FROM_PLATFORM_STRING(xboxUserId)
            );
        );
}

TitleStorageBlobMetadata::TitleStorageBlobMetadata(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ TitleStorageType storageType,
    _In_ Platform::String^ blobPath,
    _In_ TitleStorageBlobType blobType,
    _In_opt_ Platform::String^ xboxUserId,
    _In_opt_ Platform::String^ displayName,
    _In_opt_ Platform::String^ eTag
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj = title_storage_blob_metadata(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            static_cast<title_storage_type>(storageType),
            STRING_T_FROM_PLATFORM_STRING(blobPath),
            static_cast<title_storage_blob_type>(blobType),
            STRING_T_FROM_PLATFORM_STRING(xboxUserId),
            STRING_T_FROM_PLATFORM_STRING(displayName),
            STRING_T_FROM_PLATFORM_STRING(eTag)
            );
        );
}

TitleStorageBlobMetadata::TitleStorageBlobMetadata(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ TitleStorageType storageType,
    _In_ Platform::String^ blobPath,
    _In_ TitleStorageBlobType blobType,
    _In_opt_ Platform::String^ xboxUserId,
    _In_opt_ Platform::String^ displayName,
    _In_opt_ Platform::String^ eTag,
    _In_ Windows::Foundation::DateTime clientTimestamp
    )
{
    CONVERT_STD_EXCEPTION(
        m_cppObj = title_storage_blob_metadata(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            static_cast<title_storage_type>(storageType),
            STRING_T_FROM_PLATFORM_STRING(blobPath),
            static_cast<title_storage_blob_type>(blobType),
            STRING_T_FROM_PLATFORM_STRING(xboxUserId),
            STRING_T_FROM_PLATFORM_STRING(displayName),
            STRING_T_FROM_PLATFORM_STRING(eTag),
            Xbox::Services::System::UtilsWinRT::ConvertDateTimeToNativeDateTime(clientTimestamp)
            );
        );
}

TitleStorageBlobMetadata^
TitleStorageBlobMetadata::CreateTitleStorageBlobMetadataForSessionStorage(
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ blobPath,
    _In_ TitleStorageBlobType blobType,
    _In_ Platform::String^ multiplayerSessionTemplateName,
    _In_ Platform::String^ multiplayerSessionName,
    _In_opt_ Platform::String^ displayName,
    _In_opt_ Platform::String^ eTag
    )
{
    title_storage_blob_metadata cppObj;
#pragma warning(suppress: 4996)
    CONVERT_STD_EXCEPTION(
        cppObj = title_storage_blob_metadata::create_title_storage_blob_metadata_for_session_storage(
            STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
            STRING_T_FROM_PLATFORM_STRING(blobPath),
            static_cast<title_storage_blob_type>(blobType),
            STRING_T_FROM_PLATFORM_STRING(multiplayerSessionTemplateName),
            STRING_T_FROM_PLATFORM_STRING(multiplayerSessionName),
            STRING_T_FROM_PLATFORM_STRING(displayName),
            STRING_T_FROM_PLATFORM_STRING(eTag)
            );
        );

    return ref new TitleStorageBlobMetadata(cppObj);
}

xbox::services::title_storage::title_storage_blob_metadata
TitleStorageBlobMetadata::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END