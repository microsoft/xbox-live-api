// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MemberPropertyChangedEventArgs_WinRT.h"

using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

MemberPropertyChangedEventArgs::MemberPropertyChangedEventArgs(
    _In_ std::shared_ptr<member_property_changed_event_args> cppObj
    ) :
    m_cppObj(cppObj)
{
    XSAPI_ASSERT(cppObj != nullptr);
}

std::shared_ptr<member_property_changed_event_args>
MemberPropertyChangedEventArgs::GetCppObj() const
{
    return m_cppObj;
}

MultiplayerMember^
MemberPropertyChangedEventArgs::Member::get()
{
    return ref new MultiplayerMember(m_cppObj->member());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END