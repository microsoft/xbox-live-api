// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "xsapi/system.h"
#include "presence_internal.h"
#include "user_context.h"
#include "xbox_system_factory.h"

using namespace pplx;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_service_internal::presence_service_internal(
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService,
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig
    ) :
    m_realTimeActivityService(realTimeActivityService),
    m_devicePresenceChangeHandlerCounter(0),
    m_titlePresenceChangeHandlerCounter(0),
    m_userContext(userContext),
    m_xboxLiveContextSettings(xboxLiveContextSettings),
    m_appConfig(appConfig)
{
}

presence_service_internal::~presence_service_internal()
{
    m_devicePresenceChangeHandler.clear();
    m_titlePresenceChangeHandler.clear();
}

function_context
presence_service_internal::add_device_presence_changed_handler(
    _In_ xbox_live_callback<std::shared_ptr<device_presence_change_event_args_internal>> handler
    )
{
    std::lock_guard<std::mutex> lock(m_devicePresenceChangeHandlerLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_devicePresenceChangeHandlerCounter;
        m_devicePresenceChangeHandler[m_devicePresenceChangeHandlerCounter] = std::move(handler);
    }

    return context;
}

void
presence_service_internal::remove_device_presence_changed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_devicePresenceChangeHandlerLock.get());

    m_devicePresenceChangeHandler.erase(context);
}

xbox_live_result<void>
presence_service_internal::set_presence(
    _In_ bool isUserActiveInTitle,
    _In_ presence_data_internal presenceData,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<uint32_t>> callback
    )
{
    xsapi_internal_string pathAndQuery = set_presence_sub_path(
        m_userContext->xbox_user_id()
        );

    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "POST",
        utils::create_xboxlive_endpoint("userpresence", m_appConfig),
        utils::string_t_from_internal_string(pathAndQuery),
        xbox_live_api::set_presence_helper
        );

    presence_title_request request(
        isUserActiveInTitle,
        std::move(presenceData)
        );

    httpCall->set_request_body(utils::internal_string_from_string_t(request.serialize().serialize()));
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    return httpCall->get_response_with_auth(
        m_userContext, 
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            uint32_t heartbeatDelay = 0;
            auto headers = response->response_headers();

            auto iter = headers.find("X-Heartbeat-After");
            if (iter != headers.end())
            {
                auto heartbeatDelayInSeconds = iter->second;
                heartbeatDelay = utils::internal_string_to_uint32(heartbeatDelayInSeconds) / 60;
            }
            else
            {
                heartbeatDelay = 5;
            }

            auto xsapiSingleton = get_xsapi_singleton();
            if (xsapiSingleton->m_onSetPresenceFinish)
            {
                xsapiSingleton->m_onSetPresenceFinish(heartbeatDelay);
            }

            callback(xbox_live_result<uint32_t>(heartbeatDelay, response->err_code(), response->err_message().data()));
        }
    );
}

function_context
presence_service_internal::add_title_presence_changed_handler(
    _In_ xbox_live_callback<std::shared_ptr<title_presence_change_event_args_internal>> handler
    )
{
    std::lock_guard<std::mutex> lock(m_titlePresenceChangeHandlerLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_titlePresenceChangeHandlerCounter;
        m_titlePresenceChangeHandler[m_titlePresenceChangeHandlerCounter] = std::move(handler);
    }

    return context;
}

void
presence_service_internal::remove_title_presence_changed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_titlePresenceChangeHandlerLock.get());

    m_titlePresenceChangeHandler.erase(context);
}

void
presence_service_internal::device_presence_changed(
    _In_ std::shared_ptr<device_presence_change_event_args_internal> eventArgs
    )
{
    xsapi_internal_unordered_map<function_context, xbox_live_callback<std::shared_ptr<device_presence_change_event_args_internal>>> devicePresenceChangedHandlersCopy;

    {
        std::lock_guard<std::mutex> lock(m_devicePresenceChangeHandlerLock.get());
        devicePresenceChangedHandlersCopy = m_devicePresenceChangeHandler;
    }

    for (auto& handler : devicePresenceChangedHandlersCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(eventArgs);
            }
            catch (...)
            {
                LOG_ERROR("device_presence_changed call threw an exception");
            }
        }
    }
}

