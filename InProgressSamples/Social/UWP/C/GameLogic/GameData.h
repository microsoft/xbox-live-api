// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <queue>
#include <atomic>

enum APPSTATE
{
    // Playing the game
    APP_IN_GAME,

    // Done states
    APP_DONE,
    APP_CRITICAL_ERROR
};

enum GAMESTATE
{
    GAME_INIT,
    GAME_PLAY
};

namespace Sample
{
    class GameData
    {
    public:
        GameData();

        GAMESTATE GetGameState();
        APPSTATE GetAppState();
        void SetGameState(GAMESTATE gameState);
        void SetState(APPSTATE appState, GAMESTATE gameState);
        void SetStateResetCritical(APPSTATE appState, GAMESTATE gameState);

        void SetDisplayString(HRESULT hr, Platform::String^ text);
        Platform::String^ GetDisplayString();

        size_t GetActivitySelectedIndex();
        void SetActivitySelectedIndex(size_t newIndex);

        INT GetViewSelectedIndex();
        void SetViewSelectedIndex(INT newIndex);

        float GetViewGameDataOffsetX();
        void SetViewGameDataOffsetX(float targetX);

        float GetViewDebugOutputOffsetX();
        void SetViewDebugOutputOffsetX(float targetX);

        float GetViewCurrentCountersOffsetY();
        void SetViewCurrentCountersOffsetY(float targetY);

        bool GetViewDebug();
        void SetViewDebug(bool viewDebug);

        UINT GetUserSelectedIndex();
        void SetUserSelectedIndex(UINT newIndex);

    private:
        Concurrency::critical_section m_stateLock;

        APPSTATE m_appState;
        GAMESTATE m_gameState;
        float m_heartbeatTimer;
        Platform::String^ m_displayString;
        size_t m_activitySelectedIndex;
        INT m_selectedIndex;
        UINT m_userSelectedIndex;
        float m_currentGameViewOffsetX;
        float m_currentDebugOutputOffsetX;
        float m_currentCountersOffsetY;
        bool m_viewDebug;
    };
}

