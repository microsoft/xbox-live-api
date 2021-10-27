// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_live_context_internal.h"
#include "notification_internal.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32
using namespace pplx;

static const std::chrono::milliseconds RETRY_LENGTH(60 * 1000);
static const int32_t NUM_RETRY_TIMES = 10;
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN
NotificationService::NotificationService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
) :
    m_user(std::move(user)),
    m_contextSettings(contextSettings)
{}

HRESULT NotificationService::UnregisterFromNotificationService(
    _In_ AsyncContext<HRESULT> async)
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    switch (m_registrationStatus)
    {
    case RegistrationStatus::Unregistered:
    {
        // No work to do in this case
        async.Complete(S_OK);
        return S_OK;
    }
    case RegistrationStatus::Unregistering:
    case RegistrationStatus::PendingUnregistration:
    {
        // If we are already unregistering, collapse the AsyncContext and return
        m_unregistrationAsync = AsyncContext<HRESULT>::Collapse({ std::move(m_unregistrationAsync), std::move(async) });
        return S_OK;
    }
    case RegistrationStatus::Registering:
    {
        // Wait until registration finishes and then unregister
        m_registrationStatus = RegistrationStatus::PendingUnregistration;
        m_unregistrationAsync = std::move(async);
        return S_OK;
    }
    case RegistrationStatus::Registered:
    default:
    {
        // Otherwise proceed with unregistration
        m_registrationStatus = RegistrationStatus::Unregistering;
        m_unregistrationAsync = std::move(async);

        assert(!m_endpointId.empty());
        return UnregisterFromNotificationHelper(m_endpointId, async);
    }
    }
}

HRESULT NotificationService::UnregisterFromNotificationHelper(
    _In_ const String& endpointId,
    _In_ AsyncContext<HRESULT> async)
{
    auto subpath = "/system/notifications/endpoints/" + endpointId;

    Result<User> userResult = m_user.Copy();
    RETURN_HR_IF_FAILED(userResult.Hresult());

    auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
    HRESULT hr = httpCall->Init(
        m_contextSettings,
        "DELETE",
        XblHttpCall::BuildUrl("notify", subpath),
        xbox_live_api::subscribe_to_notifications
    );

    RETURN_HR_IF_FAILED(hr);

    return httpCall->Perform(AsyncContext<HttpResult>{
        async.Queue().DeriveWorkerQueue(),
            [
                thisWeakPtr = std::weak_ptr<NotificationService>{ shared_from_this() },
                async
            ](HttpResult httpResult)
        {
            if (auto pThis{ thisWeakPtr.lock() })
            {
                if (pThis == nullptr)
                {
                    async.Complete(E_XBL_RUNTIME_ERROR);
                    return;
                }

                std::lock_guard<std::recursive_mutex> lock{ pThis->m_mutex };
                // If the call fails, log but still treat it as though we are now unregistered
                pThis->m_registrationStatus = RegistrationStatus::Unregistered;
                pThis->m_endpointId.clear();
                HRESULT hr = httpResult.Hresult();
                if (SUCCEEDED(hr))
                {
                    hr = httpResult.Payload()->Result();
                    if (FAILED(hr))
                    {
                        LOG_ERROR("notification service attempt failed\n");
                        LOG_ERROR("http status code: ");
                        LOGS_ERROR << httpResult.Payload()->HttpStatus();
                    }
                }

                async.Complete(hr);
                return pThis->m_unregistrationAsync.Complete(hr);
            }
            else
            {
                return async.Complete(E_XBL_RUNTIME_ERROR);
            }
        }
    });
}

