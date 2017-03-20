// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    /// A label that describes the match. The string provides organizers 
    /// the flexibility to label the match with richer text such as "Final" or "Heat 1".
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Description, description);

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
