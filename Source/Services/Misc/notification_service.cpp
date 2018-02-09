// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#if !UNIT_TEST_SERVICES
#include "notification_service.h"
#include "xbox_system_factory.h"
#include "user_context.h"
#include "user_impl.h"

using namespace pplx;
using namespace xbox::services::system;

static const std::chrono::milliseconds RETRY_LENGTH(60 * 1000);
static const int32_t NUM_RETRY_TIMES = 10;

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

std::shared_ptr<notification_service>
notification_service::get_notification_service_singleton()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
    if (xsapiSingleton->m_notificationSingleton == nullptr)
    {
#if XSAPI_A
        xsapiSingleton->m_notificationSingleton = std::make_shared<notification_service_android>();
#elif XSAPI_I
        xsapiSingleton->m_notificationSingleton = std::make_shared<notification_service_ios>();
#elif UWP_API || TV_API
        xsapiSingleton->m_notificationSingleton = std::make_shared<notification_service_windows>();
#endif
    }
    return xsapiSingleton->m_notificationSingleton;
}

pplx::task<xbox_live_result<void>>
notification_service::subscribe_to_notifications(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    )
{
    if (m_isInitialized)
    {
        return pplx::task_from_result<xbox_live_result<void>>(xbox_live_result<void>(xbox_live_error_code::logic_error, "already is initialized"));
    }

    m_isInitialized = true;
    XSAPI_ASSERT(userContext != nullptr);
    XSAPI_ASSERT(xboxLiveContextSettings != nullptr);
    XSAPI_ASSERT(appConfig != nullptr);

    m_userContext = std::move(userContext);
    m_xboxLiveContextSettings = std::move(xboxLiveContextSettings);
    m_appConfig = std::move(appConfig);

    return subscribe_to_notifications();
}

pplx::task<xbox_live_result<void>>
notification_service::unsubscribe_from_notifications()
{
    if (m_endpointId.empty())
    {
        return pplx::task_from_result<xbox_live_result<void>>(xbox_live_result<void>(xbox_live_error_code::logic_error, "endpointId is empty"));
    }
    return unsubscribe_from_notifications_helper(m_endpointId);
}

pplx::task<xbox_live_result<void>>
notification_service::unsubscribe_from_notifications(_In_ const string_t& endpointId)
{
    if (endpointId.empty())
    {
        LOG_WARN("unsubscribe failed. endpoint ID empty");
        return pplx::task_from_result<xbox_live_result<void>>(xbox_live_result<void>(xbox_live_error_code::invalid_argument, "endpointId is an invalid argument"));
    }
    return unsubscribe_from_notifications_helper(endpointId);
}

pplx::task<xbox_live_result<void>>
notification_service::unsubscribe_from_notifications_helper(
    _In_ const string_t& endpointId
    )
{
    m_isInitialized = false;
    auto subpath = _T("/system/notifications/endpoints/") + endpointId;
    auto endpoint = utils::create_xboxlive_endpoint(_T("notify"), m_appConfig);
    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("DELETE"),
        endpoint,
        subpath,
        xbox_live_api::subscribe_to_notifications
        );

    std::weak_ptr<notification_service> thisWeakPtr = shared_from_this();
    return httpCall->get_response_with_auth(m_userContext, http_call_response_body_type::json_body)
    .then([thisWeakPtr](std::shared_ptr<http_call_response> t)
    {
        std::shared_ptr<notification_service> pThis(thisWeakPtr.lock());

        if (pThis != nullptr)
        {
            if (t->err_code())
            {
                LOG_ERROR("notification service attempt failed\n");
                LOG_ERROR("http status code: ");
                LOGS_ERROR << t->http_status();
                LOGS_ERROR << t->err_message().c_str();

                return xbox_live_result<void>(t->err_code(), t->err_message());
            }

            pThis->m_endpointId = _T("");
            return xbox_live_result<void>();
        }

        return xbox_live_result<void>(xbox_live_error_code::runtime_error, "pThis was null after trying to get auth");
    });
}