HRESULT NotificationService::RegisterForNotificationsHelper(
    _In_ const String& applicationInstanceId,
    _In_ const String& uriData,
    _In_ const String& platform,
    _In_ const String& deviceName,
    _In_ const String& platformVersion,
    _In_ const Vector<NotificationFilter> notificationFilterEnum,
    _In_ AsyncContext<HRESULT> async
)
{
    switch (m_registrationStatus)
    {
    case RegistrationStatus::PendingUnregistration:
    {
        // This indicates registration is in progress. Since another registration was requested, abort the 
        // pending unregistration, complete the unregistration AsyncContext, and collapse the register AsyncContexts
        m_registrationAsync = AsyncContext<HRESULT>::Collapse({ std::move(m_registrationAsync), std::move(async) });
        m_registrationStatus = RegistrationStatus::Registering;
        m_unregistrationAsync.Complete(E_ABORT);
        return S_OK;
    }
    case RegistrationStatus::Registered:
    {
        // If we have already registered, no work to do
        async.Complete(S_OK);
        return S_OK;
    }
    case RegistrationStatus::Registering:
    {
        // If we are already registering, collapse the AsyncContexts and return
        m_registrationAsync = AsyncContext<HRESULT>::Collapse({ std::move(m_registrationAsync), std::move(async) });
        return S_OK;
    }
    default:
    {
        auto workQueue = async.Queue().DeriveWorkerQueue();
        m_registrationAsync = AsyncContext<HRESULT>::Collapse({ std::move(m_registrationAsync), std::move(async) });
        m_registrationStatus = RegistrationStatus::Registering;

        xsapi_internal_stringstream str;
        str << AppConfig::Instance()->TitleId();
        xsapi_internal_string titleId = str.str();
        JsonDocument payload(rapidjson::kObjectType);
        JsonDocument::AllocatorType& allocator = payload.GetAllocator();
        payload.AddMember("systemId", JsonValue(applicationInstanceId.data(), allocator).Move(), allocator);
        payload.AddMember("endpointUri", JsonValue(uriData.data(), allocator).Move(), allocator);
        payload.AddMember("platform", JsonValue(platform.data(), allocator).Move(), allocator);
        if (!platformVersion.empty())
        {
            payload.AddMember("platformVersion", JsonValue(platformVersion.data(), allocator).Move(), allocator);
        }

#if HC_PLATFORM == HC_PLATFORM_IOS
        payload.AddMember("transport", "NotiHub", allocator);
        payload.AddMember("transportPath", JsonValue(AppConfig::Instance()->APNSEnvironment().c_str(), allocator).Move(), allocator);
#elif HC_PLATFORM == HC_PLATFORM_ANDROID
        payload.AddMember("transport", "FCM", allocator);
#elif HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM == HC_PLATFORM_GDK || HC_PLATFORM_IS_EXTERNAL
        payload.AddMember("transport", "RTA", allocator);
#endif
        xsapi_internal_string locale = utils::get_locales();
        payload.AddMember("locale", JsonValue(locale.c_str(), allocator).Move(), allocator);
        payload.AddMember("titleId", JsonValue(titleId.c_str(), allocator).Move(), allocator);

        if (!deviceName.empty())
        {
            payload.AddMember("deviceName", JsonValue(deviceName.data(), allocator).Move(), allocator);
        }

        JsonValue filterJson(rapidjson::kArrayType);
        for (auto& notificationFilter : notificationFilterEnum)
        {
            JsonValue filterObj(rapidjson::kObjectType);
            filterObj.AddMember("action", "Include", allocator);
            filterObj.AddMember("source", notificationFilter.sourceType, allocator);
            filterObj.AddMember("type", notificationFilter.type, allocator);
            filterJson.PushBack(filterObj, allocator);
        }

        payload.AddMember("filters", filterJson, allocator);
        Result<User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());

        auto httpCall = MakeShared<XblHttpCall>(userResult.ExtractPayload());
        RETURN_HR_IF_FAILED(httpCall->Init(
            m_contextSettings,
            "POST",
            XblHttpCall::BuildUrl("notify", "/system/notifications/endpoints"),
            xbox_live_api::subscribe_to_notifications
        ));

        RETURN_HR_IF_FAILED(httpCall->SetRequestBody(payload));
        return httpCall->Perform(AsyncContext<HttpResult>{
            workQueue,
                [
                    thisWeakPtr = std::weak_ptr<NotificationService>{ shared_from_this() }
                ](HttpResult httpResult)
            {
                std::shared_ptr<NotificationService> pThis(thisWeakPtr.lock());

                if (pThis == nullptr)
                {
                    return;
                }

                HRESULT hr = httpResult.Hresult();
                if (SUCCEEDED(hr))
                {
                    hr = httpResult.Payload()->Result();
                    if (SUCCEEDED(hr))
                    {
                        const JsonValue& responseJson = httpResult.Payload()->GetResponseBodyJson();
                        hr = JsonUtils::ExtractJsonString(responseJson, "endpointId", pThis->m_endpointId);
                        // Registration has succeeded at this point
                        switch (pThis->m_registrationStatus)
                        {
                        case RegistrationStatus::Registering:
                        {
                            pThis->m_registrationStatus = RegistrationStatus::Registered;
                            pThis->m_registrationAsync.Complete(hr);
                            break;
                        }
                        default:
                        {
                            // No other states should be possible
                            assert(false);
                        }
                        }
                    }
                    else
                    {
                        // Registration failed for some reason
                        pThis->m_registrationStatus = RegistrationStatus::Unregistered;
                        pThis->m_registrationAsync.Complete(E_XBL_RUNTIME_ERROR);
                    }
                }
            }
        });
    }
    };
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END


