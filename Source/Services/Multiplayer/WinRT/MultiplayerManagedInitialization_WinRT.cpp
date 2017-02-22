// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "Utils_WinRT.h"
#include "MultiplayerManagedInitialization_WinRT.h"

using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerManagedInitialization::MultiplayerManagedInitialization(
    _In_ xbox::services::multiplayer::multiplayer_managed_initialization cppObj
    ) :
    m_cppObj(cppObj)
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END