void
presence_service_internal::title_presence_changed(
    _In_ std::shared_ptr<title_presence_change_event_args_internal> eventArgs
)
{
    xsapi_internal_unordered_map<function_context, xbox_live_callback<std::shared_ptr<title_presence_change_event_args_internal>>> titlePresenceChangedHandlersCopy;

    {
        std::lock_guard<std::mutex> lock(m_titlePresenceChangeHandlerLock.get());
        titlePresenceChangedHandlersCopy = m_titlePresenceChangeHandler;
    }

    for (auto& handler : titlePresenceChangedHandlersCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(eventArgs);
            }
            catch (...)
            {
                LOG_ERROR("title_presence_changed call threw an exception");
            }
        }
    }
}

xbox_live_result<std::shared_ptr<device_presence_change_subscription_internal>>
presence_service_internal::subscribe_to_device_presence_change(
    _In_ const xsapi_internal_string& xboxUserId
    )
{
    std::weak_ptr<presence_service_internal> thisWeakPtr = shared_from_this();

    auto deviceSub = xsapi_allocate_shared<device_presence_change_subscription_internal>(
        xboxUserId,
        ([thisWeakPtr](std::shared_ptr<device_presence_change_event_args_internal> eventArgs)
        {
            std::shared_ptr<presence_service_internal> pThis(thisWeakPtr.lock());
            if (pThis)
            {
                pThis->device_presence_changed(eventArgs);
            }
        }),
        ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
        {
            LOG_DEBUG("device_presence_changed error occurred");
            std::shared_ptr<presence_service_internal> pThis(thisWeakPtr.lock());
            if (pThis)
            {
                pThis->m_realTimeActivityService->_Trigger_subscription_error(eventArgs);
            }
        })
        );

    auto subscriptionSucceded = m_realTimeActivityService->_Add_subscription(
        deviceSub
        );

    if (!subscriptionSucceded.err())
    {
        return xbox_live_result<std::shared_ptr<device_presence_change_subscription_internal>>(deviceSub);
    }

    return xbox_live_result<std::shared_ptr<device_presence_change_subscription_internal>>(subscriptionSucceded.err(), subscriptionSucceded.err_message());
}

xbox_live_result<void>
presence_service_internal::unsubscribe_from_device_presence_change(
    _In_ std::shared_ptr<device_presence_change_subscription_internal> subscription
    )
{
    return m_realTimeActivityService->_Remove_subscription(
        subscription
        );
}

xbox_live_result<std::shared_ptr<title_presence_change_subscription_internal>>
presence_service_internal::subscribe_to_title_presence_change(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ uint32_t titleId
    )
{
    std::weak_ptr<presence_service_internal> thisWeakPtr = shared_from_this();

    auto titleSub = std::make_shared<title_presence_change_subscription_internal>(
        xboxUserId,
        titleId,
        ([thisWeakPtr](std::shared_ptr<title_presence_change_event_args_internal> eventArgs)
        {
            std::shared_ptr<presence_service_internal> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->title_presence_changed(eventArgs);
            }
        }),
        ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
        {
            std::shared_ptr<presence_service_internal> pThis(thisWeakPtr.lock());
            LOG_DEBUG("title_presence_change error occurred");
            if (pThis != nullptr)
            {
                pThis->m_realTimeActivityService->_Trigger_subscription_error(eventArgs);
            }
        })
        );

    auto subscriptionSucceded = m_realTimeActivityService->_Add_subscription(
        titleSub
        );

    if (!subscriptionSucceded.err())
    {
        return xbox_live_result<std::shared_ptr<title_presence_change_subscription_internal>>(titleSub);
    }

    return xbox_live_result<std::shared_ptr<title_presence_change_subscription_internal>>(subscriptionSucceded.err(), subscriptionSucceded.err_message());
}

