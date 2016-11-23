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
#include "MultiplayerActivityDetails_WinRT.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerActivityDetails::MultiplayerActivityDetails(
    _In_ xbox::services::multiplayer::multiplayer_activity_details cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_sessionReference = ref new MultiplayerSessionReference(m_cppObj.session_reference());
}

MultiplayerSessionReference^ 
MultiplayerActivityDetails::SessionReference::get()
{
    return m_sessionReference;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
