// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>

namespace xbox { namespace services {
    class web_socket_connection;
    class xbox_web_socket_client;
    class xbox_live_context_impl;
    enum class web_socket_connection_state;
}}

namespace xbox {
namespace services {
namespace real_time_activity {
    class real_time_activity_service_factory;
    class real_time_activity_subscription_error_event_args;
}}}

namespace xbox { namespace services { 
    /// <summary>
    /// Contains classes and enumerations that let you subscribe
    /// to real time activity information for a player statistic
    /// on Xbox Live.
    /// </summary>
    namespace real_time_activity {

/// <summary>
/// Enumeration for the message types for the Xbox Live service.
/// The value of each enum should not be changed.
/// </summary>
enum class real_time_activity_message_type
{
    /// <summary>
    /// Indicates that this is a Subscribe message.
    /// </summary>
    subscribe = 1,

    /// <summary>
    /// Indicates that this is a Unsubscribe message.
    /// </summary>
    unsubscribe = 2,

    /// <summary>
    /// Indicates that this is a ChangeEvent message.
    /// </summary>
    change_event = 3,

    /// <summary>
    /// Indicates that this is a Resync message.
    /// </summary>
    resync = 4
};

/// <summary>
/// Enumeration for the possible states of a statistic subscription request
/// to the real-time activity service.
/// </summary>
enum real_time_activity_subscription_state
{
    /// <summary>
    /// The subscription state is unknown.
    /// </summary>
    unknown,

    /// <summary>
    /// Waiting for the server to respond to the subscription request.
    /// </summary>
    pending_subscribe,

    /// <summary>
    /// Subscription confirmed.
    /// </summary>
    subscribed,

    /// <summary>
    /// Waiting for the server to respond to the unsubscribe request.
    /// </summary>
    pending_unsubscribe,

    /// <summary>
    /// Unsubscribe confirmed.
    /// </summary>
    closed
};

/// <summary>
/// Enumeration for the possible connection states of the connection
/// to the real-time activity service.
/// </summary>
enum real_time_activity_connection_state
{
    /// <summary>
    /// Currently connected to the real-time activity service.
    /// </summary>
    connected,
    
    /// <summary>
    /// Currently connecting to the real-time activity service.
    /// </summary>
    connecting,

    /// <summary>
    /// Currently disconnected from the real-time activity service.
    /// </summary>
    disconnected
};

/// <summary>
/// The base class for real time activity subscriptions.
/// </summary>
class real_time_activity_subscription
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    real_time_activity_subscription() {}

    /// <summary>
    /// Internal function
    /// </summary>
    real_time_activity_subscription(_In_ std::function<void(const real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler);

    /// <summary>The state of the subscription request.</summary>
    _XSAPIIMP virtual real_time_activity_subscription_state state() const; // TODO this shouldn't be virtual after we migrate this class
    virtual void _Set_state(_In_ real_time_activity_subscription_state newState);

    /// <summary>The resource uri for the request.</summary>
    _XSAPIIMP virtual const string_t& resource_uri() const; // TODO this shouldn't be virtual after we migrate this class

    /// <summary>The unique subscription id for the request.</summary>
    _XSAPIIMP virtual uint32_t subscription_id() const; // TODO this shouldn't be virtual after we migrate this class
    
    virtual ~real_time_activity_subscription() {}

protected:
    void set_resource_uri(_In_ string_t uri);
    void set_subscription_id(_In_ uint32_t id);

    // Callback for each subcription's initial message
    virtual void on_subscription_created(_In_ uint32_t id, _In_ const web::json::value& data);

    // Callback for each subcription's coming events
    virtual void on_event_received(_In_ const web::json::value& data);

    // Callback for each subcription's state change
    virtual void on_state_changed(_In_ real_time_activity_subscription_state state);

    real_time_activity_subscription_state m_state;
    string_t m_resourceUri;
    uint32_t m_subscriptionId;
    std::function<void(const real_time_activity_subscription_error_event_args&)> m_subscriptionErrorHandler;
    string_t m_guid;

    friend class real_time_activity_service;
};

class real_time_activity_subscription_error_event_args
{
public:
    /// <summary>
    /// The subscription this refers to
    /// </summary>
    _XSAPIIMP const real_time_activity_subscription& subscription();

    /// <summary>
    /// The error returned by the operation.
    /// </summary>
    _XSAPIIMP const std::error_code& err() const;

    /// <summary>
    /// The error message
    /// </summary>
    _XSAPIIMP const std::string& err_message() const;

    /// <summary>
    /// Internal function
    /// </summary>
    real_time_activity_subscription_error_event_args(
        real_time_activity_subscription subscription,
        std::error_code errc,
        std::string errMessage
        );

private:
    real_time_activity_subscription m_subscription;
    std::error_code m_err;
    std::string m_errorMessage;
};

/// <summary>
/// Represents a client side service that handles connections and communications with
/// the Xbox Live real-time activity service.
/// </summary>
class real_time_activity_service : public std::enable_shared_from_this<real_time_activity_service>
{
public:
    /// <summary>
    /// Starts a background task that creates and initializes a websocket connection to the Xbox Live real-time activity service.
    /// Its recommended that titles do not activate more than MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER (5) per user per title instance.
    /// Upon reaching the limit, titles will hit an assert during development that can be temporarily disabled via
    /// xbox_live_context_settings::disable_asserts_for_maximum_number_of_websockets_activated().
    /// </summary>
    _XSAPIIMP void activate();

    /// <summary>
    /// Cancels all existing subscriptions to the Xbox Live real-time activity service,
    /// unhooks from the websocket connection, and stops the background task.
    /// </summary>
    _XSAPIIMP void deactivate();

