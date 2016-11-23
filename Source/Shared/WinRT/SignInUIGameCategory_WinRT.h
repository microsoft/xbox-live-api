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
/// Enum for current game category, sign in UI will adjust accordingly based on 
/// your game category.
/// </summary>
public enum class SignInUIGameCategory
{
    /// <summary>
    /// Enum for setting current title as standard game category.
    /// </summary>
    Standard = xbox::services::signin_ui_settings::game_category::standard,

    /// <summary>
    /// Enum for setting current title as casual game category
    /// </summary>
    Casual = xbox::services::signin_ui_settings::game_category::casual,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END