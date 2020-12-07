// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

MultiplayerQuerySearchHandleRequest::MultiplayerQuerySearchHandleRequest(
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& sessionTemplateName,
    _In_ const xsapi_internal_string& orderBy,
    _In_ bool orderAscending,
    _In_ const xsapi_internal_string& searchFilter
    ) :
    m_serviceConfigurationId(scid),
    m_sessionTemplateName(sessionTemplateName),
    m_orderBy(orderBy),
    m_orderAscending(orderAscending),
    m_searchFilter(searchFilter)
{
}

MultiplayerQuerySearchHandleRequest::MultiplayerQuerySearchHandleRequest(
    _In_ const xsapi_internal_string& scid,
    _In_ const xsapi_internal_string& sessionTemplateName
    ) :
    m_serviceConfigurationId(scid),
    m_sessionTemplateName(sessionTemplateName)
{
}

const xsapi_internal_string&
MultiplayerQuerySearchHandleRequest::Scid() const
{
    return m_serviceConfigurationId;
}

const xsapi_internal_string&
MultiplayerQuerySearchHandleRequest::SessionTemplateName() const
{
    return m_sessionTemplateName;
}

bool
MultiplayerQuerySearchHandleRequest::OrderAscending()
{
    return m_orderAscending;
}

void
MultiplayerQuerySearchHandleRequest::SetOrderAscending(_In_ bool orderAscending)
{
    m_orderAscending = orderAscending;
}

const xsapi_internal_string&
MultiplayerQuerySearchHandleRequest::OrderBy() const
{
    return m_orderBy;
}

void
MultiplayerQuerySearchHandleRequest::SetOrderBy(_In_ const xsapi_internal_string& orderBy)
{
    m_orderBy = orderBy;
}

const xsapi_internal_string&
MultiplayerQuerySearchHandleRequest::SearchFilter() const
{
    return m_searchFilter;
}

void
MultiplayerQuerySearchHandleRequest::SetSearchFilter(_In_ const xsapi_internal_string& searchFilter)
{
    m_searchFilter = searchFilter;
}

const xsapi_internal_string&
MultiplayerQuerySearchHandleRequest::SocialGroup() const
{
    return m_socialGroup;
}

void
MultiplayerQuerySearchHandleRequest::SetSocialGroup(_In_ const xsapi_internal_string& socialGroup)
{
    m_socialGroup = socialGroup;
}

void
MultiplayerQuerySearchHandleRequest::Serialize(
    _In_ uint64_t socialGroupXuid,
    _Out_ JsonValue& json,
    _In_ JsonDocument::AllocatorType& allocator
) const
{
    json.SetObject();
    json.AddMember("type", JsonValue("search", allocator).Move(), allocator);
    json.AddMember("scid", JsonValue(m_serviceConfigurationId.c_str(), allocator).Move(), allocator);
    json.AddMember("templateName", JsonValue(m_sessionTemplateName.c_str(), allocator).Move(), allocator);
    json.AddMember("global", true, allocator);

    if (!m_searchFilter.empty())
    {
        json.AddMember("filter", JsonValue(m_searchFilter.c_str(), allocator).Move(), allocator);
    }

    if (!m_orderBy.empty())
    {
        xsapi_internal_stringstream orderByQuery;
        orderByQuery << m_orderBy;
        if (m_orderAscending)
        {
            orderByQuery << " asc";
        }
        else
        {
            orderByQuery << " desc";
        }
        json.AddMember("orderBy", JsonValue(orderByQuery.str().c_str(), allocator).Move(), allocator);
    }

    if (!m_socialGroup.empty())
    {
        JsonValue ownerObject(rapidjson::kObjectType);
        JsonValue peopleObject(rapidjson::kObjectType);
        peopleObject.AddMember("moniker", JsonValue(m_socialGroup.c_str(), allocator).Move(), allocator);
        peopleObject.AddMember("monikerXuid", JsonValue(utils::uint64_to_internal_string(socialGroupXuid).c_str(), allocator).Move(), allocator);

        ownerObject.AddMember("people", peopleObject, allocator);
        json.AddMember("sessionMembers", ownerObject, allocator);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END