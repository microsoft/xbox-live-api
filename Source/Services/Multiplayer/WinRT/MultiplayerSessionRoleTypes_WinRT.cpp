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
#include "MultiplayerSessionRoleTypes_WinRT.h"
#include "Macros_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionRoleTypes::MultiplayerSessionRoleTypes(
    _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session_role_types> cppObj
) :
    m_cppObj(std::move(cppObj))
{
    m_roleTypes = UtilsWinRT::CreatePlatformMapObjectWithStringKeyFromStdMapObj<MultiplayerRoleType>(m_cppObj->role_types());
}

Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleType^>^
MultiplayerSessionRoleTypes::RoleTypes::get()
{
    return m_roleTypes->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
