// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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