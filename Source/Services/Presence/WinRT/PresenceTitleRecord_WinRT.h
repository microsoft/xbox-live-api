// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/presence.h"
#include "PresenceDeviceType_WinRT.h"
#include "PresenceTitleViewState_WinRT.h"
#include "PresenceBroadcastRecord_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN
/// <summary>
/// Represents a record containing data about title presence.
/// </summary>

public ref class PresenceTitleRecord sealed
{
public:
    /// <summary>
    /// The title ID.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TitleId, title_id, uint32);

    /// <summary>
    /// The title name.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(TitleName, title_name);

    /// <summary>
    /// The UTC timestamp when the record was last updated.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(LastModifiedDate, last_modified_date);

    /// <summary>
    /// The active state for the title.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsTitleActive, is_title_active, bool);

    /// <summary>
    /// The formatted and localized presence string.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Presence, presence);

    /// <summary>
    /// The title view state.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(TitleViewState, presence_title_view, PresenceTitleViewState);

    /// <summary>
    /// The broadcast information of what the user is broadcasting. 
    /// </summary>
    property PresenceBroadcastRecord^ BroadcastRecord { PresenceBroadcastRecord^ get(); }

internal:
    PresenceTitleRecord(_In_ xbox::services::presence::presence_title_record cppObj);

private:
    xbox::services::presence::presence_title_record m_cppObj;
    PresenceBroadcastRecord^ m_broadcastRecord;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END