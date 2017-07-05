// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

host_changed_event_args::host_changed_event_args(
    _In_ std::shared_ptr<multiplayer_member> hostMember
    ):
    m_hostMember(std::move(hostMember))
{
}

std::shared_ptr<multiplayer_member>
host_changed_event_args::host_member()
{
    return m_hostMember;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END