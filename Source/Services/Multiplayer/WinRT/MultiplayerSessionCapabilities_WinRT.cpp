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
#include "MultiplayerSessionCapabilities_WinRT.h"
#include "Utils_WinRT.h"

using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionCapabilities::MultiplayerSessionCapabilities(
    _In_ xbox::services::multiplayer::multiplayer_session_capabilities cppObj
    ) :
    m_cppObj(cppObj)
{
}

MultiplayerSessionCapabilities::MultiplayerSessionCapabilities()
{
    m_cppObj = xbox::services::multiplayer::multiplayer_session_capabilities();
}

xbox::services::multiplayer::multiplayer_session_capabilities 
MultiplayerSessionCapabilities::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END