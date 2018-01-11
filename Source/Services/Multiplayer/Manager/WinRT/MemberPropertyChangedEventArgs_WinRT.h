// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "MultiplayerEventArgs_WinRT.h"
#include "MultiplayerMember_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Notifies the title when a game member property has been added or modified.
/// </summary>
public ref class MemberPropertyChangedEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// The member whose property changed.
    /// </summary>
    property MultiplayerMember^ Member
    {
        MultiplayerMember^ get();
    }

    /// <summary>
    /// The JSON of the property that changed.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(Properties, properties);

internal:
    /// <summary>
    /// Internal function.
    /// </summary>
    MemberPropertyChangedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::member_property_changed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::member_property_changed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::member_property_changed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
