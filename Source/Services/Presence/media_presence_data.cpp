// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

media_presence_data::media_presence_data() :
    m_shouldSerialize(false)
{
}

media_presence_data::media_presence_data(
    _In_ string_t mediaId,
    _In_ presence_media_id_type mediaIdType
    ) :
    m_mediaId(std::move(mediaId)),
    m_presenceMediaIdType(mediaIdType),
    m_shouldSerialize(true)
{
    XSAPI_ASSERT(!m_mediaId.empty());
    XSAPI_ASSERT(
        m_presenceMediaIdType > presence_media_id_type::bing &&
        m_presenceMediaIdType < presence_media_id_type::media_provider
        );
}

bool
media_presence_data::should_serialize() const
{
    return m_shouldSerialize;
}

const string_t& 
media_presence_data::media_id() const
{
    return m_mediaId;
}

const presence_media_id_type 
media_presence_data::media_id_type() const
{
    return m_presenceMediaIdType;
}

web::json::value
media_presence_data::serialize() const
{
    web::json::value serializedObject;
    serializedObject[_T("id")] = web::json::value::string(m_mediaId);
    
    if (m_presenceMediaIdType == presence_media_id_type::bing)
    {
        serializedObject[_T("idType")] = web::json::value::string(_T("bing"));
    }
    else if (m_presenceMediaIdType == presence_media_id_type::media_provider)
    {
        serializedObject[_T("idType")] = web::json::value::string(_T("provider"));
    }

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END