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

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN

/// <summary>Defines values that indicate title storage blob type.</summary>
public enum class TitleStorageBlobType
{
    /// <summary>Unknown blob type.</summary>
    Unknown = xbox::services::title_storage::title_storage_blob_type::unknown,

    /// <summary>Binary blob type.</summary>
    Binary = xbox::services::title_storage::title_storage_blob_type::binary,

    /// <summary>JSON blob type.</summary>
    Json = xbox::services::title_storage::title_storage_blob_type::json,

    /// <summary>Configuration information blob type. </summary>
    Config = xbox::services::title_storage::title_storage_blob_type::config
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END