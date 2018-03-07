// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

title_presence_change_event_args::title_presence_change_event_args(
    _In_ std::shared_ptr<title_presence_change_event_args_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(title_presence_change_event_args, xbox_user_id);
DEFINE_GET_UINT32(title_presence_change_event_args, title_id);
DEFINE_GET_ENUM_TYPE(title_presence_change_event_args, title_presence_state, title_state);

title_presence_change_event_args_internal::title_presence_change_event_args_internal() :
    m_titleId(0),
    m_titleState(title_presence_state::unknown)
{
}

title_presence_change_event_args_internal::title_presence_change_event_args_internal(
    _In_ xsapi_internal_string xboxUserId,
    _In_ uint32_t titleId,
    _In_ title_presence_state titleState
    ) :
    m_xboxUserId(xboxUserId.c_str()),
    m_titleId(titleId),
    m_titleState(titleState)
{
}

const xsapi_internal_string&
title_presence_change_event_args_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

uint32_t
title_presence_change_event_args_internal::title_id() const
{
    return m_titleId;
}

title_presence_state
title_presence_change_event_args_internal::title_state() const
{
    return m_titleState;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END