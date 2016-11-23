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
/// Notifies the title when a new host member has been set. 
/// </summary>
public ref class HostChangedEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// The new host member. If an existing host leaves, the HostMember will be nullptr.
    /// </summary>
    property MultiplayerMember^ HostMember
    {
        MultiplayerMember^ get();
    }

internal:
    HostChangedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::host_changed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::host_changed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::host_changed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END