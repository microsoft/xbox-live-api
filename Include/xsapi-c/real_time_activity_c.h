// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Enumeration for the possible states of a statistic subscription request
/// to the real-time activity service.
/// </summary>
enum class XblRealTimeActivitySubscriptionState : uint32_t
{
    /// <summary>
    /// The subscription state is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Waiting for the server to respond to the subscription request.
    /// </summary>
    PendingSubscribe,

    /// <summary>
    /// Subscription confirmed.
    /// </summary>
    Subscribed,

    /// <summary>
    /// Waiting for the server to respond to the unsubscribe request.
    /// </summary>
    PendingUnsubscribe,

    /// <summary>
    /// Unsubscribe confirmed.
    /// </summary>
    Closed
};

/// <summary>
/// Enumeration for the possible connection states of the connection
/// to the real-time activity service.
/// </summary>
enum class XblRealTimeActivityConnectionState : uint32_t
{
    /// <summary>
    /// Currently connected to the real-time activity service.
    /// </summary>
    Connected,

    /// <summary>
    /// Currently connecting to the real-time activity service.
    /// </summary>
    Connecting,

    /// <summary>
    /// Currently disconnected from the real-time activity service.
    /// </summary>
    Disconnected
};

/// <summary>
/// Subscription handle.
/// </summary>
typedef struct XblRealTimeActivitySubscription* XblRealTimeActivitySubscriptionHandle;

/// <summary>
/// Get the state of the subscription.
/// DEPRECATED. The state of RTA subscriptions is no longer exposed publicly. XblRealTimeActivitySubscriptionState::Unknown
/// will always be returned.
/// </summary>
/// <param name="subscriptionHandle">Subscription handle returned from a subscribe API.</param>
/// <param name="state">Passes back the current state of the subscription.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblRealTimeActivitySubscriptionGetState(
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle,
    _Out_ XblRealTimeActivitySubscriptionState* state
) XBL_NOEXCEPT;

/// <summary>
/// Get the unique ID for the subscription.
/// DEPRECATED. The state of RTA subscriptions is no longer exposed publicly. This API will return a unique
/// client side ID, but it is in no way related to the ID assigned by the RTA service.
/// </summary>
/// <param name="subscriptionHandle">Subscription handle returned from a subscribe API.</param>
/// <param name="id">Passes back the ID for the subscription.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblRealTimeActivitySubscriptionGetId(
    _In_ XblRealTimeActivitySubscriptionHandle subscriptionHandle,
    _Out_ uint32_t* id
) XBL_NOEXCEPT;

/// <summary>
/// Forces XSAPI to open a WebSocket connection to the Xbox Live real-time activity service.
/// DEPRECATED. Calling this API is no longer required. The WebSocket connection will be made automatically by 
/// XSAPI as necessary. This API will be removed in a future release.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblRealTimeActivityActivate(
    _In_ XblContextHandle xboxLiveContext
) XBL_NOEXCEPT;

/// <summary>
/// Indicates that a client real-time activity session is complete. If there are no remaining real-time service
/// activations, the WebSocket connection will be cleaned up along with remaining subscriptions.
/// DEPRECATED. Calling this API is no longer required. The WebSocket connection will be cleaned up automatically
/// by XSAPI when it is no longer needed. This API will be removed in a future release.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblRealTimeActivityDeactivate(
    _In_ XblContextHandle xboxLiveContext
) XBL_NOEXCEPT;

/// <summary>
/// Handler function for when the client service loses or gains connectivity to the real time activity service.
/// </summary>
/// <param name="context">Caller context that will be passed back to the handler function.</param>
/// <param name="connectionState">State of the connection.</param>
/// <returns></returns>
typedef void CALLBACK XblRealTimeActivityConnectionStateChangeHandler(
    _In_opt_ void* context,
    _In_ XblRealTimeActivityConnectionState connectionState
);

/// <summary>
/// Registers a handler function to receive a notification that is sent when the client service
/// loses or gains connectivity to the real time activity service.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context that will be passed back to the handler function.</param>
/// <returns>An XblFunctionContext object that can be used to unregister the event handler.</returns>
STDAPI_(XblFunctionContext) XblRealTimeActivityAddConnectionStateChangeHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblRealTimeActivityConnectionStateChangeHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for real time activity connectivity state changes.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblRealTimeActivityRemoveConnectionStateChangeHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

/// <summary>
/// Handler function for when there is an error in the real time activity service.
/// </summary>
/// <param name="context">Caller context that will be passed back to the handler function.</param>
/// <param name="subscription">Subscription handle.</param>
/// <param name="subscriptionError">The HRESULT error code.</param>
/// <returns></returns>
typedef void CALLBACK XblRealTimeActivitySubscriptionErrorHandler(
    _In_opt_ void* context,
    _In_ XblRealTimeActivitySubscriptionHandle subscription,
    _In_ HRESULT subscriptionError
);

/// <summary>
/// Registers a handler function to receive a notification that is sent when there is an
/// error in the real time activity service.
/// DEPRECATED. RTA service errors will now be handled by XSAPI internally and callback will no longer be invoked.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context that will be passed back to the handler function.</param>
/// <returns>A XblFunctionContext object that can be used to unregister the event handler.</returns>
STDAPI_XBL_DEPRECATED_(XblFunctionContext) XblRealTimeActivityAddSubscriptionErrorHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblRealTimeActivitySubscriptionErrorHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for real time activity error notifications.
/// DEPRECATED. RTA service errors will now be handled by XSAPI internally and callback will no longer be invoked.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblRealTimeActivityRemoveSubscriptionErrorHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

/// <summary>
/// Handler function for when there is a resync message from the real time activity service.
/// </summary>
/// <param name="context">Caller context that will be passed back to the handler function.</param>
/// <returns></returns>
typedef void CALLBACK XblRealTimeActivityResyncHandler(
    _In_opt_ void* context
);

/// <summary>
/// Registers a handler function to receive a notification that is sent when there is a
/// resync message from the real time activity service.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <param name="handler">The callback function that receives notifications.</param>
/// <param name="context">Caller context that will be passed back to the handler function.</param>
/// <returns>A XblFunctionContext object that can be used to unregister the event handler.</returns>
/// <remarks>
/// This message indicates that data may have been lost and to resync all data by calling
/// corresponding REST API's. Wherever possible, XSAPI will automatically resync the subscription and
/// invoke the corresponding handler. For multiplayer session changed subscriptions, titles must resync
/// their own sessions.
/// </remarks>
STDAPI_(XblFunctionContext) XblRealTimeActivityAddResyncHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblRealTimeActivityResyncHandler* handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for real time activity resync notifications.
/// </summary>
/// <param name="xboxLiveContext">Xbox Live context handle.</param>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblRealTimeActivityRemoveResyncHandler(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

}