// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/real_time_activity.h"
#include "xsapi/multiplayer.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"
#include "multiplayer_internal.h"

using namespace pplx;
using namespace xbox::services::system;
using namespace xbox::services::real_time_activity;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN
const uint32_t c_multiplayerHandleVersionValue = 1;
const string_t c_getActivitiesSubpath = _T("/handles/query?include=relatedInfo,customProperties");
const string_t c_getSearchHandlesSubpath = _T("/handles/query?include=relatedInfo,roleInfo,customProperties");
const string_t c_multiplayerServiceContractHeaderValue = _T("107");

multiplayer_service::multiplayer_service()
{
}

multiplayer_service::multiplayer_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivity
    ) : 
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig))
{
    m_multiplayerServiceImpl = std::make_shared<multiplayer_service_impl>(realTimeActivity);
}

task<xbox_live_result<std::shared_ptr<multiplayer_session>>> 
multiplayer_service::write_session(
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ multiplayer_session_write_mode mode
    )
{
    multiplayer_session_reference sessionReference = session->session_reference();

    string_t pathAndQuery = multiplayer_session_directory_create_or_update_subpath(
        sessionReference.service_configuration_id(),
        sessionReference.session_template_name(),
        sessionReference.session_name()
        );

    return write_session_using_subpath(
        session,
        mode,
        pathAndQuery
        );
}

task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_service::write_session_by_handle(
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ multiplayer_session_write_mode mode,
    _In_ const string_t& handleId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(handleId, std::shared_ptr<multiplayer_session>, "Handle id was empty");

    string_t pathAndQuery = multiplayer_session_directory_create_or_update_by_handle_subpath(handleId);
    return write_session_using_subpath(
        session,
        mode,
        pathAndQuery
        );
}

