// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplayerMember_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

MultiplayerMember::MultiplayerMember(
    _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_member> cppObj
    ) :
    m_cppObj(cppObj)
{
    XSAPI_ASSERT(cppObj != nullptr);
}

bool MultiplayerMember::IsMemberOnSameDevice(
    _In_ MultiplayerMember^ member
    )
{
    return m_cppObj->is_member_on_same_device(
        member->GetCppObj()
        );
}

std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_member>
MultiplayerMember::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END