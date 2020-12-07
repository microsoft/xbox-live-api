// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-c/user_statistics_c.h"
#include "xsapi-cpp/real_time_activity.h"

namespace xbox { namespace services {
    class xbox_live_context;
    /// <summary>
    /// Contains classes and enumerations that let you retrieve
    /// player statistics from Xbox Live.
    /// </summary>
    namespace user_statistics {
        
/// <summary>
/// Contains information about a user statistic.
/// </summary>
class statistic
{
public:
    /// <summary>
    /// The name of the statistic.
    /// </summary>
    inline const string_t& statistic_name() const;

    /// <summary>
    /// The type of the statistic.
    /// </summary>
    inline const string_t& statistic_type() const;

    /// <summary>
    /// The value of the statistic.
    /// </summary>
    inline const string_t& value() const;

    /// <summary>
    /// Internal functions
    /// </summary>
    inline statistic(const XblStatistic& statistic);
    inline statistic(const string_t& name, const string_t& type, const string_t& value);

private:
    string_t m_statName;
    string_t m_statType;
    string_t m_value;
};

/// <summary>
/// Contains statistical information from a service configuration.
/// </summary>
class service_configuration_statistic
{
public:    
    /// <summary>
    /// The service configuration ID (SCID) associated with the leaderboard.
    /// </summary>
    inline const string_t& service_configuration_id() const;

    /// <summary>
    /// A collection of statistics used in leaderboards.
    /// </summary>
    inline const std::vector<statistic>& statistics() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline service_configuration_statistic(const XblServiceConfigurationStatistic& serviceConfigurationStatistic);

private:
    string_t m_serviceConfigurationId;
    std::vector<statistic> m_statistics;
};

/// <summary>
/// Represents the results of a user statistic query.
/// </summary>
class user_statistics_result
{
public:
    /// <summary>
    /// The Xbox User ID for the user in a statistic.
    /// </summary>
    inline const string_t& xbox_user_id() const;

    /// <summary>
    /// A collection of statistics from a service configuration.
    /// </summary>        
    inline const std::vector<service_configuration_statistic>& service_configuration_statistics() const;

    /// <summary>
    /// Internal functions
    /// </summary>
    inline user_statistics_result() = default;
    inline user_statistics_result(const XblUserStatisticsResult& userStatisticsResult);

private:
    string_t m_xboxUserId;
    std::vector<service_configuration_statistic> m_serviceConfigurationStatistics;
};

/// <summary>
/// Contains requested statistics.
/// </summary>
class requested_statistics
{
public:
    /// <summary>
    /// Constructor for an RequestedStatistics object.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statistics">A collection of statistics.</param>
    inline requested_statistics(
        _In_ string_t serviceConfigurationId,
        _In_ const std::vector<string_t>& statistics
    );

    /// <summary>
    /// Copy Constructor for an RequestedStatistics object.
    /// </summary>
    inline requested_statistics(_In_ const requested_statistics& other);

    /// <summary>
    /// The service configuration ID in use.
    /// </summary>
    inline const string_t& service_configuration_id() const;

    /// <summary>
    /// A collection of statistics.
    /// </summary>
    inline const std::vector<string_t>& statistics() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline const XblRequestedStatistics& _requested_statistics() const;

private:
    XblRequestedStatistics m_requestedStatistics;
    string_t m_scid;
    std::vector<string_t> m_statistics;
    UTF8StringArrayRef m_statisticsC;
};

/// <summary>
/// Contains information about a change to a subscribed statistic.
/// </summary>
class statistic_change_event_args
{
public:
    /// <summary>
    /// The Xbox user ID used to create the subscription.
    /// </summary> 
    inline const string_t& xbox_user_id() const;

    /// <summary>
    /// The service configuration ID used to create the subscription.
    /// </summary>
    inline const string_t& service_configuration_id() const;

    /// <summary>
    /// The statistic with an updated value.
    /// </summary>
    inline const statistic& latest_statistic() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline statistic_change_event_args(const XblStatisticChangeEventArgs& statisticEventArgs);

private:
    string_t m_xboxUserId;
    string_t m_serviceConfigurationId;
    statistic m_statistic;
};

/// <summary>
/// Handles notification when the state of a statistic subscription changes.
/// </summary>
class statistic_change_subscription : public xbox::services::real_time_activity::real_time_activity_subscription
{
public:
    /// <summary>
    /// The Xbox user ID used to create the subscription.
    /// </summary>
    inline const string_t& xbox_user_id() const;
    
    /// <summary>
    /// The service config ID used to create the subscription.
    /// </summary>
    inline const string_t& service_configuration_id() const;

