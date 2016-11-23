//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#if TV_API || UWP_API || UNIT_TEST_SERVICES
#include "social_manager_internal.h"
#endif
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_record::presence_record():
    m_userState(user_presence_state::unknown)
{
}

const string_t& 
presence_record::xbox_user_id() const
{
    return m_xboxUserId;
}

user_presence_state 
presence_record::user_state() const
{
    return m_userState;
}

const std::vector<presence_device_record>&
presence_record::presence_device_records() const
{
    return m_presenceDeviceRecords;
}

xbox_live_result<presence_record>
presence_record::_Deserialize(
    _In_ const web::json::value& json
    )
{
    presence_record returnObject;
    if (json.is_null()) return xbox_live_result<presence_record>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;
    returnObject.m_xboxUserId = utils::extract_json_string(json, _T("xuid"), errc);
    returnObject.m_userState = _Convert_string_to_user_presence_state(
        utils::extract_json_string(json, _T("state"), errc)
        );

    returnObject.m_presenceDeviceRecords = utils::extract_json_vector<presence_device_record>(
        presence_device_record::_Deserialize,
        json,
        _T("devices"),
        errc,
        false
        );
    
    return xbox_live_result<presence_record>(returnObject, errc);
}

bool
presence_record::is_user_playing_title(
    _In_ uint32_t titleId
    ) const
{
    if (m_userState == user_presence_state::offline || m_userState == user_presence_state::unknown)
    {
        return false;
    }

    for (const auto& deviceRecord : m_presenceDeviceRecords)
    {
        for (const auto& titleRecord : deviceRecord.presence_title_records())
        {
            if (titleRecord.title_id() == titleId && titleRecord.is_title_active())
            {
                return true;
            }
            else if (titleRecord.title_id() == titleId)
            {
                return false;
            }
        }
    }

    return false;
}

user_presence_state 
presence_record::_Convert_string_to_user_presence_state(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("Online")) == 0)
    {
        return user_presence_state::online;
    }
    else if (utils::str_icmp(value, _T("Away")) == 0)
    {
        return user_presence_state::away;
    }
    else if (utils::str_icmp(value, _T("Offline")) == 0)
    {
        return user_presence_state::offline;
    }

    return user_presence_state::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END