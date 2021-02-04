// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

bool MultiplayerManagerUtils::CompareSessions(
    _In_ const std::shared_ptr<XblMultiplayerSession>& session1,
    _In_ const std::shared_ptr<XblMultiplayerSession>& session2
    )
{
    if (session1 == nullptr && session2 == nullptr)
    {
        return true;
    }

    if ( (session1 == nullptr && session2 != nullptr) ||
         (session1 != nullptr && session2 == nullptr) ||
         session1->SessionInfo().ChangeNumber != session2->SessionInfo().ChangeNumber)
    {
        return false;
    }

    return true;
}


// joinable_by_friends                 : !closed & JoinRestricion::Followed
// invite_only                         : !closed & JoinRestricion::Local
// disable_while_game_in_progress      : closed & JoinRestricion::Local (when in game)
// closed                              : closed & JoinRestricion::Local

void MultiplayerManagerUtils::SetJoinability(
    _In_ XblMultiplayerJoinability value,
    _In_ std::shared_ptr<XblMultiplayerSession> sessionToCommit,
    _In_ bool isGameInProgress
    )
{
    sessionToCommit->SetClosed(false);
    sessionToCommit->SetJoinRestriction(XblMultiplayerSessionRestriction::Local);
    if (value == XblMultiplayerJoinability::JoinableByFriends)
    {
        sessionToCommit->SetJoinRestriction(XblMultiplayerSessionRestriction::Followed);
    }
    else if (value == XblMultiplayerJoinability::Closed ||
            (isGameInProgress && value == XblMultiplayerJoinability::DisableWhileGameInProgress))
    {
        sessionToCommit->SetClosed(true);
    }

    xsapi_internal_string jsonValueStr = ConvertJoinabilityToString(value);
    JsonDocument json;
    json.SetString(jsonValueStr.c_str(), json.GetAllocator());
    sessionToCommit->SetSessionCustomPropertyJson(MultiplayerLobbyClient_JoinabilityPropertyName, json);
}

XblMultiplayerJoinability MultiplayerManagerUtils::GetJoinability(
    _In_ const XblMultiplayerSessionProperties& sessionProperties
    )
{
    xsapi_internal_string joinableStr;
    JsonDocument jsonDoc;
    jsonDoc.Parse(sessionProperties.SessionCustomPropertiesJson);

    if (!jsonDoc.HasParseError())
    {
        JsonUtils::ExtractJsonString(jsonDoc, MultiplayerLobbyClient_JoinabilityPropertyName, joinableStr, false);
    }

    return ConvertStringToJoinability(joinableStr);
}

XblMultiplayerJoinability MultiplayerManagerUtils::ConvertStringToJoinability(
    _In_ const xsapi_internal_string& value
    )
{
    if (utils::str_icmp_internal(value, "joinable_by_friends") == 0)
    {
        return XblMultiplayerJoinability::JoinableByFriends;
    }
    else if (utils::str_icmp_internal(value, "invite_only") == 0)
    {
        return XblMultiplayerJoinability::InviteOnly;
    }
    else if (utils::str_icmp_internal(value, "disable_while_game_in_progress") == 0)
    {
        return XblMultiplayerJoinability::DisableWhileGameInProgress;
    }
    else if (utils::str_icmp_internal(value, "closed") == 0)
    {
        return XblMultiplayerJoinability::Closed;
    }

    return XblMultiplayerJoinability::None;
}

xsapi_internal_string MultiplayerManagerUtils::ConvertJoinabilityToString(
    _In_ XblMultiplayerJoinability value
    )
{
    switch (value)
    {
        case XblMultiplayerJoinability::JoinableByFriends: return "joinable_by_friends";
        case XblMultiplayerJoinability::InviteOnly: return "invite_only";
        case XblMultiplayerJoinability::DisableWhileGameInProgress: return "disable_while_game_in_progress";
        case XblMultiplayerJoinability::Closed: return "closed";
        default: return "none";
    }
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END