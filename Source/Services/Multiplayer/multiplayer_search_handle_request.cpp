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
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_search_handle_request::multiplayer_search_handle_request(
    _In_ multiplayer_session_reference sessionReference
    ) :
    m_sessionReference(std::move(sessionReference))
{
}

const multiplayer_session_reference&
multiplayer_search_handle_request::session_reference() const
{
    return m_sessionReference;
}

const std::vector<string_t>&
multiplayer_search_handle_request::tags() const
{
    return m_tags;
}

void
multiplayer_search_handle_request::set_tags(
    _In_ const std::vector<string_t>& value
)
{
    m_tags = std::move(value);
}

const std::unordered_map<string_t, double>&
multiplayer_search_handle_request::numbers_metadata() const
{
    return m_numbersMetadata;
}

void
multiplayer_search_handle_request::set_numbers_metadata(
    _In_ const std::unordered_map<string_t, double>& metadata
)
{
    m_numbersMetadata = std::move(metadata);
}

const std::unordered_map<string_t, string_t>&
multiplayer_search_handle_request::strings_metadata() const
{
    return m_stringsMetadata;
}

void
multiplayer_search_handle_request::set_strings_metadata(
    _In_ const std::unordered_map<string_t, string_t>& metadata
)
{
    m_stringsMetadata = std::move(metadata);
}

void
multiplayer_search_handle_request::_Set_version(_In_ uint32_t version)
{
    m_version = version;
}

web::json::value
multiplayer_search_handle_request::_Serialize() const
{
    web::json::value serializedObject;

    serializedObject[_T("type")] = web::json::value::string(_T("search"));
    serializedObject[_T("version")] = web::json::value(m_version);
    serializedObject[_T("sessionRef")] = m_sessionReference._Serialize();
    
    web::json::value searchAttributesJson;
    bool setSearchAttributes = false;
    if (!m_tags.empty())
    {
        setSearchAttributes = true;
        searchAttributesJson[_T("tags")] = utils::serialize_vector<string_t>(utils::json_string_serializer, m_tags);
    }

    if (m_numbersMetadata.size() > 0)
    {
        setSearchAttributes = true;
        web::json::value numbersPropertiesJson;
        for (const auto& metadata: m_numbersMetadata)
        {
            numbersPropertiesJson[metadata.first] = web::json::value::number(static_cast<double>(metadata.second));
        }
        searchAttributesJson[_T("numbers")] = numbersPropertiesJson;
    }

    if (m_stringsMetadata.size() > 0)
    {
        setSearchAttributes = true;
        web::json::value stringsPropertiesJson;
        for (const auto& metadata : m_stringsMetadata)
        {
            stringsPropertiesJson[metadata.first] = web::json::value::string(metadata.second);
        }
        searchAttributesJson[_T("strings")] = stringsPropertiesJson;
    }

    if (setSearchAttributes)
    {
        serializedObject[_T("searchAttributes")] = searchAttributesJson;
    }

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
