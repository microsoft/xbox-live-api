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

/// <summary> Defines values that indicate the ETag match condition used when downloading, uploading or deleting title storage data.</summary>
public enum class TitleStorageETagMatchCondition
{
    /// <summary>There is no match condition, perform the request regardless of the ETag value.</summary>
    NotUsed = xbox::services::title_storage::title_storage_e_tag_match_condition::not_used,

    /// <summary>Perform the request if the Etag value specified matches the service value.</summary>
    IfMatch = xbox::services::title_storage::title_storage_e_tag_match_condition::if_match,

    /// <summary>Perform the request if the Etag value specified does not match the service value.</summary>
    IfNotMatch = xbox::services::title_storage::title_storage_e_tag_match_condition::if_not_match
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END