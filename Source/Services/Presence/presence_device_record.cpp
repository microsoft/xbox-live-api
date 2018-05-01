// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_device_record::presence_device_record(
    _In_ std::shared_ptr<presence_device_record_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_ENUM_TYPE(presence_device_record, presence_device_type, device_type);
DEFINE_GET_VECTOR_INTERNAL_TYPE(presence_device_record, presence_title_record, presence_title_records);

bool presence_device_record::operator!=(
    _In_ const presence_device_record& rhs
    )
{
    return *m_internalObj != *(rhs.m_internalObj);
}

presence_device_record_internal::presence_device_record_internal()
{
}

presence_device_record_internal::presence_device_record_internal(
    _In_ presence_device_type deviceType
    ) :
    m_deviceType(deviceType)
{
}

presence_device_type
presence_device_record_internal::device_type() const
{
    return m_deviceType;
}

const xsapi_internal_vector<std::shared_ptr<presence_title_record_internal>>&
presence_device_record_internal::presence_title_records() const
{
    return m_presenceTitleRecords;
}

bool presence_device_record_internal::operator!=(
    _In_ const presence_device_record_internal& rhs
    )
{
    if (m_presenceTitleRecords.size() != rhs.m_presenceTitleRecords.size())
    {
        return true;
    }

    auto lhsTitleMap = create_map_from_title_records(m_presenceTitleRecords);
    auto rhsTitleMap = create_map_from_title_records(rhs.m_presenceTitleRecords);
    for (const auto& lhsTitlePair : lhsTitleMap)
    {
        if (rhsTitleMap.find(lhsTitlePair.first) == rhsTitleMap.end() || lhsTitlePair.second != rhsTitleMap[lhsTitlePair.first])
        {
            return true;
        }
    }

    for (const auto& rhsTitlePair : rhsTitleMap)
    {
        if (lhsTitleMap.find(rhsTitlePair.first) == lhsTitleMap.end())
        {
            return true;
        }
    }

    return false;
}

presence_device_type
presence_device_record_internal::convert_string_to_presence_device_type(
    _In_ const xsapi_internal_string& value
    )
{
    if (utils::str_icmp(value, "WindowsPhone") == 0)
    {
        return presence_device_type::windows_phone;
    }
    else if (utils::str_icmp(value, "WindowsPhone7") == 0)
    {
        return presence_device_type::windows_phone_7;
    }
    else if (utils::str_icmp(value, "Web") == 0)
    {
        return presence_device_type::web;
    }
    else if (utils::str_icmp(value, "Xbox360") == 0)
    {
        return presence_device_type::xbox_360;
    }
    else if (utils::str_icmp(value, "PC") == 0)
    {
        return presence_device_type::pc;
    }
    else if (utils::str_icmp(value, "MoLive") == 0)
    {
        return presence_device_type::windows_8;
    }
    else if (utils::str_icmp(value, "XboxOne") == 0 ||
        utils::str_icmp(value, "MCapensis") == 0)
    {
        return presence_device_type::xbox_one;
    }
    else if (utils::str_icmp(value, "WindowsOneCore") == 0)
    {
        return presence_device_type::windows_one_core;
    }
    else if (utils::str_icmp(value, "WindowsOneCoreMobile") == 0)
    {
        return presence_device_type::windows_one_core_mobile;
    }

    return presence_device_type::unknown;
}

xsapi_internal_string
presence_device_record_internal::convert_presence_device_type_to_string(
    _In_ presence_device_type deviceType
    )
{
    switch (deviceType)
    {
    case presence_device_type::windows_phone:
        return "WindowsPhone";

    case presence_device_type::windows_phone_7:
        return "WindowsPhone7";

    case presence_device_type::web:
        return "Web";

    case presence_device_type::xbox_360:
        return "Xbox360";

    case presence_device_type::pc:
        return "PC";

    case presence_device_type::windows_8:
        return "MoLive";

    case presence_device_type::xbox_one:
        return "XboxOne";

    case presence_device_type::windows_one_core:
        return "WindowsOneCore";

    case presence_device_type::windows_one_core_mobile:
        return "WindowsOneCoreMobile";
        
    default:
        return xsapi_internal_string();
    }
}

xsapi_internal_unordered_map<uint32_t, std::shared_ptr<presence_title_record_internal>>
presence_device_record_internal::create_map_from_title_records(
    _In_ const xsapi_internal_vector<std::shared_ptr<presence_title_record_internal>>& titleRecords
    ) const
{
    xsapi_internal_unordered_map<uint32_t, std::shared_ptr<presence_title_record_internal>> returnMap;
    for (const auto& titleRecord : titleRecords)
    {
        returnMap[titleRecord->title_id()] = titleRecord;
    }

    return returnMap;
}

xbox_live_result<std::shared_ptr<presence_device_record_internal>>
presence_device_record_internal::deserialize(
    _In_ const web::json::value& inputJson
    )
{
    auto returnObject = xsapi_allocate_shared<presence_device_record_internal>();
    if (inputJson.is_null()) return xbox_live_result<std::shared_ptr<presence_device_record_internal>>(returnObject);
    std::error_code errc = xbox_live_error_code::no_error;

    presence_device_type type = convert_string_to_presence_device_type(
        utils::extract_json_string(inputJson, "type", errc)
        );

    returnObject->m_presenceTitleRecords = utils::extract_json_vector<std::shared_ptr<presence_title_record_internal>>(
        presence_title_record_internal::deserialize,
        inputJson, 
        "titles",
        errc,
        false
        );

    returnObject->m_deviceType = type;
    return xbox_live_result<std::shared_ptr<presence_device_record_internal>>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END