task<xbox_live_result<std::shared_ptr<multiplayer_session>>> 
multiplayer_service::write_session_using_subpath(
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ multiplayer_session_write_mode mode,
    _In_ const string_t& subpathAndQuery
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(subpathAndQuery, std::shared_ptr<multiplayer_session>, "Subpath and query was empty");

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("PUT"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        subpathAndQuery,
        xbox_live_api::write_session_using_subpath
        );

    httpCall->set_retry_allowed(false);
    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);

    switch (mode)
    {
        case multiplayer_session_write_mode::create_new:
        {
            httpCall->set_custom_header(_T("If-None-Match"), _T("*"));
            break;
        }
        case multiplayer_session_write_mode::update_existing:
        {
            httpCall->set_custom_header(_T("If-Match"), _T("*"));
            break;
        }
        case multiplayer_session_write_mode::update_or_create_new:
        {
            // No match header
            break;
        }
        case multiplayer_session_write_mode::synchronized_update:
        {
            if (session->e_tag().empty())
            {
                httpCall->set_custom_header(_T("If-None-Match"), _T("*"));
            }
            else
            {
                httpCall->set_custom_header(_T("If-Match"), session->e_tag());
            }
            break;
        }
        default:
        {
            return pplx::task_from_result(xbox_live_result<std::shared_ptr<multiplayer_session>>(xbox_live_error_code::invalid_argument, "multiplayer session write mode is out of range"));
        }
    }

    task<xbox_live_result<string_t>> subscriptionTask;
    bool subscriptionsEnabled = m_multiplayerServiceImpl->subscriptions_enabled();
    if (subscriptionsEnabled && session->current_user() != nullptr && session->current_user()->_Member_request() != nullptr)
    {
        subscriptionTask = m_multiplayerServiceImpl->ensure_multiplayer_subscription()
        .then([httpCall, session](xbox_live_result<string_t> connectionId)
        {
            if (connectionId.err())
            {
                return xbox_live_result<string_t>(connectionId.err(), connectionId.err_message());
            }
            session->current_user()->_Set_rta_connection_id(connectionId.payload());
            return xbox_live_result<string_t>(session->_Session_request()->serialize().serialize(), connectionId.err(), connectionId.err_message());
        });
    }
    else
    {
        subscriptionTask = task_from_result(xbox_live_result<string_t>(session->_Session_request()->serialize().serialize()));
    }

    auto userContext = m_userContext;
    multiplayer_session_reference sessionReference = session->session_reference();
    auto task = subscriptionTask.then([httpCall, userContext](xbox_live_result<string_t> body)
    {
        if (body.err())
        {
            return task_from_result(xbox_live_result<std::shared_ptr<http_call_response>>(body.err(), body.err_message()));
        }

        httpCall->set_request_body(body.payload());
        auto httpResponse = httpCall->get_response_with_auth(userContext);
        return task_from_result(xbox_live_result<std::shared_ptr<http_call_response>>(httpResponse.get()));
    }) 
    .then([sessionReference, httpCall, userContext](xbox_live_result<std::shared_ptr<http_call_response>> responseResult)
    {
        auto& response = responseResult.payload();
        if (responseResult.err())
        {
            return xbox_live_result<std::shared_ptr<multiplayer_session>>(responseResult.err(), responseResult.err_message());
        }

        auto& errCode = response->err_code();
        if (errCode && errCode != xbox_live_error_code::http_status_412_precondition_failed)
        {
            auto errMessage = response->err_message();
            if (errMessage.empty() && response->response_body_json().is_string())
            {
                string_t debugString = response->response_body_json().as_string();
                if (!debugString.empty())
                {
                    errMessage = utility::conversions::to_utf8string(debugString);
                }
            }
            return xbox_live_result<std::shared_ptr<multiplayer_session>>(response->err_code(), errMessage);
        }

        auto status = response->http_status();

        if (status == 204)
        {
            return xbox_live_result<std::shared_ptr<multiplayer_session>>(nullptr);
        }

        xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSessionResult = xbox_live_result<std::shared_ptr<multiplayer_session>>(response->err_code(), response->err_message());
        if (response->response_body_json().size() > 0)
        {
            auto multiplayerSession = multiplayer_session::_Deserialize(
                response->response_body_json()
                );

            if (multiplayerSession.err() && !response->err_code())
            {
                response->_Internal_response()->set_error_info(xbox_live_error_code::json_error, "WriteSession failed due to deserialization error");
            }

            xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSessionShared(
                std::make_shared<multiplayer_session>(multiplayerSession.payload()),
                multiplayerSession.err(),
                multiplayerSession.err_message()
                );

            multiplayerSessionResult = utils::generate_xbox_live_result<std::shared_ptr<multiplayer_session>>(
                multiplayerSessionShared,
                response
                );

            auto newSession = std::make_shared<multiplayer_session>(multiplayerSession.payload());

            multiplayer_session_reference localSessionRef;
            if (sessionReference.is_null())
            {
                web::http::http_headers header = response->response_headers();
                web::http::http_headers::key_type contentKey(_T("Content-Location"));

                localSessionRef = multiplayer_session_reference::parse_from_uri_path(
                    header[contentKey]
                );
            }
            else
            {
                localSessionRef = std::move(sessionReference);
            }

            newSession->_Initialize_after_deserialize(
                response->e_tag(),
                response->response_date(),
                localSessionRef,
                utils::string_t_from_internal_string(userContext->xbox_user_id())
                );

            newSession->_Set_write_session_status(
                response->http_status()
                );

            multiplayerSessionResult.set_payload(newSession);
        }

        return multiplayerSessionResult;
    });

    return utils::create_exception_free_task<std::shared_ptr<multiplayer_session>>(
        task
        );
}

