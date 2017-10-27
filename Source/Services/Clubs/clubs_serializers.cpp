// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"
#include "clubs_serializers.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

using namespace web;
using namespace xbox::services;

xbox_live_result<std::vector<club>> clubs_serializers::deserialize_clubs_from_clubhub_reponse(
    _In_ const json::value& clubHubResponse,
    _In_ std::shared_ptr<clubs_service_impl> service,
    _In_ const string_t& targetXuid
    )
{
    std::error_code errc;
    std::vector<club> clubVector;

    auto clubsJsonArray = utils::extract_json_as_array(
        utils::extract_json_field(clubHubResponse, _T("clubs"), errc, false),
        errc
        );

    for (const auto& clubJson : clubsJsonArray)
    {
        auto clubResult = _deserialize_club_from_clubhub_club(clubJson, service, targetXuid);
        if (clubResult.err())
        {
            return xbox_live_result<std::vector<club>>(clubResult.err(), clubResult.err_message());
        }
        clubVector.push_back(clubResult.payload());
    }
    return xbox_live_result<std::vector<club>>(clubVector, errc);
}

xbox_live_result<std::vector<club_user_presence_record>> 
clubs_serializers::deserialize_club_user_presence_record_from_clubhub_response(
    _In_ const json::value& clubHubResponse
    )
{
    std::error_code errc;
    std::vector<club_user_presence_record> userPresenceRecordsVector;

    auto clubsArray = utils::extract_json_as_array(
        utils::extract_json_field(clubHubResponse, _T("clubs"), errc, false),
        errc
        );

    if (clubsArray.size() == 0)
    {
        return xbox_live_result<std::vector<club_user_presence_record>>(userPresenceRecordsVector, errc);
    }

    auto clubPresenceArray = utils::extract_json_as_array(
        utils::extract_json_field(clubsArray[0], _T("clubPresence"), errc, false),
        errc
        );

    for (const auto& clubPresence : clubPresenceArray)
    {
        club_user_presence_record userPresenceRecord;

        userPresenceRecord.m_xuid = utils::extract_json_string(clubPresence, _T("xuid"), errc);
        userPresenceRecord.m_lastSeen = utils::extract_json_time(clubPresence, _T("lastSeenTimestamp"), errc);
        userPresenceRecord.m_lastSeenState = convert_string_to_club_user_presence(utils::extract_json_string(clubPresence, _T("lastSeenState"), errc)).payload();

        userPresenceRecordsVector.push_back(userPresenceRecord);
    }
    return xbox_live_result<std::vector<club_user_presence_record>>(userPresenceRecordsVector, errc);
}

xbox_live_result<std::vector<club_recommendation_t>> clubs_serializers::deserialize_recommendations_from_clubhub_response(
    _In_ const web::json::value& clubHubResponse,
    _In_ std::shared_ptr<clubs_service_impl> service
    )
{
    std::error_code errc;
    std::vector<club_recommendation_t> recommendations;

    auto clubsJsonArray = utils::extract_json_as_array(
        utils::extract_json_field(clubHubResponse, _T("clubs"), errc, false),
        errc
        );

    for (const auto& clubJson : clubsJsonArray)
    {
        auto clubResult = _deserialize_club_from_clubhub_club(clubJson, service);
        if (clubResult.err())
        {
            return xbox_live_result<std::vector<club_recommendation_t>>(clubResult.err(), clubResult.err_message());
        }

        auto recommendationJson = utils::extract_json_field(clubJson, _T("recommendation"), errc, false);
        auto reasonsJsonArray = utils::extract_json_as_array(
                utils::extract_json_field(recommendationJson, _T("reasons"), errc, false),
                errc
                );

        std::vector<string_t> reasons;
    
        for (const auto& reasonJson : reasonsJsonArray)
        {
            string_t reason = utils::extract_json_string(reasonJson, _T("localizedText"), errc);
            reasons.push_back(reason);
        }               
        recommendations.push_back(std::make_pair(clubResult.payload(), reasons));
    }
    return xbox_live_result<std::vector<club_recommendation_t>>(recommendations, errc);
}

