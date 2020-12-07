// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;

std::shared_ptr<RefCounter> XblMultiplayerSearchHandleDetails::GetSharedThis()
{
    return shared_from_this();
}

const XblMultiplayerSessionReference&
XblMultiplayerSearchHandleDetails::SessionReference() const
{
    return m_sessionReference;
}

const xsapi_internal_string&
XblMultiplayerSearchHandleDetails::HandleId() const
{
    return m_handleId;
}

const xsapi_internal_vector<uint64_t>&
XblMultiplayerSearchHandleDetails::SessionOwnerXuids() const
{
    return m_sessionOwners;
}

const RoleTypes&
XblMultiplayerSearchHandleDetails::RoleTypes() const
{
    return m_roleTypes;
}

const xsapi_internal_vector<XblMultiplayerSessionTag>&
XblMultiplayerSearchHandleDetails::Tags() const
{
    return m_tags;
}

const xsapi_internal_vector<XblMultiplayerSessionNumberAttribute>&
XblMultiplayerSearchHandleDetails::NumberAttributes() const
{
    return m_numberAttributes;
}

const xsapi_internal_vector<XblMultiplayerSessionStringAttribute>&
XblMultiplayerSearchHandleDetails::StringAttributes() const
{
    return m_stringAttributes;
}

XblMultiplayerSessionVisibility
XblMultiplayerSearchHandleDetails::Visibility() const
{
    return m_visibility;
}

XblMultiplayerSessionRestriction
XblMultiplayerSearchHandleDetails::JoinRestriction() const
{
    return m_joinRestriction;
}

bool
XblMultiplayerSearchHandleDetails::Closed() const
{
    return m_closed;
}

size_t
XblMultiplayerSearchHandleDetails::MaxMembersCount() const
{
    return m_maxMembersCount;
}

size_t
XblMultiplayerSearchHandleDetails::MembersCount() const
{
    return m_membersCount;
}

const xbox::services::datetime&
XblMultiplayerSearchHandleDetails::HandleCreationTime() const
{
    return m_handleCreationTime;
}

