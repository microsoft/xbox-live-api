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
    MemberPropertyChangedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::member_property_changed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::member_property_changed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::member_property_changed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END