xbox_live_result<clubs_search_result> clubs_serializers::deserialize_clubs_search_result_from_clubhub_response(
    _In_ const web::json::value& clubHubResponse,
    _In_ std::shared_ptr<clubs_service_impl> service
    )
{
    std::error_code errc;
    clubs_search_result searchResult;

    searchResult.m_clubs = deserialize_clubs_from_clubhub_reponse(clubHubResponse, service).payload();

    auto searchFacetResultsField = utils::extract_json_field(clubHubResponse, _T("searchFacetResults"), errc, false); 
    
    if (!searchFacetResultsField.is_null())
    {
        for (const auto& dictEntry : searchFacetResultsField.as_object())
        {
            auto searchFacetResultArray = dictEntry.second.as_array();
            std::vector<club_search_facet_result> searchFacetResultsVector;

            for (const auto& searchFacetResultJson : searchFacetResultArray)
            {
                club_search_facet_result searchFacetResult;
                searchFacetResult.m_count = utils::extract_json_int(searchFacetResultJson, _T("count"), errc);
                searchFacetResult.m_value = utils::extract_json_string(searchFacetResultJson, _T("value"), errc);
                searchFacetResultsVector.push_back(searchFacetResult);
            }
            searchResult.m_searchFacetResults[dictEntry.first] = searchFacetResultsVector;
        }
    }
    return xbox_live_result<clubs_search_result>(searchResult, errc);
}

xbox_live_result<std::vector<club_search_auto_complete>> clubs_serializers::deserialize_club_suggestions(
    _In_ const web::json::value& suggestResults
    )
{
    std::error_code errc;
    std::vector<club_search_auto_complete> suggestions;

    auto suggestResultArray = utils::extract_json_as_array(
        utils::extract_json_field(suggestResults, _T("results"), errc, false),
        errc
        );

    for (const auto& suggestionJson : suggestResultArray)
    {
        club_search_auto_complete suggestion;

        suggestion.m_suggestedQueryText = utils::extract_json_string(suggestionJson, _T("text"), errc);

        auto resultJson = utils::extract_json_field(suggestionJson, _T("result"), errc, false);

        suggestion.m_description = utils::extract_json_string(resultJson, _T("description"), errc);
        suggestion.m_displayImageUrl = utils::extract_json_string(resultJson, _T("displayImageUrl"), errc);
        suggestion.m_id = utils::extract_json_string(resultJson, _T("id"), errc);
        suggestion.m_name = utils::extract_json_string(resultJson, _T("name"), errc);
        suggestion.m_score = utils::extract_json_double(resultJson, _T("score"), errc);
        suggestion.m_tags = utils::extract_json_string_vector(resultJson, _T("tags"), false);
        suggestion.m_associatedTitles = utils::extract_json_string_vector(resultJson, _T("titles"), false);

        suggestions.push_back(suggestion);
    }
    return xbox_live_result<std::vector<club_search_auto_complete>>(suggestions, errc);
}

xbox_live_result<clubs_owned_result> clubs_serializers::deserialize_get_clubs_owned_response(
    _In_ const json::value& getClubsOwnedResponse
    )
{
    std::error_code errc;
    clubs_owned_result finalResult;

    auto clubsArray = utils::extract_json_as_array(
        utils::extract_json_field(getClubsOwnedResponse, _T("clubs"), errc, false),
        errc
        );

    for (const auto& clubJson : clubsArray)
    {
        auto id = utils::extract_json_string(clubJson, _T("id"), errc, false);   
        finalResult.m_clubIds.push_back(id);
    }
    
    finalResult.m_remainingClubs = utils::string_t_to_uint32(
        utils::extract_json_string(getClubsOwnedResponse, _T("remainingOpenAndClosedClubs"), errc, false)
        );
    finalResult.m_remainingClubs += utils::string_t_to_uint32(
        utils::extract_json_string(getClubsOwnedResponse, _T("remainingSecretClubs"), errc, false)
        );
   
    return xbox_live_result<clubs_owned_result>(finalResult, errc);
}

