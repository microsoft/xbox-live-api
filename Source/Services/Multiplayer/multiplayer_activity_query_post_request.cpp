// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

MultiplayerActivityQueryPostRequest::MultiplayerActivityQueryPostRequest()
{
}

MultiplayerActivityQueryPostRequest::MultiplayerActivityQueryPostRequest(
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_vector<uint64_t>& xuids
    ) :
    m_scid(scid),
    m_xuids(xuids)
{
}

MultiplayerActivityQueryPostRequest::MultiplayerActivityQueryPostRequest(
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& socialGroup,
    _In_ uint64_t socialGroupXuid
    ) :
    m_scid(scid),
    m_socialGroupXuid(socialGroupXuid),
    m_socialGroup(socialGroup)
{
}

const xsapi_internal_string&
MultiplayerActivityQueryPostRequest::Scid() const
{
    return m_scid;
}

const xsapi_internal_vector<uint64_t>&
MultiplayerActivityQueryPostRequest::Xuids() const
{
    return m_xuids;
}
const xsapi_internal_string&
MultiplayerActivityQueryPostRequest::SocialGroup() const
{
    return m_socialGroup;
}

uint64_t
MultiplayerActivityQueryPostRequest::SocialGroupXuid() const
{
    return m_socialGroupXuid;
}

void
MultiplayerActivityQueryPostRequest::Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator)
{
    XSAPI_ASSERT(m_socialGroup.empty() || m_xuids.empty());
    XSAPI_ASSERT(!m_socialGroup.empty() || !m_xuids.empty());

    json.SetObject();
    json.AddMember("type", "activity", allocator);
    json.AddMember("scid", JsonValue(m_scid.c_str(), allocator).Move(), allocator);

    JsonValue ownerObject(rapidjson::kObjectType);
    if (!m_xuids.empty())
    {
        JsonValue xuidsJson;
        JsonUtils::SerializeVector<uint64_t>(JsonUtils::JsonXuidSerializer, m_xuids, xuidsJson, allocator);
        ownerObject.AddMember("xuids", xuidsJson, allocator);
    }
    else
    {
        JsonValue peopleObject(rapidjson::kObjectType);
        peopleObject.AddMember("moniker", JsonValue(m_socialGroup.c_str(), allocator).Move(), allocator);
        peopleObject.AddMember("monikerXuid", JsonValue(utils::uint64_to_internal_string(m_socialGroupXuid).c_str(), allocator).Move(), allocator);
        ownerObject.AddMember("people", peopleObject, allocator);
    }

    json.AddMember("owners", ownerObject, allocator);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END