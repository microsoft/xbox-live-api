// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_event::multiplayer_event(
    _In_ std::error_code errorCode,
    _In_ std::string errorMessage,
    _In_ multiplayer_event_type eventType,
    _In_ std::shared_ptr<multiplayer_event_args> eventArgs,
    _In_ multiplayer_session_type sessionType,
    _In_opt_ context_t context
    ):
    m_errorCode(std::move(errorCode)),
    m_errorMessage(std::move(errorMessage)),
    m_eventType(eventType),
    m_eventArgs(std::move(eventArgs)),
    m_sessionType(sessionType),
    m_pContext(context)
{
}

const std::error_code&
multiplayer_event::err() const
{
    return m_errorCode;
}

const std::string&
multiplayer_event::err_message() const
{
    return m_errorMessage;
}

context_t
multiplayer_event::context()
{
    return m_pContext;
}

multiplayer_event_type
multiplayer_event::event_type() const
{
    return m_eventType;
}

std::shared_ptr<multiplayer_event_args>
multiplayer_event::event_args()
{
    return m_eventArgs;
}

multiplayer_session_type
multiplayer_event::session_type() const
{
    return m_sessionType;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END