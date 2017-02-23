// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_data::presence_data() : 
    m_shouldSerialize(false)
{
}

presence_data::presence_data(
    _In_ string_t serviceConfigurationId,
    _In_ string_t presenceId
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_presenceId(std::move(presenceId)),
    m_shouldSerialize(true)
{
    XSAPI_ASSERT(!m_serviceConfigurationId.empty());
    XSAPI_ASSERT(!m_presenceId.empty());
}

presence_data::presence_data(
    _In_ string_t serviceConfigurationId,
    _In_ string_t presenceId,
    _In_ std::vector<string_t> presenceTokenIds
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_presenceId(std::move(presenceId)),
    m_presenceTokenIds(std::move(presenceTokenIds)),
    m_shouldSerialize(true)
{
    XSAPI_ASSERT(!m_serviceConfigurationId.empty());
    XSAPI_ASSERT(!m_presenceId.empty());
    XSAPI_ASSERT(!m_presenceTokenIds.empty());
#ifndef NDEBUG
    for (const auto& token : m_presenceTokenIds)
    {
        XSAPI_ASSERT(!token.empty());
    }
#endif
}

const string_t&
presence_data::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const string_t&
presence_data::presence_id() const
{
    return m_presenceId;
}

bool
presence_data::_Should_serialize() const
{
    return m_shouldSerialize;
}

const std::vector<string_t>&
presence_data::presence_token_ids() const
{
    return m_presenceTokenIds;
}

web::json::value
presence_data::_Serialize() const
{
    web::json::value serializedObject;

    serializedObject[_T("id")] = web::json::value::string(m_presenceId);
    serializedObject[_T("scid")] = web::json::value::string(m_serviceConfigurationId);
    if (!m_presenceTokenIds.empty())
    {
        serializedObject[_T("params")] = utils::serialize_vector<string_t>(utils::json_string_serializer, m_presenceTokenIds);
    }

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END