task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_service::get_current_session(
    _In_ multiplayer_session_reference sessionReference
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(sessionReference.is_null(), std::shared_ptr<multiplayer_session>, "Session reference is null");

    string_t pathAndQuery = multiplayer_session_directory_create_or_update_subpath(
        sessionReference.service_configuration_id(),
        sessionReference.session_template_name(),
        sessionReference.session_name()
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_current_session
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);

    auto userContextShared = m_userContext;

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([sessionReference, userContextShared](std::shared_ptr<http_call_response> response)
    {
        if (response->http_status() == 204)
        {
            return xbox_live_result<std::shared_ptr<multiplayer_session>>(xbox_live_error_code::http_status_204_resource_data_not_found, "Content not found on get_current_session");
        }

        auto multiplayerSession = multiplayer_session::_Deserialize(
            response->response_body_json()
            );

        auto multiplayerSessionResult = utils::generate_xbox_live_result<multiplayer_session>(
            multiplayerSession,
            response
            );

        xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSessionSharedResult(
            std::make_shared<multiplayer_session>(multiplayerSessionResult.payload()), 
            multiplayerSessionResult.err(), 
            multiplayerSessionResult.err_message()
            );

        multiplayerSessionSharedResult.payload()->_Initialize_after_deserialize(
            response->e_tag(), 
            response->response_date(),
            sessionReference,
            utils::string_t_from_internal_string(userContextShared->xbox_user_id())
            );

        return multiplayerSessionSharedResult;
    });

    return utils::create_exception_free_task<std::shared_ptr<multiplayer_session>>(
        task
        );
}

