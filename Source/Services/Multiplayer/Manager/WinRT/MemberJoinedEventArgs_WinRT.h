// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    /// <summary>
    /// Internal function.
    /// </summary>
    MemberJoinedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::member_joined_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::member_joined_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::member_joined_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
