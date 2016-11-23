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
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Describes the type of thumbnail</summary>
public enum class ContextualSearchGameClipThumbnailType
{
    /// <summary>Thumbnail type is not specified. This is an invalid value in most cases.</summary>
    None = xbox::services::contextual_search::contextual_search_game_clip_thumbnail_type::none,

    /// <summary>Specifies a small thumbnail. Small is 370 pixels in height by 208 pixels in width.</summary>
    SmallThumbnail = xbox::services::contextual_search::contextual_search_game_clip_thumbnail_type::small_thumbnail,

    /// <summary>Specifies a large thumbnail. Large is 754 pixels in height by 424 pixels in width.</summary>
    LargeThumbnail = xbox::services::contextual_search::contextual_search_game_clip_thumbnail_type::large_thumbnail
};

/// <summary>Contains information about a contextual search game clip thumbnail.</summary>
public ref class ContextualSearchGameClipThumbnail sealed
{
public:
    /// <summary>
    /// The URI for the thumbnail image.
    /// </summary>
    DEFINE_PROP_GET_URI_OBJ(Url, url);

    /// <summary>
    /// The total file size of the thumbnail image.
    /// </summary>
    DEFINE_PROP_GET_OBJ(FileSize, file_size, uint64);

    /// <summary>
    /// The type of thumbnail image.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(ThumbnailType, thumbnail_type, Microsoft::Xbox::Services::ContextualSearch::ContextualSearchGameClipThumbnailType);

internal:
    ContextualSearchGameClipThumbnail(
        _In_ xbox::services::contextual_search::contextual_search_game_clip_thumbnail cppObj
        );

private:
    xbox::services::contextual_search::contextual_search_game_clip_thumbnail m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
