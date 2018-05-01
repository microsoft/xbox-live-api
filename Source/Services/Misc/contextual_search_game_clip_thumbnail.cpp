// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_search_game_clip_thumbnail::contextual_search_game_clip_thumbnail()
{
}

contextual_search_game_clip_thumbnail::contextual_search_game_clip_thumbnail(
    _In_ web::uri url,
    _In_ uint64_t fileSize,
    _In_ contextual_search_game_clip_thumbnail_type thumbnailType
    ) :
    m_url(std::move(url)),
    m_fileSize(fileSize),
    m_thumbnailType(thumbnailType)
{
}

contextual_search_game_clip_thumbnail_type
contextual_search_game_clip_thumbnail::convert_string_to_thumbnail_type(_In_ const string_t& value)
{
    if (utils::str_icmp(value, _T("Large")) == 0)
    {
        return contextual_search_game_clip_thumbnail_type::large_thumbnail;
    }
    else if (utils::str_icmp(value, _T("Small")) == 0)
    {
        return contextual_search_game_clip_thumbnail_type::small_thumbnail;
    }

    return contextual_search_game_clip_thumbnail_type::none;
}

xbox_live_result<contextual_search_game_clip_thumbnail>
contextual_search_game_clip_thumbnail::_Deserialize(_In_ const web::json::value& inputJson)
{
    if (inputJson.is_null()) return xbox_live_result<contextual_search_game_clip_thumbnail>();

    std::error_code errc = xbox_live_error_code::no_error;

    contextual_search_game_clip_thumbnail result(
        utils::extract_json_string(inputJson, _T("Uri"), errc, false),
        utils::extract_json_uint52(inputJson, "FileSize", errc, false),
        convert_string_to_thumbnail_type(utils::extract_json_string(inputJson, _T("ThumbnailType"), errc, false))
        );

    return xbox_live_result<contextual_search_game_clip_thumbnail>(result, errc);
}

const web::uri& contextual_search_game_clip_thumbnail::url() const
{
    return m_url;
}

uint64_t contextual_search_game_clip_thumbnail::file_size() const
{
    return m_fileSize;
}

contextual_search_game_clip_thumbnail_type contextual_search_game_clip_thumbnail::thumbnail_type() const
{
    return m_thumbnailType;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END
