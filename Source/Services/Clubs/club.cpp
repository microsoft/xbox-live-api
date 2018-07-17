// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/clubs.h"
#include "clubs_service_impl.h"
#include "user_context.h"
#include "clubs_serializers.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

using namespace web;

club::club(_In_ std::shared_ptr<clubs_service_impl> clubsService)
    : m_service(std::move(clubsService))
{
}

const string_t& club::owner() const 
{
    return m_owner;
}

const string_t& club::id() const
{
    return m_id;
}

club_type club::type() const
{
    return m_type;
}

const utility::datetime& club::created() const
{
    return m_created;
}

const string_t& club::title_family_id() const
{
    return m_titleFamilyId;
}

const string_t& club::title_family_name() const
{
    return m_titleFamilyName;
}

const string_t& club::short_name() const
{
    return m_shortName;
}

const club_profile& club::profile() const
{
    return m_profile;
}

const string_t& club::founder_xuid() const
{
    return m_founderXuid;
}

const string_t& club::glyph_image_url() const
{
    return m_glyphImageUrl;
}

const string_t& club::banner_image_url() const
{
    return m_bannerImageUrl;
}

uint32_t club::followers_count() const
{
    return m_followersCount;
}

bool club::is_club_suspended() const
{
    return m_isClubSuspended;
}

const utility::datetime& club::suspended_until() const
{
    return m_suspendedUntil;
}

uint32_t club::presence_count() const
{
    return m_presenceCount;
}

uint32_t club::presence_today_count() const
{
    return m_presenceTodayCount;
}

uint32_t club::members_count() const
{
    return m_membersCount;
}

uint32_t club::moderators_count() const
{
    return m_moderatorsCount;
}

uint32_t club::recommended_count() const
{
    return m_recommendedCount;
}

uint32_t club::requested_to_join_count() const
{
    return m_requestedToJoinCount;
}

uint32_t club::report_count() const
{
    return m_reportCount;
}

uint32_t club::reported_items_count() const
{
    return m_reportedItemsCount;
}

const club_action_settings& club::action_settings() const
{
    return m_actionSettings;
}

const std::vector<club_role>& club::viewer_roles() const
{
    return m_viewerRoles;
}

