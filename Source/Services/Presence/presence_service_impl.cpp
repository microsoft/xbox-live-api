// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/presence.h"
#include "xsapi/system.h"
#include "presence_internal.h"
#include "user_context.h"
#include "xbox_system_factory.h"

using namespace pplx;
using namespace XBOX_LIVE_NAMESPACE::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_service_impl::presence_service_impl(
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService,
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_realTimeActivityService(realTimeActivityService),
    m_devicePresenceChangeHandlerCounter(0),
    m_titlePresenceChangeHandlerCounter(0),
    m_userContext(userContext),
    m_xboxLiveContextSettings(xboxLiveContextSettings),
    m_appConfig(appConfig)
{
}

presence_service_impl::~presence_service_impl()
{
    m_devicePresenceChangeHandler.clear();
    m_titlePresenceChangeHandler.clear();
}

function_context
presence_service_impl::add_device_presence_changed_handler(
    _In_ std::function<void(const device_presence_change_event_args&)> handler
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
presence_service_impl::remove_device_presence_changed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_devicePresenceChangeHandlerLock.get());

    m_devicePresenceChangeHandler.erase(context);
}

pplx::task<xbox_live_result<void>>
presence_service_impl::set_presence(
    _In_ bool isUserActiveInTitle
    )
{
    return set_presence_helper(isUserActiveInTitle, presence_data())
    .then([](xbox_live_result<uint32_t> result)
    {
        return xbox_live_result<void>(result.err(), result.err_message());
    });
}

pplx::task<xbox_live_result<void>> 
presence_service_impl::set_presence(
    _In_ bool isUserActiveInTitle,
    _In_ presence_data presenceData
    )
{
    return set_presence_helper(isUserActiveInTitle, std::move(presenceData))
    .then([](xbox_live_result<uint32_t> result)
    {
        return xbox_live_result<void>(result.err(), result.err_message());
    });
}


pplx::task<xbox_live_result<uint32_t>>
presence_service_impl::set_presence_helper(
    _In_ bool isUserActiveInTitle,
    _In_ presence_data presenceData
    )
{
    string_t xboxUserId = m_userContext->xbox_user_id();

    string_t pathAndQuery = set_presence_sub_path(
        xboxUserId
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("userpresence"), m_appConfig),
        pathAndQuery,
        xbox_live_api::set_presence_helper
        );

    presence_title_request request(
        isUserActiveInTitle,
        std::move(presenceData)
        );

    httpCall->set_request_body(request.serialize().serialize());
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        uint32_t heartbeatDelay = 0;
        auto headers = response->response_headers();
        if (headers.has(_T("X-Heartbeat-After")))
        {
            auto heartbeatDelayInSeconds = headers[_T("X-Heartbeat-After")];
            heartbeatDelay = utils::string_t_to_uint32(heartbeatDelayInSeconds) / 60;
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

        return xbox_live_result<uint32_t>(heartbeatDelay, response->err_code(), response->err_message());
    });

    return utils::create_exception_free_task<uint32_t>(
        task
        );
}

function_context
presence_service_impl::add_title_presence_changed_handler(
    _In_ std::function<void(const title_presence_change_event_args&)> handler
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
presence_service_impl::remove_title_presence_changed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_titlePresenceChangeHandlerLock.get());

    m_titlePresenceChangeHandler.erase(context);
}

