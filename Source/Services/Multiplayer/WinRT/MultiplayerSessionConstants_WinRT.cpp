// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "MultiplayerSessionConstants_WinRT.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionConstants::MultiplayerSessionConstants(
    _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session_constants> cppObj
    ) :
    m_cppObj(cppObj)
{
    THROW_INVALIDARGUMENT_IF_NULL(m_cppObj);
}

IVectorView<Platform::String^>^
MultiplayerSessionConstants::InitiatorXboxUserIds::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj->initiator_xbox_user_ids())->GetView();
}

MultiplayerManagedInitialization^
MultiplayerSessionConstants::ManagedInitialization::get()
{
#pragma warning(suppress: 4996)
    return ref new MultiplayerManagedInitialization(m_cppObj->managed_initialization());
}

MultiplayerMemberInitialization^
MultiplayerSessionConstants::MemberInitialization::get()
{
    return ref new MultiplayerMemberInitialization(m_cppObj->member_initialization());
}

MultiplayerPeerToPeerRequirements^
MultiplayerSessionConstants::PeerToPeerRequirements::get()
{
    return ref new MultiplayerPeerToPeerRequirements(m_cppObj->peer_to_peer_requirements());
}

MultiplayerPeerToHostRequirements^
MultiplayerSessionConstants::PeerToHostRequirements::get()
{
    return ref new MultiplayerPeerToHostRequirements(m_cppObj->peer_to_host_requirements());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END