xbox_live_result<void>
presence_service_internal::unsubscribe_from_title_presence_change(
    _In_ std::shared_ptr<title_presence_change_subscription_internal> subscription
    )
{
    return m_realTimeActivityService->_Remove_subscription(
        subscription
        );
}

xbox_live_result<void>
presence_service_internal::get_presence(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<presence_record_internal>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xboxUserId is empty");

    xsapi_internal_string pathAndQuery = get_presence_sub_path(
        xboxUserId
        );

    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "GET",
        utils::create_xboxlive_endpoint("userpresence", m_appConfig),
        utils::string_t_from_internal_string(pathAndQuery),
        xbox_live_api::get_presence
        );
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            callback(utils::generate_xbox_live_result<std::shared_ptr<presence_record_internal>>(
                presence_record_internal::deserialize(response->response_body_json()),
                response
                ));
        }
    );

    return xbox_live_result<void>();
}

xbox_live_result<void>
presence_service_internal::get_presence_for_multiple_users(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserIds.empty(), void, "xboxUserIds is empty");

    xsapi_internal_string pathAndQuery = get_presence_user_batch_subpath();

    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "POST",
        utils::create_xboxlive_endpoint("userpresence", m_appConfig),
        utils::string_t_from_internal_string(pathAndQuery),
        xbox_live_api::get_presence_for_multiple_users
        );

    presence_user_batch_request request(
        xboxUserIds,
        xsapi_internal_string(),
        xsapi_internal_string(),
        xsapi_internal_vector<presence_device_type>(),
        xsapi_internal_vector<uint32_t>(),
        presence_detail_level::default_level,
        false,
        false
        );

    httpCall->set_request_body(utils::internal_string_from_string_t(request.serialize().serialize()));
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            std::error_code errc = xbox_live_error_code::no_error;
            auto presenceRecords = utils::extract_xbox_live_result_json_vector_internal<std::shared_ptr<presence_record_internal>>(
                presence_record_internal::deserialize,
                response->response_body_json(),
                errc,
                true
                );

            callback(utils::generate_xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>(
                presenceRecords,
                response
                ));
        });

    return xbox_live_result<void>();
}

xbox_live_result<void>
presence_service_internal::get_presence_for_multiple_users(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
    _In_ const xsapi_internal_vector<presence_device_type>& deviceTypes,
    _In_ const xsapi_internal_vector<uint32_t>& titleIds,
    _In_ presence_detail_level presenceDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserIds.empty(), void, "xboxUserIds are empty");

    xsapi_internal_string pathAndQuery = get_presence_user_batch_subpath();

    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "POST",
        utils::create_xboxlive_endpoint("userpresence", m_appConfig),
        utils::string_t_from_internal_string(pathAndQuery),
        xbox_live_api::get_presence_for_multiple_users
        );

    presence_user_batch_request request(
        xboxUserIds,
        xsapi_internal_string(),
        xsapi_internal_string(),
        deviceTypes,
        titleIds,
        presenceDetailLevel,
        onlineOnly,
        broadcastingOnly
        );

    httpCall->set_request_body(utils::internal_string_from_string_t(request.serialize().serialize()));
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            std::error_code errc = xbox_live_error_code::no_error;
            auto responseBody = response->response_body_json();
            auto presenceRecords = utils::extract_xbox_live_result_json_vector_internal<std::shared_ptr<presence_record_internal>>(
                presence_record_internal::deserialize,
                responseBody,
                errc,
                true
                );

            callback(utils::generate_xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>(
                presenceRecords,
                response
                ));
        });

    return xbox_live_result<void>();
}

