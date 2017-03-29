// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "..\GameLogic\Multiplayer.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;

#pragma region Sample Helper Methods

int32_t Sample::GetRandomizedGameModeIndex()
{
    UINT mode = rand() % ARRAYSIZE(g_gameModeStrings);
    while (mode == m_gameModeIndex)
    {
        // Test code just to ensure we are setting new properties everytime.
        mode = rand() % ARRAYSIZE(g_gameModeStrings);
    }

    m_gameModeIndex = mode;
    return m_gameModeIndex;
}

int32_t Sample::GetRandomizedMapIndex()
{
    UINT mode = rand() % ARRAYSIZE(g_mapStrings);
    while (mode == m_mapIndex)
    {
        // Test code just to ensure we are setting new properties everytime.
        mode = rand() % ARRAYSIZE(g_mapStrings);
    }

    m_mapIndex = mode;
    return m_mapIndex;
}

joinability Sample::GetRandJoinabilityValue()
{
    joinability joinabilityVal = joinability::joinable_by_friends;
    if (m_multiplayerManager->joinability() == joinability::joinable_by_friends)
    {
        joinabilityVal = joinability::invite_only;
    }
    else if (m_multiplayerManager->joinability() == joinability::invite_only)
    {
        joinabilityVal = joinability::disable_while_game_in_progress;
    }
    else if (m_multiplayerManager->joinability() == joinability::disable_while_game_in_progress)
    {
        joinabilityVal = joinability::closed;
    }

    return joinabilityVal;
}

string_t Sample::CreateGuid()
{
#ifdef _WIN32
    GUID guid = { 0 };
    if (FAILED(CoCreateGuid(&guid)))
    {
        throw std::runtime_error("Failed creating a guid");
    }

    WCHAR wszGuid[50];
    if (FAILED(::StringFromGUID2(guid, wszGuid, ARRAYSIZE(wszGuid))))
    {
        throw std::runtime_error("Failed creating a guid");
    }

    string_t strGuid = wszGuid;
#else
    uuid_t uuid;
    uuid_generate_random(uuid);
    char s[37] = { 0 };
    uuid_unparse(uuid, s);
    string_t strGuid = s;
#endif

    if (strGuid.length() > 3 && strGuid[0] == L'{')
    {
        // Remove the { } 
        strGuid.erase(0, 1);
        strGuid.erase(strGuid.end() - 1, strGuid.end());
    }

    return strGuid;
}
#pragma endregion