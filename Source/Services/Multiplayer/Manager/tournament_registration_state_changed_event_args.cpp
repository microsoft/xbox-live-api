// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

tournament_registration_state_changed_event_args::tournament_registration_state_changed_event_args(
    _In_ tournament_registration_state state,
    _In_ tournament_registration_reason reason
    ):
    m_registrationState(state),
    m_registrationReason(reason)
{
}

tournament_registration_state
tournament_registration_state_changed_event_args::registration_state() const
{
    return m_registrationState;
}

tournament_registration_reason
tournament_registration_state_changed_event_args::registration_reason() const
{
    return m_registrationReason;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END