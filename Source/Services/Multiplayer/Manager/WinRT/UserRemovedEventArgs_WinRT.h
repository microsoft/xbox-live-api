// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    /// <summary>
    /// Internal function.
    /// </summary>
    UserRemovedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::user_removed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::user_removed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::user_removed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
