// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_user_batch_request::presence_user_batch_request(
    _In_ std::vector<string_t> xboxUserIds,
    _In_ string_t socialGroup,
    _In_ string_t socialGroupOwnerXboxUserId,
    _In_ std::vector<presence_device_type> deviceTypes,
    _In_ std::vector<uint32_t> titleIds,
    _In_ presence_detail_level presenceDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    ) :
    m_xboxUserIds(std::move(xboxUserIds)),
    m_socialGroup(std::move(socialGroup)),
    m_socialGroupOwnerXboxUserId(std::move(socialGroupOwnerXboxUserId)),
    m_presenceDetailLevel(presenceDetailLevel),
    m_onlineOnly(onlineOnly),
    m_broadcastingOnly(broadcastingOnly)
{
    XSAPI_ASSERT(!m_xboxUserIds.empty() || !m_socialGroup.empty());
    XSAPI_ASSERT(m_xboxUserIds.empty() || m_socialGroup.empty());

    for (auto& deviceType : deviceTypes)
    {
        m_deviceTypes.push_back(
            presence_device_record::_Convert_presence_device_type_to_string(deviceType)
            );
    }

    for (const auto& titleId : titleIds)
    {
        stringstream_t titleIdStream;
        titleIdStream << titleId;
        m_titleIds.push_back(
            titleIdStream.str()
            );
    }
}

string_t
presence_user_batch_request::convert_detail_level_to_string(
    _In_ presence_detail_level level
    )
{
    switch (level)
    {
    case presence_detail_level::user:
        return _T("user");

    case presence_detail_level::device:
        return _T("device");

    case presence_detail_level::title:
        return _T("title");

    case presence_detail_level::all:
        return _T("all");

    default:
        return _T("");
    }
}


web::json::value
presence_user_batch_request::serialize() const
{
    web::json::value serializedObject;

    if (!m_xboxUserIds.empty())
    {
        serializedObject[_T("users")] = utils::serialize_vector<string_t>(
            utils::json_string_serializer, 
            m_xboxUserIds
            );
    }
    else if (!m_socialGroup.empty())
    {
        serializedObject[_T("group")] = web::json::value::string(m_socialGroup);
        if (!m_socialGroupOwnerXboxUserId.empty())
        {
            serializedObject[_T("groupXuid")] = web::json::value::string(m_socialGroupOwnerXboxUserId);
        }
    }

    if (m_deviceTypes.size() > 0)
    {
        serializedObject[_T("deviceTypes")] = utils::serialize_vector<string_t>(
            utils::json_string_serializer,
            m_deviceTypes
            );
    }

    if (m_titleIds.size() > 0)
    {
        serializedObject[_T("titles")] = utils::serialize_vector<string_t>(
            utils::json_string_serializer,
            m_titleIds
            );
    }

    string_t presenceDetailLevel = convert_detail_level_to_string(m_presenceDetailLevel);
    if (!presenceDetailLevel.empty())
    {
        serializedObject[_T("level")] = web::json::value::string(presenceDetailLevel);
    }

    serializedObject[_T("onlineOnly")] = web::json::value::boolean(m_onlineOnly);
    serializedObject[_T("broadcastingOnly")] = web::json::value::boolean(m_broadcastingOnly);

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END