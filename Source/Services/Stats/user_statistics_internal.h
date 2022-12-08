// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/user_statistics_c.h"
#include "real_time_activity_subscription.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

class Statistic
{
public:
    Statistic();

    Statistic(
        _In_ String name,
        _In_ String type,
        _In_ String value
    );

    static Result<Statistic> Deserialize(
        _In_ const JsonValue& json
    );

    const String& StatisticName() const;
    const String& StatisticType() const;
    const String& Value() const;

    void SetStatisticName(_In_ String name);
    void SetStatisticType(_In_ String type);
    void SetStatisticValue(_In_ String value);

    size_t SizeOf() const;
    char* Serialize(XblStatistic* statistic, char* buffer) const;

private:
    String m_statName;
    String m_statType;
    String m_value;
};

class ServiceConfigurationStatistic
{
public:
    ServiceConfigurationStatistic();

    ServiceConfigurationStatistic(
        _In_ String serviceConfigurationId,
        _In_ Vector<Statistic> stats
    );

    static Result<ServiceConfigurationStatistic> Deserialize(
        _In_ const JsonValue& json
    );

    const String& ServiceConfigurationId() const;
    const Vector<Statistic>& Statistics() const;
    
    void SetServiceConfigurationId(_In_ String serviceConfigId);

    size_t SizeOf() const;
    char* Serialize(XblServiceConfigurationStatistic* serviceConfigStat, char* buffer) const;

private:
    String m_serviceConfigurationId;
    Vector<Statistic> m_stats;
};

class UserStatisticsResult
{
public:
    UserStatisticsResult();

    UserStatisticsResult(
        _In_ String xboxUserId,
        _In_ Vector<ServiceConfigurationStatistic> serviceConfigStatistics
    );

    static Result<UserStatisticsResult> Deserialize(
        _In_ const JsonValue& json
    );

    const String& XboxUserId() const;
    const Vector<ServiceConfigurationStatistic>& ServiceConfigurationStatistics() const;

    void SetServiceConfigurationId(_In_ String serviceConfigId);

    size_t SizeOf() const;
    char* Serialize(char* buffer) const;
    char* Serialize(XblUserStatisticsResult* userStatResult, char* buffer) const;

private:
    String m_xboxUserId;
    Vector<ServiceConfigurationStatistic> m_serviceConfigStatistics;
};

class RequestedStatistics
{
public:
    RequestedStatistics();

    RequestedStatistics(
        _In_ String serviceConfigurationId,
        _In_ Vector<String> statistics
    );

    RequestedStatistics(
        _In_ XblRequestedStatistics requestedStatistics
    );

    const String& ServiceConfigurationId() const;
    const Vector<String>& Statistics() const;

private:
    String m_serviceConfigurationId;
    Vector<String> m_statistics;
};

class StatisticChangeEventArgs : public XblStatisticChangeEventArgs
{
public:
    StatisticChangeEventArgs(
        _In_ uint64_t xboxUserId,
        _In_ const String& serviceConfigurationId,
        _In_ String statisticName,
        _In_ String statisticType,
        _In_ String value
    ) noexcept;

    ~StatisticChangeEventArgs() = default;

private:
    String m_statisticName;
    String m_statisticType;
    String m_value;
};

class StatisticChangeSubscription : public real_time_activity::Subscription, public std::enable_shared_from_this<StatisticChangeSubscription>
{
public:
    StatisticChangeSubscription(
        _In_ uint64_t xuid,
        _In_ String scid,
        _In_ String statisticName,
        _In_ std::shared_ptr<class UserStatisticsService> statisticsService
    ) noexcept;

protected:
    void OnSubscribe(_In_ const JsonValue& data) noexcept override;
    void OnEvent(_In_ const JsonValue& data) noexcept override;

private:
    const uint64_t m_xuid;
    const String m_scid;
    const String m_statisticName;
    String m_statisticType;
    const std::weak_ptr<class UserStatisticsService> m_statisticsService;
};

class UserStatisticsService : public std::enable_shared_from_this<UserStatisticsService>
{
public:
    UserStatisticsService(
        _In_ User&& user,
        _In_ const TaskQueue& backgroundQueue,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
    ) noexcept;

    ~UserStatisticsService() noexcept;

    HRESULT GetSingleUserStatistic(
        _In_ uint64_t xuid,
        _In_ const String& serviceConfigurationId,
        _In_ const String& statisticName,
        _In_ AsyncContext<Result<UserStatisticsResult>> async
    ) const noexcept;

    HRESULT GetSingleUserStatistics(
        _In_ uint64_t xuid,
        _In_ const String& serviceConfigurationId,
        _In_ const Vector<String>& statisticNames,
        _In_ AsyncContext<Result<UserStatisticsResult>> async
    ) const noexcept;

    HRESULT GetMultipleUserStatistics(
        _In_ const Vector<uint64_t>& xuids,
        _In_ const String& serviceConfigurationId,
        _In_ const Vector<String>& statisticNames,
        _In_ AsyncContext<Result<Vector<UserStatisticsResult>>> async
    ) const noexcept;

    HRESULT GetMultipleUserStatisticsForMultipleServiceConfigurations(
        _In_ const Vector<uint64_t>& xuids,
        _In_ const Vector<RequestedStatistics>& requestedServiceConfigurationStatisticsCollection,
        _In_ AsyncContext<Result<Vector<UserStatisticsResult>>> async
    ) const noexcept;

    typedef Callback<const StatisticChangeEventArgs&> StatisticChangeHandler;

    XblFunctionContext AddStatisticChangedHandler(
        StatisticChangeHandler handler
    ) noexcept;

    void RemoveStatisticChangedHandler(
        XblFunctionContext token
    ) noexcept;

    HRESULT TrackStatistics(
        _In_ const Vector<uint64_t> xuids,
        _In_ const String& scid,
        _In_ const Vector<String>& statNames
    ) noexcept;

    HRESULT StopTrackingStatistics(
        _In_ const Vector<uint64_t> xuids,
        _In_ const String& scid,
        _In_ const Vector<String>& statNames
    ) noexcept;

    HRESULT StopTrackingUsers(
        _In_ const Vector<uint64_t>& xuids
    ) noexcept;

private:
    void HandleStatisticChanged(
        const StatisticChangeEventArgs& args
    ) const noexcept;

    void HandleRTAResync();

    static String UserStatsSubpath(
        _In_ uint64_t xuid,
        _In_ const String& serviceConfigurationId,
        _In_ Vector<String> statNames
    ) noexcept;

    User m_user;
    TaskQueue m_queue;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> m_rtaManager;

    XblFunctionContext m_resyncHandlerToken{ 0 };
    Map<XblFunctionContext, StatisticChangeHandler> m_statisticChangeHandlers;
    XblFunctionContext m_nextToken{ 1 };

    struct SubscriptionHolder
    {
        size_t refCount{ 0 };
        std::shared_ptr<StatisticChangeSubscription> subscription;
    };
    // Indexing on Xuid before StatName because the set of tracked Users is probably more
    // likely to change than the set of tracked Stats.
    Map<uint64_t, Map<std::pair<String, String>, SubscriptionHolder>> m_trackedStatsByUser;

    // Tracked stats by scid. Needed to perform RTA resync
    Map<String, Vector<String>> m_trackedStatsByScid;

    mutable std::recursive_mutex m_mutex;

    friend class StatisticChangeSubscription;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END