task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_service::get_current_session_by_handle(
    _In_ const string_t& handleId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(handleId, std::shared_ptr<multiplayer_session>, "Handle id was empty");
    string_t pathAndQuery = multiplayer_session_directory_create_or_update_by_handle_subpath(
        handleId
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_current_session_by_handle
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    auto userContextShared = m_userContext;

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([userContextShared](std::shared_ptr<http_call_response> response)
    {
        if (response->http_status() == 204)
        {
            return xbox_live_result<std::shared_ptr<multiplayer_session>>(xbox_live_error_code::http_status_204_resource_data_not_found, "Content not found on get_current_session");
        }

        auto multiplayerSession = multiplayer_session::_Deserialize(
            response->response_body_json()
            );

        auto multiplayerSessionResult = utils::generate_xbox_live_result<multiplayer_session>(
            multiplayerSession,
            response
            );

        xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSessionShared(    // TOOD: Implement a way to quit creating a new result object...
            std::make_shared<multiplayer_session>(multiplayerSessionResult.payload()),
            multiplayerSessionResult.err(),
            multiplayerSessionResult.err_message()
            );

        web::http::http_headers header = response->response_headers();
        web::http::http_headers::key_type contentKey(_T("Content-Location"));

        multiplayer_session_reference sessionReference = multiplayer_session_reference::parse_from_uri_path(
            header[contentKey]
            );

        if (sessionReference.is_null()) return xbox_live_result<std::shared_ptr<multiplayer_session>>(response->err_code(), response->err_message());

        multiplayerSessionShared.payload()->_Initialize_after_deserialize(
            response->e_tag(), 
            response->response_date(),
            sessionReference,
            utils::string_t_from_internal_string(userContextShared->xbox_user_id())
            );

        return multiplayerSessionShared;
    });

    return utils::create_exception_free_task<std::shared_ptr<multiplayer_session>>(
        task
        );
}

task<xbox_live_result<std::vector<multiplayer_session_states>>>
multiplayer_service::get_sessions(
    _In_ multiplayer_get_sessions_request getSessionsRequest
    )
{
    auto filter = getSessionsRequest.visibility_filter();

    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(getSessionsRequest.service_configuration_id(), std::vector<multiplayer_session_states>, "serviceConfigurationId was empty in request");

    RETURN_TASK_CPP_INVALIDARGUMENT_IF(!getSessionsRequest.xbox_user_ids_filter().empty() && !getSessionsRequest.xbox_user_id_filter().empty(), std::vector<multiplayer_session_states>, "xboxUserIdsFilter and xboxUserIdFilter cannot both be set for request")

    RETURN_TASK_CPP_INVALIDARGUMENT_IF(getSessionsRequest.xbox_user_id_filter().empty() && getSessionsRequest.keyword_filter().empty() && getSessionsRequest.xbox_user_ids_filter().empty(), std::vector<multiplayer_session_states>, "Must have xboxUserIdFilter, xboxUserIdsFilter, or keywordFilter set for request");

    RETURN_TASK_CPP_INVALIDARGUMENT_IF(getSessionsRequest.include_reservations() && getSessionsRequest.xbox_user_id_filter().empty() && getSessionsRequest.xbox_user_ids_filter().empty(), std::vector<multiplayer_session_states>, "Cannot include reservations in request without xboxUserIdFilter or xboxUserIdsFilter");
    
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(getSessionsRequest.include_inactive_sessions() && getSessionsRequest.xbox_user_id_filter().empty() && getSessionsRequest.xbox_user_ids_filter().empty(), std::vector<multiplayer_session_states>, "Cannot include inactive sessions in request without xboxUserIdFilter or xboxUserIdsFilter");

    auto visibilityFilterToString = multiplayer_session_states::_Convert_multiplayer_session_visibility_to_string(filter);
    RETURN_TASK_CPP_IF_ERR(visibilityFilterToString, std::vector<multiplayer_session_states>);

    auto xboxUserIdFilters = getSessionsRequest.xbox_user_ids_filter();
    bool hasXboxUserIdsFilter = !xboxUserIdFilters.empty();

    string_t pathAndQuery = multiplayer_session_directory_get_sessions_sub_path(
        getSessionsRequest.service_configuration_id(),
        getSessionsRequest.session_template_name_filter(),
        getSessionsRequest.xbox_user_id_filter(),
        getSessionsRequest.keyword_filter(),
        visibilityFilterToString.payload(),
        getSessionsRequest.contract_version_filter(),
        getSessionsRequest.include_private_sessions(),
        getSessionsRequest.include_reservations(),
        getSessionsRequest.include_inactive_sessions(),
        hasXboxUserIdsFilter,
        getSessionsRequest.max_items()
        );

    string_t requestType;
    if (hasXboxUserIdsFilter)
    {
        requestType = _T("POST");
    }
    else
    {
        requestType = _T("GET");
    }

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        requestType,
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_sessions
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    if (hasXboxUserIdsFilter)
    {
        web::json::value xuidsJson;
        xuidsJson[_T("xuids")] = utils::serialize_vector<string_t>(utils::json_string_serializer, xboxUserIdFilters);
        httpCall->set_request_body(
            xuidsJson.serialize()
            );
    }

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto sessionStates = utils::extract_xbox_live_result_json_vector<multiplayer_session_states>(
            multiplayer_session_states::_Deserialize,
            response->response_body_json(),
            _T("results"),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<multiplayer_session_states>>(
            sessionStates,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<multiplayer_session_states>>(
        task
        );
}

task<xbox_live_result<void>>
multiplayer_service::set_activity(
    _In_ multiplayer_session_reference sessionReference
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(sessionReference.is_null(), void, "Session reference was not initialized properly");

    multiplayer_activity_handle_post_request request(
        std::move(sessionReference),
        c_multiplayerHandleVersionValue
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        _T("/handles"),
        xbox_live_api::set_activity
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    httpCall->set_request_body(
        request.serialize().serialize()
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<void>(
        task
        );
}

task<xbox_live_result<void>> 
multiplayer_service::clear_activity(
    _In_ const string_t& serviceConfigurationId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, void, "Service configuration id is empty");

    std::vector<string_t> xuidVector;
    xuidVector.push_back(utils::string_t_from_internal_string(m_userContext->xbox_user_id()));

    auto sharedXboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;
    auto sharedUserContext = m_userContext;

    auto getActivityTask = get_activities_for_users(serviceConfigurationId, xuidVector);
    auto taskResult = getActivityTask.then([sharedXboxLiveContextSettings, appConfig, sharedUserContext](task<xbox_live_result<std::vector<multiplayer_activity_details>>> t)
    {
        auto response = t.get();
        if (response.err()) return xbox_live_result<string_t>(response.err(), response.err_message());

        auto activityDetails = response.payload();
        string_t handleId;
        size_t responseSize = activityDetails.size();
        if (responseSize == 0)
        {
            return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "There should be at least one activity per user");
        }
        else if (responseSize == 1)
        {
            handleId = activityDetails.at(0).handle_id();
        }
        else
        {
            return xbox_live_result<string_t>(xbox_live_error_code::invalid_argument, "There should only be one activity per user");
        }

        return xbox_live_result<string_t>(handleId);
    }).then([sharedXboxLiveContextSettings, appConfig, sharedUserContext](xbox_live_result<string_t> handleResult)
    {
        RETURN_TASK_CPP_IF_ERR(handleResult, void);
        auto handleId = handleResult.payload();
        stringstream_t handleStream;
        handleStream << "/handles/";
        handleStream << handleId;

        std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
            sharedXboxLiveContextSettings,
            _T("DELETE"),
            utils::create_xboxlive_endpoint(_T("sessiondirectory"), appConfig),
            handleStream.str(),
            xbox_live_api::clear_activity
            );

        httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
        return httpCall->get_response_with_auth(sharedUserContext)
        .then([](std::shared_ptr<http_call_response> response)
        {
            return xbox_live_result<void>(response->err_code(), response->err_message());
        });
    });

    return utils::create_exception_free_task<void>(
        taskResult
        );
}

task<xbox_live_result<string_t>>
multiplayer_service::set_transfer_handle(
    _In_ multiplayer_session_reference targetSessionReference,
    _In_ multiplayer_session_reference originSessionReference
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(targetSessionReference.is_null(), string_t, "Session reference was not initialized properly");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(originSessionReference.is_null(), string_t, "Session reference was not initialized properly");

    multiplayer_transfer_handle_post_request request(
        std::move(targetSessionReference),
        std::move(originSessionReference),
        c_multiplayerHandleVersionValue
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        _T("/handles"),
        xbox_live_api::set_transfer_handle
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    httpCall->set_request_body(
        request.serialize().serialize()
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        auto multiplayerInvite = multiplayer_invite::deserialize(
            response->response_body_json()
            );

        auto invite = utils::generate_xbox_live_result<multiplayer_invite>(
            multiplayerInvite,
            response
            );

        if (invite.err())
        {
            return xbox_live_result<string_t>(response->err_code(), response->err_message());
        }

        string_t handleId = invite.payload().handle_id();
        return xbox_live_result<string_t>(handleId);
    });

    return utils::create_exception_free_task<string_t>(
        task
        );
}

pplx::task<xbox_live_result<void>>
multiplayer_service::set_search_handle(
    _In_ multiplayer_search_handle_request searchHandleRequest
    )
{
    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        _T("/handles"),
        xbox_live_api::set_search_handle
        );

    searchHandleRequest._Set_version(c_multiplayerHandleVersionValue);
    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    httpCall->set_request_body(
        searchHandleRequest._Serialize().serialize()
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<void>(task);
}

pplx::task<xbox_live_result<void>> 
multiplayer_service::clear_search_handle(
    _In_ const string_t& handleId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(handleId, void, "HandleId is empty");

    string_t handleStr = _T("/handles/") + handleId;
    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("DELETE"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        handleStr,
        xbox_live_api::clear_search_handle
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return xbox_live_result<void>(response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<void>(task);
}

task<xbox_live_result<std::vector<string_t>>>
multiplayer_service::send_invites(
    _In_ multiplayer_session_reference sessionReference,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ uint32_t titleId
    )
{
    return send_invites(
        std::move(sessionReference),
        xboxUserIds,
        titleId,
        _T(""),
        _T("")
        );
}

task<xbox_live_result<std::vector<string_t>>>
multiplayer_service::send_invites(
    _In_ multiplayer_session_reference sessionReference,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ uint32_t titleId,
    _In_ const string_t& contextStringId,
    _In_ const string_t& customActivationContext
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(sessionReference.is_null(), std::vector<string_t>, "sessionReference was not explicity constructed");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(xboxUserIds, std::vector<string_t>, "xboxUserIds are empty");

    auto sharedXboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;
    auto sharedUserContext = m_userContext;

    auto task = create_task([sessionReference, xboxUserIds, titleId, contextStringId, customActivationContext, sharedXboxLiveContextSettings, appConfig, sharedUserContext]()
    {
        xbox_live_result<std::vector<string_t>> inviteHandles;
        for (const auto& xuid : xboxUserIds)
        {
            multiplayer_invite_handle_post_request request(
                std::move(sessionReference),
                c_multiplayerHandleVersionValue,
                xuid,
                titleId,
                contextStringId,
                customActivationContext
                );

            std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
                sharedXboxLiveContextSettings,
                _T("POST"),
                utils::create_xboxlive_endpoint(_T("sessiondirectory"), appConfig),
                _T("/handles"),
                xbox_live_api::send_invites
                );

            httpCall->set_retry_allowed(false);
            httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
            httpCall->set_request_body(
                request.serialize().serialize()
                );

            auto& inviteHandlePayload = inviteHandles.payload();
            httpCall->get_response_with_auth(sharedUserContext)
            .then([&inviteHandles, &inviteHandlePayload](std::shared_ptr<http_call_response> response)
            {
                auto multiplayerInvite = multiplayer_invite::deserialize(
                    response->response_body_json()
                    );

                auto invite = utils::generate_xbox_live_result<multiplayer_invite>(
                    multiplayerInvite,
                    response
                    );

                if (invite.err())
                {
                    inviteHandles._Set_err(response->err_code());
                    inviteHandles._Set_err_message(response->err_message());
                    return;
                }
                string_t handleId = invite.payload().handle_id();
                inviteHandlePayload.push_back(std::move(handleId));
            }).wait();
        }

        return inviteHandles;
    });

    return utils::create_exception_free_task<std::vector<string_t>>(
        task
        );
}

task<xbox_live_result<std::vector<multiplayer_activity_details>>> 
multiplayer_service::get_activities_for_social_group(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& socialGroupOwnerXboxUserId,
    _In_ const string_t& socialGroup
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, std::vector<multiplayer_activity_details>, "serviceConfigurationId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(socialGroupOwnerXboxUserId, std::vector<multiplayer_activity_details>, "socialGroupOwnerXboxUserId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(socialGroup, std::vector<multiplayer_activity_details>, "socialGroup is empty");

    multiplayer_activity_query_post_request request(
        serviceConfigurationId,
        socialGroup,
        socialGroupOwnerXboxUserId
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        c_getActivitiesSubpath,
        xbox_live_api::get_activities_for_social_group
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    httpCall->set_request_body(
        request.serialize().serialize()
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto activityDetails = utils::extract_xbox_live_result_json_vector<multiplayer_activity_details>(
            multiplayer_activity_details::_Deserialize,
            response->response_body_json(),
            _T("results"),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<multiplayer_activity_details>>(
            activityDetails,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<multiplayer_activity_details>>(
        task
        );
}

task<xbox_live_result<std::vector<multiplayer_activity_details>>>
multiplayer_service::get_activities_for_users(
    _In_ const string_t& serviceConfigurationId,
    _In_ const std::vector<string_t>& xboxUserIds
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(serviceConfigurationId, std::vector<multiplayer_activity_details>, "serviceConfigurationId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserIds.empty(), std::vector<multiplayer_activity_details>, "xboxUserIds are empty");

    multiplayer_activity_query_post_request request(
        std::move(serviceConfigurationId),
        std::move(xboxUserIds)
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        c_getActivitiesSubpath,
        xbox_live_api::get_activities_for_users
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    httpCall->set_request_body(
        request.serialize().serialize()
        );

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto activityDetails = utils::extract_xbox_live_result_json_vector<multiplayer_activity_details>(
            multiplayer_activity_details::_Deserialize,
            response->response_body_json(),
            _T("results"),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<multiplayer_activity_details>>(
            activityDetails,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<multiplayer_activity_details>>(
        task
        );
}

pplx::task<xbox_live_result<std::vector<multiplayer_search_handle_details>>>
multiplayer_service::get_search_handles(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sessionTemplateName,
    _In_ const string_t& orderBy,
    _In_ bool orderAscending,
    _In_ const string_t& searchFilter
    )
{
    multiplayer_query_search_handle_request searchHandleRequest(
        serviceConfigurationId, 
        sessionTemplateName,
        orderBy,
        orderAscending,
        searchFilter);

    return get_search_handles(searchHandleRequest);
}

pplx::task<xbox_live_result<std::vector<multiplayer_search_handle_details>>>
multiplayer_service::get_search_handles(
    _In_ const multiplayer_query_search_handle_request& searchHandleRequest
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(searchHandleRequest.service_configuration_id(), std::vector<multiplayer_search_handle_details>, "serviceConfigurationId is empty");
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(searchHandleRequest.session_template_name(), std::vector<multiplayer_search_handle_details>, "sessionTemplateName is empty");

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("sessiondirectory"), m_appConfig),
        c_getSearchHandlesSubpath,
        xbox_live_api::get_search_handles
        );

    httpCall->set_xbox_contract_version_header_value(c_multiplayerServiceContractHeaderValue);
    httpCall->set_request_body(searchHandleRequest._Serialize(utils::string_t_from_internal_string(m_userContext->xbox_user_id())));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto searchHandleDetails = utils::extract_xbox_live_result_json_vector<multiplayer_search_handle_details>(
            multiplayer_search_handle_details::_Deserialize,
            response->response_body_json(),
            _T("results"),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<multiplayer_search_handle_details>>(
            searchHandleDetails,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<multiplayer_search_handle_details>>(
        task
        );
}

string_t
multiplayer_service::multiplayer_session_directory_create_or_update_subpath(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sessionTemplateName,
    _In_ const string_t& sessionName
    )
{
    stringstream_t source;
    source << _T("/serviceconfigs/");
    source << serviceConfigurationId;
    source << _T("/sessionTemplates/");
    source << sessionTemplateName;
    source << _T("/sessions/");
    source << sessionName;
    return source.str();
}

string_t 
multiplayer_service::multiplayer_session_directory_create_or_update_by_handle_subpath(
    _In_ const string_t& handleId
    )
{
    stringstream_t source;
    source << _T("/handles/");
    source << handleId;
    source << _T("/session");

    return source.str();
}

string_t
multiplayer_service::multiplayer_session_directory_get_sessions_sub_path(
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& sessionTemplateNameFilter,
    _In_ const string_t& xboxUserIdFilter,
    _In_ const string_t& keywordFilter,
    _In_ const string_t& visibilityFilter,
    _In_ uint32_t contextVersionFilter,
    _In_ bool includePrivateSessions,
    _In_ bool includeReservations,
    _In_ bool includeInactiveSessions,
    _In_ bool isBatch,
    _In_ uint32_t maxItems
    )
{
    stringstream_t source;

    source << _T("/serviceconfigs/");
    source << serviceConfigurationId;
    if (!sessionTemplateNameFilter.empty())
    {
        source << _T("/sessiontemplates/");
        source << sessionTemplateNameFilter;
    }

    if (isBatch)
    {
        source << _T("/batch");
    }
    else
    {
        source << _T("/sessions");
    }

    std::vector<string_t> params;
    if (!xboxUserIdFilter.empty())
    {
        stringstream_t param;
        param << _T("xuid=");
        param << web::uri::encode_uri(xboxUserIdFilter);
        params.push_back(param.str());
    }

    if (!keywordFilter.empty())
    {
        stringstream_t param;
        param << _T("keyword=");
        param << web::uri::encode_uri(keywordFilter);
        params.push_back(param.str());
    }

    if (!visibilityFilter.empty() && 
        utils::str_icmp(visibilityFilter, _T("any")) != 0)
    {
        stringstream_t param;
        param << _T("visibility=");
        param << web::uri::encode_uri(visibilityFilter);
        params.push_back(param.str());
    }

    if (contextVersionFilter != 0)
    {
        stringstream_t param;
        param << _T("version=");
        param << contextVersionFilter;
        params.push_back(param.str());
    }

    if (includePrivateSessions)
    {
        params.push_back(_T("private=true"));
    }

    if (includeReservations)
    {
        params.push_back(_T("reservations=true"));
    }

    if (includeInactiveSessions)
    {
        params.push_back(_T("inactive=true"));
    }

    if (maxItems != 0)
    {
        stringstream_t param;
        param << _T("take=");
        param << maxItems;
        params.push_back(param.str());
    }

    source << utils::get_query_from_params(params);

    return source.str();
}

std::error_code
multiplayer_service::enable_multiplayer_subscriptions()
{
    return m_multiplayerServiceImpl->enable_multiplayer_subscriptions();
}
 
bool
multiplayer_service::subscriptions_enabled()
{
    return m_multiplayerServiceImpl->subscriptions_enabled();
}

void
multiplayer_service::disable_multiplayer_subscriptions()
{
    return m_multiplayerServiceImpl->disable_multiplayer_subscriptions();
}

function_context
multiplayer_service::add_multiplayer_session_changed_handler(
    _In_ std::function<void(const multiplayer_session_change_event_args&)> handler
    )
{
    return m_multiplayerServiceImpl->add_multiplayer_session_changed_handler(
        std::move(handler)
        );
}

void
multiplayer_service::remove_multiplayer_session_changed_handler(
    _In_ function_context context
    )
{
    return m_multiplayerServiceImpl->remove_multiplayer_session_changed_handler(
        context
        );
}

function_context
multiplayer_service::add_multiplayer_subscription_lost_handler(
    _In_ std::function<void()> handler
    )
{
    return m_multiplayerServiceImpl->add_multiplayer_subscription_lost_handler(
        std::move(handler)
        );
}

void
multiplayer_service::remove_multiplayer_subscription_lost_handler(
    _In_ function_context context
    )
{
    m_multiplayerServiceImpl->remove_multiplayer_subscription_lost_handler(
        context
        );
}

tournament_game_result_state
multiplayer_service::_Convert_string_to_game_result_state(_In_ const string_t& value)
{
    if (utils::str_icmp(value, _T("win")) == 0)
    {
        return tournament_game_result_state::win;
    }
    else if (utils::str_icmp(value, _T("loss")) == 0)
    {
        return tournament_game_result_state::loss;
    }
    else if (utils::str_icmp(value, _T("draw")) == 0)
    {
        return tournament_game_result_state::draw;
    }
    else if (utils::str_icmp(value, _T("rank")) == 0)
    {
        return tournament_game_result_state::rank;
    }
    else if (utils::str_icmp(value, _T("noShow")) == 0)
    {
        return tournament_game_result_state::no_show;
    }

    return tournament_game_result_state::no_contest;
}

string_t
multiplayer_service::_Convert_game_result_state_to_string(_In_ tournament_game_result_state value)
{
    switch (value)
    {
    case tournament_game_result_state::win:
        return _T("win");
        break;
    case tournament_game_result_state::loss:
        return _T("loss");
        break;
    case tournament_game_result_state::draw:
        return _T("draw");
        break;
    case tournament_game_result_state::no_show:
        return _T("noShow");
        break;
    case tournament_game_result_state::rank:
        return _T("rank");
        break;
    case tournament_game_result_state::no_contest:
    default:
        return _T("noContest");
        break;
    }
}

tournament_game_result_source
multiplayer_service::_Convert_string_to_game_result_source(_In_ const string_t& value)
{
    if (utils::str_icmp(value, _T("adjusted")) == 0)
    {
        return tournament_game_result_source::adjusted;
    }
    else if (utils::str_icmp(value, _T("arbitration")) == 0)
    {
        return tournament_game_result_source::arbitration;
    }
    else if (utils::str_icmp(value, _T("server")) == 0)
    {
        return tournament_game_result_source::server;
    }

    return tournament_game_result_source::none;
}

tournament_arbitration_status
multiplayer_service::_Convert_string_to_arbitration_status(
    _In_ const string_t& value
)
{
    if (utils::str_icmp(value, _T("waiting")) == 0)
    {
        return tournament_arbitration_status::waiting;
    }
    else if (utils::str_icmp(value, _T("inprogress")) == 0)
    {
        return tournament_arbitration_status::in_progress;
    }
    else if (utils::str_icmp(value, _T("complete")) == 0)
    {
        return tournament_arbitration_status::complete;
    }
    else if (utils::str_icmp(value, _T("playing")) == 0)
    {
        return tournament_arbitration_status::playing;
    }
    else if (utils::str_icmp(value, _T("joining")) == 0)
    {
        return tournament_arbitration_status::joining;
    }

    return tournament_arbitration_status::incomplete;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END