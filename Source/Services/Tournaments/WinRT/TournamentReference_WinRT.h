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
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents a reference to a multiplayer session.
/// </summary>
public ref class TournamentReference sealed
{
public:
    TournamentReference();

    /// <summary>
    /// The definition name of the tournament.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DefinitionName, definition_name);

    /// <summary>
    /// The tournament ID specific to the tournament.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(TournamentId, tournament_id);

    /// <summary>
    /// The organizer name of the tournament.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Organizer, organizer);

    /// <summary>
    /// The service configuration ID specific to the tournament.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

internal:
    TournamentReference(
        _In_ xbox::services::tournaments::tournament_reference cppObj
        );

    const xbox::services::tournaments::tournament_reference& GetCppObj() const;
private:
    xbox::services::tournaments::tournament_reference m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