xbox_live_result<void>
presence_service_internal::get_presence_for_social_group(
    _In_ const xsapi_internal_string& socialGroup,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(socialGroup.empty(), void, "socialGroup is empty");

    xsapi_internal_string pathAndQuery = get_presence_for_social_group_subpath(
        m_userContext->xbox_user_id(),
        socialGroup
        );

    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "GET",
        utils::create_xboxlive_endpoint("userpresence", m_appConfig),
        utils::string_t_from_internal_string(pathAndQuery),
        xbox_live_api::get_presence_for_social_group
        );
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            std::error_code errc = xbox_live_error_code::no_error;
            auto responseBody = response->response_body_json();
            auto presenceRecords = utils::extract_xbox_live_result_json_vector_internal<std::shared_ptr<presence_record_internal>>(
                presence_record_internal::deserialize,
                responseBody,
                errc,
                true
                );

            callback(utils::generate_xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>(
                presenceRecords,
                response
                ));
        });

    return xbox_live_result<void>();
}

xbox_live_result<void>
presence_service_internal::get_presence_for_social_group(
    _In_ const xsapi_internal_string& socialGroup,
    _In_ const xsapi_internal_string& socialGroupOwnerXboxUserId,
    _In_ const xsapi_internal_vector<presence_device_type>& deviceTypes,
    _In_ const xsapi_internal_vector<uint32_t>& titleIds,
    _In_ presence_detail_level peoplehubDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(socialGroup.empty(), void, "socialGroup is empty");

    xsapi_internal_string pathAndQuery = get_presence_user_batch_subpath();

    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "POST",
        utils::create_xboxlive_endpoint("userpresence", m_appConfig),
        utils::string_t_from_internal_string(pathAndQuery),
        xbox_live_api::get_presence_for_social_group
        );

    presence_user_batch_request request(
        xsapi_internal_vector<xsapi_internal_string>(),
        socialGroup,
        socialGroupOwnerXboxUserId,
        deviceTypes,
        titleIds,
        peoplehubDetailLevel,
        onlineOnly,
        broadcastingOnly
        );

    httpCall->set_request_body(utils::internal_string_from_string_t(request.serialize().serialize()));
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(
        m_userContext,
        http_call_response_body_type::json_body,
        false,
        queue,
        [callback](std::shared_ptr<http_call_response_internal> response)
        {
            std::error_code errc = xbox_live_error_code::no_error;
            auto responseBody = response->response_body_json();
            auto presenceRecords = utils::extract_xbox_live_result_json_vector_internal<std::shared_ptr<presence_record_internal>>(
                presence_record_internal::deserialize,
                responseBody,
                errc,
                true
                );

            callback(utils::generate_xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>>(
                presenceRecords,
                response
                ));
        });

    return xbox_live_result<void>();
}

std::shared_ptr<xbox_live_context_settings> 
presence_service_internal::xbox_live_context_settings()
{
    return m_xboxLiveContextSettings;
}

xsapi_internal_string 
presence_service_internal::set_presence_sub_path(
    _In_ const xsapi_internal_string& xboxUserId
    )
{
    xsapi_internal_stringstream subpath;
    subpath << "/users/xuid(";
    subpath << xboxUserId;
    subpath << ")/devices/current/titles/current";
    return subpath.str();
}

xsapi_internal_string
presence_service_internal::get_presence_sub_path(
    _In_ const xsapi_internal_string& xboxUserId
    )
{
    xsapi_internal_stringstream subpath;
    subpath << "/users/xuid(";
    subpath << xboxUserId;
    subpath << ")?level=all";
    return subpath.str();
}

xsapi_internal_string
presence_service_internal::get_presence_user_batch_subpath()
{
    return "/users/batch";
}

xsapi_internal_string
presence_service_internal::get_presence_for_social_group_subpath(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ const xsapi_internal_string& socialGroup
    )
{
    xsapi_internal_stringstream subpath;
    subpath << "/users/xuid(";
    subpath << xboxUserId;
    subpath << ")/groups/";
    subpath << socialGroup;
    subpath << "?level=all";

    return subpath.str();
}

void presence_service_internal::set_presence_set_finished_handler(const std::function<void(int heartBeatDelayInMins)>& onSetPresenceFinish)
{ 
    auto xsapiSingleton = get_xsapi_singleton();
    xsapiSingleton->m_onSetPresenceFinish = onSetPresenceFinish;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END