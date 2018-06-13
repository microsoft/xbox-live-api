// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/privacy.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"


NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

// Helper classes for serialization/deserialization
class avoid_list_item
{
private:
    static xbox_live_result<avoid_list_item> deserializer(_In_ const web::json::value& json)
    {
        if (json.is_null()) return xbox_live_result<avoid_list_item>(xbox_live_error_code::json_error, "Json is null or empty");

        avoid_list_item result;

        std::error_code errc = xbox_live_error_code::no_error;
        result.m_xboxUserId = utils::extract_json_string(json, _T("xuid"), errc, true);

        return xbox_live_result<avoid_list_item>(result, errc);
    }

    string_t m_xboxUserId;

    friend class privacy_service;
    friend class avoid_list;
    template<class T> friend class xbox_live_result;
};

class avoid_list
{
private:
    static xbox_live_result<avoid_list> deserializer(_In_ const web::json::value& json)
    {
        if (json.is_null()) return xbox_live_result<avoid_list>(xbox_live_error_code::json_error, "Json is null or empty");

        avoid_list result;

        std::error_code errc = xbox_live_error_code::no_error;
        result.m_avoidList = utils::extract_json_vector<avoid_list_item>(avoid_list_item::deserializer, json, _T("users"), errc, true);

        return xbox_live_result<avoid_list>(result, errc);
    }

    std::vector<avoid_list_item> m_avoidList;

    friend class avoid_list_item;
    friend class privacy_service;
    template<class T> friend class xbox_live_result;
};

// Privacy service
privacy_service::privacy_service( 
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
}

pplx::task<xbox_live_result<std::vector<string_t>>>
privacy_service::get_avoid_list()
{
    return get_avoid_or_mute_list(_T("avoid"));
}

pplx::task<xbox_live_result<std::vector<string_t>>>
privacy_service::get_mute_list()
{
    return get_avoid_or_mute_list(_T("mute"));
}

pplx::task<xbox_live_result<std::vector<string_t>>>
privacy_service::get_avoid_or_mute_list(
    _In_ const string_t& subPathName
    )
{
    string_t xboxUserId = utils::string_t_from_internal_string(m_userContext->xbox_user_id());
    string_t subPathAndQuery = avoid_mute_list_sub_path(xboxUserId, subPathName);

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("privacy"), m_appConfig),
        subPathAndQuery,
        xbox_live_api::get_avoid_or_mute_list
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        auto result = avoid_list::deserializer(response->response_body_json());

        std::vector<string_t> xuidVector;

        for (auto& item : result.payload().m_avoidList)
        {
            if (!item.m_xboxUserId.empty())
            {
                xuidVector.push_back(item.m_xboxUserId);
            }
        }

        xbox_live_result<std::vector<string_t>> xuidVectorResult(
            xuidVector,
            result.err(),
            result.err_message()
            );

        return utils::generate_xbox_live_result<std::vector<string_t>>(
            xuidVectorResult,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<string_t>>(
        task
        );
}

const string_t 
privacy_service::avoid_mute_list_sub_path(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& subPathName
    )
{
    web::uri_builder subPathBuilder;
    stringstream_t path;
    path << _T("/users/xuid(");
    path << xboxUserId;
    path << _T(")/people/");
    path << subPathName;

    subPathBuilder.append_path(path.str());

    return subPathBuilder.to_string();
}

const string_t 
privacy_service::permission_validate_sub_path(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& setting,
    _In_ const string_t& targetXboxUserId
    )
{
    // users/xuid({xuid})/permission/validate?setting={setting}&target=xuid({targetXuid})

    web::uri_builder subPathBuilder;
    stringstream_t path;
    path << _T("/users/xuid(");
    path << xboxUserId;
    path << _T(")/permission/validate");

    subPathBuilder.append_path(path.str());
    subPathBuilder.append_query(_T("setting"), setting);

    stringstream_t xuidPath;
    xuidPath << _T("xuid(");
    xuidPath << targetXboxUserId;
    xuidPath << _T(")");
    subPathBuilder.append_query(_T("target"), xuidPath.str());

    return subPathBuilder.to_string();
}

