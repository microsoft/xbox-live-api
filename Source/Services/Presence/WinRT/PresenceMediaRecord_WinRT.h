// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/presence.h"
#include "PresenceMediaIdType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN
/// <summary>
/// Represents a media record for Rich Presence. 
/// </summary>

public ref class PresenceMediaRecord sealed
{
public:
    /// <summary>
    /// ID of the media used by the Bing catalog or the provider catalog. 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(MediaId, media_id);

    /// <summary>
    /// The ID type of the media.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(MediaIdType, media_id_type, PresenceMediaIdType);

    /// <summary>
    /// Localized name of the media content.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

internal:
    PresenceMediaRecord(_In_ xbox::services::presence::presence_media_record cppObj);

private:
    xbox::services::presence::presence_media_record m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END