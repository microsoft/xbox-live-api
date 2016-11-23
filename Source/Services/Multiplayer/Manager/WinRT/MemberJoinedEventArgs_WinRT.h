//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once 
#include "MultiplayerEventArgs_WinRT.h"
#include "MultiplayerMember_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Notifies the title when a new game member joins the game. 
/// </summary>
public ref class MemberJoinedEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// A list of members that joined the game.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<MultiplayerMember^>^ Members
    {
        Windows::Foundation::Collections::IVectorView<MultiplayerMember^>^ get();
    }

internal:
    MemberJoinedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::member_joined_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::member_joined_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::member_joined_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END