xbox_live_result<club_presence_counts> clubs_serializers::deserialize_club_presence_info(
    _In_ const json::value& clubMemberCountResponse
    )
{
    std::error_code errc;
    club_presence_counts presenceInfo;

    presenceInfo.m_totalCount = utils::extract_json_int(clubMemberCountResponse, _T("totalCount"), errc);
    presenceInfo.m_hereNow = utils::extract_json_int(clubMemberCountResponse, _T("activeCount"), errc);
    presenceInfo.m_hereToday = utils::extract_json_int(clubMemberCountResponse, _T("hereTodayCount"), errc);

    return xbox_live_result<club_presence_counts>(presenceInfo, errc);
}

xbox_live_result<std::vector<club_role>> clubs_serializers::deserialize_club_roles_from_club_member(
    _In_ const json::value& clubMemberObject
    )
{
    return _deserialize_club_role_array(clubMemberObject, _T("roles"));
}

xbox_live_result<string_t> clubs_serializers::deserialize_error_description(
    _In_ const web::json::value& json
)
{
    std::error_code errc;
    auto errorDescription = utils::extract_json_string(json, _T("description"), errc, true);

    return xbox_live_result<string_t>(errorDescription, errc);
}

xbox_live_result<void> clubs_serializers::generate_xbox_live_result(
    _In_ const std::shared_ptr<xbox::services::http_call_response>& response
    )
{
    xbox_live_result<void> result;

    const std::error_code& httpErrorCode = response->err_code();
    if (httpErrorCode != xbox_live_error_code::no_error)
    {
        result._Set_err(httpErrorCode);

        auto errorMessageFromJsonResult = deserialize_error_description(response->response_body_json());
        if (!errorMessageFromJsonResult.err())
        {
            result._Set_err_message(
                utility::conversions::to_utf8string(errorMessageFromJsonResult.payload()));
        }
        else
        {
            result._Set_err_message(response->err_message());
        }
    }

    return result;
}


xbox_live_result<club_role> clubs_serializers::convert_string_to_club_role(
    _In_ const string_t& role
    )
{
    if (utils::str_icmp(role, _T("member")) == 0)
    {
        return club_role::member;
    }
    else if (utils::str_icmp(role, _T("moderator")) == 0)
    {
        return club_role::moderator;
    }
    else if (utils::str_icmp(role, _T("owner")) == 0)
    {
        return club_role::owner;
    }
    else if (utils::str_icmp(role, _T("requestedtojoin")) == 0)
    {
        return club_role::requested_to_join;
    }
    else if (utils::str_icmp(role, _T("recommended")) == 0)
    {
        return club_role::recommended;
    }
    else if (utils::str_icmp(role, _T("invited")) == 0)
    {
        return club_role::invited;
    }
    else if (utils::str_icmp(role, _T("banned")) == 0)
    {
        return club_role::banned;
    }
    else if (utils::str_icmp(role, _T("follower")) == 0)
    {
        return club_role::follower;
    }
    else if (utils::str_icmp(role, _T("nonmember")) == 0)
    {
        return club_role::nonmember;
    }
    return xbox_live_result<club_role>(xbox_live_error_code::invalid_argument);
}

xbox_live_result<club_type> clubs_serializers::convert_string_to_club_type(
    _In_ const string_t& type
    )
{
    if (utils::str_icmp(type, _T("open")) == 0)
    {
        return club_type::public_club;
    }
    else if (utils::str_icmp(type, _T("closed")) == 0)
    {
        return club_type::private_club;
    }
    else if (utils::str_icmp(type, _T("secret")) == 0)
    {
        return club_type::hidden_club;
    }
    else
    {
        return club_type::unknown;
    }
}

