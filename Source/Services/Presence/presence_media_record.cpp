// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "presence_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_media_record::presence_media_record(
    _In_ std::shared_ptr<presence_media_record_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(presence_media_record, media_id);
DEFINE_GET_ENUM_TYPE(presence_media_record, presence_media_id_type, media_id_type);
DEFINE_GET_STRING(presence_media_record, name);

presence_media_record_internal::presence_media_record_internal() :
    m_mediaIdType(presence_media_id_type::unknown)
{
}

const xsapi_internal_string& 
presence_media_record_internal::media_id() const
{
    return m_mediaId;
}

presence_media_id_type 
presence_media_record_internal::media_id_type() const
{
    return m_mediaIdType;
}

const xsapi_internal_string& 
presence_media_record_internal::name() const
{
    return m_name;
}

xbox_live_result<std::shared_ptr<presence_media_record_internal>>
presence_media_record_internal::deserialize(
    _In_ const web::json::value& inputJson
    )
{
    auto returnObject = xsapi_allocate_shared<presence_media_record_internal>();
    if (inputJson.is_null()) return xbox_live_result<std::shared_ptr<presence_media_record_internal>>(returnObject);

    returnObject->m_mediaId = utils::extract_json_string(inputJson, "id");
    returnObject->m_mediaIdType = convert_string_to_media_id_type(
        utils::extract_json_string(inputJson, "idType")
        );
    returnObject->m_name = utils::extract_json_string(inputJson, "name");

    return returnObject;
}

presence_media_id_type 
presence_media_record_internal::convert_string_to_media_id_type(
    _In_ const xsapi_internal_string& value
    )
{
    if (utils::str_icmp(value, "bing") == 0)
    {
        return presence_media_id_type::bing;
    }
    else if (utils::str_icmp(value, "provider") == 0)
    {
        return presence_media_id_type::media_provider;
    }

    return presence_media_id_type::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END