    /// <summary>
    /// Registers a handler function to receive a notification that is sent when the client service
    /// loses or gains connectivity to the real time activity service.
    /// Event handlers receive a real_time_activity_connection_state object.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    _XSAPIIMP function_context add_connection_state_change_handler(_In_ std::function<void(real_time_activity_connection_state)> handler);

    /// <summary>
    /// Unregisters an event handler for real time activity connectivity state changes.
    /// </summary>
    /// <param name="remove">The function_context object that was returned when the event handler was registered. </param>
    _XSAPIIMP void remove_connection_state_change_handler(_In_ function_context remove);

    /// <summary>
    /// Registers a handler function to receive a notification that is sent when there is an
    /// error in the real time activity service.
    /// Event handlers receive a real_time_activity_subscription_error_event_args&amp; object.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    _XSAPIIMP function_context add_subscription_error_handler(_In_ std::function<void(const real_time_activity_subscription_error_event_args&)> handler);

    /// <summary>
    /// Unregisters an event handler for real time activity error notifications.
    /// </summary>
    /// <param name="remove">The function_context object that was returned when the event handler was registered. </param>
    _XSAPIIMP void remove_subscription_error_handler(_In_ function_context remove);

    /// <summary>
    /// Registers a handler function to receive a notification that is sent when there is a
    /// resync message from the real time activity service.
    /// This message indicates that data may have been lost and to resync all data by calling
    /// corresponding REST API's
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    _XSAPIIMP function_context add_resync_handler(_In_ std::function<void()> handler);

    /// <summary>
    /// Unregisters an event handler for real time activity resync notifications.
    /// </summary>
    /// <param name="remove">The function_context object that was returned when the event handler was registered. </param>
    _XSAPIIMP void remove_resync_handler(_In_ function_context remove);

    /// <summary>
    /// Internal function
    /// </summary>
    void _Trigger_subscription_error(
        real_time_activity_subscription_error_event_args args
        );

    /// <summary>
    /// Internal function
    /// </summary>
    ~real_time_activity_service();

    /// <summary>
    /// Internal function
    /// </summary>
    real_time_activity_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    /// <summary>
    /// Internal function
    /// Add a subscription to real_time_activity_service to manage.
    /// </summary>
    xbox_live_result<void> _Add_subscription(
        _In_ std::shared_ptr<real_time_activity_subscription> subscription
        );

    /// <summary>
    /// Internal function
    /// Remove a subscription from real_time_activity_service.
    /// </summary>
    xbox_live_result<void> _Remove_subscription(
        _In_ std::shared_ptr<real_time_activity_subscription> subscription
        );

    std::shared_ptr<xbox_live_context_settings> _Xbox_live_context_settings() { return m_xboxLiveContextSettings; }

    /// <summary>
    /// Internal function
    /// Remove total count of subscription
    /// </summary>
    inline size_t _Subscription_Count()
    {
        return m_pendingSubmission.size() + m_pendingResponseSubscriptions.size() + m_subscriptions.size() + m_pendingUnsubscriptions.size();
    }

    /// <summary>
    /// Internal function
    /// </summary>
    static std::unordered_map<xsapi_internal_string, uint32_t> _Rta_activation_map();

    /// <summary>
    /// Internal function
    /// </summary>
    static std::unordered_map<xsapi_internal_string, uint32_t> _Rta_manager_activation_map();

private:
    std::unordered_map<function_context, std::function<void(real_time_activity_connection_state)>> m_connectionStateChangeHandler;
    std::unordered_map<function_context, std::function<void(real_time_activity_subscription_error_event_args)>> m_subscriptionErrorHandler;
    std::unordered_map<function_context, std::function<void()>> m_resyncHandler;

    void _Close_websocket(); 

    void complete_subscribe(
        _In_ web::json::value& message
        );

    void complete_unsubscribe(
        _In_ web::json::value& message
        );

    void handle_change_event(
        _In_ web::json::value& message
        );
    
    void trigger_resync_event();
    void trigger_connection_state_changed_event(_In_ real_time_activity_connection_state connectionState);

    void submit_subscriptions();

    std::error_code convert_rta_error_code_to_xbox_live_error_code(_In_ int32_t rtaErrorCode);

    void clear_all_subscriptions();

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    // web socket events callbacks
    void on_socket_message_received(_In_ const xsapi_internal_string& message);
    void on_socket_connection_state_change(_In_ web_socket_connection_state oldState, _In_ web_socket_connection_state newState);

    volatile long m_sequenceNumber;

    std::vector<std::shared_ptr<real_time_activity_subscription>> m_pendingSubmission;
    std::map<uint32_t, std::shared_ptr<real_time_activity_subscription>> m_pendingResponseSubscriptions;
    std::map<uint32_t, std::shared_ptr<real_time_activity_subscription>> m_subscriptions;
    std::map<uint32_t, std::shared_ptr<real_time_activity_subscription>> m_pendingUnsubscriptions;
    std::recursive_mutex m_lock;

    real_time_activity_connection_state m_connectionState;
    std::shared_ptr<xbox::services::web_socket_connection> m_webSocketConnection;

#if UWP_API || TV_API || UNIT_TEST_SERVICES
    Windows::Foundation::EventRegistrationToken m_rtaShutdownToken;
#endif

    function_context m_connectionStateChangeHandlerCounter;
    function_context m_subscriptionErrorHandlerCounter;
    function_context m_resyncHandlerCounter;

    friend xbox_live_context_impl;
    friend real_time_activity_service_factory;
};

}}}