    /// <summary>
    /// The statistic the subscription is for.
    /// </summary>
    inline const xbox::services::user_statistics::statistic& statistic() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline statistic_change_subscription(
        _In_ string_t xboxUserId,
        _In_ string_t serviceConfigurationId,
        _In_ xbox::services::user_statistics::statistic newStat,
        _In_ XblRealTimeActivitySubscriptionHandle handle
    );

private:
    string_t m_xboxUserId;
    string_t m_serviceConfigurationId;
    xbox::services::user_statistics::statistic m_statistic;

    friend class user_statistics_service;
};

/// <summary>
/// Represents an endpoint that you can use to access the user statistic service.
/// </summary>
class user_statistics_service
{
public:
    /// <summary>
    /// Get a specified statistic for a specified user.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player to get statistics for.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticName">The name of the statistic to return.</param>
    /// <returns>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The result of the asynchronous operation is the requested statistic.
    /// </returns>
    /// <remarks>
    /// Calls V1 GET /users/xuid({xuid})/scids/{scid}/stats/{statname1}
    /// </remarks>
    inline pplx::task<xbox_live_result<user_statistics_result>> get_single_user_statistics(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& statisticName
        );

    /// <summary>
    /// Get specified statistics for a single user.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player to get statistics for.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticNames">A collection of statistic names to lookup.</param>
    /// <returns>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The result is a user_statistics_result object that contains the requested statistics.
    /// Only statistics with values are returned. For example, if you ask for 3 statistic names and only 2 have values,
    /// only 2 statistics are returned by the service.
    /// </returns>
    /// <remarks>Calls V1 GET /users/xuid({xuid})/scids/{scid}/stats/{statname1},...,{statnameN}</remarks>
    inline pplx::task<xbox_live_result<user_statistics_result>> get_single_user_statistics(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const std::vector<string_t>& statisticNames
        );

    /// <summary>
    /// Get statistics for multiple users.
    /// </summary>
    /// <param name="xboxUserIds">A list of the user Xbox user IDs to get stats for.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticNames">A collection of statistic names to lookup.</param>
    /// <returns>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The result is a collection of user_statistics_result objects that contain the requested statistics.
    /// Only statistics with values are returned. For example, if you ask for 3 statistic names and only 2 have values,
    /// only 2 statistics are returned by the service.
    /// </returns>
    /// <remarks>Calls V1 POST /batch</remarks>
    inline pplx::task<xbox_live_result<std::vector<user_statistics_result>>> get_multiple_user_statistics(
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const string_t& serviceConfigurationId,
        _In_ std::vector<string_t>& statisticNames
        );

    /// <summary>
    /// Get statistics for users across different Service configurations.
    /// </summary>
    /// <param name="xboxUserIds">A list of the user Xbox user ID to get stats for.</param>
    /// <param name="requestedServiceConfigurationStatisticsCollection">A list of the service config IDs and its associated array of statistics.</param>
    /// <returns>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// The result is a collection of user_statistics_result objects that contain the requested statistics.
    /// Only statistics with values are returned. For example, if you ask for 3 statistic names and only 2 have values,
    /// only 2 statistics are returned by the service.
    /// </returns>
    /// <remarks>Calls V1 POST /batch</remarks>
    inline pplx::task<xbox_live_result<std::vector<user_statistics_result>>> get_multiple_user_statistics_for_multiple_service_configurations(
        _In_ const std::vector<string_t>& xboxUserIds,
        _In_ const std::vector<requested_statistics>& requestedServiceConfigurationStatisticsCollection
        );

    /// <summary>
    /// Subscribes to statistic update notifications via the StatisticChanged event.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the player requesting the subscription.</param>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statisticName">The name of the statistic to subscribe to.</param>
    /// <returns>A statistic_change_subscription object that contains the state of the subscription. 
    /// You can register an event handler for statistic changes by calling set_statistic_changed_handler().
    /// </returns>
    inline xbox_live_result<std::shared_ptr<statistic_change_subscription>> subscribe_to_statistic_change(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& statisticName
        );

    /// <summary>
    /// Unsubscribes a previously created statistic change subscription.
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe</param>
    inline xbox_live_result<void> unsubscribe_from_statistic_change(
        _In_ std::shared_ptr<statistic_change_subscription> subscription
        );

    /// <summary>
    /// Registers an event handler for statistic change notifications.
    /// Event handlers receive a statistic_change_event_args object.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    inline function_context add_statistic_changed_handler(_In_ std::function<void(statistic_change_event_args)> handler);

    /// <summary>
    /// Unregisters an event handler for statistic change notifications.
    /// </summary>
    /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
    /// <param name="handler">The callback function that receives notifications.</param>
    inline void remove_statistic_changed_handler(_In_ function_context context);

    inline user_statistics_service(const user_statistics_service& other);
    inline user_statistics_service& operator=(user_statistics_service other);
    inline ~user_statistics_service();

private:
    inline user_statistics_service(_In_ XblContextHandle contextHandle);

    struct HandlerContext;

    XblContextHandle m_xblContext;

    friend xbox_live_context;
};

}}}

#include "impl/user_statistics.hpp"
