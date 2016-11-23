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