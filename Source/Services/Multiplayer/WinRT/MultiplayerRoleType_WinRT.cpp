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
#include "MultiplayerRoleType_WinRT.h"
#include "Macros_WinRT.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerRoleType::MultiplayerRoleType()
{
}

MultiplayerRoleType::MultiplayerRoleType(
    _In_ xbox::services::multiplayer::multiplayer_role_type cppObj
) :
    m_cppObj(std::move(cppObj))
{
    m_roles = UtilsWinRT::CreatePlatformMapObjectWithStringKeyFromStdMapObj<MultiplayerRoleInfo>(m_cppObj.roles())->GetView();
    
    m_mutableRoleSettings = ref new Platform::Collections::Vector<MutableRoleSetting>();
    for (const auto& setting : m_cppObj.mutable_role_settings())
    {
        m_mutableRoleSettings->Append(static_cast<MutableRoleSetting>(setting));
    }
}

xbox::services::multiplayer::multiplayer_role_type
MultiplayerRoleType::GetCppObj() const
{
    return m_cppObj;
}

IVectorView<MutableRoleSetting>^
MultiplayerRoleType::MutableRoleSettings::get()
{
    return m_mutableRoleSettings->GetView();
}

Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleInfo^>^
MultiplayerRoleType::Roles::get()
{
    return m_roles;
}
void
MultiplayerRoleType::Roles::set(
    _In_ Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleInfo^>^ roles
    )
{
    m_cppObj.set_roles(UtilsWinRT::CreateStdMapObjectWithStringKeyFromPlatformMapObj<xbox::services::multiplayer::multiplayer_role_info>(roles));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
