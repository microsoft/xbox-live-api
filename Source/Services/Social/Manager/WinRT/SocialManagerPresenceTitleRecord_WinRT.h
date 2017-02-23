// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social_manager.h"
#include "PresenceDeviceType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

public ref class SocialManagerPresenceTitleRecord sealed
{
public:
    /// <summary>
    /// The title ID.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsTitleActive, is_title_active, bool);

    /// <summary>
    /// The active state for the title.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TitleId, title_id, uint32_t);

    /// <summary>
    /// The formatted and localized presence string.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(PresenceText, presence_text);

    /// <summary>
    /// The active state for the title.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsBroadcasting, is_broadcasting, bool);

    DEFINE_PROP_GET_ENUM_OBJ(DeviceType, device_type, Microsoft::Xbox::Services::Presence::PresenceDeviceType);

internal:
    SocialManagerPresenceTitleRecord(
        _In_ xbox::services::social::manager::social_manager_presence_title_record cppObj
        );

private:
    xbox::services::social::manager::social_manager_presence_title_record m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END