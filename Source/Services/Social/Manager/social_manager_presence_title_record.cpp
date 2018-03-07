// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

using namespace xbox::services::presence;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

social_manager_presence_title_record::social_manager_presence_title_record() :
    m_titleId(0),
    m_isTitleActive(false),
    m_isBroadcasting(false),
    m_deviceType(presence_device_type::unknown),
    m_isNull(true)
{
    initialize_char_arr(m_presenceText);
}

social_manager_presence_title_record::social_manager_presence_title_record(
    _In_ const xbox::services::presence::presence_title_record& presenceTitleRecord,
    _In_ presence_device_type deviceType
    ) :
    m_titleId(presenceTitleRecord.title_id()),
    m_isBroadcasting(presenceTitleRecord.broadcast_record().start_time() != utility::datetime()),
    m_isTitleActive(presenceTitleRecord.is_title_active()),
    m_deviceType(deviceType),
    m_isNull(false)
{
    utils::char_t_copy(m_presenceText, ARRAYSIZE(m_presenceText), presenceTitleRecord.presence().c_str());
}

uint32_t
social_manager_presence_title_record::title_id() const
{
    return m_titleId;
}

bool
social_manager_presence_title_record::is_title_active() const
{
    return m_isTitleActive;
}

const char_t*
social_manager_presence_title_record::presence_text() const
{
    return m_presenceText;
}

bool social_manager_presence_title_record::is_broadcasting() const
{
    return m_isBroadcasting;
}

presence_device_type
social_manager_presence_title_record::device_type() const
{
    return m_deviceType;
}

bool
social_manager_presence_title_record::_Is_null() const
{
    return m_isNull;
}

xbox_live_result<social_manager_presence_title_record>
social_manager_presence_title_record::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return social_manager_presence_title_record();

    social_manager_presence_title_record returnObject;
    std::error_code errc;

    auto deviceString = utils::extract_json_string(json, "Device", errc);
    returnObject.m_deviceType = presence_device_record_internal::convert_string_to_presence_device_type(deviceString);
    utils::extract_json_string_to_char_t_array(json, _T("PresenceText"), errc, returnObject.m_presenceText, ARRAYSIZE(returnObject.m_presenceText));
    auto state = utils::extract_json_string(json, _T("State"), errc);
    returnObject.m_isTitleActive = (!state.empty() && utils::str_icmp(state, _T("active")) == 0);
    returnObject.m_titleId = utils::string_t_to_uint32(utils::extract_json_string(json, _T("TitleId"), errc));
    returnObject.m_isNull = false;

    return xbox_live_result<social_manager_presence_title_record>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END