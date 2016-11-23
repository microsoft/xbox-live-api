///*********************************************************
///
/// Copyright (c) Microsoft. All rights reserved.
/// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
/// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
/// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
/// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
///
///*********************************************************
#pragma once
#include "xsapi/social_manager.h"
#include "SocialManagerPresenceRecord_WinRT.h"
#include "TitleHistory_WinRT.h"
#include "PreferredColor_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

/// <summary>
/// Represents a xbox social user containing social information
///</summary>
public ref class XboxSocialUser sealed
{
public:
    /// <summary>
    /// The Xbox user id
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(XboxUserId, xbox_user_id);

    /// <summary>
    /// Whether they are a favorite
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsFavorite, is_favorite, bool);

    /// <summary>
    /// Whether the calling user is following them
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsFollowingUser, is_following_user, bool);

    /// <summary>
    /// Whether they calling user is followed by this person
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsFollowedByCaller, is_followed_by_caller, bool);

    /// <summary>
    /// The display name
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(DisplayName, display_name);

    /// <summary>
    /// The real name
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(RealName, real_name);

    /// <summary>
    /// The display picture uri
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(DisplayPicUrlRaw, display_pic_url_raw);

    /// <summary>
    /// Whether to use the players avatar
    /// </summary>
    DEFINE_PROP_GET_OBJ(UseAvatar, use_avatar, bool);

    /// <summary>
    /// The player's gamertag
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(Gamertag, gamertag);

    /// <summary>
    /// The player's gamerscore
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ_FROM_WCHAR_PTR(Gamerscore, gamerscore);

    /// <summary>
    /// The user's presence record
    /// </summary>
    property Microsoft::Xbox::Services::Social::Manager::SocialManagerPresenceRecord^ PresenceRecord { Microsoft::Xbox::Services::Social::Manager::SocialManagerPresenceRecord^ get(); }

    /// <summary>
    /// Title history for the user
    /// </summary>
    property Microsoft::Xbox::Services::Social::Manager::TitleHistory^ TitleHistory { Microsoft::Xbox::Services::Social::Manager::TitleHistory^ get(); }

    /// <summary>
    /// Preferred color for the user
    /// </summary>
    property Microsoft::Xbox::Services::Social::Manager::PreferredColor^ PreferredColor { Microsoft::Xbox::Services::Social::Manager::PreferredColor^ get(); }

internal:
    XboxSocialUser(
        _In_ xbox::services::social::manager::xbox_social_user* cppObj
        );

private:
    Microsoft::Xbox::Services::Social::Manager::SocialManagerPresenceRecord^ m_presenceRecord;
    Microsoft::Xbox::Services::Social::Manager::PreferredColor^ m_preferredColor;
    Microsoft::Xbox::Services::Social::Manager::TitleHistory^ m_titleHistory;
    xbox::services::social::manager::xbox_social_user m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END