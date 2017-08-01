// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/real_time_activity.h"

namespace xbox { namespace services {
    class xbox_live_context_impl;
    /// <summary>
    /// Contains classes and enumerations that let you retrieve
    /// player statistics from Xbox Live.
    /// </summary>
    namespace user_statistics {

class user_statistics_service_impl;

/// <summary>
/// Contains information about a user statistic.
/// </summary>
class statistic
{
public:

    /// <summary>
    /// Internal function
    /// </summary>
    statistic();

    /// <summary>
    /// Internal function
    /// </summary>
    statistic(
        _In_ string_t name,
        _In_ string_t type,
        _In_ string_t value
        );

    /// <summary>
    /// The name of the statistic.
    /// </summary>
    _XSAPIIMP const string_t& statistic_name() const;

    /// <summary>
    /// The type of the statistic.
    /// </summary>
    _XSAPIIMP const string_t& statistic_type() const;

    /// <summary>
    /// The value of the statistic.
    /// </summary>
    _XSAPIIMP const string_t& value() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_statistic_name(_In_ string_t name);
    
    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_statistic_type(_In_ string_t type);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_statistic_value(_In_ string_t value);

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<statistic> _Deserialize(_In_ const web::json::value& json);

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
    /// Internal function
    /// </summary>
    service_configuration_statistic();

    /// <summary>
    /// Internal function
    /// </summary>
    service_configuration_statistic(
        _In_ string_t serviceConfigurationId,
        _In_ std::vector<statistic> stats
        );

    /// <summary>
    /// The service configuration ID (SCID) associated with the leaderboard.
    /// </summary>
    _XSAPIIMP const string_t& service_configuration_id() const;

    /// <summary>
    /// A collection of statistics used in leaderboards.
    /// </summary>        
    _XSAPIIMP const std::vector<statistic>& statistics() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<service_configuration_statistic> _Deserialize(_In_ const web::json::value& json);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_input_service_configuration_id(string_t serviceConfigId);

private:
    string_t m_serviceConfigurationId;
    std::vector<statistic> m_stats;
};

/// <summary>
/// Represents the results of a user statistic query.
/// </summary>
class user_statistics_result
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    user_statistics_result();

    /// <summary>
    /// Internal function
    /// </summary>
    user_statistics_result(
        _In_ string_t xboxUserId,
        _In_ std::vector<service_configuration_statistic> serviceConfigStatistics
        );

    /// <summary>
    /// The Xbox User ID for the user in a statistic.
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// A collection of statistics from a service configuration.
    /// </summary>        
    _XSAPIIMP const std::vector<service_configuration_statistic>& service_configuration_statistics() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_input_service_configuration_id(string_t serviceConfigId);

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<user_statistics_result> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_xboxUserId;
    std::vector<service_configuration_statistic> m_serviceConfigStatistics;
};

/// <summary>
/// Contains requested statistics.
/// </summary>
class requested_statistics
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    requested_statistics();
    
    /// <summary>
    /// Constructor for an RequestedStatistics object.
    /// </summary>
    /// <param name="serviceConfigurationId">The service configuration ID (SCID) of the title</param>
    /// <param name="statistics">A collection of statistics.</param>
    _XSAPIIMP requested_statistics(
        _In_ string_t m_serviceConfigurationId,
        _In_ std::vector<string_t> m_statistics
        );

    /// <summary>
    /// The service configuration ID in use.
    /// </summary>
    _XSAPIIMP const string_t& service_configuration_id() const;

    /// <summary>
    /// A collection of statistics.
    /// </summary>        
    _XSAPIIMP const std::vector<string_t>& statistics() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<requested_statistics> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_serviceConfigurationId;
    std::vector<string_t> m_statistics;
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
    _XSAPIIMP const string_t& xbox_user_id() const;

    /// <summary>
    /// The service configuration ID used to create the subscription.
    /// </summary>
    _XSAPIIMP const string_t& service_configuration_id() const;

    /// <summary>
    /// The statistic with an updated value.
    /// </summary>
    _XSAPIIMP const statistic& latest_statistic() const;

    /// <summary>
    /// Internal function
    /// </summary>
    statistic_change_event_args();

    /// <summary>
    /// Internal function
    /// </summary>
    statistic_change_event_args(
        _In_ string_t xboxUserId,
        _In_ string_t serviceConfigurationId,
        _In_ statistic latestStatistic
        );

private:
    string_t m_xboxUserId;
    string_t m_serviceConfigurationId;
    statistic m_latestStatistic;
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
    _XSAPIIMP const string_t& xbox_user_id() const;
    
    /// <summary>
    /// The service config ID used to create the subscription.
    /// </summary>
    _XSAPIIMP const string_t& service_configuration_id() const;

    /// <summary>
    /// The statistic the subscription is for.
    /// </summary>
    _XSAPIIMP const xbox::services::user_statistics::statistic& statistic() const;

    /// <summary>
    /// Internal function
    /// </summary>
    statistic_change_subscription(
        _In_ string_t xboxUserId,
        _In_ string_t serviceConfigurationId,
        _In_ xbox::services::user_statistics::statistic newStat,
        _In_ std::function<void(const statistic_change_event_args&)> handler,
        _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
        );
   
    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_statistic_change_subscription_handler(_In_ std::function<void(const statistic_change_event_args&)> statisticChangeHandler);

protected:
    void on_subscription_created(_In_ uint32_t id, _In_ const web::json::value& data) override;
    void on_event_received(_In_ const web::json::value& data) override;

private:
    std::function<void(const statistic_change_event_args&)> m_statisticChangeHandler;
    string_t m_xboxUserId;
    string_t m_serviceConfigurationId;
    xbox::services::user_statistics::statistic m_statistic;
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
    _XSAPIIMP pplx::task<xbox_live_result<user_statistics_result>> get_single_user_statistics(
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
    _XSAPIIMP pplx::task<xbox_live_result<user_statistics_result>> get_single_user_statistics(
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
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<user_statistics_result>>> get_multiple_user_statistics(
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
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<user_statistics_result>>> get_multiple_user_statistics_for_multiple_service_configurations(
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
    _XSAPIIMP xbox_live_result<std::shared_ptr<statistic_change_subscription>> subscribe_to_statistic_change(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& statisticName
        );

    /// <summary>
    /// Unsubscribes a previously created statistic change subscription.
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe</param>
    _XSAPIIMP xbox_live_result<void> unsubscribe_from_statistic_change(
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
    _XSAPIIMP function_context add_statistic_changed_handler(_In_ std::function<void(statistic_change_event_args)> handler);

    /// <summary>
    /// Unregisters an event handler for statistic change notifications.
    /// </summary>
    /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
    /// <param name="handler">The callback function that receives notifications.</param>
    _XSAPIIMP void remove_statistic_changed_handler(_In_ function_context context);

    std::shared_ptr<xbox_live_context_settings> _Xbox_live_context_settings() { return m_xboxLiveContextSettings; }

private:
    user_statistics_service();

    user_statistics_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService
        );

    static string_t user_stats_sub_path(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& serviceConfigurationId,
        _In_ std::vector<string_t> statNames
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    std::shared_ptr<user_statistics_service_impl> m_userStatisticsServiceImpl;

    friend xbox_live_context_impl;
};

}}}
