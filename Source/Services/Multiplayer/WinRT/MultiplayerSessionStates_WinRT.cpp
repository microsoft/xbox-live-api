// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "MultiplayerSessionStates_WinRT.h"
#include "Utils_WinRT.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionStates::MultiplayerSessionStates(
    _In_ xbox::services::multiplayer::multiplayer_session_states cppObj
    ) :
    m_cppObj(cppObj)
{
    m_sessionReference = ref new MultiplayerSessionReference(m_cppObj.session_reference());
    m_keywords = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.keywords())->GetView();
}

MultiplayerSessionReference^
MultiplayerSessionStates::SessionReference::get()
{
    return m_sessionReference;
}

IVectorView<Platform::String^>^
MultiplayerSessionStates::Keywords::get()
{
    return m_keywords;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END