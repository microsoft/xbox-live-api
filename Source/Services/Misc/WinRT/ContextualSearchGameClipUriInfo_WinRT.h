// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Describes the type of game clip URI.</summary>
public enum class ContextualSearchGameClipUriType
{
    /// <summary>Game clip URI type is not known or not set.</summary>
    None = xbox::services::contextual_search::contextual_search_game_clip_uri_type::none,

    /// <summary>Game clip URI is the location of the original game clip in unprocessed form.</summary>
    Original = xbox::services::contextual_search::contextual_search_game_clip_uri_type::original,

    /// <summary>Game clip URI is the location of the clip available for download.</summary>
    Download = xbox::services::contextual_search::contextual_search_game_clip_uri_type::download,

    /// <summary>Game clip URI is the IIS Smooth Streaming URL(manifest pointer).</summary>
    SmoothStreaming = xbox::services::contextual_search::contextual_search_game_clip_uri_type::smooth_streaming,

    /// <summary>Game clip URI is the Apple HTTP Live Streaming URL.</summary>
    HttpLiveStreaming = xbox::services::contextual_search::contextual_search_game_clip_uri_type::http_live_streaming
};

/// <summary>Contains information about a contextual search game clips uri info.</summary>
public ref class ContextualSearchGameClipUriInfo sealed
{
public:
    /// <summary>
    /// The URI to the location of the video asset.
    /// </summary>
    DEFINE_PROP_GET_URI_OBJ(Url, url);

    /// <summary>
    /// The file size at the location of the URL for formats that are downloaded
    /// This is 0 for streaming formats
    /// </summary>
    DEFINE_PROP_GET_OBJ(FileSize, file_size, uint64);

    /// <summary>
    /// Specifies the type of the URI.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(UriType, uri_type, Microsoft::Xbox::Services::ContextualSearch::ContextualSearchGameClipUriType);

    /// <summary>
    /// The expiration time of the URI that is included in this response.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(Expiration, expiration);

internal:
    ContextualSearchGameClipUriInfo(
        _In_ xbox::services::contextual_search::contextual_search_game_clip_uri_info cppObj
        );

private:
    xbox::services::contextual_search::contextual_search_game_clip_uri_info m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
