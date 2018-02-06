// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_search_game_clip_uri_info::contextual_search_game_clip_uri_info() :
    m_fileSize(0),
    m_uriType(contextual_search_game_clip_uri_type::none)
{
}

contextual_search_game_clip_uri_info::contextual_search_game_clip_uri_info(
    _In_ web::uri url,
    _In_ uint64_t fileSize,
    _In_ contextual_search_game_clip_uri_type uriType,
    _In_ utility::datetime expiration
    ) :
    m_url(std::move(url)),
    m_fileSize(fileSize),
    m_uriType(uriType),
    m_expiration(std::move(expiration))
{
}

contextual_search_game_clip_uri_type
contextual_search_game_clip_uri_info::convert_string_to_clip_uri_type(_In_ const string_t& value)
{
    if (utils::str_icmp(value, _T("Original")) == 0)
    {
        return contextual_search_game_clip_uri_type::original;
    }
    else if (utils::str_icmp(value, _T("Download")) == 0)
    {
        return contextual_search_game_clip_uri_type::download;
    }
    else if (utils::str_icmp(value, _T("Ahls")) == 0)
    {
        return contextual_search_game_clip_uri_type::http_live_streaming;
    }
    else if (utils::str_icmp(value, _T("SmoothStreaming")) == 0)
    {
        return contextual_search_game_clip_uri_type::smooth_streaming;
    }

    return contextual_search_game_clip_uri_type::none;
}

xbox_live_result<contextual_search_game_clip_uri_info>
contextual_search_game_clip_uri_info::_Deserialize(_In_ const web::json::value& inputJson)
{
    if (inputJson.is_null()) return xbox_live_result<contextual_search_game_clip_uri_info>();

    std::error_code errc = xbox_live_error_code::no_error;

    contextual_search_game_clip_uri_info result(
        utils::extract_json_string(inputJson, _T("Uri"), errc, false),
        utils::extract_json_uint52(inputJson, "FileSize", errc, false),
        convert_string_to_clip_uri_type(utils::extract_json_string(inputJson, _T("UriType"), errc, false)),
        utils::extract_json_time(inputJson, _T("Expiration"), errc, false)
        );

    return xbox_live_result<contextual_search_game_clip_uri_info>(result, errc);
}


const web::uri& contextual_search_game_clip_uri_info::url() const
{
    return m_url;
}

uint64_t contextual_search_game_clip_uri_info::file_size() const
{
    return m_fileSize;
}

contextual_search_game_clip_uri_type contextual_search_game_clip_uri_info::uri_type() const
{
    return m_uriType;
}

const utility::datetime& contextual_search_game_clip_uri_info::expiration() const
{
    return m_expiration;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END
