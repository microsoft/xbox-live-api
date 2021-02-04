// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "notification_internal.h"
#include "xsapi_utils.h"
#include "xsapi-cpp/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

UWPNotificationService::UWPNotificationService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
) noexcept :
    NotificationService(std::move(user), contextSettings)
{
}

HRESULT UWPNotificationService::RegisterWithNotificationService(
    _In_ AsyncContext<HRESULT> async
) noexcept
{

    std::weak_ptr<UWPNotificationService> thisWeak = std::dynamic_pointer_cast<UWPNotificationService>(shared_from_this());

    //hold onto a reference to global state to prevent cleanup while the subscribe call is still out
    std::shared_ptr<GlobalState> holdCleanup{ GlobalState::Get() };

    // Setup WNS Channel for Push Notifications
    auto notificationChannelAsyncOp = Windows::Networking::PushNotifications::PushNotificationChannelManager::CreatePushNotificationChannelForApplicationAsync();
    notificationChannelAsyncOp->Completed = ref new Windows::Foundation::AsyncOperationCompletedHandler<Windows::Networking::PushNotifications::PushNotificationChannel ^>(
        [thisWeak, async, holdCleanup](Windows::Foundation::IAsyncOperation<Windows::Networking::PushNotifications::PushNotificationChannel^>^ asyncOp, Windows::Foundation::AsyncStatus status)
    {
        if (status == Windows::Foundation::AsyncStatus::Completed)
        {
            try
            {
                auto channel = asyncOp->GetResults();
                channel->PushNotificationReceived += ref new Windows::Foundation::TypedEventHandler<Windows::Networking::PushNotifications::PushNotificationChannel ^, Windows::Networking::PushNotifications::PushNotificationReceivedEventArgs ^>(
                    [thisWeak, async](Windows::Networking::PushNotifications::PushNotificationChannel ^ channel, Windows::Networking::PushNotifications::PushNotificationReceivedEventArgs^ args)
                {
                    try
                    {
                        std::shared_ptr<UWPNotificationService> pThis(thisWeak.lock());
                        if (pThis != nullptr)
                        {
                            try
                            {
                                pThis->OnPushNotificationReceived(channel, args);
                            }
                            catch (...)
                            {
                                LOG_ERROR("OnPushNotificationReceived error");
                            }
                        }
                    }

                    catch (...)
                    {
                        LOG_ERROR("OnPushNotificationReceived error");
                    }
                });

                auto family = Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily;
                auto form = Windows::System::Profile::AnalyticsInfo::DeviceForm;
                auto version = Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamilyVersion;
                xsapi_internal_string platform;
                if (family == "Windows.Mobile")
                {
                    platform = "WindowsOneCoreMobile";
                }
                else if (family == "Windows.Xbox")
                {
                    platform = "Durango";
                }
                else
                {
                    platform = "WindowsOneCore";
                }

                Vector<NotificationFilter> notificationFilterList;
                std::shared_ptr<UWPNotificationService> pThis(thisWeak.lock());
                if (pThis != nullptr)
                {
                    pThis->RegisterForNotificationsHelper(
                        utils::create_guid(true), // applicationInstanceId
                        utils::internal_string_from_string_t(channel->Uri->Data()),
                        platform,
                        "",
                        "",
                        notificationFilterList,
                        {
                            async.Queue(),
                            [thisWeak, async](HRESULT hr)
                            {
                               if (auto pThis{ thisWeak.lock() })
                               {
                                  if (SUCCEEDED(hr))
                                  {
                                      async.Complete(hr);
                                  }
                                  else
                                  {
                                      LOG_ERROR("Failed to successfully register with notification service");
                                      async.Complete(E_XBL_RUNTIME_ERROR);
                                  }
                               }
                            }
                        });
                }
            }
            catch (...)
            {
                LOG_ERROR("Failed to successfully register with notification service");
            }
        }
    });

    return S_OK;
}

void UWPNotificationService::OnPushNotificationReceived(
    _In_ Windows::Networking::PushNotifications::PushNotificationChannel ^sender,
    _In_ Windows::Networking::PushNotifications::PushNotificationReceivedEventArgs ^args
)
{
    HRESULT errc = S_OK;
    if (args && args->RawNotification && args->RawNotification->Content)
    {
        string_t content = args->RawNotification->Content->Data();
        JsonDocument parsedJson;
        parsedJson.Parse(utils::internal_string_from_string_t(content).c_str());

        if (parsedJson.HasParseError())
        {
            errc = WEB_E_INVALID_JSON_STRING;
        }

        xsapi_internal_string notificationTypeString;
        xsapi_internal_string xuid;
        if (parsedJson.IsObject() && parsedJson.HasMember("xboxLiveNotification"))
        {
            const JsonValue& xboxLiveNotificationJson = parsedJson["xboxLiveNotification"];
            errc |= JsonUtils::ExtractJsonString(xboxLiveNotificationJson, "notificationType", notificationTypeString);
            errc |= JsonUtils::ExtractJsonString(xboxLiveNotificationJson, "userXuid", xuid);
        }

        if (!errc)
        {
            LOGS_INFO << "Received WNS notification, type: " << notificationTypeString << ", xuid: " << xuid;

            if (utils::str_icmp(notificationTypeString.c_str(), "spop") == 0)
            {
                User::SetTokenExpired(utils::internal_string_to_uint64(xuid));
            }
        }
        else
        {
            LOGS_ERROR << "Receiving WNS notification error: " << errc << ", message: " << utils::convert_hresult_to_error_name(errc);
        }
    }
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END