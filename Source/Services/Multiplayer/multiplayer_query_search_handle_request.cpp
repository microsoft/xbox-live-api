// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_query_search_handle_request::multiplayer_query_search_handle_request(
    _In_ string_t serviceConfigurationId,
    _In_ string_t sessionTemplateName,
    _In_ string_t orderBy,
    _In_ bool orderAscending,
    _In_ string_t searchFilter
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_sessionTemplateName(std::move(sessionTemplateName)),
    m_orderBy(std::move(orderBy)),
    m_orderAscending(orderAscending),
    m_searchFilter(std::move(searchFilter))
{
}

multiplayer_query_search_handle_request::multiplayer_query_search_handle_request(
    _In_ string_t serviceConfigurationId,
    _In_ string_t sessionTemplateName
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_sessionTemplateName(std::move(sessionTemplateName)),
    m_orderAscending(false)
{
}

const string_t&
multiplayer_query_search_handle_request::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const string_t&
multiplayer_query_search_handle_request::session_template_name() const
{
    return m_sessionTemplateName;
}

bool
multiplayer_query_search_handle_request::order_ascending()
{
    return m_orderAscending;
}

void
multiplayer_query_search_handle_request::set_order_ascending(_In_ bool orderAscending)
{
    m_orderAscending = orderAscending;
}

const string_t&
multiplayer_query_search_handle_request::order_by() const
{
    return m_orderBy;
}

void
multiplayer_query_search_handle_request::set_order_by(_In_ const string_t& orderBy)
{
    m_orderBy = orderBy;
}

const string_t&
multiplayer_query_search_handle_request::search_filter() const
{
    return m_searchFilter;
}

void
multiplayer_query_search_handle_request::set_search_filter(_In_ const string_t& searchFilter)
{
    m_searchFilter = searchFilter;
}

const string_t&
multiplayer_query_search_handle_request::social_group() const
{
    return m_socialGroup;
}

void
multiplayer_query_search_handle_request::set_social_group(_In_ const string_t& socialGroup)
{
    m_socialGroup = socialGroup;
}

web::json::value
multiplayer_query_search_handle_request::_Serialize(
    _In_ const string_t& socialGroupXuid
    ) const
{
    web::json::value serializedObject;
    serializedObject[_T("type")] = web::json::value::string(_T("search"));
    serializedObject[_T("scid")] = web::json::value::string(m_serviceConfigurationId);
    serializedObject[_T("templateName")] = web::json::value::string(m_sessionTemplateName);
    serializedObject[_T("filter")] = web::json::value::string(m_searchFilter);
    serializedObject[_T("global")] = web::json::value::boolean(true);

    if (!m_orderBy.empty())
    {
        stringstream_t orderByQuery;
        orderByQuery << m_orderBy;
        if (m_orderAscending)
        {
            orderByQuery << _T(" asc");
        }
        else
        {
            orderByQuery << _T(" desc");
        }
        serializedObject[_T("orderBy")] = web::json::value::string(orderByQuery.str());
    }

    if (!m_socialGroup.empty())
    {
        web::json::value ownerObject;
        web::json::value peopleObject;
        peopleObject[_T("moniker")] = web::json::value::string(m_socialGroup);
        peopleObject[_T("monikerXuid")] = web::json::value::string(socialGroupXuid);

        ownerObject[_T("people")] = std::move(peopleObject);
        serializedObject[_T("sessionMembers")] = ownerObject;
    }

    return serializedObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END