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
#include "MultiplayerRoleInfo_WinRT.h"
#include "Macros_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerRoleInfo::MultiplayerRoleInfo()
{
}

MultiplayerRoleInfo::MultiplayerRoleInfo(
    _In_ xbox::services::multiplayer::multiplayer_role_info cppObj
) :
    m_cppObj(std::move(cppObj))
{
    m_memberXboxUserIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.member_xbox_user_ids());
}

xbox::services::multiplayer::multiplayer_role_info
MultiplayerRoleInfo::GetCppObj() const
{
    return m_cppObj;
}

IVectorView<Platform::String^>^
MultiplayerRoleInfo::MemberXboxUserIds::get()
{
    return m_memberXboxUserIds->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END