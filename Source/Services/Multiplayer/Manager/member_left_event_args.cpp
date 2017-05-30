// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

member_left_event_args::member_left_event_args(
    _In_ std::vector<std::shared_ptr<multiplayer_member>> members
    ):
    m_members(std::move(members))
{
}

std::vector<std::shared_ptr<multiplayer_member>>
member_left_event_args::members()
{
    return m_members;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END