pplx::task<xbox_live_result<void>>
notification_service::subscribe_to_notifications_helper(
    _In_ const string_t& applicationInstanceId,
    _In_ const string_t& uriData,
    _In_ const string_t& platform,
    _In_ const string_t& deviceName,
    _In_ const string_t& platformVersion,
    _In_ const std::vector<notification_filter> notificationFilterEnum
    )
{
    m_isInitialized = true;
    stringstream_t str;
    str << xbox_live_app_config::get_app_config_singleton()->title_id();
    string_t titleId = str.str();
    web::json::value payload;
    payload[_T("systemId")] = web::json::value::string(applicationInstanceId);
    payload[_T("endpointUri")] = web::json::value::string(uriData);
    payload[_T("platform")] = web::json::value::string(platform);
    payload[_T("platformVersion")] = web::json::value::string(platformVersion);
#if XSAPI_I
    payload[_T("transport")] = web::json::value::string(_T("NotiHub"));
    payload[_T("transportPath")] = web::json::value::string(xbox_live_app_config::get_app_config_singleton()->_Apns_Environment());
#endif
    if (!deviceName.empty())
    {
        payload[_T("deviceName")] = web::json::value::string(deviceName);
    }
    payload[_T("locale")] = web::json::value::string(utils::string_t_from_internal_string(utils::get_locales()));
    payload[_T("titleId")] = web::json::value::string(titleId);

    web::json::value filterJson;
    uint32_t i = 0;
    for (auto& notificationFilter : notificationFilterEnum)
    {
        web::json::value filterObj;
        filterObj[_T("action")] = web::json::value::string(_T("Include"));
        filterObj[_T("source")] = web::json::value(notificationFilter.sourceType);
        filterObj[_T("type")] = web::json::value(notificationFilter.type);
        filterJson[i] = filterObj;
        ++i;
    }

    payload[_T("filters")] = filterJson;
    auto endpoint = utils::create_xboxlive_endpoint(_T("notify"), m_appConfig);

    std::weak_ptr<notification_service> thisWeakPtr = shared_from_this();
    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto task = pplx::create_task([thisWeakPtr, userContext, endpoint, xboxLiveContextSettings, payload]()
    {
        int32_t numRetryTime = NUM_RETRY_TIMES;
        std::error_code xboxLiveResultCode;
        do
        {
            std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
                xboxLiveContextSettings,
                _T("POST"),
                endpoint,
                _T("/system/notifications/endpoints"),
                xbox_live_api::subscribe_to_notifications
                );

            httpCall->set_request_body(payload.serialize());

            auto result = httpCall->get_response_with_auth(userContext, http_call_response_body_type::json_body)
            .then([thisWeakPtr](pplx::task<std::shared_ptr<http_call_response>> t0)
            {
                auto t = t0.get();
                std::shared_ptr<notification_service> pThis(thisWeakPtr.lock());

                if (pThis == nullptr)
                {
                    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "pThis is null after trying to get auth");
                }

                auto responseJson = t->response_body_json();
                std::error_code errc;
                pThis->m_endpointId = utils::extract_json_string(responseJson, _T("endpointId"), errc);
                auto localConfig = xbox_system_factory::get_factory()->create_local_config();
                auto localConfigResult = localConfig->write_value_to_local_storage(pThis->ENDPOINT_ID_CACHE_NAME, utils::internal_string_from_string_t(pThis->m_endpointId));
                if (localConfigResult.err())
                {
                    LOG_ERROR("Writing endpoint id to local config failed");
                }
                if (t->err_code() || errc)
                {
                    return xbox_live_result<void>(errc);
                }

                return xbox_live_result<void>();
            }).get();
            xboxLiveResultCode = result.err();

            --numRetryTime;
            if (xboxLiveResultCode != xbox_live_error_code::no_error)
            {
                LOG_WARN("retrying notification");
                std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(RETRY_LENGTH));
            }
            else
            {
                numRetryTime = 0;
            }

        } while (numRetryTime > 0);

        return xbox_live_result<void>(xboxLiveResultCode);
    });

    return task;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
#endif
