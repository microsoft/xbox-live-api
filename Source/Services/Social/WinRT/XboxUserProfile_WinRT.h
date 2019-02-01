// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/profile.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN
/// <summary> 
/// Represents a user's Xbox Live profile.
/// </summary>

public ref class XboxUserProfile sealed
{
public:
    /// <summary>
    /// The user's display name to be used in application UI.  This value is privacy gated and could
    /// be a user's real name or their gamertag.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ApplicationDisplayName, app_display_name)

    /// <summary>
    /// Uri for the user's display picture to be used in application UI.
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending '&amp;format={format}&amp;w={width}&amp;h={height}:
    /// Format: png
    /// Width   Height
    /// 64      64
    /// 208     208
    /// 424     424
    /// </summary>
    DEFINE_PROP_GET_URI_OBJ(ApplicationDisplayPictureResizeUri, app_display_picture_resize_uri)

    /// <summary>
    /// The user's display name to be used in game UI.  This value is privacy gated and could
    /// be a user's real name or their gamertag.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(GameDisplayName, game_display_name)

    /// <summary>
    /// Uri for the user's display picture to be used in games.
    /// The Uri is a resizable Uri. It can be used to specify one of the following sizes and formats by appending '&amp;format={format}&amp;w={width}&amp;h={height}:
    /// Format: png
    /// Width   Height
    /// 64      64
    /// 208     208
    /// 424     424
    /// </summary>
    DEFINE_PROP_GET_URI_OBJ(GameDisplayPictureResizeUri, game_display_picture_resize_uri)

    /// <summary>
    /// The user's gamerscore.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Gamerscore, gamerscore)

    /// <summary>
    /// The user's gamertag.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Gamertag, gamertag)

    /// <summary>
    /// The user's Xbox user ID.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id)

    /// <summary>
    /// Is narrator enabled.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsNarratorEnabled, narratorEnabled, bool)

internal:
    XboxUserProfile(
        _In_ xbox::services::social::xbox_user_profile cppObj
        );

private:
    xbox::services::social::xbox_user_profile m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END