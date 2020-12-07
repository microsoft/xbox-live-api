// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

statistic::statistic(const XblStatistic& statistic)
    : 
    m_statName(Utils::StringTFromUtf8(statistic.statisticName)),
    m_statType(Utils::StringTFromUtf8(statistic.statisticType)),
    m_value(Utils::StringTFromUtf8(statistic.value))
{ }

statistic::statistic(
    const string_t& name,
    const string_t& type,
    const string_t& value
) :
    m_statName{ name },
    m_statType{ type },
    m_value{ value }
{ }

const string_t& statistic::statistic_name() const
{
    return m_statName; 
}

const string_t& statistic::statistic_type() const
{
    return m_statType;
}

const string_t& statistic::value() const
{
    return m_value;
}

service_configuration_statistic::service_configuration_statistic(const XblServiceConfigurationStatistic& serviceConfigurationStatistic)
    : m_serviceConfigurationId(Utils::StringTFromUtf8(serviceConfigurationStatistic.serviceConfigurationId))
{
    for (uint32_t i = 0; i < serviceConfigurationStatistic.statisticsCount; i++)
    {
        m_statistics.push_back(serviceConfigurationStatistic.statistics[i]);
    }
}

const string_t& service_configuration_statistic::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const std::vector<statistic>& service_configuration_statistic::statistics() const
{
    return m_statistics;
}

user_statistics_result::user_statistics_result(const XblUserStatisticsResult& userStatisticsResult)
    : m_xboxUserId(Utils::StringTFromUint64(userStatisticsResult.xboxUserId))
{
    for (uint32_t i = 0; i < userStatisticsResult.serviceConfigStatisticsCount; i++)
    {
        m_serviceConfigurationStatistics.push_back(userStatisticsResult.serviceConfigStatistics[i]);
    }
}

const string_t& user_statistics_result::xbox_user_id() const
{
    return m_xboxUserId;
}

const std::vector<service_configuration_statistic>& user_statistics_result::service_configuration_statistics() const
{
    return m_serviceConfigurationStatistics;
}

requested_statistics::requested_statistics(
    _In_ string_t serviceConfigurationId,
    _In_ const std::vector<string_t>& statistics
) :
    m_scid(serviceConfigurationId),
    m_statistics(statistics),
    m_statisticsC{ statistics }
{
    Utils::Utf8FromCharT(serviceConfigurationId.c_str(), m_requestedStatistics.serviceConfigurationId, XBL_SCID_LENGTH);

    m_requestedStatistics.statistics = m_statisticsC.Data();
    m_requestedStatistics.statisticsCount = (uint32_t)m_statisticsC.Size();
}

requested_statistics::requested_statistics(_In_ const requested_statistics& other) :
    m_scid(other.m_scid),
    m_statistics(other.m_statistics),
    m_statisticsC{ other.m_statistics }
{
    Utils::Utf8FromCharT(m_scid.c_str(), m_requestedStatistics.serviceConfigurationId, XBL_SCID_LENGTH);

    m_requestedStatistics.statistics = m_statisticsC.Data();
    m_requestedStatistics.statisticsCount = (uint32_t)m_statisticsC.Size();
}

const string_t& requested_statistics::service_configuration_id() const
{
    return m_scid;
}

const std::vector<string_t>& requested_statistics::statistics() const
{
    return m_statistics;
}

const XblRequestedStatistics& requested_statistics::_requested_statistics() const
{
    return m_requestedStatistics;
}

statistic_change_event_args::statistic_change_event_args(const XblStatisticChangeEventArgs& statisticEventArgs)
    :
    m_xboxUserId { Utils::StringTFromUint64(statisticEventArgs.xboxUserId) },
    m_serviceConfigurationId { Utils::StringTFromUtf8(statisticEventArgs.serviceConfigurationId) },
    m_statistic { statisticEventArgs.latestStatistic }
{ }

const string_t& statistic_change_event_args::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t& statistic_change_event_args::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const statistic& statistic_change_event_args::latest_statistic() const
{
    return m_statistic;
}

statistic_change_subscription::statistic_change_subscription(
    _In_ string_t xboxUserId,
    _In_ string_t serviceConfigurationId,
    _In_ xbox::services::user_statistics::statistic newStat,
    _In_ XblRealTimeActivitySubscriptionHandle handle
) :
    real_time_activity_subscription(handle),
    m_xboxUserId{ xboxUserId },
    m_serviceConfigurationId{ serviceConfigurationId },
    m_statistic{ newStat }
{ }

