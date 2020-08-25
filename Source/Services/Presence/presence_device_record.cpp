// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_device_record::presence_device_record()
{
}

presence_device_record::presence_device_record(
    _In_ presence_device_type deviceType
    ) :
    m_deviceType(deviceType)
{
}

presence_device_type
presence_device_record::device_type() const
{
    return m_deviceType;
}

const std::vector<presence_title_record>&
presence_device_record::presence_title_records() const
{
    return m_presenceTitleRecords;
}

bool presence_device_record::operator!=(
    _In_ const presence_device_record& rhs
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
presence_device_record::_Convert_string_to_presence_device_type(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("WindowsPhone")) == 0)
    {
        return presence_device_type::windows_phone;
    }
    else if (utils::str_icmp(value, _T("WindowsPhone7")) == 0)
    {
        return presence_device_type::windows_phone_7;
    }
    else if (utils::str_icmp(value, _T("Web")) == 0)
    {
        return presence_device_type::web;
    }
    else if (utils::str_icmp(value, _T("Xbox360")) == 0)
    {
        return presence_device_type::xbox_360;
    }
    else if (utils::str_icmp(value, _T("PC")) == 0)
    {
        return presence_device_type::pc;
    }
    else if (utils::str_icmp(value, _T("MoLive")) == 0)
    {
        return presence_device_type::windows_8;
    }
    else if (utils::str_icmp(value, _T("XboxOne")) == 0 ||
        utils::str_icmp(value, _T("MCapensis")) == 0)
    {
        return presence_device_type::xbox_one;
    }
    else if (utils::str_icmp(value, _T("WindowsOneCore")) == 0)
    {
        return presence_device_type::windows_one_core;
    }
    else if (utils::str_icmp(value, _T("WindowsOneCoreMobile")) == 0)
    {
        return presence_device_type::windows_one_core_mobile;
    }
    else if (utils::str_icmp(value, _T("iOS")) == 0)
    {
        return presence_device_type::iOS;
    }
    else if (utils::str_icmp(value, _T("Android")) == 0)
    {
        return presence_device_type::android;
    }
    else if (utils::str_icmp(value, _T("AppleTV")) == 0)
    {
        return presence_device_type::appleTV;
    }
    else if (utils::str_icmp(value, _T("Nintendo")) == 0)
    {
        return presence_device_type::nintendo;
    }
    else if (utils::str_icmp(value, _T("PlayStation")) == 0)
    {
        return presence_device_type::playStation;
    }
    else if (utils::str_icmp(value, _T("Win32")) == 0)
    {
        return presence_device_type::win32;
    }
    else if (utils::str_icmp(value, _T("Scarlett")) == 0)
    {
        return presence_device_type::scarlett;
    }

    return presence_device_type::unknown;
}

string_t 
presence_device_record::_Convert_presence_device_type_to_string(
    _In_ presence_device_type deviceType
    )
{
    switch (deviceType)
    {
    case presence_device_type::windows_phone:
        return _T("WindowsPhone");

    case presence_device_type::windows_phone_7:
        return _T("WindowsPhone7");

    case presence_device_type::web:
        return _T("Web");

    case presence_device_type::xbox_360:
        return _T("Xbox360");

    case presence_device_type::pc:
        return _T("PC");

    case presence_device_type::windows_8:
        return _T("MoLive");

    case presence_device_type::xbox_one:
        return _T("XboxOne");

    case presence_device_type::windows_one_core:
        return _T("WindowsOneCore");

    case presence_device_type::windows_one_core_mobile:
        return _T("WindowsOneCoreMobile");

    case presence_device_type::iOS:
        return _T("iOS");

    case presence_device_type::android:
        return _T("Android");

    case presence_device_type::appleTV:
        return _T("AppleTV");

    case presence_device_type::nintendo:
        return _T("Nintendo");

    case presence_device_type::playStation:
        return _T("PlayStation");

    case presence_device_type::win32:
        return _T("Win32");

    case presence_device_type::scarlett:
        return _T("Scarlett");
        
    default:
        return string_t();
    }
}

std::unordered_map<uint32_t, presence_title_record>
presence_device_record::create_map_from_title_records(
    _In_ const std::vector<presence_title_record>& titleRecords
    ) const
{
    std::unordered_map<uint32_t, presence_title_record> returnMap;
    for (const auto& titleRecord : titleRecords)
    {
        returnMap[titleRecord.title_id()] = titleRecord;
    }

    return returnMap;
}

void
presence_device_record::_Add_title_record(
    _In_ uint32_t titleId,
    _In_ title_presence_state titlePresenceState
    )
{
    presence_title_record presenceTitleRecord(titleId, titlePresenceState);
    m_presenceTitleRecords.push_back(presenceTitleRecord);
}

xbox_live_result<presence_device_record>
presence_device_record::_Deserialize(
    _In_ const web::json::value& inputJson
    )
{
    presence_device_record returnObject;
    if (inputJson.is_null()) return xbox_live_result<presence_device_record>(returnObject);
    std::error_code errc = xbox_live_error_code::no_error;

    presence_device_type type = _Convert_string_to_presence_device_type(
        utils::extract_json_string(inputJson, _T("type"), errc)
        );

    returnObject.m_presenceTitleRecords = utils::extract_json_vector<presence_title_record>(
        presence_title_record::_Deserialize,
        inputJson, 
        _T("titles"),
        errc,
        false
        );

    returnObject.m_deviceType = type;
    return xbox_live_result<presence_device_record>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END