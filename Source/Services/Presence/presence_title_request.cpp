// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

TitleRequest::TitleRequest(
    _In_ bool isUserActive,
    _In_opt_ const XblPresenceRichPresenceIds* richPresenceIds
) :
    m_isUserActive{ isUserActive }
{
    if (richPresenceIds)
    {
        m_scid = richPresenceIds->scid;
        m_presenceId = richPresenceIds->presenceId;

        for (auto i = 0u; i < richPresenceIds->presenceTokenIdsCount; ++i)
        {
            m_presenceTokenIds.push_back(richPresenceIds->presenceTokenIds[i]);
        }
    }
}

void TitleRequest::Serialize(_Out_ JsonValue& serializedObject, _In_ JsonDocument::AllocatorType& allocator)
{
    serializedObject.SetObject();
    xsapi_internal_string state = m_isUserActive ? "active" : "inactive";
    serializedObject.AddMember("state", JsonValue(state.c_str(), allocator).Move(), allocator);

    if (!m_scid.empty() && !m_presenceId.empty())
    {
        JsonValue richPresenceJson(rapidjson::kObjectType);

        richPresenceJson.AddMember("id", JsonValue(m_presenceId.c_str(), allocator).Move(), allocator);
        richPresenceJson.AddMember("scid", JsonValue(m_scid.c_str(), allocator).Move(), allocator);
        if (!m_presenceTokenIds.empty())
        {
            JsonValue presenceTokenIDsJson(rapidjson::kArrayType);
            JsonUtils::SerializeVector(JsonUtils::JsonStringSerializer, m_presenceTokenIds, presenceTokenIDsJson, allocator);
            richPresenceJson.AddMember("params", presenceTokenIDsJson, allocator);
        }

        serializedObject.AddMember("activity", JsonValue(rapidjson::kObjectType).AddMember("richPresence", richPresenceJson, allocator).Move(), allocator);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END