const xsapi_internal_string&
XblMultiplayerSearchHandleDetails::CustomSessionPropertiesJson() const
{
    return m_customSessionPropertiesJson;
}

Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>>
XblMultiplayerSearchHandleDetails::Deserialize(
    _In_ const JsonValue& json
)
{
    if (json.IsNull())
    {
        return Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>>{ S_OK };
    }

    auto returnResult{ MakeShared<XblMultiplayerSearchHandleDetails>() };

    HRESULT errc = S_OK;

    xsapi_internal_string type;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "type", type));
    if (utils::str_icmp_internal(type, "search") != 0)
    {
        return Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>>{ E_UNEXPECTED };
    }

    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "id", returnResult->m_handleId));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonTime(json, "createTime", returnResult->m_handleCreationTime));
    if (json.IsObject() && json.HasMember("sessionRef"))
    {
        returnResult->m_sessionReference = Serializers::DeserializeSessionReference(json["sessionRef"]).Payload();
    }
    else
    {
        returnResult->m_sessionReference = XblMultiplayerSessionReference();
    }

    if (json.IsObject() && json.HasMember("customProperties"))
    {
        const JsonValue& customPropertiesObject = json["customProperties"];
        if (!customPropertiesObject.IsNull())
        {
        returnResult->m_customSessionPropertiesJson = JsonUtils::SerializeJson(customPropertiesObject);
        }
    }

    if (json.IsObject() && json.HasMember("searchAttributes"))
    {
        const JsonValue& searchAttributesObject = json["searchAttributes"];
        if (!searchAttributesObject.IsNull())
        {
            if (searchAttributesObject.IsObject() && searchAttributesObject.HasMember("tags"))
            {
                const JsonValue& tagAttributesJson = searchAttributesObject["tags"];
                if (!tagAttributesJson.IsNull() && tagAttributesJson.IsArray())
                {
                    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<XblMultiplayerSessionTag>(
                        [](const JsonValue& json)
                        {
                            if (!json.IsString())
                            {
                                return Result<XblMultiplayerSessionTag>{ WEB_E_INVALID_JSON_STRING };
                            }
                            XblMultiplayerSessionTag tag{};
                            utils::strcpy(tag.value, sizeof(tag.value), json.GetString());

                            return Result<XblMultiplayerSessionTag>{ tag };
                        },
                        searchAttributesObject,
                            "tags",
                            returnResult->m_tags,
                            false
                            ));
                }
            }

            if (searchAttributesObject.IsObject() && searchAttributesObject.HasMember("strings"))
            {
                const JsonValue& stringAttributesJson = searchAttributesObject["strings"];
                if (!stringAttributesJson.IsNull() && stringAttributesJson.IsObject())
                {
                    for (const auto& stringsMetadata : stringAttributesJson.GetObject())
                    {
                        XblMultiplayerSessionStringAttribute attr{};
                        utils::strcpy(attr.name, sizeof(attr.name), stringsMetadata.name.GetString());
                        utils::strcpy(attr.value, sizeof(attr.name), stringsMetadata.value.GetString());

                        returnResult->m_stringAttributes.push_back(std::move(attr));
                    }
                }
            }

            if (searchAttributesObject.IsObject() && searchAttributesObject.HasMember("numbers"))
            {
                const JsonValue& numbersMetadataJson = searchAttributesObject["numbers"];
                if (!numbersMetadataJson.IsNull() && numbersMetadataJson.IsObject())
                {
                    for (const auto& numbersMetadata : numbersMetadataJson.GetObject())
                    {
                        XblMultiplayerSessionNumberAttribute attr{};
                        utils::strcpy(attr.name, sizeof(attr.name), numbersMetadata.name.GetString());
                        attr.value = numbersMetadata.value.GetDouble();

                        returnResult->m_numberAttributes.push_back(std::move(attr));
                    }
                }
            }
        }
    }

    if (json.IsObject() && json.HasMember("relatedInfo"))
    {
        const JsonValue& relatedInfoObject = json["relatedInfo"];
        if (!relatedInfoObject.IsNull())
        {
            int membersCount = 0;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(relatedInfoObject, "membersCount", membersCount, true));
            returnResult->m_membersCount = membersCount;
            int maxMembersCount = 0;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(relatedInfoObject, "maxMembersCount", maxMembersCount, true));
            returnResult->m_maxMembersCount = maxMembersCount;
            xsapi_internal_string joinRestriction;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(relatedInfoObject, "joinRestriction", joinRestriction));
            returnResult->m_joinRestriction = Serializers::MultiplayerSessionRestrictionFromString(joinRestriction);
            xsapi_internal_string visibility;
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(relatedInfoObject, "visibility", visibility));
            returnResult->m_visibility = Serializers::MultiplayerSessionVisibilityFromString(visibility);

            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonBool(relatedInfoObject, "closed", returnResult->m_closed));
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<uint64_t>(JsonUtils::JsonXuidExtractor, relatedInfoObject, "sessionOwners", returnResult->m_sessionOwners, false));
        }
    }

    if (json.IsObject() && json.HasMember("roleInfo"))
    {
        const JsonValue& roleInfo = json["roleInfo"];
        if (!roleInfo.IsNull())
        {
            if (roleInfo.IsObject() && roleInfo.HasMember("roleTypes"))
            {
                auto roleTypesResult{ RoleTypes::Deserialize(roleInfo["roleTypes"]) };
                RETURN_HR_IF_FAILED(roleTypesResult.Hresult());
                returnResult->m_roleTypes = roleTypesResult.ExtractPayload();
            }
        }
    }

    return Result<std::shared_ptr<XblMultiplayerSearchHandleDetails>>{ returnResult, errc };
}