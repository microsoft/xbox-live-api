// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameData.h"
#include "Game.h"
#include "Utils\Utils.h"

using namespace Sample;
using namespace xbox::services::social::manager;

GameData::GameData() :
    m_appState(APP_IN_GAME),
    m_gameState(GAME_PLAY),
    m_heartbeatTimer(0.0f),
    m_userSelectedIndex(0),
    m_activitySelectedIndex(0)
{
}

GAMESTATE GameData::GetGameState()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_gameState;
}

APPSTATE GameData::GetAppState()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_appState;
}

void GameData::SetGameState(GAMESTATE gameState)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_gameState = gameState;
}

void GameData::SetState( APPSTATE appState, GAMESTATE gameState )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_appState = appState;
    m_gameState = gameState;
}

void GameData::SetStateResetCritical( APPSTATE appState, GAMESTATE gameState )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);

    m_appState = appState;
    m_gameState = gameState;
}


void GameData::SetDisplayString( HRESULT hr, Platform::String^ text )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    if( hr != S_OK )
    {
        m_displayString = text + Utils::GetErrorString(hr);
    }
    else
    {
        m_displayString = text;
    }
}

Platform::String^ GameData::GetDisplayString()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_displayString;
}

INT GameData::GetViewSelectedIndex()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_selectedIndex;
}

size_t GameData::GetActivitySelectedIndex()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_activitySelectedIndex;
}

void GameData::SetActivitySelectedIndex( size_t newIndex )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_activitySelectedIndex = newIndex;
}

void GameData::SetViewSelectedIndex(INT newIndex)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_selectedIndex = newIndex;
}

float GameData::GetViewGameDataOffsetX()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_currentGameViewOffsetX;
}

void GameData::SetViewGameDataOffsetX(float targetX)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_currentGameViewOffsetX = targetX;
}

float GameData::GetViewDebugOutputOffsetX()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_currentDebugOutputOffsetX;
}

void GameData::SetViewDebugOutputOffsetX(float targetX)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_currentDebugOutputOffsetX = targetX;
}

float GameData::GetViewCurrentCountersOffsetY()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_currentCountersOffsetY;
}

void GameData::SetViewCurrentCountersOffsetY(float targetY)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_currentCountersOffsetY = targetY;
}

bool GameData::GetViewDebug()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_viewDebug;
}

void GameData::SetViewDebug(bool viewDebug)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_viewDebug = viewDebug;
}

UINT GameData::GetUserSelectedIndex()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_userSelectedIndex;
}

void GameData::SetUserSelectedIndex( UINT newIndex )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_userSelectedIndex = newIndex;
}

