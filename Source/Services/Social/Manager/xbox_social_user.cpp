// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager.h"
#include "social_manager_internal.h"

using namespace xbox::services;
using namespace xbox::services::presence;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

xbox_social_user::xbox_social_user() :
    m_xboxUserIdAsInt(0),
    m_isFavorite(false),
    m_isFollowingCaller(false),
    m_isFollowedByCaller(false),
    m_useAvatar(false)
{
    initialize_char_arr(m_gamerscore);
    initialize_char_arr(m_gamertag);
    initialize_char_arr(m_xboxUserId);
    initialize_char_arr(m_displayName);
    initialize_char_arr(m_realName);
    initialize_char_arr(m_displayPicUrlRaw);
}

const char_t*
xbox_social_user::xbox_user_id() const
{
    return m_xboxUserId;
}

uint64_t
xbox_social_user::_Xbox_user_id_as_integer() const
{
    return m_xboxUserIdAsInt;
}

bool
xbox_social_user::is_favorite() const
{
    return m_isFavorite;
}

bool
xbox_social_user::is_following_user() const
{
    return m_isFollowingCaller;
}

bool
xbox_social_user::is_followed_by_caller() const
{
    return m_isFollowedByCaller;
}

const char_t*
xbox_social_user::display_name() const
{
    return m_displayName;
}

const char_t*
xbox_social_user::real_name() const
{
    return m_realName;
}

const char_t*
xbox_social_user::display_pic_url_raw() const
{
    return m_displayPicUrlRaw;
}

bool
xbox_social_user::use_avatar() const
{
    return m_useAvatar;
}

const char_t*
xbox_social_user::gamerscore() const
{
    return m_gamerscore;
}

const char_t*
xbox_social_user::gamertag() const
{
    return m_gamertag;
}

const social_manager_presence_record&
xbox_social_user::presence_record() const
{
    return m_presenceRecord;
}

void xbox_social_user::_Set_presence_record(
    _In_ const social_manager_presence_record& presenceRecord
    )
{
    m_presenceRecord = presenceRecord;
}

void xbox_social_user::_Set_is_followed_by_caller(
    _In_ bool isFollowed
    )
{
    m_isFollowedByCaller = isFollowed;
}

const xbox::services::social::manager::title_history&
xbox_social_user::title_history() const
{
    return m_titleHistory;
}

const preferred_color&
xbox_social_user::preferred_color() const
{
    return m_preferredColor;
}

change_list_enum xbox_social_user::_Compare(
    _In_ const xbox_social_user& previous,
    _In_ const xbox_social_user& current
    )
{
    change_list_enum changeResult = change_list_enum::no_change;

    if (
        utils::str_icmp(previous.m_gamerscore, current.m_gamerscore) != 0 ||
        previous.m_titleHistory != current.m_titleHistory ||
        utils::str_icmp(previous.m_displayPicUrlRaw, current.m_displayPicUrlRaw) != 0 ||
        previous.m_useAvatar != current.m_useAvatar ||
        utils::str_icmp(previous.m_gamertag, current.m_gamertag) != 0 ||
        utils::str_icmp(previous.m_displayName, current.m_displayName) != 0 ||
        utils::str_icmp(previous.m_realName, current.m_realName) != 0 ||
        previous.m_preferredColor != current.m_preferredColor
        )
    {
        changeResult = static_cast<change_list_enum>(changeResult | change_list_enum::profile_change);
    }
    
    if (
        previous.m_isFollowedByCaller != current.m_isFollowedByCaller ||
        previous.m_isFollowingCaller != current.m_isFollowingCaller ||
        previous.m_isFavorite != current.m_isFavorite
        )
    {
        changeResult = static_cast<change_list_enum>(changeResult | change_list_enum::social_relationship_change);
    }

    if (previous.m_presenceRecord._Compare(current.m_presenceRecord))
    {
        changeResult = static_cast<change_list_enum>(changeResult | change_list_enum::presence_change);
    }

    return changeResult;
}

xbox_live_result<xbox_social_user>
xbox_social_user::_Deserialize(
    _In_ const web::json::value& json
    )
{
    xbox_social_user returnObject;
    if (json.is_null()) return xbox_live_result<xbox_social_user>();

    std::error_code errc = xbox_live_error_code::no_error;
    utils::extract_json_string_to_char_t_array(json, _T("xuid"), errc, returnObject.m_xboxUserId, ARRAYSIZE(returnObject.m_xboxUserId));
    returnObject.m_xboxUserIdAsInt = utils::string_t_to_uint64(returnObject.m_xboxUserId);
    returnObject.m_isFavorite = utils::extract_json_bool(json, _T("isFavorite"), errc);
    returnObject.m_isFollowedByCaller = utils::extract_json_bool(json, _T("isFollowedByCaller"), errc);
    returnObject.m_isFollowingCaller = utils::extract_json_bool(json, _T("isFollowingCaller"), errc);
    utils::extract_json_string_to_char_t_array(json, _T("displayName"), errc, returnObject.m_displayName, ARRAYSIZE(returnObject.m_displayName));
    utils::extract_json_string_to_char_t_array(json, _T("realName"), errc, returnObject.m_realName, ARRAYSIZE(returnObject.m_realName));
    utils::extract_json_string_to_char_t_array(json, _T("displayPicRaw"), errc, returnObject.m_displayPicUrlRaw, ARRAYSIZE(returnObject.m_displayPicUrlRaw));
    returnObject.m_useAvatar = utils::extract_json_bool(json, _T("useAvatar"), errc);
    utils::extract_json_string_to_char_t_array(json, _T("gamertag"), errc, returnObject.m_gamertag, ARRAYSIZE(returnObject.m_gamertag));
    utils::extract_json_string_to_char_t_array(json, _T("gamerScore"), errc, returnObject.m_gamerscore, ARRAYSIZE(returnObject.m_gamerscore));

    returnObject.m_presenceRecord = social_manager_presence_record::_Deserialize(
            json,
            errc
        ).payload();

    returnObject.m_presenceRecord._Set_xbox_user_id(returnObject.m_xboxUserIdAsInt);

    returnObject.m_preferredColor = preferred_color::_Deserialize(
        utils::extract_json_field(
            json,
            _T("preferredColor"),
            errc,
            false
            ),
            errc
        ).payload();
    
    returnObject.m_titleHistory = title_history::_Deserialize(
        utils::extract_json_field(
            json,
            _T("titleHistory"),
            errc,
            false
            ),
            errc
        ).payload();

    return returnObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
