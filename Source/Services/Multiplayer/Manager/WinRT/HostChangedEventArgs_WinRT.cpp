// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "HostChangedEventArgs_WinRT.h"

using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

HostChangedEventArgs::HostChangedEventArgs(
    _In_ std::shared_ptr<host_changed_event_args> cppObj
    ) :
    m_cppObj(cppObj)
{
    XSAPI_ASSERT(cppObj != nullptr);
}

std::shared_ptr<host_changed_event_args>
HostChangedEventArgs::GetCppObj() const
{
    return m_cppObj;
}

MultiplayerMember^
HostChangedEventArgs::HostMember::get()
{
    return ref new MultiplayerMember(m_cppObj->host_member());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END