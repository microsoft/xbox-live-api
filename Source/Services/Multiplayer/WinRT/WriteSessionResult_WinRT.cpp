// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "WriteSessionResult_WinRT.h"

using namespace xbox::services;
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

WriteSessionResult::WriteSessionResult(
    _In_ bool succeeded,
    _In_opt_ MultiplayerSession^ session,
    _In_ WriteSessionStatus writeSessionStatus
    ) :
    m_succeeded(succeeded),
    m_session(session),
    m_status(writeSessionStatus)
{
}

bool
WriteSessionResult::Succeeded::get()
{
    return m_succeeded;
}

MultiplayerSession^
WriteSessionResult::Session::get()
{
    return m_session;
}

WriteSessionStatus
WriteSessionResult::Status::get()
{
    return m_status;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END