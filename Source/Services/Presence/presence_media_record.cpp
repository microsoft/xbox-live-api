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

presence_media_record::presence_media_record() :
    m_mediaIdType(presence_media_id_type::unknown)
{
}

const string_t& 
presence_media_record::media_id() const
{
    return m_mediaId;
}

presence_media_id_type 
presence_media_record::media_id_type() const
{
    return m_mediaIdType;
}

const string_t& 
presence_media_record::name() const
{
    return m_name;
}

xbox_live_result<presence_media_record>
presence_media_record::_Deserialize(
    _In_ const web::json::value& inputJson
    )
{
    presence_media_record returnObject;
    if (inputJson.is_null()) return xbox_live_result<presence_media_record>(returnObject);

    returnObject.m_mediaId = utils::extract_json_string(inputJson, _T("id"));
    returnObject.m_mediaIdType = _Convert_string_to_media_id_type(
        utils::extract_json_string(inputJson, _T("idType"))
        );
    returnObject.m_name = utils::extract_json_string(inputJson, _T("name"));

    return returnObject;
}

presence_media_id_type 
presence_media_record::_Convert_string_to_media_id_type(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("bing")) == 0)
    {
        return presence_media_id_type::bing;
    }
    else if (utils::str_icmp(value, _T("provider")) == 0)
    {
        return presence_media_id_type::media_provider;
    }

    return presence_media_id_type::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END