xbox_live_result<club_user_presence> clubs_serializers::convert_string_to_club_user_presence(
    _In_ const string_t& userPresence
    )
{
    if (utils::str_icmp(userPresence, _T("NotInClub")) == 0)
    {
        return club_user_presence::not_in_club;
    }
    else if (utils::str_icmp(userPresence, _T("InClub")) == 0)
    {
        return club_user_presence::in_club;
    }
    else if (utils::str_icmp(userPresence, _T("Chat")) == 0)
    {
        return club_user_presence::chat;
    }
    else if (utils::str_icmp(userPresence, _T("Feed")) == 0)
    {
        return club_user_presence::feed;
    }
    else if (utils::str_icmp(userPresence, _T("Roster")) == 0)
    {
        return club_user_presence::roster;
    }
    else if (utils::str_icmp(userPresence, _T("Play")) == 0)
    {
        return club_user_presence::play;
    }
    return xbox_live_result<club_user_presence>(xbox_live_error_code::invalid_argument);
}

xbox_live_result<club> clubs_serializers::_deserialize_club_from_clubhub_club(
    _In_ const web::json::value& clubJson,
    _In_ std::shared_ptr<clubs_service_impl> service,
    _In_ const string_t& targetXuid
    )
{
    std::error_code errc;
    club club(service);

    club.m_id = utils::extract_json_string(clubJson, _T("id"), errc, true);

    auto profileJson = utils::extract_json_field(clubJson, _T("profile"), errc, true);
    if (!profileJson.is_null())
    {
        club.m_profile.m_nameSetting = _deserialize_club_string_setting(profileJson, _T("name")).payload();
        club.m_profile.m_descriptionSetting = _deserialize_club_string_setting(profileJson, _T("description")).payload();
        club.m_profile.m_matureContentEnabledSetting = _deserialize_club_bool_setting(profileJson, _T("matureContentEnabled")).payload();
        club.m_profile.m_watchClubTitlesOnlySetting = _deserialize_club_bool_setting(profileJson, _T("watchClubTitlesOnly")).payload();
        club.m_profile.m_isSearchableSetting = _deserialize_club_bool_setting(profileJson, _T("isSearchable")).payload();
        club.m_profile.m_isRecommendableSetting = _deserialize_club_bool_setting(profileJson, _T("isRecommendable")).payload();
        club.m_profile.m_requestToJoinEnabledSetting = _deserialize_club_bool_setting(profileJson, _T("requestToJoinEnabled")).payload();
        club.m_profile.m_leaveEnabledSetting = _deserialize_club_bool_setting(profileJson, _T("leaveEnabled")).payload();
        club.m_profile.m_transferOwnershipEnabledSetting = _deserialize_club_bool_setting(profileJson, _T("transferOwnershipEnabled")).payload();
        club.m_profile.m_displayImageUrlSetting = _deserialize_club_string_setting(profileJson, _T("displayImageUrl")).payload();
        club.m_profile.m_backgroundImageUrlSetting = _deserialize_club_string_setting(profileJson, _T("backgroundImageUrl")).payload();
        club.m_profile.m_tagsSetting = _deserialize_club_string_multi_setting(profileJson, _T("tags")).payload();
        club.m_profile.m_preferredLocaleSetting = _deserialize_club_string_setting(profileJson, _T("preferredLocale")).payload();
        club.m_profile.m_associatedTitlesSetting = _deserialize_club_string_multi_setting(profileJson, _T("associatedTitles")).payload();
        club.m_profile.m_primaryColorSetting = _deserialize_club_string_setting(profileJson, _T("primaryColor")).payload();
        club.m_profile.m_secondaryColorSetting = _deserialize_club_string_setting(profileJson, _T("secondaryColor")).payload();
        club.m_profile.m_tertiaryColorSetting = _deserialize_club_string_setting(profileJson, _T("tertiaryColor")).payload();
    }

    auto typeJson = utils::extract_json_field(clubJson, _T("clubType"), errc, false);
    if (!typeJson.is_null())
    {
        club.m_type = convert_string_to_club_type(utils::extract_json_string(typeJson, _T("type"), errc)).payload();
        club.m_titleFamilyId = utils::extract_json_string(typeJson, _T("titleFamilyId"), errc);
        club.m_titleFamilyName = utils::extract_json_string(typeJson, _T("localizedTitleFamilyName"), errc);
    }

    club.m_created = utils::extract_json_time(clubJson, _T("creationDateUtc"), errc);
    club.m_owner = utils::extract_json_string(clubJson, _T("ownerXuid"), errc);

    club.m_shortName = utils::extract_json_string(clubJson, _T("shortName"), errc);
    club.m_glyphImageUrl = utils::extract_json_string(clubJson, _T("glyphImageUrl"), errc);
    club.m_bannerImageUrl = utils::extract_json_string(clubJson, _T("bannerImageUrl"), errc);
    club.m_isClubSuspended = utils::str_icmp(utils::extract_json_string(clubJson, _T("state"), errc), _T("Suspended")) == 0;
    club.m_suspendedUntil = utils::extract_json_time(clubJson, _T("suspendedUntilUtc"), errc);
    club.m_founderXuid = utils::extract_json_string(clubJson, _T("founderXuid"), errc);
    club.m_followersCount = utils::extract_json_int(clubJson, _T("followersCount"), errc);
    club.m_presenceCount = utils::extract_json_int(clubJson, _T("clubPresenceCount"), errc);
    club.m_presenceTodayCount = utils::extract_json_int(clubJson, _T("clubPresenceTodayCount"), errc);
    club.m_membersCount = utils::extract_json_int(clubJson, _T("membersCount"), errc);
    club.m_moderatorsCount = utils::extract_json_int(clubJson, _T("moderatorsCount"), errc);
    club.m_recommendedCount = utils::extract_json_int(clubJson, _T("recommendedCount"), errc);
    club.m_requestedToJoinCount = utils::extract_json_int(clubJson, _T("requestedToJoinCount"), errc);
    club.m_reportCount = utils::extract_json_int(clubJson, _T("reportCount"), errc);
    club.m_reportedItemsCount = utils::extract_json_int(clubJson, _T("reportedItemsCount"), errc);

    auto settingsJson = utils::extract_json_field(clubJson, _T("settings"), errc, false);
    if (!settingsJson.is_null())
    {
        // feed actionSettings
        auto feedJson = utils::extract_json_field(settingsJson, _T("feed"), errc, false);
        if (!feedJson.is_null())
        {
            club.m_actionSettings.m_postToFeedSetting = _deserialize_club_action_setting(feedJson, _T("post")).payload();
            club.m_actionSettings.m_viewFeedSetting = _deserialize_club_action_setting(feedJson, _T("view")).payload();
        }

        // chat actionSettings
        auto chatJson = utils::extract_json_field(settingsJson, _T("chat"), errc, false);
        if (!chatJson.is_null())
        {
            club.m_actionSettings.m_writeInChatSetting = _deserialize_club_action_setting(chatJson, _T("write")).payload();
            club.m_actionSettings.m_viewChatSetting = _deserialize_club_action_setting(chatJson, _T("view")).payload();
            club.m_actionSettings.m_setChatTopicSetting = _deserialize_club_action_setting(chatJson, _T("setChatTopic")).payload();
        }

        // lfg actionSettings
        auto lfgJson = utils::extract_json_field(settingsJson, _T("lfg"), errc, false);
        if (!lfgJson.is_null())
        {
            club.m_actionSettings.m_joinLFGSetting = _deserialize_club_action_setting(lfgJson, _T("join")).payload();
            club.m_actionSettings.m_createLFGSetting = _deserialize_club_action_setting(lfgJson, _T("create")).payload();
            club.m_actionSettings.m_viewLFGSetting = _deserialize_club_action_setting(lfgJson, _T("view")).payload();
        }

        // roster actionSettings
        auto rosterJson = utils::extract_json_field(settingsJson, _T("roster"), errc, false);
        if (!rosterJson.is_null())
        {
            club.m_actionSettings.m_inviteOrAcceptJoinRequestsSetting = _deserialize_club_action_setting(rosterJson, _T("inviteOrAccept")).payload();
            club.m_actionSettings.m_kickOrBanSetting = _deserialize_club_action_setting(rosterJson, _T("kickOrBan")).payload();
            club.m_actionSettings.m_viewRosterSetting = _deserialize_club_action_setting(rosterJson, _T("view")).payload();
        }

        // profile actionSetting
        auto profileActionSettingJson = utils::extract_json_field(settingsJson, _T("profile"), errc, false);
        if (!profileActionSettingJson.is_null())
        {
            club.m_actionSettings.m_updateProfileSetting = _deserialize_club_action_setting(profileActionSettingJson, _T("update")).payload();
            club.m_actionSettings.m_deleteProfileSetting = _deserialize_club_action_setting(profileActionSettingJson, _T("delete")).payload();
            club.m_actionSettings.m_viewProfileSetting = _deserialize_club_action_setting(profileActionSettingJson, _T("view")).payload();
        }

        // viewer roles
        auto viewerRoles = utils::extract_json_field(settingsJson, _T("viewerRoles"), errc, false);
        if (!viewerRoles.is_null())
        {
            club.m_viewerRoles = _deserialize_club_role_array(viewerRoles, _T("roles")).payload();
        }
    }

    auto targetRolesJson = utils::extract_json_field(clubJson, _T("targetRoles"), errc, false);
    if (!targetRolesJson.is_null())
    {
        auto roleRecords = _deserialize_club_role_details_array(
            targetRolesJson,
            _T("roles"),
            string_t(),
            targetXuid).payload();

        club.m_roleRecords[targetXuid] = roleRecords;
    }

    return xbox_live_result<clubs::club>(club, errc);
}