const string_t 
privacy_service::permission_batch_validate_sub_path(
    _In_ const string_t& xboxUserId
    )
{
    // users/xuid({xuid})/permission/validate

    web::uri_builder subPathBuilder;
    stringstream_t path;
    path << _T("/users/xuid(");
    path << xboxUserId;
    path << _T(")/permission/validate");

    subPathBuilder.append_path(path.str());

    return subPathBuilder.to_string();
}

pplx::task<xbox_live_result<permission_check_result>>
privacy_service::check_permission_with_target_user(
    _In_ const string_t& permissionId,
    _In_ const string_t& targetXboxUserId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(permissionId, permission_check_result, "PermissionID is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(targetXboxUserId, permission_check_result, "Target Xbox User Id is empty");

    string_t xboxUserId = utils::string_t_from_internal_string(m_userContext->xbox_user_id());
    string_t subpathAndQuery = permission_validate_sub_path(xboxUserId, permissionId, targetXboxUserId);

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("privacy"), m_appConfig),
        subpathAndQuery,
        xbox_live_api::check_permission_with_target_user
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
        .then([permissionId](std::shared_ptr<http_call_response> response)
    {
        auto result = permission_check_result::_Deserializer(response->response_body_json());
        auto permissionResult = result.payload();
        permissionResult.initialize(permissionId);

        return utils::generate_xbox_live_result<permission_check_result>(
            permissionResult,
            response
            );
    });

    return utils::create_exception_free_task<permission_check_result>(
        task
        );
}


pplx::task<xbox_live_result<std::vector<multiple_permissions_check_result>>>
privacy_service::check_multiple_permissions_with_multiple_target_users(
    _In_ std::vector<string_t> permissionIds,
    _In_ std::vector<string_t> targetXboxUserIds
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(permissionIds.empty(), std::vector<multiple_permissions_check_result>, "Permission Ids are empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(targetXboxUserIds.empty(), std::vector<multiple_permissions_check_result>, "Target Xbox User Ids are empty");

    string_t xboxUserId = utils::string_t_from_internal_string(m_userContext->xbox_user_id());
    web::uri subpathAndQuery = permission_batch_validate_sub_path(xboxUserId);

    // Set request body to something like:
    //{
    //    "users":
    //    [
    //        {"xuid":"12345"},
    //        {"xuid":"54321"}
    //    ],
    //    "permissions":
    //    [
    //        "ViewTargetGameHistory",
    //        "ViewTargetProfile"
    //    ]
    //}

    web::json::value serializedObject;

    web::json::value privacyUsers;
    for (uint32_t i = 0; i < targetXboxUserIds.size(); ++i)
    {
        web::json::value userJson;
        userJson[_T("xuid")] = web::json::value(targetXboxUserIds[i]);
        privacyUsers[i] = userJson;
    }

    serializedObject[_T("users")] = privacyUsers;
    serializedObject[_T("permissions")] = utils::serialize_vector<string_t>(utils::json_string_serializer, permissionIds);

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("privacy"), m_appConfig),
        subpathAndQuery,
        xbox_live_api::check_multiple_permissions_with_multiple_target_users
        );

    httpCall->set_request_body(
        serializedObject.serialize()
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([permissionIds](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto results = utils::extract_json_vector<multiple_permissions_check_result>(multiple_permissions_check_result::_Deserializer, response->response_body_json(), _T("responses"), errc, true);

        for (auto& result : results)
        {
            auto permissionResults = result.items();

            if (permissionResults.size() != permissionIds.size()) return xbox_live_result<std::vector<multiple_permissions_check_result>>(xbox_live_error_code::runtime_error, "The resulting number of items did not match the number of items requested!");

            for (uint32_t i = 0; i < permissionResults.size(); i++)
            {
                result.initialize(i, permissionIds[i]);
            }
        }

        xbox_live_result<std::vector<multiple_permissions_check_result>> multiplePermissionResults(
            results,
            errc,
            ""
            );

        return utils::generate_xbox_live_result<std::vector<multiple_permissions_check_result>>(
            multiplePermissionResults,
            response
            );
    });

    return utils::create_exception_free_task< std::vector<multiple_permissions_check_result> >(
        task
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END
