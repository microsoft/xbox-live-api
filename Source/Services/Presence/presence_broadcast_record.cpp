// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_broadcast_record::presence_broadcast_record(
    _In_ std::shared_ptr<presence_broadcast_record_internal> internalObj
    ):
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(presence_broadcast_record, broadcast_id);
DEFINE_GET_STRING(presence_broadcast_record, session);
DEFINE_GET_STRING(presence_broadcast_record, provider);
DEFINE_GET_UINT32(presence_broadcast_record, viewer_count);
DEFINE_GET_OBJECT_REF(presence_broadcast_record, utility::datetime, start_time);

bool presence_broadcast_record::operator!=(
    _In_ const presence_broadcast_record& rhs
    ) const
{
    return *m_internalObj != *(rhs.m_internalObj);
}

presence_broadcast_record_internal::presence_broadcast_record_internal()
    : m_viewerCount(0)
{
}

const xsapi_internal_string&
presence_broadcast_record_internal::broadcast_id() const
{
    return m_broadcastId;
}

const xsapi_internal_string&
presence_broadcast_record_internal::session() const
{
    return m_session;
}

const xsapi_internal_string&
presence_broadcast_record_internal::provider() const
{
    return m_provider;
}

uint32_t
presence_broadcast_record_internal::viewer_count() const
{
    return m_viewerCount;
}

const utility::datetime&
presence_broadcast_record_internal::start_time() const
{
    return m_startTime;
}

bool presence_broadcast_record_internal::operator!=(
    _In_ const presence_broadcast_record_internal& rhs
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

xbox_live_result<std::shared_ptr<presence_broadcast_record_internal>>
presence_broadcast_record_internal::deserialize(
    _In_ const web::json::value& inputJson
    )
{
    auto returnObject = xsapi_allocate_shared<presence_broadcast_record_internal>();
    if (inputJson.is_null()) return xbox_live_result<std::shared_ptr<presence_broadcast_record_internal>>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;

    returnObject->m_broadcastId = utils::extract_json_string(inputJson, "id", errc);
    returnObject->m_session = utils::extract_json_string(inputJson, "session", errc);
    returnObject->m_provider = utils::extract_json_string(inputJson, "provider", errc);
    returnObject->m_viewerCount = utils::extract_json_int(inputJson, "viewers", errc);
    returnObject->m_startTime = utils::extract_json_time(inputJson, "started", errc);

    return xbox_live_result<std::shared_ptr<presence_broadcast_record_internal>>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END