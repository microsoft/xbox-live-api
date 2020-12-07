// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

MultiplayerSearchHandleRequest::MultiplayerSearchHandleRequest(
    _In_ XblMultiplayerSessionReference sessionReference
) :
    m_sessionReference(std::move(sessionReference))
{
}

const XblMultiplayerSessionReference&
MultiplayerSearchHandleRequest::SessionReference() const
{
    return m_sessionReference;
}

const xsapi_internal_vector<XblMultiplayerSessionTag>&
MultiplayerSearchHandleRequest::Tags() const
{
    return m_tags;
}

void
MultiplayerSearchHandleRequest::SetTags(
    _In_ xsapi_internal_vector<XblMultiplayerSessionTag>&& value
)
{
    m_tags = value;
}

const xsapi_internal_vector<XblMultiplayerSessionNumberAttribute>&
MultiplayerSearchHandleRequest::NumberAttributes() const
{
    return m_numberAttributes;
}

void
MultiplayerSearchHandleRequest::SetNumberAttributes(
    _In_ xsapi_internal_vector<XblMultiplayerSessionNumberAttribute>&& attributes
)
{
    m_numberAttributes = attributes;
}

const xsapi_internal_vector<XblMultiplayerSessionStringAttribute>&
MultiplayerSearchHandleRequest::StringAttributes() const
{
    return m_stringAttributes;
}

void
MultiplayerSearchHandleRequest::SetStringAttributes(
    _In_ xsapi_internal_vector<XblMultiplayerSessionStringAttribute>&& attributes
)
{
    m_stringAttributes = attributes;
}

void
MultiplayerSearchHandleRequest::Serialize(_Out_ JsonValue& json, _In_ JsonDocument::AllocatorType& allocator) const
{
    json.SetObject();

    json.AddMember("type", "search", allocator);
    json.AddMember("version", m_version, allocator);
    JsonValue sessionRefJson;
    Serializers::SerializeSessionReference(m_sessionReference, sessionRefJson, allocator);
    json.AddMember("sessionRef", sessionRefJson, allocator);

    JsonValue searchAttributesJson(rapidjson::kObjectType);
    bool setSearchAttributes = false;
    if (!m_tags.empty())
    {
        setSearchAttributes = true;
        JsonValue tagsJson(rapidjson::kArrayType);
        JsonUtils::SerializeVector<XblMultiplayerSessionTag>(
            [](XblMultiplayerSessionTag tag, JsonValue& j, JsonDocument::AllocatorType& a)
            {
                j.SetString(tag.value, a);
            },
            m_tags,
            tagsJson,
            allocator);
        searchAttributesJson.AddMember("tags", tagsJson, allocator);
    }

    if (!m_numberAttributes.empty())
    {
        setSearchAttributes = true;
        JsonValue numberAttributesJson(rapidjson::kObjectType);
        for (auto& attr : m_numberAttributes)
        {
            numberAttributesJson.AddMember(JsonValue(attr.name, allocator).Move(), attr.value, allocator);
        }
        searchAttributesJson.AddMember("numbers", numberAttributesJson, allocator);
    }

    if (!m_stringAttributes.empty())
    {
        setSearchAttributes = true;
        JsonValue stringAttributesJson(rapidjson::kObjectType);
        for (auto& attr : m_stringAttributes)
        {
            stringAttributesJson.AddMember(JsonValue(attr.name, allocator).Move(), JsonValue(attr.value, allocator).Move(), allocator);
        }
        searchAttributesJson.AddMember("strings", stringAttributesJson, allocator);
    }

    if (setSearchAttributes)
    {
        json.AddMember("searchAttributes", searchAttributesJson, allocator);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
