// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_activity_query_post_request::multiplayer_activity_query_post_request()
{
}

multiplayer_activity_query_post_request::multiplayer_activity_query_post_request(
    _In_ string_t scid, 
    _In_ std::vector<string_t> xuids
    ) :
    m_scid(std::move(scid)),
    m_xuids(std::move(xuids))
{
}

multiplayer_activity_query_post_request::multiplayer_activity_query_post_request(
    _In_ string_t scid, 
    _In_ string_t socialGroup, 
    _In_ string_t socialGroupXuid
    ) :
    m_scid(std::move(scid)),
    m_socialGroup(std::move(socialGroup)),
    m_socialGroupXuid(std::move(socialGroupXuid))
{
}

const string_t& 
multiplayer_activity_query_post_request::scid() const
{
    return m_scid;
}

const std::vector<string_t>& 
multiplayer_activity_query_post_request::xuids() const
{
    return m_xuids;
}
const string_t& 
multiplayer_activity_query_post_request::social_group() const
{
    return m_socialGroup;
}

const string_t& 
multiplayer_activity_query_post_request::social_group_xuid() const
{
    return m_socialGroupXuid;
}

web::json::value 
multiplayer_activity_query_post_request::serialize()
{
    XSAPI_ASSERT(m_socialGroup.empty() || m_xuids.empty());
    XSAPI_ASSERT(!m_socialGroup.empty() || !m_xuids.empty());

    web::json::value serializedObject;
    serializedObject[_T("type")] = web::json::value::string(_T("activity"));
    serializedObject[_T("scid")] = web::json::value::string(m_scid);

    web::json::value ownerObject;
    if (!m_xuids.empty())
    {
        ownerObject[_T("xuids")] = utils::serialize_vector<string_t>(utils::json_string_serializer, m_xuids);
    }
    else
    {
        web::json::value peopleObject;
        peopleObject[_T("moniker")] = web::json::value::string(m_socialGroup);
        peopleObject[_T("monikerXuid")] = web::json::value::string(m_socialGroupXuid);
        ownerObject[_T("people")] = std::move(peopleObject);
    }

    serializedObject[_T("owners")] = ownerObject;

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END