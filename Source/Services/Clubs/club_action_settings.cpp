// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

club_role club_action_setting::required_role() const
{
    return m_requiredRole;
}

bool club_action_setting::can_viewer_act() const
{
    return m_canViewerAct;
}

const std::vector<club_role>& club_action_setting::allowed_values() const
{
    return m_allowedValues;
}

bool club_action_setting::can_viewer_change_setting() const
{
    return m_canViewerChangeSetting;
}

const club_action_setting& club_action_settings::post_to_feed() const
{
    return m_postToFeedSetting;
}

const club_action_setting& club_action_settings::view_feed() const
{
    return m_viewFeedSetting;
}

const club_action_setting& club_action_settings::write_in_chat() const
{
    return m_writeInChatSetting;
}

const club_action_setting& club_action_settings::view_chat() const
{
    return m_viewChatSetting;
}

const club_action_setting& club_action_settings::set_chat_topic() const
{
    return m_setChatTopicSetting;
}

const club_action_setting& club_action_settings::join_looking_for_game() const
{
    return m_joinLFGSetting;
}

const club_action_setting& club_action_settings::create_looking_for_game() const
{
    return m_createLFGSetting;
}

const club_action_setting& club_action_settings::view_looking_for_game() const
{
    return m_viewLFGSetting;
}

const club_action_setting& club_action_settings::invite_or_accept_join_requests() const
{
    return m_inviteOrAcceptJoinRequestsSetting;
}

const club_action_setting& club_action_settings::kick_or_ban() const
{
    return m_kickOrBanSetting;
}

const club_action_setting& club_action_settings::view_roster() const
{
    return m_viewRosterSetting;
}

const club_action_setting& club_action_settings::update_profile() const
{
    return m_updateProfileSetting;
}

const club_action_setting& club_action_settings::delete_profile() const
{
    return m_deleteProfileSetting;
}

const club_action_setting& club_action_settings::view_profile() const
{
    return m_viewProfileSetting;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END