// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
