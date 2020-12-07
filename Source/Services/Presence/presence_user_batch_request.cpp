// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

UserBatchRequest::UserBatchRequest(
    _In_ uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_opt_ XblPresenceQueryFilters* filters
) noexcept :
    UserBatchRequest{ filters }
{
    for (size_t i = 0; i < xuidsCount; ++i)
    {
        m_xuids.push_back(utils::uint64_to_internal_string(xuids[i]));
    }
}

UserBatchRequest::UserBatchRequest(
    _In_ xsapi_internal_string&& socialGroup,
    _In_opt_ uint64_t* socialGroupOwnerXuid,
    _In_opt_ XblPresenceQueryFilters* filters
) noexcept :
    UserBatchRequest{ filters }
{
    m_socialGroup = socialGroup;
    if (socialGroupOwnerXuid)
    {
        m_socialGroupOwnerXuid = utils::uint64_to_internal_string(*socialGroupOwnerXuid);
    }
}

UserBatchRequest::UserBatchRequest(
    _In_opt_ XblPresenceQueryFilters* filters
) noexcept
{
    if (filters)
    {
        for (size_t i = 0; i < filters->deviceTypesCount; ++i)
        {
            m_deviceTypes.push_back(DeviceRecord::DeviceTypeAsString(filters->deviceTypes[i]));
        }

        for (size_t i = 0; i < filters->titleIdsCount; ++i)
        {
            m_titleIds.push_back(utils::uint64_to_internal_string(filters->titleIds[i]));
        }

        m_presenceDetailLevel = filters->detailLevel;
        m_onlineOnly = filters->onlineOnly;
        m_broadcastingOnly = filters->broadcastingOnly;
    }
}

xsapi_internal_string UserBatchRequest::StringFromDetailLevel(
    _In_ XblPresenceDetailLevel level
)
{
    switch (level)
    {
    case XblPresenceDetailLevel::User:
        return "user";

    case XblPresenceDetailLevel::Device:
        return "device";

    case XblPresenceDetailLevel::Title:
        return "title";

    case XblPresenceDetailLevel::All:
        return "all";

    default:
        return "";
    }
}

void UserBatchRequest::Serialize(_Out_ JsonValue& serializedObject, _In_ JsonDocument::AllocatorType& allocator) const
{
    serializedObject.SetObject();

    if (!m_xuids.empty())
    {
        JsonValue xuidsVectorJson(rapidjson::kObjectType);
        JsonUtils::SerializeVector<xsapi_internal_string>(
            JsonUtils::JsonStringSerializer, 
            m_xuids,
            xuidsVectorJson,
            allocator
        );

        serializedObject.AddMember("users", xuidsVectorJson, allocator);
    }
    else if (!m_socialGroup.empty())
    {
        serializedObject.AddMember("group", JsonValue(m_socialGroup.c_str(), allocator).Move(), allocator);
        if (!m_socialGroupOwnerXuid.empty())
        {
            serializedObject.AddMember("groupXuid",JsonValue( m_socialGroupOwnerXuid.c_str(), allocator).Move(), allocator);
        }
    }

    if (m_deviceTypes.size() > 0)
    {
        JsonValue deviceTypesJson(rapidjson::kArrayType);
        JsonUtils::SerializeVector<xsapi_internal_string>(
            JsonUtils::JsonStringSerializer,
            m_deviceTypes,
            deviceTypesJson,
            allocator
        );
        serializedObject.AddMember("deviceTypes", deviceTypesJson, allocator);
    }

    if (m_titleIds.size() > 0)
    {
        JsonValue titleIdsJson(rapidjson::kArrayType);
        JsonUtils::SerializeVector<xsapi_internal_string>(
            JsonUtils::JsonStringSerializer,
            m_titleIds,
            titleIdsJson,
            allocator
        );
        serializedObject.AddMember("titles", titleIdsJson, allocator);
    }

    auto presenceDetailLevel = StringFromDetailLevel(m_presenceDetailLevel);
    if (!presenceDetailLevel.empty())
    {
        serializedObject.AddMember("level", JsonValue(presenceDetailLevel.c_str(), allocator).Move(), allocator);
    }

    serializedObject.AddMember("onlineOnly", m_onlineOnly, allocator);
    serializedObject.AddMember("broadcastingOnly", m_broadcastingOnly, allocator);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END