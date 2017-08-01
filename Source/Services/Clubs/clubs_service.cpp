// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
//#include "shared_macros.h"
//#include "xbox_system_factory.h"
#include "xsapi/clubs.h"
#include "clubs_service_impl.h"

using namespace xbox::services;
using namespace web;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

clubs_service::clubs_service(
    _In_ std::shared_ptr<user_context> userContext,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox_live_app_config> appConfig
    ) :
    m_clubsServiceImpl(std::make_shared<clubs_service_impl>(
        userContext,
        xboxLiveContextSettings,
        appConfig))
{
}

pplx::task<xbox_live_result<club>> clubs_service::get_club(
    _In_ const string_t& clubId
    )
{
    return m_clubsServiceImpl->get_club(clubId);
}

pplx::task<xbox_live_result<std::vector<club>>> clubs_service::get_clubs(
    _In_ const std::vector<string_t>& clubIds
    )
{
    return m_clubsServiceImpl->get_clubs(clubIds);
}

pplx::task<xbox_live_result<clubs_owned_result>> clubs_service::get_clubs_owned()
{
    return m_clubsServiceImpl->get_clubs_owned();
}

pplx::task<xbox_live_result<club>> clubs_service::create_club(
    _In_ string_t name,
    _In_ club_type type,
    _In_ string_t titleFamilyId
    )
{
    return m_clubsServiceImpl->create_club(name, type, titleFamilyId);
}

pplx::task<xbox_live_result<void>> clubs_service::delete_club(
    _In_ const string_t& clubId
    )
{
    return m_clubsServiceImpl->delete_club(clubId);
}

pplx::task<xbox_live_result<std::vector<club>>> clubs_service::get_club_associations()
{
    return m_clubsServiceImpl->get_club_associations();
}

pplx::task<xbox_live_result<std::vector<club>>> clubs_service::get_club_associations(
    _In_ const string_t& xuid
    )
{
    return m_clubsServiceImpl->get_club_associations(xuid);
}

pplx::task<xbox_live_result<std::vector<club_recommendation_t>>> clubs_service::get_club_recommendations()
{
    return m_clubsServiceImpl->get_club_recommendations();
}

pplx::task<xbox_live_result<clubs_search_result>> clubs_service::search_clubs(
    _In_ const string_t& queryString,
    _In_ const std::vector<string_t>& titleIds,
    _In_ const std::vector<string_t>& tags    
)
{
    return m_clubsServiceImpl->search_clubs(queryString, titleIds, tags);
}

pplx::task<xbox_live_result<std::vector<club_search_auto_complete>>> clubs_service::suggest_clubs(
    _In_ const string_t& queryString,
    _In_ const std::vector<string_t>& titleIds,
    _In_ const std::vector<string_t>& tags
    )
{
    return m_clubsServiceImpl->suggest_clubs(queryString, titleIds, tags);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END