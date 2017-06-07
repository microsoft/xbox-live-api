//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#include "pch.h"
#include "GameLogic\Multiplayer.h"
#include "Utils\PerformanceCounters.h"

using namespace Concurrency;
using namespace Windows::Foundation::Collections;
using namespace Windows::Xbox::ApplicationModel;
using namespace Windows::Xbox::System;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;

void Sample::PublishSearchHandle()
{
    auto searchHandleReq = multiplayer_search_handle_request(m_multiplayerManager->game_session()->session_reference());

    std::vector<string_t> tags;
    tags.push_back(L"micsrequired");

    std::unordered_map<string_t, double> numbersMetadata;
    numbersMetadata[L"skill"] = 10;

    std::unordered_map<string_t, string_t> stringMetadata;
    stringMetadata[L"mode"] = L"deathmatch";

    searchHandleReq.set_tags(tags);
    searchHandleReq.set_numbers_metadata(numbersMetadata);
    searchHandleReq.set_strings_metadata(stringMetadata);

    auto context = m_liveResources->GetLiveContext();
    auto result = context->multiplayer_service().set_search_handle(searchHandleReq)
    .then([this](xbox_live_result<void> result)
    {
        if (result.err())
        {
            LogErrorFormat( L"PublishSearchHandle failed: %S\n", result.err_message().c_str() );
        }
    });
}

void Sample::BrowseSearchHandles()
{
    m_searchHandles = std::vector<multiplayer_search_handle_details>();
    auto context = m_liveResources->GetLiveContext();
    context->multiplayer_service().get_search_handles(GAME_SERVICE_CONFIG_ID, GAME_SESSION_TEMPLATE_NAME, L"", true, L"tolower(strings/mode) eq 'deathmatch'")
    .then([this](xbox_live_result<std::vector<multiplayer_search_handle_details>> searchHandles)
    {
        if (searchHandles.err())
        {
            LogErrorFormat( L"BrowseSearchHandles failed: %S\n", searchHandles.err_message().c_str() );
        }
        else
        {
            m_searchHandles = searchHandles.payload();

            // Join the game session

            auto handleId = m_searchHandles.at(0).handle_id();
            auto sessionRef = multiplayer_session_reference(m_searchHandles.at(0).session_reference());
            auto gameSession = std::make_shared<multiplayer_session>(m_liveResources->GetLiveContext()->xbox_live_user_id(), sessionRef);
            gameSession->join(web::json::value::null(), false, false, false);

            m_liveResources->GetLiveContext()->multiplayer_service().write_session_by_handle(gameSession, multiplayer_session_write_mode::update_existing, handleId)
            .then([this, sessionRef](xbox_live_result<std::shared_ptr<multiplayer_session>> writeResult)
            {
                if (!writeResult.err())
                {
                    // Join the game session via MPM
                    m_multiplayerManager->join_game(sessionRef.session_name(), sessionRef.session_template_name());
                }
            });
        }
    });
}

void Sample::SetRoleInfo()
{
    auto sessionRef = multiplayer_session_reference(m_multiplayerManager->lobby_session()->session_reference());
    auto latestLobbySession = std::make_shared<multiplayer_session>(m_liveResources->GetLiveContext()->xbox_live_user_id(), sessionRef);
    latestLobbySession->join(web::json::value::null(), false, false, false);

    std::unordered_map<string_t, string_t> roleInfo;
    roleInfo[L"playerclass"] = L"assault";
    roleInfo[L"admin"] = L"super";

    latestLobbySession->set_current_user_role_info(roleInfo);
    m_liveResources->GetLiveContext()->multiplayer_service().write_session(latestLobbySession, multiplayer_session_write_mode::update_existing)
    .then([this](xbox_live_result<std::shared_ptr<multiplayer_session>> writeResult)
    {
        return xbox_live_result<void>(writeResult.err(), writeResult.err_message());

    });
}