xbox::services::xbox_live_result<club_roster> clubs_serializers::deserialize_club_roster_from_clubhub_reponse(
    _In_ const web::json::value& clubHubResponse
    )
{
    std::error_code errc;
    club_roster roster;

    auto clubsJsonArray = utils::extract_json_as_array(
        utils::extract_json_field(clubHubResponse, _T("clubs"), errc, false),
        errc
        );

    if (clubsJsonArray.size() != 1)
    {
        return xbox_live_result<club_roster>(errc);
    }

    auto clubRosterJson = utils::extract_json_field(clubsJsonArray[0], _T("roster"), errc, false);

    if (!clubRosterJson.is_null())
    {
        roster.m_moderators = _deserialize_club_role_details_array(
            clubRosterJson,
            _T("moderator"),
            _T("moderator"),
            string_t())
            .payload();

        roster.m_requestedToJoin = _deserialize_club_role_details_array(
            clubRosterJson,
            _T("requestedToJoin"),
            _T("requestedToJoin"),
            string_t())
            .payload();

        roster.m_recommended = _deserialize_club_role_details_array(
            clubRosterJson,
            _T("recommended"),
            _T("recommended"),
            string_t())
            .payload();

        roster.m_banned = _deserialize_club_role_details_array(
            clubRosterJson,
            _T("banned"),
            _T("banned"),
            string_t())
            .payload();
    }
    return xbox_live_result<club_roster>(roster, errc);
}

