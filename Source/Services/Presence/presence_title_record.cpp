// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_title_record::presence_title_record(
    _In_ std::shared_ptr<presence_title_record_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_UINT32(presence_title_record, title_id);
DEFINE_GET_STRING(presence_title_record, title_name);
DEFINE_GET_OBJECT_REF(presence_title_record, utility::datetime, last_modified_date);
DEFINE_GET_BOOL(presence_title_record, is_title_active);
DEFINE_GET_STRING(presence_title_record, presence);
DEFINE_GET_ENUM_TYPE(presence_title_record, presence_title_view_state, presence_title_view);

presence_broadcast_record
presence_title_record::broadcast_record() const
{
    return presence_broadcast_record(m_internalObj->broadcast_record());
}

bool
presence_title_record::operator!=(
    _In_ const presence_title_record& rhs
    ) const
{
    return *m_internalObj != *rhs.m_internalObj;
}

presence_title_record_internal::presence_title_record_internal() :
    m_titleViewState(presence_title_view_state::unknown),
    m_titleId(0),
    m_isTitleActive(false)
{
}

presence_title_record_internal::presence_title_record_internal(
    _In_ uint32_t titleId,
    _In_ title_presence_state titlePresenceState
    ) :
    m_titleId(titleId),
    m_titleViewState(presence_title_view_state::unknown)
{
    m_isTitleActive = (titlePresenceState == title_presence_state::started);
}

uint32_t
presence_title_record_internal::title_id() const
{
    return m_titleId;
}

const xsapi_internal_string&
presence_title_record_internal::title_name() const
{
    return m_titleName;
}

const utility::datetime&
presence_title_record_internal::last_modified_date() const
{
    return m_lastModifiedDate;
}

bool
presence_title_record_internal::is_title_active() const
{
    return m_isTitleActive;
}

const xsapi_internal_string&
presence_title_record_internal::presence() const
{
    return m_presence;
}

presence_title_view_state
presence_title_record_internal::presence_title_view() const
{
    return m_titleViewState;
}

const std::shared_ptr<presence_broadcast_record_internal> 
presence_title_record_internal::broadcast_record() const
{
    return m_broadcastRecord;
}


bool
presence_title_record_internal::operator!=(
    _In_ const presence_title_record_internal& rhs
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

xbox_live_result<std::shared_ptr<presence_title_record_internal>>
presence_title_record_internal::deserialize(
    _In_ const web::json::value& json
    )
{
    auto returnObject = xsapi_allocate_shared<presence_title_record_internal>();
    if (json.is_null()) return xbox_live_result<std::shared_ptr<presence_title_record_internal>>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;
    auto activityJson = utils::extract_json_field(json, "activity", errc, false);

    returnObject->m_titleId = utils::internal_string_to_uint32(utils::extract_json_string(json, "id", errc));
    returnObject->m_titleName = utils::extract_json_string(json, "name", errc);
    returnObject->m_lastModifiedDate = utils::extract_json_time(json, "lastModified", errc);

    xsapi_internal_string state = utils::extract_json_string(json, "state", errc);
    returnObject->m_isTitleActive = (!state.empty() && utils::str_icmp(state, "active") == 0);
    returnObject->m_presence = utils::extract_json_string(activityJson, "richPresence", errc);
    returnObject->m_titleViewState = convert_string_to_presence_title_view_state(
        utils::extract_json_string(json, "placement", errc)
        );

    auto broadcastRecord = presence_broadcast_record_internal::deserialize(
        utils::extract_json_field(activityJson, "broadcast", errc, false)
        );

    if (broadcastRecord.err())
    {
        errc = broadcastRecord.err();
    }

    returnObject->m_broadcastRecord = broadcastRecord.payload();

    return xbox_live_result<std::shared_ptr<presence_title_record_internal>>(returnObject, errc);
}

presence_title_view_state
presence_title_record_internal::convert_string_to_presence_title_view_state(
    _In_ const xsapi_internal_string &value
    )
{
    if (utils::str_icmp(value, "full") == 0)
    {
        return presence_title_view_state::full_screen;
    }
    else if (utils::str_icmp(value, "fill") == 0)
    {
        return presence_title_view_state::filled;
    }
    else if (utils::str_icmp(value, "snapped") == 0)
    {
        return presence_title_view_state::snapped;
    }
    else if (utils::str_icmp(value, "background") == 0)
    {
        return presence_title_view_state::background;
    }

    return presence_title_view_state::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END