// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "shared_macros.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

class clubs_serializers
{
public:
    static xbox::services::xbox_live_result<std::vector<club>> deserialize_clubs_from_clubhub_reponse(
        _In_ const web::json::value& clubHubResponse,
        _In_ std::shared_ptr<clubs_service_impl> service,
        _In_ const string_t& targetXuid = string_t()
        );

    static xbox::services::xbox_live_result<club_roster> deserialize_club_roster_from_clubhub_reponse(
        _In_ const web::json::value& clubHubResponse
        ); 

    static xbox::services::xbox_live_result<std::vector<club_user_presence_record>> deserialize_club_user_presence_record_from_clubhub_response(
        _In_ const web::json::value& clubHubResponse
        );

    static xbox::services::xbox_live_result<std::vector<club_recommendation_t>> deserialize_recommendations_from_clubhub_response(
        _In_ const web::json::value& clubHubResponse,
        _In_ std::shared_ptr<clubs_service_impl> service
        );

    static xbox::services::xbox_live_result<clubs_search_result> deserialize_clubs_search_result_from_clubhub_response(
        _In_ const web::json::value& clubHubResponse,
        _In_ std::shared_ptr<clubs_service_impl> service
        );

    static xbox::services::xbox_live_result<std::vector<club_search_auto_complete>> deserialize_club_suggestions(
        _In_ const web::json::value& suggestResults
        );

    static xbox::services::xbox_live_result<clubs_owned_result> deserialize_get_clubs_owned_response(
        _In_ const web::json::value& getClubsOwnedResponse
        );

    static xbox::services::xbox_live_result<club_presence_counts> deserialize_club_presence_info(
        _In_ const web::json::value& clubMemberCountResponse
        );

    static xbox::services::xbox_live_result<std::vector<club_role>> deserialize_club_roles_from_club_member(
        _In_ const web::json::value& clubMemberObject
        );

    static xbox::services::xbox_live_result<string_t> deserialize_error_description(
        _In_ const web::json::value& json
        );

    template<typename T>
    static xbox::services::xbox_live_result<T> generate_xbox_live_result(
        _Inout_ xbox::services::xbox_live_result<T> deserializationResult,
        _In_ const std::shared_ptr<xbox::services::http_call_response>& response
        );

    static xbox::services::xbox_live_result<void> generate_xbox_live_result(
        _In_ const std::shared_ptr<xbox::services::http_call_response>& response
        );

    static xbox_live_result<club_role> convert_string_to_club_role(_In_ const string_t& role);
    static xbox_live_result<club_type> convert_string_to_club_type(_In_ const string_t& type);
    static xbox_live_result<club_user_presence> convert_string_to_club_user_presence(_In_ const string_t& userPresence);

private:

    static xbox::services::xbox_live_result<club> _deserialize_club_from_clubhub_club(
        _In_ const web::json::value& clubJson,
        _In_ std::shared_ptr<clubs_service_impl> service,
        _In_ const string_t& targetXuid = string_t()
        );

    static xbox::services::xbox_live_result<std::vector<club_role>> _deserialize_club_role_array(
        _In_ const web::json::value& json,
        _In_ const string_t& fieldName
        );

    static xbox::services::xbox_live_result<std::vector<club_role_record>> _deserialize_club_role_details_array(
        _In_ const web::json::value& json,
        _In_ const string_t& fieldName,
        _In_ const string_t& defaultRole,
        _In_ const string_t& defaultXuid
        );

    static xbox::services::xbox_live_result<club_action_setting> _deserialize_club_action_setting(
        _In_ const web::json::value& json,
        _In_ const string_t& fieldName
        );

    static xbox::services::xbox_live_result<club_setting<string_t>> _deserialize_club_string_setting(
        _In_ const web::json::value& json,
        _In_ const string_t& fieldName
        );

    static xbox::services::xbox_live_result<club_multi_setting<string_t>> _deserialize_club_string_multi_setting(
        _In_ const web::json::value& json,
        _In_ const string_t& fieldName
        );

    static xbox::services::xbox_live_result<club_setting<bool>> _deserialize_club_bool_setting(
        _In_ const web::json::value& json,
        _In_ const string_t& fieldName
        );

    clubs_serializers() {}
};

template<typename T>
xbox::services::xbox_live_result<T> clubs_serializers::generate_xbox_live_result(
    _Inout_ xbox::services::xbox_live_result<T> deserializationResult,
    _In_ const std::shared_ptr<xbox::services::http_call_response>& response
    )
{
    if (deserializationResult.err())
    {
        deserializationResult.set_payload(T());
    }

    const std::error_code& httpErrorCode = response->err_code();
    if (httpErrorCode != xbox_live_error_code::no_error)
    {
        deserializationResult._Set_err(httpErrorCode);

        auto errorMessageFromJsonResult = deserialize_error_description(response->response_body_json());
        if (!errorMessageFromJsonResult.err())
        {
            deserializationResult._Set_err_message(
                utility::conversions::to_utf8string(errorMessageFromJsonResult.payload()));
        }
        else
        {
            deserializationResult._Set_err_message(response->err_message());
        }
    }

    return deserializationResult;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END