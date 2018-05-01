// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_search_game_clip::contextual_search_game_clip() :
    m_durationInSeconds(0),
    m_gameClipType(contextual_search_game_clip_type::none),
    m_views(0)
{
}

contextual_search_game_clip::contextual_search_game_clip(
    _In_ string_t clipName,
    _In_ uint64_t durationInSeconds,
    _In_ string_t gameClipId,
    _In_ string_t gameClipLocale,
    _In_ std::vector<contextual_search_game_clip_uri_info> gameClipUris,
    _In_ std::vector<contextual_search_game_clip_thumbnail> thumbnails,
    _In_ contextual_search_game_clip_type gameClipType,
    _In_ uint64_t views,
    _In_ string_t xboxUserId,
    _In_ std::vector<contextual_search_game_clip_stat> stats
    ) :
    m_clipName(std::move(clipName)),
    m_durationInSeconds(durationInSeconds),
    m_gameClipId(std::move(gameClipId)),
    m_gameClipLocale(std::move(gameClipLocale)),
    m_gameClipUris(std::move(gameClipUris)),
    m_thumbnails(std::move(thumbnails)),
    m_gameClipType(gameClipType),
    m_views(views),
    m_xboxUserId(std::move(xboxUserId)),
    m_stats(std::move(stats))
{
}

string_t 
contextual_search_game_clip::convert_xuid_to_string(
    _In_ uint64_t xuid,
    _In_ string_t xuidName
)
{
    if (!xuidName.empty()) return xuidName;
    stringstream_t ss;
    ss << xuid;
    return ss.str();
}

contextual_search_game_clip_type 
contextual_search_game_clip::convert_string_to_clip_type(_In_ const string_t& value)
{
    if (utils::str_icmp(value, _T("Achievement")) == 0)
    {
        return contextual_search_game_clip_type::achievement;
    }
    else if (utils::str_icmp(value, _T("DeveloperInitiated")) == 0)
    {
        return contextual_search_game_clip_type::developer_initiated;
    }
    else if (utils::str_icmp(value, _T("UserGenerated")) == 0)
    {
        return contextual_search_game_clip_type::user_generated;
    }

    return contextual_search_game_clip_type::none;
}

xbox_live_result<contextual_search_game_clip>
contextual_search_game_clip::_Deserialize(_In_ const web::json::value& inputJson)
{
    if (inputJson.is_null()) return xbox_live_result<contextual_search_game_clip>();

    std::error_code errc = xbox_live_error_code::no_error;

    contextual_search_game_clip result(
        utils::extract_json_string(inputJson, _T("clipName"), errc, false),
        utils::extract_json_uint52(inputJson, "durationInSeconds", errc, false),
        utils::extract_json_string(inputJson, _T("gameClipId"), errc, false),
        utils::extract_json_string(inputJson, _T("gameClipLocale"), errc, false),
        utils::extract_json_vector<contextual_search_game_clip_uri_info>(contextual_search_game_clip_uri_info::_Deserialize, inputJson, _T("gameClipUris"), errc, false),
        utils::extract_json_vector<contextual_search_game_clip_thumbnail>(contextual_search_game_clip_thumbnail::_Deserialize, inputJson, _T("thumbnails"), errc, false),
        convert_string_to_clip_type(utils::extract_json_string(inputJson, _T("type"), false)),
        utils::extract_json_uint52(inputJson, "views", errc, false),
        convert_xuid_to_string(utils::extract_json_uint52(inputJson, "xuid", errc, false), utils::extract_json_string(inputJson, _T("xboxUserId"), errc, false)),
        utils::extract_json_vector<contextual_search_game_clip_stat>(contextual_search_game_clip_stat::_Deserialize, inputJson, _T("stats"), errc, false)
        );

    return xbox_live_result<contextual_search_game_clip>(result, errc);
}

const string_t& contextual_search_game_clip::clip_name() const
{
    return m_clipName;
}

uint64_t contextual_search_game_clip::duration_in_seconds() const
{
    return m_durationInSeconds;
}

const string_t& contextual_search_game_clip::game_clip_id() const
{
    return m_gameClipId;
}

const string_t& contextual_search_game_clip::game_clip_locale() const
{
    return m_gameClipLocale;
}

const std::vector<contextual_search_game_clip_uri_info>& contextual_search_game_clip::game_clip_uris() const
{
    return m_gameClipUris;
}

const std::vector<contextual_search_game_clip_thumbnail>& contextual_search_game_clip::thumbnails() const
{
    return m_thumbnails;
}

contextual_search_game_clip_type contextual_search_game_clip::game_clip_type() const
{
    return m_gameClipType;
}

uint64_t contextual_search_game_clip::views() const
{
    return m_views;
}

const string_t& contextual_search_game_clip::xbox_user_id() const
{
    return m_xboxUserId;
}

const std::vector<contextual_search_game_clip_stat>& contextual_search_game_clip::stats() const
{
    return m_stats;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END
