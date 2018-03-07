// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

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

presence_data_internal::presence_data_internal()
    : m_shouldSerialize(false)
{
}

presence_data_internal::presence_data_internal(
    _In_ xsapi_internal_string serviceConfigurationId,
    _In_ xsapi_internal_string presenceId
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_presenceId(std::move(presenceId)),
    m_shouldSerialize(true)
{
    XSAPI_ASSERT(!m_serviceConfigurationId.empty());
    XSAPI_ASSERT(!m_presenceId.empty());
}

presence_data_internal::presence_data_internal(
    _In_ xsapi_internal_string serviceConfigurationId,
    _In_ xsapi_internal_string presenceId,
    _In_ xsapi_internal_vector<xsapi_internal_string> presenceTokenIds
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

presence_data_internal::presence_data_internal(
    _In_ const presence_data& presenceData
    ) :
    m_shouldSerialize(presenceData._Should_serialize())
{
    if (m_shouldSerialize)
    {
        m_serviceConfigurationId = utils::internal_string_from_string_t(presenceData.service_configuration_id());
        m_presenceId = utils::internal_string_from_string_t(presenceData.presence_id());
        m_presenceTokenIds = utils::internal_string_vector_from_std_string_vector(presenceData.presence_token_ids());
    }
}

const xsapi_internal_string& presence_data_internal::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const xsapi_internal_string& presence_data_internal::presence_id() const
{
    return m_presenceId;
}


const xsapi_internal_vector<xsapi_internal_string>& presence_data_internal::presence_token_ids() const
{
    return m_presenceTokenIds;
}

bool presence_data_internal::should_serialize() const
{
    return m_shouldSerialize;
}

web::json::value presence_data_internal::serialize() const
{
    web::json::value serializedObject;

    serializedObject[_T("id")] = web::json::value::string(utils::string_t_from_internal_string(m_presenceId));
    serializedObject[_T("scid")] = web::json::value::string(utils::string_t_from_internal_string(m_serviceConfigurationId));
    if (!m_presenceTokenIds.empty())
    {
        serializedObject[_T("params")] = utils::serialize_vector<xsapi_internal_string>(utils::json_internal_string_serializer, m_presenceTokenIds);
    }

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END