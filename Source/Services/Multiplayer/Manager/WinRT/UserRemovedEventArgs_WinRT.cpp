// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UserRemovedEventArgs_WinRT.h"

using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

UserRemovedEventArgs::UserRemovedEventArgs(
    _In_ std::shared_ptr<user_removed_event_args> cppObj
    ) :
    m_cppObj(cppObj)
{
    XSAPI_ASSERT(cppObj != nullptr);
}

std::shared_ptr<user_removed_event_args>
UserRemovedEventArgs::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END