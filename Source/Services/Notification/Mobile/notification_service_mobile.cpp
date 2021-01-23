#include "pch.h"
#include "notification_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN
#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID
MobileNotificationService::MobileNotificationService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
) : NotificationService(std::move(user), contextSettings)
{}

HRESULT MobileNotificationService::RegisterWithNotificationService(
    _In_ const String& endpointId,
    _In_ AsyncContext<HRESULT> async) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(endpointId);

    // http://xboxwiki/wiki/DeviceEndpoint_Object#Filter_Sources
    Vector<NotificationFilter> notificationFilterList;
#if HC_PLATFORM == HC_PLATFORM_ANDROID
    notificationFilterList.push_back({ NotificationTypeFilterSourceType::Multiplayer, 1 });  // Game invites
    notificationFilterList.push_back({ NotificationTypeFilterSourceType::Multiplayer, 8 });  // Game invites with connection string
    notificationFilterList.push_back({ NotificationTypeFilterSourceType::Achievements, 1 }); // Achievement unlocked
#endif
    auto currentEndPointId = AppConfig::Instance()->EndpointId();
    if (!currentEndPointId.empty())
    {
        std::weak_ptr<MobileNotificationService> thisWeak = std::dynamic_pointer_cast<MobileNotificationService>(shared_from_this());
        // If already subscribed before, unsubscribe the old endpoint first
        return UnregisterFromNotificationHelper(
            currentEndPointId,
            {
                async.Queue(),
                [
                    thisWeak,
                    endpointId,
                    async,
                    notificationFilterList
                ](HRESULT result)
                {
                    if (FAILED(result))
                    {
                        return async.Complete(result);
                    }

                    std::shared_ptr<MobileNotificationService> pThis(thisWeak.lock());
                    if (pThis != nullptr)
                    {
                        pThis->RegisterForNotificationsHelper(
                            utils::create_guid(true),
                            endpointId,
#if HC_PLATFORM == HC_PLATFORM_ANDROID
                            _T("Android"),
                            _T("AndroidDevice"),
                            _T("XSAPI_ANDROID"),
#elif HC_PLATFORM == HC_PLATFORM_IOS
                            _T("iOS"),
                            _T("iOSDevice"),
                            _T("XSAPI_I"),
#endif
                            notificationFilterList,
                            async
                        );
                    }
                }
            });
    }
    else {
        return RegisterForNotificationsHelper(
            utils::create_guid(true),
            endpointId,
#if HC_PLATFORM == HC_PLATFORM_ANDROID
            _T("Android"),
            _T("AndroidDevice"),
            _T("XSAPI_ANDROID"),
#elif HC_PLATFORM == HC_PLATFORM_IOS
            _T("iOS"),
            _T("iOSDevice"),
            _T("XSAPI_I"),
#endif
            notificationFilterList,
            async
        );
    }
}

#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
