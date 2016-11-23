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
#include "UserStatisticsService_WinRT.h"
#include "Utils_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"

using namespace concurrency;
using namespace Platform::Collections;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::user_statistics;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

UserStatisticsService::UserStatisticsService(
    _In_ user_statistics_service cppObj
    ) :
    m_cppObj(cppObj)
{
    m_userStatisticsServiceEventBind = std::make_shared<UserStatisticsServiceEventBind>(Platform::WeakReference(this), m_cppObj);
    m_userStatisticsServiceEventBind->AddUserStatisticsServiceEvent();
}

UserStatisticsService::~UserStatisticsService()
{
    m_userStatisticsServiceEventBind->RemoveUserStatisticChangeEvent(m_cppObj);
}

UserStatisticsServiceEventBind::UserStatisticsServiceEventBind(
    _In_ Platform::WeakReference setting,
    _In_ xbox::services::user_statistics::user_statistics_service& cppObj
    ) : 
    m_setting(setting),
    m_cppObj(cppObj)
{
}

void
UserStatisticsServiceEventBind::RemoveUserStatisticChangeEvent(
    _In_ xbox::services::user_statistics::user_statistics_service& cppObj
    )
{
    cppObj.remove_statistic_changed_handler(m_functionContext);
}

void UserStatisticsServiceEventBind::StatisticChangeRouter(_In_ const xbox::services::user_statistics::statistic_change_event_args& statisticChangeEventArgs)
{
    UserStatisticsService^ setting = m_setting.Resolve<UserStatisticsService>();
    if (setting != nullptr)
    {
        if (m_cppObj._Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([setting, statisticChangeEventArgs]()
            {
                setting->RaiseStatisticChange(ref new StatisticChangeEventArgs(statisticChangeEventArgs));
            }));
        }
        else
        {
            setting->RaiseStatisticChange(ref new StatisticChangeEventArgs(statisticChangeEventArgs));
        }
    }
}

void UserStatisticsServiceEventBind::AddUserStatisticsServiceEvent()
{
    std::weak_ptr<UserStatisticsServiceEventBind> thisWeakPtr = shared_from_this();
    m_functionContext = m_cppObj.add_statistic_changed_handler([thisWeakPtr](_In_ const xbox::services::user_statistics::statistic_change_event_args& statisticChangeEventArgs)
    {
        std::shared_ptr<UserStatisticsServiceEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->StatisticChangeRouter(statisticChangeEventArgs);
        }
    });
}

void UserStatisticsService::RaiseStatisticChange(_In_ StatisticChangeEventArgs^ args)
{
    StatisticChanged(this, args);
}

IAsyncOperation<UserStatisticsResult^>^ 
UserStatisticsService::GetSingleUserStatisticsAsync(
    _In_ String^ xboxUserId,
    _In_ String^ serviceConfigurationId,
    _In_ String^ statName
    )
{
    auto task = m_cppObj.get_single_user_statistics(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(statName)
        )
    .then([](xbox_live_result<user_statistics_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new UserStatisticsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<UserStatisticsResult^>^ 
UserStatisticsService::GetSingleUserStatisticsAsync(
    _In_ String^ xboxUserId,
    _In_ String^ serviceConfigurationId,
    _In_ IVectorView<String^>^ statNames
    )
{
    std::vector<string_t> vecStatNames = UtilsWinRT::CovertVectorViewToStdVectorString(statNames);
    auto task = m_cppObj.get_single_user_statistics(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        vecStatNames
        )
    .then([](xbox_live_result<user_statistics_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new UserStatisticsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}


IAsyncOperation<IVectorView<UserStatisticsResult^>^>^
UserStatisticsService::GetMultipleUserStatisticsAsync(
_In_ IVectorView<String^>^ xboxUserIds,
_In_ String^ serviceConfigurationId,
_In_ IVectorView<String^>^ statNames
)
{
    std::vector<string_t> vecXboxUserIds = UtilsWinRT::CovertVectorViewToStdVectorString(xboxUserIds);
    std::vector<string_t> vecStatNames = UtilsWinRT::CovertVectorViewToStdVectorString(statNames);
    auto task = m_cppObj.get_multiple_user_statistics(
        vecXboxUserIds,
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        vecStatNames
        )
    .then([](xbox_live_result<std::vector<user_statistics_result>> cppResults)
    {
        THROW_IF_ERR(cppResults);
        Vector<UserStatisticsResult^>^ results = ref new Platform::Collections::Vector<UserStatisticsResult^>();
        for (auto& cppResult : cppResults.payload())
        {
            results->Append(ref new UserStatisticsResult(cppResult));
        }

        return results->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<IVectorView<UserStatisticsResult^>^>^
UserStatisticsService::GetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
    _In_ IVectorView<String^>^ xboxUserIds,
    _In_ IVectorView<RequestedStatistics^>^ requestedServiceConfigurationStatisticsCollection
    )
{
    std::vector<string_t> vecxboxUserIds = UtilsWinRT::CovertVectorViewToStdVectorString(xboxUserIds);

    std::vector<requested_statistics> requests;
    for(const auto& requestedServiceConfig : requestedServiceConfigurationStatisticsCollection)
    {
        requests.push_back(requestedServiceConfig->GetCppObj());
    }

    auto task = m_cppObj.get_multiple_user_statistics_for_multiple_service_configurations(
        vecxboxUserIds,
        requests
        )
    .then([](xbox_live_result<std::vector<user_statistics_result>> cppResults)
    {
        Vector<UserStatisticsResult^>^ results = ref new Platform::Collections::Vector<UserStatisticsResult^>();
        for (auto& cppResult : cppResults.payload())
        {
            results->Append(ref new UserStatisticsResult(cppResult));
        }

        return results->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

StatisticChangeSubscription^ 
UserStatisticsService::SubscribeToStatisticChange(
    _In_ Platform::String^ xboxUserId,
    _In_ Platform::String^ serviceConfigurationId,
    _In_ Platform::String^ statisticName
    )
{
    std::shared_ptr<statistic_change_subscription> subscription;
    auto subscriptionResult = m_cppObj.subscribe_to_statistic_change(
        STRING_T_FROM_PLATFORM_STRING(xboxUserId),
        STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId),
        STRING_T_FROM_PLATFORM_STRING(statisticName)
        );

    THROW_IF_ERR(subscriptionResult);

    subscription = subscriptionResult.payload();

    return ref new StatisticChangeSubscription(subscription);
}

void 
UserStatisticsService::UnsubscribeFromStatisticChange(
    _In_ StatisticChangeSubscription^ subscription
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(subscription);

    auto unsubscribeResult = m_cppObj.unsubscribe_from_statistic_change(
        subscription->GetCppObj()
        );

    THROW_IF_ERR(unsubscribeResult);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END