// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/leaderboard.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

/// <summary>
/// Represents a row in a collection of leaderboard items.
/// </summary>
public ref class LeaderboardRow sealed
{
public:
    
    /// <summary>
    /// The Gamertag of the player.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Gamertag, gamertag)

    /// <summary>
    /// The Xbox user ID of the player.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id)

    /// <summary>
    /// The percentile rank of the player.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Percentile, percentile, double);

    /// <summary>
    /// The rank of the player.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Rank, rank, uint32);

    /// <summary>
    /// Values for each column in the leaderboard row for the player.
    /// </summary>        
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Values
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

internal:
    LeaderboardRow(
        _In_ xbox::services::leaderboard::leaderboard_row cppObj
        );

private:
    xbox::services::leaderboard::leaderboard_row m_cppObj;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_values;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END