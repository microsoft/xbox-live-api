// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplayerPeerToHostRequirements_WinRT.h"
#include "Utils_WinRT.h"

using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerPeerToHostRequirements::MultiplayerPeerToHostRequirements(
    _In_ xbox::services::multiplayer::multiplayer_peer_to_host_requirements cppObj
    ) :
    m_cppObj(cppObj)
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END