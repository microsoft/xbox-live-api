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
#include "xsapi/tournaments.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents a reference to a multiplayer session.
/// </summary>
public ref class MatchMetadata sealed
{
public:

    /// <summary>
    /// A formatted string that describes the team's standing in the tournament. For example '1W - 0L'.
    /// References a format string and provides values for the substitution tokens.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Label, label);

    /// <summary>
    /// The time this match is scheduled to start. 
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(StartTIme, start_time);

    /// <summary>
    /// True if the match is a bye which means that the opposing team does not play.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsBye, is_bye, bool);

    /// <summary>
    /// The team IDs of all the opposing teams. This should be absent if bye is true.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ OpposingTeamIds
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

internal:
    MatchMetadata(
        _In_ xbox::services::tournaments::match_metadata cppObj
        );

    const xbox::services::tournaments::match_metadata& GetCppObj() const;

private:
    xbox::services::tournaments::match_metadata m_cppObj;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_teamIds;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
