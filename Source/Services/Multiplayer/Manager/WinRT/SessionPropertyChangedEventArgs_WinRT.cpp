// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SessionPropertyChangedEventArgs_WinRT.h"

using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

SessionPropertyChangedEventArgs::SessionPropertyChangedEventArgs(
    _In_ std::shared_ptr<session_property_changed_event_args> cppObj
    ) :
    m_cppObj(cppObj)
{
    XSAPI_ASSERT(cppObj != nullptr);
}

std::shared_ptr<session_property_changed_event_args>
SessionPropertyChangedEventArgs::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END