// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_title_record::presence_title_record() :
    m_titleViewState(presence_title_view_state::unknown),
    m_titleId(0),
    m_isTitleActive(false)
{
}

presence_title_record::presence_title_record(
    _In_ uint32_t titleId,
    _In_ title_presence_state titlePresenceState
    ) :
    m_titleId(titleId),
    m_titleViewState(presence_title_view_state::unknown)
{
    _Set_title_state(titlePresenceState);
}

uint32_t
presence_title_record::title_id() const
{
    return m_titleId;
}

const string_t&
presence_title_record::title_name() const
{
    return m_titleName;
}

const utility::datetime
presence_title_record::last_modified_date() const
{
    return m_lastModifiedDate;
}

bool
presence_title_record::is_title_active() const
{
    return m_isTitleActive;
}

const string_t&
presence_title_record::presence() const
{
    return m_presence;
}

presence_title_view_state
presence_title_record::presence_title_view() const
{
    return m_titleViewState;
}

const presence_broadcast_record&
presence_title_record::broadcast_record() const
{
    return m_broadcastRecord;
}

void
presence_title_record::_Set_title_state(
    _In_ title_presence_state state
    )
{
    m_isTitleActive = (state == title_presence_state::started);
}

bool
presence_title_record::operator!=(
    _In_ const presence_title_record& rhs
    ) const
{
    return (
        m_broadcastRecord != rhs.m_broadcastRecord ||
        m_isTitleActive != rhs.m_isTitleActive ||
        m_lastModifiedDate != rhs.m_lastModifiedDate ||
        m_presence != rhs.m_presence ||
        m_titleId != rhs.m_titleId ||
        m_titleName != rhs.m_titleName ||
        m_titleViewState != rhs.m_titleViewState
        );
}

xbox_live_result<presence_title_record>
presence_title_record::_Deserialize(
    _In_ const web::json::value& json
    )
{
    presence_title_record returnObject;
    if (json.is_null()) return xbox_live_result<presence_title_record>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;
    auto activityJson = utils::extract_json_field(json, _T("activity"), errc, false);

    returnObject.m_titleId = utils::string_t_to_uint32(utils::extract_json_string(json, _T("id"), errc));
    returnObject.m_titleName = utils::extract_json_string(json, _T("name"), errc);
    returnObject.m_lastModifiedDate = utils::extract_json_time(json, _T("lastModified"), errc);
    
    string_t state = utils::extract_json_string(json, _T("state"), errc);
    returnObject.m_isTitleActive = (!state.empty() && utils::str_icmp(state, _T("active")) == 0);
    returnObject.m_presence = utils::extract_json_string(activityJson, _T("richPresence"), errc);
    returnObject.m_titleViewState = _Convert_string_to_presence_title_view_state(
        utils::extract_json_string(json, _T("placement"), errc)
        );

    auto broadcastRecord = presence_broadcast_record::_Deserialize(
        utils::extract_json_field(activityJson, _T("broadcast"), errc, false)
        );

    if (broadcastRecord.err())
    {
        errc = broadcastRecord.err();
    }

    returnObject.m_broadcastRecord = broadcastRecord.payload();

    return xbox_live_result<presence_title_record>(returnObject, errc);
}

presence_title_view_state
presence_title_record::_Convert_string_to_presence_title_view_state(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("full")) == 0)
    {
        return presence_title_view_state::full_screen;
    }
    else if (utils::str_icmp(value, _T("fill")) == 0)
    {
        return presence_title_view_state::filled;
    }
    else if (utils::str_icmp(value, _T("snapped")) == 0)
    {
        return presence_title_view_state::snapped;
    }
    else if (utils::str_icmp(value, _T("background")) == 0)
    {
        return presence_title_view_state::background;
    }

    return presence_title_view_state::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END