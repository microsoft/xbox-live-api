//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_change_event_args::multiplayer_session_change_event_args(
    _In_ multiplayer_session_reference sessionRef,
    _In_ string_t branch,
    _In_ uint64_t changeNumber
    ) :
    m_sessionReference(std::move(sessionRef)),
    m_branch(std::move(branch)),
    m_changeNumber(changeNumber)
{
}

const multiplayer_session_reference& 
multiplayer_session_change_event_args::session_reference() const
{ 
    return m_sessionReference;
}

const string_t&
multiplayer_session_change_event_args::branch() const
{
    return m_branch;
}

uint64_t
multiplayer_session_change_event_args::change_number() const
{
    return m_changeNumber;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END