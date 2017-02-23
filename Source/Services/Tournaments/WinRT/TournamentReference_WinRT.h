// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