void
presence_service_impl::device_presence_changed(
    _In_ const device_presence_change_event_args& eventArgs
    )
{
    std::unordered_map<function_context, std::function<void(const device_presence_change_event_args&)>> devicePresenceChangedHandlersCopy;

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
presence_service_impl::title_presence_changed(
    _In_ const title_presence_change_event_args& eventArgs
)
{
    std::unordered_map<function_context, std::function<void(const title_presence_change_event_args&)>> titlePresenceChangedHandlersCopy;

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

xbox_live_result<std::shared_ptr<device_presence_change_subscription>>
presence_service_impl::subscribe_to_device_presence_change(
    _In_ const string_t& xboxUserId
    )
{
    std::weak_ptr<presence_service_impl> thisWeakPtr = shared_from_this();

    std::shared_ptr<device_presence_change_subscription> deviceSub = std::make_shared<device_presence_change_subscription>(
        xboxUserId,
        ([thisWeakPtr](const device_presence_change_event_args& eventArgs)
        {
            std::shared_ptr<presence_service_impl> pThis(thisWeakPtr.lock());
            if (pThis)
            {
                pThis->device_presence_changed(eventArgs);
            }
        }),
        ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
        {
            LOG_DEBUG("device_presence_changed error occurred");
            std::shared_ptr<presence_service_impl> pThis(thisWeakPtr.lock());
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
        return xbox_live_result<std::shared_ptr<device_presence_change_subscription>>(deviceSub);
    }

    return xbox_live_result<std::shared_ptr<device_presence_change_subscription>>(subscriptionSucceded.err(), subscriptionSucceded.err_message());
}

xbox_live_result<void>
presence_service_impl::unsubscribe_from_device_presence_change(
    _In_ std::shared_ptr<device_presence_change_subscription> subscription
    )
{
    return m_realTimeActivityService->_Remove_subscription(
        subscription
        );
}

xbox_live_result<std::shared_ptr<title_presence_change_subscription>>
presence_service_impl::subscribe_to_title_presence_change(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId
    )
{
    std::weak_ptr<presence_service_impl> thisWeakPtr = shared_from_this();

    std::shared_ptr<title_presence_change_subscription> titleSub = std::make_shared<title_presence_change_subscription>(
        xboxUserId,
        titleId,
        ([thisWeakPtr](const title_presence_change_event_args& eventArgs)
        {
            std::shared_ptr<presence_service_impl> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->title_presence_changed(eventArgs);
            }
        }),
        ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
        {
            std::shared_ptr<presence_service_impl> pThis(thisWeakPtr.lock());
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
        return xbox_live_result<std::shared_ptr<title_presence_change_subscription>>(titleSub);
    }

    return xbox_live_result<std::shared_ptr<title_presence_change_subscription>>(subscriptionSucceded.err(), subscriptionSucceded.err_message());
}

xbox_live_result<void>
presence_service_impl::unsubscribe_from_title_presence_change(
    _In_ std::shared_ptr<title_presence_change_subscription> subscription
    )
{
    return m_realTimeActivityService->_Remove_subscription(
        subscription
        );
}

pplx::task<xbox_live_result<presence_record>> 
presence_service_impl::get_presence(
    _In_ const string_t& xboxUserId
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(xboxUserId, presence_record, "xboxUserId is empty");

    string_t pathAndQuery = get_presence_sub_path(
        xboxUserId
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("userpresence"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_presence
        );
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        return utils::generate_xbox_live_result<presence_record>(
            presence_record::_Deserialize(response->response_body_json()),
            response
            );
    });

    return utils::create_exception_free_task<presence_record>(
        task
        );
}

pplx::task<xbox_live_result<std::vector<presence_record>>>
presence_service_impl::get_presence_for_multiple_users(
    _In_ const std::vector<string_t>& xboxUserIds
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserIds.empty(), std::vector<presence_record>, "xboxUserIds is empty");

    string_t pathAndQuery = get_presence_user_batch_subpath();

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("userpresence"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_presence_for_multiple_users
        );

    presence_user_batch_request request(
        xboxUserIds,
        string_t(),
        string_t(),
        std::vector<presence_device_type>(),
        std::vector<uint32_t>(),
        presence_detail_level::default_level,
        false,
        false
        );

    httpCall->set_request_body(request.serialize().serialize());
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto presenceRecords = utils::extract_xbox_live_result_json_vector<presence_record>(
            presence_record::_Deserialize,
            response->response_body_json(),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<presence_record>>(
            presenceRecords,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<presence_record>>(
        task
        );
}

pplx::task<xbox_live_result<std::vector<presence_record>>>
presence_service_impl::get_presence_for_multiple_users(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<presence_device_type>& deviceTypes,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ presence_detail_level presenceDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(xboxUserIds.empty(), std::vector<presence_record>, "xboxUserIds are empty");

    string_t pathAndQuery = get_presence_user_batch_subpath();

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("userpresence"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_presence_for_multiple_users
        );

    presence_user_batch_request request(
        xboxUserIds,
        string_t(),
        string_t(),
        deviceTypes,
        titleIds,
        presenceDetailLevel,
        onlineOnly,
        broadcastingOnly
        );

    httpCall->set_request_body(request.serialize().serialize());
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto responseBody = response->response_body_json();
        auto presenceRecords = utils::extract_xbox_live_result_json_vector<presence_record>(
            presence_record::_Deserialize,
            responseBody,
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<presence_record>>(
            presenceRecords,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<presence_record>>(
        task
        );
}

pplx::task<xbox_live_result<std::vector<presence_record>>> 
presence_service_impl::get_presence_for_social_group(
    _In_ const string_t& socialGroup
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(socialGroup, std::vector<presence_record>, "socialGroup is empty");
    string_t xboxUserId = m_userContext->xbox_user_id();

    string_t pathAndQuery = get_presence_for_social_group_subpath(
        xboxUserId,
        socialGroup
        );

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("userpresence"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_presence_for_social_group
        );
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto presenceRecords = utils::extract_xbox_live_result_json_vector<presence_record>(
            presence_record::_Deserialize,
            response->response_body_json(),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<presence_record>>(
            presenceRecords,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<presence_record>>(
        task
        );
}

pplx::task<xbox_live_result<std::vector<presence_record>>>
presence_service_impl::get_presence_for_social_group(
    _In_ const string_t& socialGroup,
    _In_ const string_t& socialGroupOwnerXboxUserId,
    _In_ const std::vector<presence_device_type>& deviceTypes,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ presence_detail_level peoplehubDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF_STRING_EMPTY(socialGroup, std::vector<presence_record>, "socialGroup is empty");

    string_t pathAndQuery = get_presence_user_batch_subpath();

    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("POST"),
        utils::create_xboxlive_endpoint(_T("userpresence"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_presence_for_social_group
        );

    presence_user_batch_request request(
        std::vector<string_t>(),
        socialGroup,
        socialGroupOwnerXboxUserId,
        deviceTypes,
        titleIds,
        peoplehubDetailLevel,
        onlineOnly,
        broadcastingOnly
        );

    httpCall->set_request_body(request.serialize().serialize());
    httpCall->set_xbox_contract_version_header_value(_T("3"));

    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([](std::shared_ptr<http_call_response> response)
    {
        std::error_code errc = xbox_live_error_code::no_error;
        auto presenceRecords = utils::extract_xbox_live_result_json_vector<presence_record>(
            presence_record::_Deserialize,
            response->response_body_json(),
            errc,
            true
            );

        return utils::generate_xbox_live_result<std::vector<presence_record>>(
            presenceRecords,
            response
            );
    });

    return utils::create_exception_free_task<std::vector<presence_record>>(
        task
        );
}

string_t 
presence_service_impl::set_presence_sub_path(
    _In_ const string_t& xboxUserId
    )
{
    stringstream_t subpath;
    subpath << _T("/users/xuid(");
    subpath << xboxUserId;
    subpath << _T(")/devices/current/titles/current");
    return subpath.str();
}

string_t 
presence_service_impl::get_presence_sub_path(
    _In_ const string_t& xboxUserId
    )
{
    stringstream_t subpath;
    subpath << _T("/users/xuid(");
    subpath << xboxUserId;
    subpath << _T(")?level=all");
    return subpath.str();
}

string_t
presence_service_impl::get_presence_user_batch_subpath()
{
    return _T("/users/batch");
}

string_t
presence_service_impl::get_presence_for_social_group_subpath(
    _In_ const string_t& xboxUserId, 
    _In_ const string_t& socialGroup
    )
{
    stringstream_t subpath;
    subpath << _T("/users/xuid(");
    subpath << xboxUserId;
    subpath << _T(")/groups/");
    subpath << socialGroup;
    subpath << _T("?level=all");

    return subpath.str();
}

void presence_service_impl::set_presence_set_finished_handler(const std::function<void(int heartBeatDelayInMins)>& onSetPresenceFinish)
{ 
    auto xsapiSingleton = get_xsapi_singleton();
    xsapiSingleton->m_onSetPresenceFinish = onSetPresenceFinish;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END