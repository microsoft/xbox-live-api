//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "PresenceService_WinRT.h"
#include "Utils_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace XBOX_LIVE_NAMESPACE::presence;
using namespace Microsoft::Xbox::Services::System;
using namespace XBOX_LIVE_NAMESPACE;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

PresenceService::PresenceService(
    _In_ xbox::services::presence::presence_service cppObj
    ) : 
    m_cppObj(cppObj)
{
    m_devicePresenceChangeEventBind = std::make_shared<DevicePresenceChangeEventBind>(Platform::WeakReference(this), m_cppObj);
    m_titlePresenceChangeEventBind = std::make_shared<TitlePresenceChangeEventBind>(Platform::WeakReference(this), m_cppObj);

    m_devicePresenceChangeEventBind->AddDevicePresenceChangeEvent();
    m_titlePresenceChangeEventBind->AddTitlePresenceChangeEvent();
}

PresenceService::~PresenceService()
{
    m_devicePresenceChangeEventBind->RemoveDevicePresenceChangeEvent(m_cppObj);
    m_titlePresenceChangeEventBind->RemoveTitlePresenceChangeEvent(m_cppObj);
}

DevicePresenceChangeEventBind::DevicePresenceChangeEventBind(
    _In_ Platform::WeakReference setting,
    _In_ xbox::services::presence::presence_service& cppObj
    ) : 
    m_setting(setting),
    m_cppObj(cppObj)
{
}

void
DevicePresenceChangeEventBind::RemoveDevicePresenceChangeEvent( 
    _In_ xbox::services::presence::presence_service& cppObj 
    )
{
    cppObj.remove_device_presence_changed_handler(m_functionContext);
}

void
DevicePresenceChangeEventBind::DevicePresenceChangeRouter(
    _In_ const xbox::services::presence::device_presence_change_event_args& presenceChangeEventArgs
    )
{
    PresenceService^ presenceService = m_setting.Resolve<PresenceService>();
    if (presenceService != nullptr)
    {
        if (m_cppObj._Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([presenceService, presenceChangeEventArgs]()
            {
                presenceService->RaiseDevicePresenceChange(ref new DevicePresenceChangeEventArgs(presenceChangeEventArgs));
            }));
        }
        else
        {
            presenceService->RaiseDevicePresenceChange(ref new DevicePresenceChangeEventArgs(presenceChangeEventArgs));
        }
    }
}

void DevicePresenceChangeEventBind::AddDevicePresenceChangeEvent()
{
    std::weak_ptr<DevicePresenceChangeEventBind> thisWeakPtr = shared_from_this();
    m_functionContext = m_cppObj.add_device_presence_changed_handler([thisWeakPtr](_In_ const xbox::services::presence::device_presence_change_event_args& presenceChangeEventArgs)
    {
        std::shared_ptr<DevicePresenceChangeEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->DevicePresenceChangeRouter(presenceChangeEventArgs);
        }
    });
}

TitlePresenceChangeEventBind::TitlePresenceChangeEventBind(
    _In_ Platform::WeakReference setting,
    _In_ xbox::services::presence::presence_service& cppObj
    ) : 
    m_setting(setting),
    m_cppObj(cppObj)
{
}

void
TitlePresenceChangeEventBind::RemoveTitlePresenceChangeEvent( 
    _In_ xbox::services::presence::presence_service& cppObj 
    )
{
    cppObj.remove_title_presence_changed_handler(m_functionContext);
}

void
TitlePresenceChangeEventBind::TitlePresenceChangeRouter(
    _In_ const xbox::services::presence::title_presence_change_event_args& presenceChangeEventArgs
    )
{
    PresenceService^ presenceService = m_setting.Resolve<PresenceService>();

    if (presenceService != nullptr)
    {
        if (m_cppObj._Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([presenceService, presenceChangeEventArgs]()
            {
                presenceService->RaiseTitlePresenceChange(ref new TitlePresenceChangeEventArgs(presenceChangeEventArgs));
            }));
        }
        else
        {
            presenceService->RaiseTitlePresenceChange(ref new TitlePresenceChangeEventArgs(presenceChangeEventArgs));
        }
    }
}

void TitlePresenceChangeEventBind::AddTitlePresenceChangeEvent()
{
    std::weak_ptr<TitlePresenceChangeEventBind> thisWeakPtr = shared_from_this();
    m_functionContext = m_cppObj.add_title_presence_changed_handler([thisWeakPtr](_In_ const xbox::services::presence::title_presence_change_event_args& presenceChangeEventArgs)
    {
        std::shared_ptr<TitlePresenceChangeEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->TitlePresenceChangeRouter(presenceChangeEventArgs);
        }
    });
}

void
PresenceService::RaiseDevicePresenceChange(_In_ DevicePresenceChangeEventArgs^ args)
{
    DevicePresenceChanged(this, args);
}

void
PresenceService::RaiseTitlePresenceChange(_In_ TitlePresenceChangeEventArgs^ args)
{
    TitlePresenceChanged(this, args);
}

