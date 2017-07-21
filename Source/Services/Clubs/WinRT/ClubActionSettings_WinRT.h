// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "ClubSetting_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubActionSettings sealed
{
public:
    /// <summary>Setting controlling who can post to the club feed</summary>
    property ClubActionSetting^ PostToFeed { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can view the club feed</summary>
    property ClubActionSetting^ ViewFeed { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can write in chat</summary>
    property ClubActionSetting^ WriteInChat { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who view chat</summary>
    property ClubActionSetting^ ViewChat { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can set chat topic</summary>
    property ClubActionSetting^ SetChatTopic { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can join club looking for game requests</summary>
    property ClubActionSetting^ JoinLookingForGame { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can create club looking for game requests</summary>
    property ClubActionSetting^ CreateLookingForGame { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who view club looking for game requests</summary>
    property ClubActionSetting^ ViewLookingForGame { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who invite people or accept requests to join the club</summary>
    property ClubActionSetting^ InviteOrAcceptJoinRequests { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who kick and ban club members</summary>
    property ClubActionSetting^ KickOrBan { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can view the club roster</summary>
    property ClubActionSetting^ ViewRoster { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can update the club profile</summary>
    property ClubActionSetting^ UpdateProfile { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can delete the club profile</summary>
    property ClubActionSetting^ DeleteProfile { ClubActionSetting^ get(); }

    /// <summary>Setting controlling who can view the club profile</summary>
    property ClubActionSetting^ ViewProfile { ClubActionSetting^ get(); }

internal:
    ClubActionSettings(_In_ xbox::services::clubs::club_action_settings cppObj);

private:
    xbox::services::clubs::club_action_settings m_cppObj;
    ClubActionSetting^ m_postToFeed;
    ClubActionSetting^ m_viewFeed;
    ClubActionSetting^ m_writeInChat;
    ClubActionSetting^ m_viewChat;
    ClubActionSetting^ m_setChatTopic;
    ClubActionSetting^ m_joinLFG;
    ClubActionSetting^ m_createLFG;
    ClubActionSetting^ m_viewLFG;
    ClubActionSetting^ m_inviteOrAcceptJoinRequest;
    ClubActionSetting^ m_kickOrBan;
    ClubActionSetting^ m_viewRoster;
    ClubActionSetting^ m_updateProfile;
    ClubActionSetting^ m_deleteProfile;
    ClubActionSetting^ m_viewProfile;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END