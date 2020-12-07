// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

MultiplayerInviteHandlePostRequest::MultiplayerInviteHandlePostRequest(
    _In_ const XblMultiplayerSessionReference& sessionReference,
    _In_ uint64_t invitedXuid,
    _In_ uint32_t titleId,
    _In_ const String& contextString,
    _In_ const String& customActivationContext
) noexcept :
    m_json{ rapidjson::kObjectType }
{
    assert(XblMultiplayerSessionReferenceIsValid(&sessionReference));

    auto& a{ m_json.GetAllocator() };

    m_json.AddMember("type", "invite", a);
    JsonValue sessionRefJson;
    Serializers::SerializeSessionReference(sessionReference, sessionRefJson, a);
    m_json.AddMember("sessionRef", sessionRefJson, a);
    m_json.AddMember("version", MULTIPLAYER_HANDLE_VERSION, a);
    m_json.AddMember("invitedXuid", JsonValue{ utils::uint64_to_internal_string(invitedXuid).c_str(), a }.Move(), a);

    JsonValue jsonInviteAttributes(rapidjson::kObjectType);
    jsonInviteAttributes.AddMember("titleId", JsonValue{ utils::uint32_to_internal_string(titleId).c_str(), a }.Move(), a);

    if (!contextString.empty())
    {
        jsonInviteAttributes.AddMember("contextString", JsonValue{ contextString.c_str(), a }.Move(), a);
    }

    if (!customActivationContext.empty())
    {
        jsonInviteAttributes.AddMember("context", JsonValue{ customActivationContext.c_str(), a }.Move(), a);
    }
    m_json.AddMember("inviteAttributes", jsonInviteAttributes, a);
}

void MultiplayerInviteHandlePostRequest::SetInvitedXuid(uint64_t invitedXuid) noexcept
{
    JsonUtils::SetMember(m_json, "invitedXuid", JsonValue{ utils::uint64_to_internal_string(invitedXuid).c_str(), m_json.GetAllocator() }.Move());
}

const JsonValue& MultiplayerInviteHandlePostRequest::Json() const noexcept
{
    return m_json;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
