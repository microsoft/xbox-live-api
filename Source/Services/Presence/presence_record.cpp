// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if TV_API || UWP_API || UNIT_TEST_SERVICES
#include "social_manager_internal.h"
#endif
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_record::presence_record(
    _In_ std::shared_ptr<presence_record_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(presence_record, xbox_user_id);
DEFINE_GET_ENUM_TYPE(presence_record, user_presence_state, user_state);
DEFINE_GET_VECTOR_INTERNAL_TYPE(presence_record, presence_device_record, presence_device_records);

bool presence_record::is_user_playing_title(_In_ uint32_t titleId) const
{
    return m_internalObj->is_user_playing_title(titleId);
}

presence_record_internal::presence_record_internal():
    m_userState(user_presence_state::unknown)
{
}

const xsapi_internal_string& 
presence_record_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

user_presence_state 
presence_record_internal::user_state() const
{
    return m_userState;
}

const xsapi_internal_vector<std::shared_ptr<presence_device_record_internal>>&
presence_record_internal::presence_device_records() const
{
    return m_presenceDeviceRecords;
}

xbox_live_result<std::shared_ptr<presence_record_internal>>
presence_record_internal::deserialize(
    _In_ const web::json::value& json
    )
{
    auto returnObject = xsapi_allocate_shared<presence_record_internal>();
    if (json.is_null()) return xbox_live_result<std::shared_ptr<presence_record_internal>>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;
    returnObject->m_xboxUserId = utils::extract_json_string(json, "xuid", errc);
    returnObject->m_userState = convert_string_to_user_presence_state(
        utils::extract_json_string(json, "state", errc)
        );

    returnObject->m_presenceDeviceRecords = utils::extract_json_vector<std::shared_ptr<presence_device_record_internal>>(
        presence_device_record_internal::deserialize,
        json,
        "devices",
        errc,
        false
        );

    return xbox_live_result<std::shared_ptr<presence_record_internal>>(returnObject, errc);
}

bool
presence_record_internal::is_user_playing_title(
    _In_ uint32_t titleId
    ) const
{
    if (m_userState == user_presence_state::offline || m_userState == user_presence_state::unknown)
    {
        return false;
    }

    for (const auto& deviceRecord : m_presenceDeviceRecords)
    {
        for (const auto& titleRecord : deviceRecord->presence_title_records())
        {
            if (titleRecord->title_id() == titleId && titleRecord->is_title_active())
            {
                return true;
            }
            else if (titleRecord->title_id() == titleId)
            {
                return false;
            }
        }
    }

    return false;
}

user_presence_state 
presence_record_internal::convert_string_to_user_presence_state(
    _In_ const xsapi_internal_string& value
    )
{
    if (utils::str_icmp(value, "Online") == 0)
    {
        return user_presence_state::online;
    }
    else if (utils::str_icmp(value, "Away") == 0)
    {
        return user_presence_state::away;
    }
    else if (utils::str_icmp(value, "Offline") == 0)
    {
        return user_presence_state::offline;
    }

    return user_presence_state::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END