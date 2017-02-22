// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_search_game_clips_result::contextual_search_game_clips_result()
{
}

contextual_search_game_clips_result::contextual_search_game_clips_result(
    _In_ std::vector<contextual_search_game_clip> clips
    ) :
    m_clips(std::move(clips)),
    m_skipItems(0)
{
}

xbox_live_result<contextual_search_game_clips_result> 
contextual_search_game_clips_result::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<contextual_search_game_clips_result>();

    std::error_code errCode = xbox_live_error_code::no_error;

    auto result = contextual_search_game_clips_result(
        utils::extract_json_vector<contextual_search_game_clip>(contextual_search_game_clip::_Deserialize, json, _T("value"), errCode, false)
        );

    return xbox_live_result<contextual_search_game_clips_result>(result, errCode);
}

void contextual_search_game_clips_result::_Initialize(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ uint32_t titleId,
    _In_ uint32_t skipItems
    )
{
    m_userContext = std::move(userContext);
    m_xboxLiveContextSettings = std::move(xboxLiveContextSettings);
    m_appConfig = appConfig;
    m_titleId  = titleId;
    m_skipItems = skipItems;
}

pplx::task<xbox_live_result<contextual_search_game_clips_result>> 
contextual_search_game_clips_result::get_next(
    _In_ uint32_t maxItems
    )
{
    contextual_search_service service(m_userContext, m_xboxLiveContextSettings, m_appConfig);

    return service.get_game_clips_internal(
        m_titleId,
        m_skipItems,
        maxItems,
        _T(""),
        false,
        _T("")
        );
}

const std::vector<contextual_search_game_clip>& contextual_search_game_clips_result::items() const
{
    return m_clips;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END
