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

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Enum for emphasizing Xbox Live features your title has.
/// </summary>
public enum class SignInUIEmphasisFeature
{
    /// <summary>
    /// Enum for emphasizing Xbox Live achievement feature.
    /// </summary>
    Achievements = xbox::services::signin_ui_settings::emphasis_feature::achievements,

    /// <summary>
    /// Enum for emphasizing Xbox Live connected storage feature.
    /// </summary>
    ConnectedStorage = xbox::services::signin_ui_settings::emphasis_feature::connected_storage,

    /// <summary>
    /// Enum for emphasizing Xbox Live find players feature.
    /// </summary>
    FindPlayers = xbox::services::signin_ui_settings::emphasis_feature::find_players,

    /// <summary>
    /// Enum for emphasizing Xbox Live game bar feature.
    /// </summary>
    GameBar = xbox::services::signin_ui_settings::emphasis_feature::game_bar,

    /// <summary>
    /// Enum for emphasizing Xbox Live game DVR feature.
    /// </summary>
    GameDVR = xbox::services::signin_ui_settings::emphasis_feature::game_dvr,

    /// <summary>
    /// Enum for emphasizing Xbox Live leaderboards feature.
    /// </summary>
    Leaderboards = xbox::services::signin_ui_settings::emphasis_feature::leaderboards,

    /// <summary>
    /// Enum for emphasizing Xbox Live multiplayer feature.
    /// </summary>
    Multiplayer = xbox::services::signin_ui_settings::emphasis_feature::multiplayer,

    /// <summary>
    /// Enum for emphasizing Xbox Live purchase feature.
    /// </summary>
    Purchase = xbox::services::signin_ui_settings::emphasis_feature::purchase,

    /// <summary>
    /// Enum for emphasizing Xbox Live shared content feature.
    /// </summary>
    SharedContent = xbox::services::signin_ui_settings::emphasis_feature::shared_content,

    /// <summary>
    /// Enum for emphasizing Xbox Live social feature.
    /// </summary>
    Social = xbox::services::signin_ui_settings::emphasis_feature::social,

    /// <summary>
    /// Enum for emphasizing Xbox Live tournaments feature.
    /// </summary>
    Tournaments = xbox::services::signin_ui_settings::emphasis_feature::tournaments,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END