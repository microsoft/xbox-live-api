// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubActionSettings_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

using namespace Xbox::Services::System;
using namespace xbox::services::clubs;

ClubActionSettings::ClubActionSettings(_In_ xbox::services::clubs::club_action_settings cppObj)
    : m_cppObj(std::move(cppObj))
{
}

ClubActionSetting^ ClubActionSettings::PostToFeed::get()
{
    if (m_postToFeed == nullptr)
    {
        m_postToFeed = ref new ClubActionSetting(m_cppObj.post_to_feed());
    }
    return m_postToFeed;
}

ClubActionSetting^ ClubActionSettings::ViewFeed::get()
{
    if (m_viewFeed == nullptr)
    {
        m_viewFeed = ref new ClubActionSetting(m_cppObj.view_feed());
    }
    return m_viewFeed;
}

ClubActionSetting^ ClubActionSettings::WriteInChat::get()
{
    if (m_writeInChat == nullptr)
    {
        m_writeInChat = ref new ClubActionSetting(m_cppObj.write_in_chat());
    }
    return m_writeInChat;
}

ClubActionSetting^ ClubActionSettings::ViewChat::get()
{
    if (m_viewChat == nullptr)
    {
        m_viewChat = ref new ClubActionSetting(m_cppObj.view_chat());
    }
    return m_viewChat;
}

ClubActionSetting^ ClubActionSettings::SetChatTopic::get()
{
    if (m_setChatTopic == nullptr)
    {
        m_setChatTopic = ref new ClubActionSetting(m_cppObj.set_chat_topic());
    }
    return m_setChatTopic;
}

ClubActionSetting^ ClubActionSettings::JoinLookingForGame::get()
{
    if (m_joinLFG == nullptr)
    {
        m_joinLFG = ref new ClubActionSetting(m_cppObj.join_looking_for_game());
    }
    return m_joinLFG;
}

ClubActionSetting^ ClubActionSettings::CreateLookingForGame::get()
{
    if (m_createLFG == nullptr)
    {
        m_createLFG = ref new ClubActionSetting(m_cppObj.create_looking_for_game());
    }
    return m_createLFG;
}

ClubActionSetting^ ClubActionSettings::ViewLookingForGame::get()
{
    if (m_viewLFG == nullptr)
    {
        m_viewLFG = ref new ClubActionSetting(m_cppObj.view_looking_for_game());
    }
    return m_viewLFG;
}

ClubActionSetting^ ClubActionSettings::InviteOrAcceptJoinRequests::get()
{
    if (m_inviteOrAcceptJoinRequest == nullptr)
    {
        m_inviteOrAcceptJoinRequest = ref new ClubActionSetting(m_cppObj.invite_or_accept_join_requests());
    }
    return m_inviteOrAcceptJoinRequest;
}

ClubActionSetting^ ClubActionSettings::KickOrBan::get()
{
    if (m_kickOrBan == nullptr)
    {
        m_kickOrBan = ref new ClubActionSetting(m_cppObj.kick_or_ban());
    }
    return m_kickOrBan;
}

ClubActionSetting^ ClubActionSettings::ViewRoster::get()
{
    if (m_viewRoster == nullptr)
    {
        m_viewRoster = ref new ClubActionSetting(m_cppObj.view_roster());
    }
    return m_viewRoster;
}

ClubActionSetting^ ClubActionSettings::UpdateProfile::get()
{
    if (m_updateProfile == nullptr)
    {
        m_updateProfile = ref new ClubActionSetting(m_cppObj.update_profile());
    }
    return m_updateProfile;
}

ClubActionSetting^ ClubActionSettings::DeleteProfile::get()
{
    if (m_deleteProfile == nullptr)
    {
        m_deleteProfile = ref new ClubActionSetting(m_cppObj.delete_profile());
    }
    return m_deleteProfile;
}

ClubActionSetting^ ClubActionSettings::ViewProfile::get()
{
    if (m_viewProfile == nullptr)
    {
        m_viewProfile = ref new ClubActionSetting(m_cppObj.view_profile());
    }
    return m_viewProfile;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END