const string_t& statistic_change_subscription::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t& statistic_change_subscription::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const xbox::services::user_statistics::statistic& statistic_change_subscription::statistic() const
{
    return m_statistic;
}

user_statistics_service::user_statistics_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

user_statistics_service::user_statistics_service(const user_statistics_service& other)
{
    XblContextDuplicateHandle(other.m_xblContext, &m_xblContext);
}

user_statistics_service& user_statistics_service::operator=(user_statistics_service other)
{
    std::swap(m_xblContext, other.m_xblContext);
    return *this;
}

user_statistics_service::~user_statistics_service()
{
    XblContextCloseHandle(m_xblContext);
}

pplx::task<xbox_live_result<user_statistics_result>> user_statistics_service::get_single_user_statistics(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& statisticName
)
{
    auto asyncWrapper = new AsyncWrapper<user_statistics_result>(
        [](XAsyncBlock* async, user_statistics_result& result)
    {
        size_t bufferSize;
        auto hr = XblUserStatisticsGetSingleUserStatisticResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            auto buffer = new char[bufferSize];
            XblUserStatisticsResult* resultPtr;
            hr = XblUserStatisticsGetSingleUserStatisticResult(async, bufferSize, buffer, &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = user_statistics_result(*resultPtr);
            }
            delete[] buffer;
        }
        return hr;
    });

    auto hr = XblUserStatisticsGetSingleUserStatisticAsync(
        m_xblContext,
        Utils::Uint64FromStringT(xboxUserId),
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        Utils::StringFromStringT(statisticName).c_str(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<user_statistics_result>> user_statistics_service::get_single_user_statistics(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const std::vector<string_t>& statisticNames
)
{
    UTF8StringArrayRef statisticNamesC{ statisticNames };

    auto asyncWrapper = new AsyncWrapper<user_statistics_result>(
        [](XAsyncBlock* async, user_statistics_result& result)
    {
        size_t bufferSize;
        auto hr = XblUserStatisticsGetSingleUserStatisticsResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            auto buffer = new char[bufferSize];
            XblUserStatisticsResult* resultPtr;
            hr = XblUserStatisticsGetSingleUserStatisticsResult(async, bufferSize, buffer, &resultPtr, nullptr);

            if (SUCCEEDED(hr))
            {
                result = user_statistics_result(*resultPtr);
            }
            delete[] buffer;
        }
        return hr;
    });

    auto hr = XblUserStatisticsGetSingleUserStatisticsAsync(
        m_xblContext,
        Utils::Uint64FromStringT(xboxUserId),
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        statisticNamesC.Data(),
        statisticNamesC.Size(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<user_statistics_result>>> user_statistics_service::get_multiple_user_statistics(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const string_t& serviceConfigurationId,
    _In_ std::vector<string_t>& statisticNames
)
{
    std::vector<uint64_t> xboxUserIdsC = Utils::XuidVectorFromXuidStringVector(xboxUserIds);
    UTF8StringArrayRef statisticNamesC{ statisticNames };

    auto asyncWrapper = new AsyncWrapper<std::vector<user_statistics_result>>(
        [](XAsyncBlock* async, std::vector<user_statistics_result>& results)
    {
        size_t bufferSize;
        auto hr = XblUserStatisticsGetMultipleUserStatisticsResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            auto buffer = new char[bufferSize];
            XblUserStatisticsResult* resultsPtr;
            size_t resultsCount;
            hr = XblUserStatisticsGetMultipleUserStatisticsResult(async, bufferSize, buffer, &resultsPtr, &resultsCount, nullptr);

            if (SUCCEEDED(hr))
            {
                for (size_t i = 0; i < resultsCount; i++)
                {
                    results.push_back(user_statistics_result(resultsPtr[i]));
                }
            }
            delete[] buffer;
        }
        return hr;
    });

    auto hr = XblUserStatisticsGetMultipleUserStatisticsAsync(
        m_xblContext,
        xboxUserIdsC.data(),
        xboxUserIdsC.size(),
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        statisticNamesC.Data(),
        statisticNamesC.Size(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

pplx::task<xbox_live_result<std::vector<user_statistics_result>>> user_statistics_service::get_multiple_user_statistics_for_multiple_service_configurations(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<requested_statistics>& requestedServiceConfigurationStatisticsCollection
)
{
    std::vector<uint64_t> xboxUserIdsC = Utils::XuidVectorFromXuidStringVector(xboxUserIds);
    std::vector<XblRequestedStatistics> requestedStatsC;
    for (size_t i = 0; i < requestedServiceConfigurationStatisticsCollection.size(); i++)
    {
        requestedStatsC.push_back(requestedServiceConfigurationStatisticsCollection[i]._requested_statistics());
    }

    auto asyncWrapper = new AsyncWrapper<std::vector<user_statistics_result>>(
        [](XAsyncBlock* async, std::vector<user_statistics_result>& results)
    {
        size_t bufferSize;
        auto hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize(async, &bufferSize);
        if (SUCCEEDED(hr))
        {
            auto buffer = new char[bufferSize];
            XblUserStatisticsResult* resultsPtr;
            size_t resultsCount;
            hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult(async, bufferSize, buffer, &resultsPtr, &resultsCount, nullptr);

            if (SUCCEEDED(hr))
            {
                for (size_t i = 0; i < resultsCount; i++)
                {
                    results.push_back(user_statistics_result(resultsPtr[i]));
                }
            }
            delete[] buffer;
        }
        return hr;
    });

    auto hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
        m_xblContext,
        xboxUserIdsC.data(),
        (uint32_t)xboxUserIdsC.size(),
        requestedStatsC.data(),
        (uint32_t)requestedStatsC.size(),
        &asyncWrapper->async
    );

    return asyncWrapper->Task(hr);
}

xbox_live_result<std::shared_ptr<statistic_change_subscription>> user_statistics_service::subscribe_to_statistic_change(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& statisticName
)
{
    XblRealTimeActivitySubscriptionHandle subscription;

    HRESULT hr = XblUserStatisticsSubscribeToStatisticChange(
        m_xblContext,
        Utils::Uint64FromStringT(xboxUserId),
        Utils::StringFromStringT(serviceConfigurationId).c_str(),
        Utils::StringFromStringT(statisticName).c_str(),
        &subscription
    );

    auto statistic = xbox::services::user_statistics::statistic(statisticName, Utils::StringTFromUtf8(""), Utils::StringTFromUtf8(""));
    auto result = std::make_shared<statistic_change_subscription>(xboxUserId, serviceConfigurationId, statistic, subscription);

    return xbox_live_result<std::shared_ptr<statistic_change_subscription>>(result, Utils::ConvertHr(hr), "");
}

xbox_live_result<void> user_statistics_service::unsubscribe_from_statistic_change(
    _In_ std::shared_ptr<statistic_change_subscription> subscription
)
{
    if (subscription == nullptr)
    {
        return xbox_live_result<void>(Utils::ConvertHr(E_INVALIDARG), "");
    }

    HRESULT hr = XblUserStatisticsUnsubscribeFromStatisticChange(
        m_xblContext,
        subscription->m_handle
    );

    return xbox_live_result<void>(Utils::ConvertHr(hr), "");
}

struct user_statistics_service::HandlerContext
{
    XblFunctionContext internalContext;
    std::function<void(statistic_change_event_args)> statisticChangedHandler;
};

function_context user_statistics_service::add_statistic_changed_handler(_In_ std::function<void(statistic_change_event_args)> handler)
{
    auto context = new HandlerContext{};
    context->statisticChangedHandler = std::move(handler);

    context->internalContext = XblUserStatisticsAddStatisticChangedHandler(
        m_xblContext,
        [](XblStatisticChangeEventArgs args, void* context)
        {
            auto handler = static_cast<HandlerContext*>(context);
            if (handler && handler->statisticChangedHandler)
            {
                handler->statisticChangedHandler(args);
            }
        }, context);

    return context;
}

void user_statistics_service::remove_statistic_changed_handler(_In_ function_context functionContext)
{
    if (functionContext != nullptr)
    {
        auto context{ static_cast<HandlerContext*>(functionContext) };
        XblUserStatisticsRemoveStatisticChangedHandler(m_xblContext, context->internalContext);
        delete context;
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END

XBL_WARNING_POP