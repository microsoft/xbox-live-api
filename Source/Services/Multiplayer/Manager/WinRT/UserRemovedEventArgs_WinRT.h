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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Notifies the title when a user was removed.
/// </summary>
public ref class UserRemovedEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// Xbox User ID of the member that that was removed.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

internal:
    UserRemovedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::user_removed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::user_removed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::user_removed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END