Windows::Foundation::IAsyncAction^
PresenceService::SetPresenceAsync(
    _In_ bool isUserActiveInTitle
    )
{
    auto task = m_cppObj.set_presence(isUserActiveInTitle)
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncAction^
PresenceService::SetPresenceAsync(
    _In_ bool isUserActiveInTitle,
    _In_opt_ PresenceData^ presenceData
    )
{
    presence_data data;
    
    if (presenceData != nullptr)
    {
        data = presenceData->GetCppObj();
    }

    auto task = m_cppObj.set_presence(isUserActiveInTitle, data)
    .then([](xbox_live_result<void> result)
    {
        THROW_IF_ERR(result);
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<PresenceRecord^>^ 
PresenceService::GetPresenceAsync(
    _In_ String^ xboxUserId
    )
{
    THROW_INVALIDARGUMENT_IF(xboxUserId == nullptr || xboxUserId->IsEmpty());

    auto task = m_cppObj.get_presence(STRING_T_FROM_PLATFORM_STRING(xboxUserId))
    .then([](xbox_live_result<presence_record> record)
    {
        THROW_IF_ERR(record);
        return ref new PresenceRecord(record.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<PresenceRecord^>^>^ 
PresenceService::GetPresenceForMultipleUsersAsync(
    _In_ IVectorView<String^>^ xboxUserIds
    )
{
    THROW_INVALIDARGUMENT_IF(xboxUserIds == nullptr || xboxUserIds->Size == 0);

    auto task = m_cppObj.get_presence_for_multiple_users(
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIds)
        )
    .then([](xbox_live_result<std::vector<presence_record>> records)
    {
        THROW_IF_ERR(records);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<PresenceRecord, presence_record>(
            records.payload()
            )->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<PresenceRecord^>^>^ 
PresenceService::GetPresenceForMultipleUsersAsync(
    _In_ IVectorView<String^>^ xboxUserIds,
    _In_opt_ IVectorView<PresenceDeviceType>^ deviceTypes,
    _In_opt_ IVectorView<uint32>^ titleIds,
    _In_ PresenceDetailLevel detailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    )
{
    std::vector<presence_device_type> types;

    if (deviceTypes != nullptr)
    {
        for (const auto& deviceType : deviceTypes)
        {
            types.push_back(static_cast<presence_device_type>(deviceType));
        }
    }

    std::vector<uint32_t> ids;
    if (titleIds != nullptr)
    {
        for (const auto& titleId : titleIds)
        {
            ids.push_back(titleId);
        }
    }

    auto task = m_cppObj.get_presence_for_multiple_users(
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIds),
        types,
        ids,
        static_cast<presence_detail_level>(detailLevel),
        onlineOnly,
        broadcastingOnly
        )
    .then([](xbox_live_result<std::vector<presence_record>> records)
    {
        THROW_IF_ERR(records);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<PresenceRecord, presence_record>(
            records.payload()
            )->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<PresenceRecord^>^>^ 
PresenceService::GetPresenceForSocialGroupAsync(
    _In_ String^ socialGroup
    )
{
    auto task = m_cppObj.get_presence_for_social_group(
        STRING_T_FROM_PLATFORM_STRING(socialGroup)
        )
    .then([](xbox_live_result<std::vector<presence_record>> records)
    {
        THROW_IF_ERR(records);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<PresenceRecord, presence_record>(
            records.payload()
            )->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<PresenceRecord^>^>^ 
PresenceService::GetPresenceForSocialGroupAsync(
    _In_ String^ socialGroup,
    _In_opt_ String^ socialGroupOwnerXboxuserId,
    _In_opt_ IVectorView<PresenceDeviceType>^ deviceTypes,
    _In_opt_ IVectorView<uint32>^ titleIds,
    _In_ PresenceDetailLevel detailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    )
{
    std::vector<presence_device_type> types;

    if (deviceTypes != nullptr)
    {
        for (const auto& deviceType : deviceTypes)
        {
            types.push_back(static_cast<presence_device_type>(deviceType));
        }
    }

    std::vector<uint32_t> ids;

    if (titleIds != nullptr)
    {
        for (const auto& titleId : titleIds)
        {
            ids.push_back(titleId);
        }
    }

    auto task = m_cppObj.get_presence_for_social_group(
        STRING_T_FROM_PLATFORM_STRING(socialGroup),
        STRING_T_FROM_PLATFORM_STRING(socialGroupOwnerXboxuserId),
        types,
        ids,
        static_cast<presence_detail_level>(detailLevel),
        onlineOnly,
        broadcastingOnly
        )
    .then([](xbox_live_result<std::vector<presence_record>> records)
    {
        THROW_IF_ERR(records);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<PresenceRecord, presence_record>(
            records.payload()
            )->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

DevicePresenceChangeSubscription^ 
PresenceService::SubscribeToDevicePresenceChange(
    _In_ Platform::String^ xboxUserId
    )
{
    std::shared_ptr<device_presence_change_subscription> subscription;
    auto subscriptionResult = m_cppObj.subscribe_to_device_presence_change(
        xboxUserId->Data()
        );
    THROW_IF_ERR(subscriptionResult);
    subscription = subscriptionResult.payload();

    return ref new DevicePresenceChangeSubscription(subscription);
}

void 
PresenceService::UnsubscribeFromDevicePresenceChange(
    _In_ DevicePresenceChangeSubscription^ subscription
    )
{
    auto unsubscribeResult = m_cppObj.unsubscribe_from_device_presence_change(
        subscription->GetCppObj()
        );

    THROW_IF_ERR(unsubscribeResult);
}

TitlePresenceChangeSubscription^
PresenceService::SubscribeToTitlePresenceChange(
    _In_ Platform::String^ xboxUserId,
    _In_ uint32 titleId
    )
{
    std::shared_ptr<title_presence_change_subscription> subscription;
    auto subscriptionResult = m_cppObj.subscribe_to_title_presence_change(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        titleId
        );

    THROW_IF_ERR(subscriptionResult);
    subscription = subscriptionResult.payload();

    return ref new TitlePresenceChangeSubscription(subscription);
}

void 
PresenceService::UnsubscribeFromTitlePresenceChange(
    _In_ TitlePresenceChangeSubscription^ subscription
    )
{
    auto unsubscribeResult = m_cppObj.unsubscribe_from_title_presence_change(
        subscription->GetCppObj()
        );

    THROW_IF_ERR(unsubscribeResult);
}

const xbox::services::presence::presence_service&
PresenceService::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END