xbox_live_result<std::vector<club_role>> clubs_serializers::_deserialize_club_role_array(
    _In_ const json::value& json,
    _In_ const string_t& fieldName
    )
{
    std::error_code errc;
    auto roles = utils::extract_json_vector<club_role>([&errc](const json::value& role)
        {
            return convert_string_to_club_role(utils::extract_json_as_string(role, errc));
        },
        json, fieldName, errc, false);

    return xbox_live_result<std::vector<club_role>>(roles, errc);
}

xbox_live_result<std::vector<club_role_record>> clubs_serializers::_deserialize_club_role_details_array(
    _In_ const web::json::value& json,
    _In_ const string_t& fieldName,
    _In_ const string_t& defaultRole,
    _In_ const string_t& defaultXuid
    )
{
    std::error_code errc;
    std::vector<club_role_record> roleDetailsVector;

    auto field = utils::extract_json_field(json, fieldName, errc, true);
    auto jsonArray = utils::extract_json_as_array(field, errc);

    for (const auto& roleDetailsJson : jsonArray)
    {
        club_role_record roleDetails;

        roleDetails.m_actorXuid = utils::extract_json_string(roleDetailsJson, _T("actorXuid"), errc);
        roleDetails.m_createdDate = utils::extract_json_time(roleDetailsJson, _T("createdDate"), errc);
        roleDetails.m_xuid = utils::extract_json_string(roleDetailsJson, _T("xuid"), errc, false, defaultXuid);
        roleDetails.m_role = convert_string_to_club_role(utils::extract_json_string(roleDetailsJson, _T("role"), errc, false, defaultRole)).payload();

        roleDetailsVector.push_back(roleDetails);
    }
    return xbox_live_result<std::vector<club_role_record>>(roleDetailsVector, errc);
}