pplx::task<xbox_live_result<club_roster>> club::get_roster()
{
    std::vector<string_t> decorations(1, _T("roster"));

    auto task = m_service->make_clubs_http_call(
        _T("GET"),
        _T("clubhub"),
        clubs_service_impl::clubhub_club_id_subpath(m_id, decorations),
        xbox_live_api::get_club
        )
    .then([](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<club_roster>(
            clubs_serializers::deserialize_club_roster_from_clubhub_reponse(response->response_body_json()),
            response
            );
    });
    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<std::vector<club_user_presence_record>>> club::get_club_user_presence_records()
{
    std::vector<string_t> decorations;
    decorations.push_back(_T("clubPresence"));

    auto task = m_service->make_clubs_http_call(
        _T("GET"),
        _T("clubhub"),
        clubs_service_impl::clubhub_club_id_subpath(m_id, decorations),
        xbox_live_api::get_club
        )
    .then([](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<std::vector<club_user_presence_record>>(
            clubs_serializers::deserialize_club_user_presence_record_from_clubhub_response(response->response_body_json()),
            response
            );
    });
    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<void>> club::rename_club(
    _In_ const string_t& newName
    )
{
    json::value body;
    body[_T("method")] = json::value::string(_T("ChangeName"));
    body[_T("name")] = json::value::string(newName);

    auto task = m_service->make_clubs_http_call(
        _T("POST"),
        _T("clubaccounts"),
        clubs_service_impl::clubaccounts_clubs_subpath(m_id),
        xbox_live_api::rename_club,
        body
        )
    .then([](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result(response);
    });

    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::get_roles(
    _In_ const string_t& xuid
)
{
    if (xuid == utils::string_t_from_internal_string(m_service->m_userContext->xbox_user_id()))
    {
        return pplx::task_from_result(xbox_live_result<std::vector<club_role>>(m_viewerRoles));
    }

    auto task = get_role_records(xuid)
    .then([](xbox_live_result<std::vector<club_role_record>> result)
    {
        std::vector<club_role> roles;
        if (!result.err())
        {
            for (const auto& record : result.payload())
            {
                roles.push_back(record.role());
            }
        }
        return xbox_live_result<std::vector<club_role>>(roles, result.err(), result.err_message());
    });
    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<std::vector<club_role_record>>> club::get_role_records()
{
    return get_role_records(utils::string_t_from_internal_string(m_service->m_userContext->xbox_user_id()));
}

pplx::task<xbox_live_result<std::vector<club_role_record>>> club::get_role_records(
    _In_ const string_t& xuid
    )
{
    auto iter = m_roleRecords.find(xuid);
    if (iter != m_roleRecords.end())
    {
        return pplx::task_from_result(xbox_live_result<std::vector<club_role_record>>(iter->second));
    }

    auto task = m_service->get_club_associations(xuid)
    .then([this, xuid](xbox_live_result<std::vector<club>> result)
    {
        std::vector<club_role_record> records;

        if (!result.err())
        {
            auto clubs = result.payload();
            string_t clubId = m_id;

            auto iter = std::find_if(clubs.begin(), clubs.end(), [&clubId](club club)->bool
            {
                return club.id() == clubId;
            });
            
            if (iter != clubs.end())
            {
                records = iter->m_roleRecords[xuid];

                this->m_roleRecords[xuid] = records;
            }            
        }
        return xbox_live_result<std::vector<club_role_record>>(records, result.err(), result.err_message());
    });
    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<club_presence_counts>> club::get_presence_counts()
{
    auto task = m_service->make_clubs_http_call(
        _T("GET"), 
        _T("clubpresence"), 
        clubs_service_impl::clubpresence_subpath(m_id),
        xbox_live_api::get_club
        )
    .then([](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<club_presence_counts>(
            clubs_serializers::deserialize_club_presence_info(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<void>> club::set_presence_within_club(
    _In_ club_user_presence presence
    )
{
    json::value body;
    body[_T("userPresenceState")] = json::value::string(clubs_service_impl::convert_user_presence_to_string(presence));

    auto task = m_service->make_clubs_http_call(
        _T("POST"),
        _T("clubpresence"),
        clubs_service_impl::clubpresence_subpath(m_id, utils::string_t_from_internal_string(m_service->m_userContext->xbox_user_id())),
        xbox_live_api::set_user_presence_within_club,
        body
        )
    .then([](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result(response);
    });

    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::add_user_to_club()
{
    return update_users_club_roles(utils::string_t_from_internal_string(m_service->m_userContext->xbox_user_id()), true);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::remove_user_from_club()
{
    return update_users_club_roles(utils::string_t_from_internal_string(m_service->m_userContext->xbox_user_id()), false);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::follow_club()
{
    return set_users_club_roles(utils::string_t_from_internal_string(m_service->m_userContext->xbox_user_id()), club_role::follower, true);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::unfollow_club()
{
    return set_users_club_roles(utils::string_t_from_internal_string(m_service->m_userContext->xbox_user_id()), club_role::follower, false);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::add_user_to_club(
    _In_ const string_t& xuid
    )
{
    return update_users_club_roles(xuid, true);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::remove_user_from_club(
    _In_ const string_t& xuid
    )
{
    return update_users_club_roles(xuid, false);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::ban_user_from_club(
    _In_ const string_t& xuid
    )
{
    return set_users_club_roles(xuid, club_role::banned, true);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::unban_user_from_club(
    _In_ const string_t& xuid
    )
{
    return set_users_club_roles(xuid, club_role::banned, false);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::add_club_moderator(
    _In_ const string_t& xuid
    )
{
    return set_users_club_roles(xuid, club_role::moderator, true);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::remove_club_moderator(
    _In_ const string_t& xuid
    )
{
    return set_users_club_roles(xuid, club_role::moderator, false);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::update_users_club_roles(
    _In_ const string_t& xuid,
    _In_ bool advanceWithinClub
    )
{
    auto httpMethod = advanceWithinClub ? _T("PUT") : _T("DELETE");
    auto api = advanceWithinClub ? xbox_live_api::add_user_to_club : xbox_live_api::remove_user_from_club;

    auto task = m_service->make_clubs_http_call(
        httpMethod,
        _T("clubroster"),
        clubs_service_impl::clubroster_subpath(m_id, xuid),
        api
        )
    .then([this](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<std::vector<club_role>>(
            clubs_serializers::deserialize_club_roles_from_club_member(response->response_body_json()),
            response
            );
    });
    return utils::create_exception_free_task(task);
}

pplx::task<xbox_live_result<std::vector<club_role>>> club::set_users_club_roles(
    _In_ const string_t& xuid,
    _In_ club_role role,
    _In_ bool addRole
    )
{
    json::value requestBody;
    if (addRole)
    {
        std::vector<json::value> rolesArray;
        rolesArray.push_back(json::value::string(clubs_service_impl::convert_club_role_to_string(role)));
        requestBody[_T("roles")] = json::value::array(rolesArray);
    }

    auto httpMethod = addRole ? _T("POST") : _T("DELETE");
    auto path = addRole ? clubs_service_impl::clubroster_roles_subpath(m_id, xuid) : clubs_service_impl::clubroster_roles_subpath(m_id, xuid, role);
    auto api = addRole ? xbox_live_api::add_club_role : xbox_live_api::remove_club_role;

    auto task = m_service->make_clubs_http_call(
        httpMethod,
        _T("clubroster"),
        path,
        api,
        requestBody
        )
    .then([this](std::shared_ptr<http_call_response> response)
    {
        return clubs_serializers::generate_xbox_live_result<std::vector<club_role>>(
            clubs_serializers::deserialize_club_roles_from_club_member(response->response_body_json()),
            response
            );
    });
    return utils::create_exception_free_task(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END