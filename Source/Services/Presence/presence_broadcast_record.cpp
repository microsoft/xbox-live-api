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
#include "xsapi/presence.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_broadcast_record::presence_broadcast_record():
    m_viewerCount(0)
{
}

const string_t&
presence_broadcast_record::broadcast_id() const
{
    return m_broadcastId;
}

const string_t&
presence_broadcast_record::session() const
{
    return m_session;
}

const string_t&
presence_broadcast_record::provider() const
{
    return m_provider;
}

uint32_t
presence_broadcast_record::viewer_count() const
{
    return m_viewerCount;
}

const utility::datetime&
presence_broadcast_record::start_time() const
{
    return m_startTime;
}

bool presence_broadcast_record::operator!=(
    _In_ const presence_broadcast_record& rhs
    ) const
{
    if (
        m_broadcastId != rhs.m_broadcastId ||
        m_provider != rhs.m_provider ||
        m_session != rhs.m_session ||
        m_startTime != rhs.m_startTime ||
        m_viewerCount != rhs.m_viewerCount
        )
    {
        return true;
    }

    return false;
}

xbox_live_result<presence_broadcast_record>
presence_broadcast_record::_Deserialize(
    _In_ const web::json::value& inputJson
    )
{
    presence_broadcast_record returnObject;
    if (inputJson.is_null()) return xbox_live_result<presence_broadcast_record>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;

    returnObject.m_broadcastId = utils::extract_json_string(inputJson, _T("id"), errc);
    returnObject.m_session = utils::extract_json_string(inputJson, _T("session"), errc);
    returnObject.m_provider = utils::extract_json_string(inputJson, _T("provider"), errc);
    returnObject.m_viewerCount = utils::extract_json_int(inputJson, _T("viewers"), errc);
    returnObject.m_startTime = utils::extract_json_time(inputJson, _T("started"), errc);

    return xbox_live_result<presence_broadcast_record>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END