xbox_live_result<club_action_setting> clubs_serializers::_deserialize_club_action_setting(
    _In_ const json::value& json,
    _In_ const string_t& fieldName
    )
{
    std::error_code errc;
    club_action_setting actionSetting;

    auto settingJson = utils::extract_json_field(json, fieldName, true);
    if (!settingJson.is_null())
    {
        actionSetting.m_requiredRole = convert_string_to_club_role(utils::extract_json_string(settingJson, _T("value"), errc)).payload();
        actionSetting.m_allowedValues = _deserialize_club_role_array(settingJson, _T("allowedValues")).payload();
        actionSetting.m_canViewerAct = utils::extract_json_bool(settingJson, _T("canViewerAct"), errc);
        actionSetting.m_canViewerChangeSetting = utils::extract_json_bool(settingJson, _T("canViewerChangeSetting"), errc);
    }

    return xbox_live_result<club_action_setting>(actionSetting, errc);
}

xbox_live_result<club_setting<string_t>> clubs_serializers::_deserialize_club_string_setting(
    _In_ const json::value& json,
    _In_ const string_t& fieldName
    )
{
    std::error_code errc;
    club_setting<string_t> setting;

    auto settingJson = utils::extract_json_field(json, fieldName, false);
    if (!settingJson.is_null())
    {
        setting.m_value = utils::extract_json_string(settingJson, _T("value"), errc);
        setting.m_allowedValues = utils::extract_json_string_vector(settingJson, _T("allowedValues"), errc, false);
        setting.m_canViewerChangeSetting = utils::extract_json_bool(settingJson, _T("canViewerChangeSetting"), errc);
    }
    return xbox_live_result<club_setting<string_t>>(setting, errc);
}

xbox_live_result<club_multi_setting<string_t>> clubs_serializers::_deserialize_club_string_multi_setting(
    _In_ const json::value& json,
    _In_ const string_t& fieldName
    )
{
    std::error_code errc;
    club_multi_setting<string_t> setting;

    auto settingJson = utils::extract_json_field(json, fieldName, false);
    if (!settingJson.is_null())
    {
        setting.m_values = utils::extract_json_string_vector(settingJson, _T("value"), errc, false);
        setting.m_allowedValues = utils::extract_json_string_vector(settingJson, _T("allowedValues"), errc, false);
        setting.m_canViewerChangeSetting = utils::extract_json_bool(settingJson, _T("canViewerChangeSetting"), errc);
    }
    return xbox_live_result<club_multi_setting<string_t>>(setting, errc);
}

xbox::services::xbox_live_result<club_setting<bool>> clubs_serializers::_deserialize_club_bool_setting(
    _In_ const web::json::value& json,
    _In_ const string_t& fieldName
    )
{
    std::error_code errc;
    club_setting<bool> setting;

    auto settingJson = utils::extract_json_field(json, fieldName, false);
    if (!settingJson.is_null())
    {
        setting.m_value = utils::extract_json_bool(settingJson, _T("value"), errc);
        setting.m_allowedValues = utils::extract_json_vector<bool>([&errc](const json::value& value)
            {
                return value.as_bool();
            },
            settingJson, _T("allowedValues"), false);
        setting.m_canViewerChangeSetting = utils::extract_json_bool(settingJson, _T("canViewerChangeSetting"), errc);
    }
    return xbox_live_result<club_setting<bool>>(setting, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END