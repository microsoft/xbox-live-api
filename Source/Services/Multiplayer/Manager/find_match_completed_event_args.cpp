// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"
using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

find_match_completed_event_args::find_match_completed_event_args(
    _In_ xbox::services::multiplayer::manager::match_status status,
    _In_ multiplayer_measurement_failure failure
    ):
    m_matchStatus(status),
    m_initializationFailure(failure)
{
}

xbox::services::multiplayer::manager::match_status
find_match_completed_event_args::match_status() const
{
    return m_matchStatus;
}

multiplayer_measurement_failure
find_match_completed_event_args::initialization_failure_cause() const
{
    return m_initializationFailure;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END