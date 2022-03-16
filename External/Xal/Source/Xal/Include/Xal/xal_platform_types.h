#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <Xal/xal_types.h>

extern "C"
{

//-----------------------------------------------------------------------------
// Types for for platform hooks
//-----------------------------------------------------------------------------

/// <summary>
/// Handle to an operation Xal requested the client to perform.
/// </summary>
typedef struct XalPlatformOperationToken* XalPlatformOperation;

/// <summary>
/// Enum defining the results of a client operation.
/// </summary>
typedef enum XalPlatformOperationResult
{
    /// <summary>
    /// Successful client operation.
    /// </summary>
    XalPlatformOperationResult_Success = 0,
    /// <summary>
    /// Failed client operation.
    /// </summary>
    XalPlatformOperationResult_Failure = 1,
    /// <summary>
    /// Canceled client operation.
    /// </summary>
    XalPlatformOperationResult_Canceled = 2,
} XalPlatformOperationResult;

/// <summary>
/// Enum defining the possible reasons a platform web event might occur.
/// </summary>
typedef enum XalShowUrlType
{
    /// <summary>
    /// The client should show the URL in a shared system browser if
    /// one is present, otherwise the client should use an embedded browser.
    /// </summary>
    XalShowUrlType_Normal = 0,
    /// <summary>
    /// This case is deprecated and no longer used. Cookie removal is now
    /// signaled exclusively using
    /// XalShowUrlType_CookieRemovalSkipIfSharedCredentials.
    /// </summary>
    /// <remarks>
    /// Formerly, this value indicated that the browser was being raised for the
    /// purposes of deleting cookies from both shared and embedded browsers.
    /// </remarks>
    XalShowUrlType_CookieRemoval_DEPRECATED = 1,
    /// <summary>
    /// The browser is being raised for the purposes of deleting
    /// cookies. If the client is using a shared system browser, this call
    /// should be ignored and the client should immediately call
    /// XalPlatformWebShowUrlComplete passing in success, and forwarding the
    /// received final URL back into Xal. If an embedded browser is being used,
    /// the cookies should be cleared without showing UI if possible. If silent
    /// cookie-clearing is impossible, the URL should be loaded as normal.
    /// </summary>
    XalShowUrlType_CookieRemovalSkipIfSharedCredentials = 2,
    /// <summary>
    /// This is a web flow which does not rely on cookies. The client
    /// may use a shared system browser or an embedded browser depending on
    /// whichever browser would give the best user experience. If headers are
    /// required for the web request, this will be the show type asked for. If
    /// this is the case, an embedded browser might be required so those
    /// request headers can be set.
    /// </summary>
    XalShowUrlType_NonAuthFlow = 3,
} XalShowUrlType;

/// <summary>
/// The userIdentifier value Xal will use for data that is not specific to a
/// single user.
/// </summary>
uint32_t const XAL_NO_USER_IDENTIFIER = static_cast<uint32_t>(-1);

//-----------------------------------------------------------------------------
// Memory (optional on all platforms)

/// <summary>
/// A callback invoked every time a new memory buffer must be dynamically
/// allocated by the library. This callback is optionally installed by calling
/// XalMemSetFunctions.
///
/// The callback must allocate and return a pointer to a contiguous block of
/// memory of the specified size that will remain valid until the app's
/// corresponding XalMemFreeFunc callback is invoked to release it.
///
/// Every non-null pointer returned by this method will be subsequently passed
/// to the corresponding XalMemFreeFunc callback once the memory is no longer
/// needed.
/// </summary>
/// <param name="size">The size of the allocation to be made. This value will
/// never be zero.</param>
/// <param name="tag">An opaque identifier representing the internal category
/// of memory being allocated.</param>
/// <returns>A pointer to an allocated block of memory of the specified size, or
/// a null pointer if allocation failed.</returns>
typedef _Ret_maybenull_ _Post_writable_byte_size_(size) void* XalMemAllocFunc(size_t size, uint32_t tag);

/// <summary>
/// A callback invoked every time a previously allocated memory buffer is no
/// longer needed by the library and can be freed. This callback is optionally
/// installed by calling XalMemSetFunctions.
///
/// The callback is invoked whenever the library has finished using a memory
/// buffer previously returned by the app's corresponding XalMemAllocFunc such
/// that the application can free the memory buffer.
/// </summary>
/// <param name="pointer">The pointer to the memory buffer previously allocated.
/// This value will never be a null pointer.</param>
/// <param name="tag">An opaque identifier representing the internal category
/// of memory being allocated.</param>
/// <returns></returns>
typedef void XalMemFreeFunc(_In_ _Post_invalid_ void* pointer, uint32_t tag);

//-----------------------------------------------------------------------------
// Web view (ignored on OneCore platforms, optional on iOS and Android)

/// <summary>
/// Show url event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userIdentifier">The user identifier that was passed to Xal when
/// the user was added.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="startUrl">The url to navigate to.</param>
/// <param name="finalUrl">The url that indicates the web flow is
/// complete.</param>
/// <param name="showUrlType">Enum indicating the type of flow occurring. This
/// flag dictates what correct behavior for the client is expected to be.
/// </param>
/// <param name="requestHeaderCount">The number of request headers present in
/// the requestHeaders array.</param>
/// <param name="requestHeaders">Request headers that must be added to the web
/// session request for the best user experience.</param>
/// <returns></returns>
/// <remarks>
/// This event is raised when Xal needs to show a web flow to the user, the
/// client should navigate to startUrl and wait for a redirect to finalUrl. Once
/// the redirect to finalUrl occurs the client should close the web ui and invoke
/// XalPlatformWebShowUrlComplete passing the full redirect url.
///
/// This handler is optional for Android and iOS platforms. If it is not set on
/// these platforms, Xal will provide default browser behavior. On UWP, and XDK
/// platforms, this handler is ignored.
///
/// Depending on the value of showUrlType and the type of browser the client is
/// using, different behavior is expected. See the definition for
/// XalShowUrlType for more information.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformWebShowUrlEventHandler2)(
    _In_opt_ void* context,
    _In_ uint32_t userIdentifier,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* startUrl,
    _In_z_ char const* finalUrl,
    _In_ XalShowUrlType showUrlType,
    _In_ uint32_t requestHeaderCount,
    _In_reads_(requestHeaderCount) XalHttpHeader const* requestHeaders
);

//-----------------------------------------------------------------------------
// Storage (ignored on OneCore platforms, optional on iOS and Android)

/// <summary>
/// Write to storage event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userIdentifier">The user identifier that was passed to Xal when
/// the user was added.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="key">Identifies the data being written.</param>
/// <param name="dataSize">The size (in bytes) of the data.</param>
/// <param name="data">The data to write.</param>
/// <returns></returns>
/// <remarks>
/// This event is raised when Xal needs to write data to storage, the client
/// should write the data and when done invoke XalPlatformStorageWriteComplete.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformStorageWriteEventHandler2)(
    _In_opt_ void* context,
    _In_ uint32_t userIdentifier,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key,
    _In_ size_t dataSize,
    _In_reads_bytes_(dataSize) void const* data
);

/// <summary>
/// Read from storage event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userIdentifier">The user identifier that was passed to Xal when
/// the user was added.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="key">Identifies the data being read.</param>
/// <returns></returns>
/// <remarks>
/// This event is raised when Xal needs to read data from storage, the client
/// should read the data and when done invoke XalPlatformStorageReadComplete.
/// If the key is not found, the client should complete with
/// XalPlatformOperationResult_Success and no data.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformStorageReadEventHandler2)(
    _In_opt_ void* context,
    _In_ uint32_t userIdentifier,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key
);

/// <summary>
/// Clear from storage event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userIdentifier">The user identifier that was passed to Xal when
/// the user was added.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="key">Identifies the data being cleared.</param>
/// <returns></returns>
/// <remarks>
/// This event is raised when Xal needs to clear data from storage, the client
/// should clear the data and when done invoke XalPlatformStorageClearComplete.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformStorageClearEventHandler2)(
    _In_opt_ void* context,
    _In_ uint32_t userIdentifier,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key
);

/// <summary>
/// Struct encapsulating the storage event handlers.
/// </summary>
/// <remarks>
/// All 3 handlers must be set at the same time.
/// </remarks>
typedef struct XalPlatformStorageEventHandlers2
{
    /// <summary>
    /// Write to storage handler.
    /// </summary>
    XalPlatformStorageWriteEventHandler2* write;

    /// <summary>
    /// Read from storage handler.
    /// </summary>
    XalPlatformStorageReadEventHandler2* read;

    /// <summary>
    /// Clear from storage handler.
    /// </summary>
    XalPlatformStorageClearEventHandler2* clear;

    /// <summary>
    /// Optional pointer to data used by the event handlers.
    /// </summary>
    void* context;
} XalPlatformStorageEventHandlers2;

//-----------------------------------------------------------------------------
// Remote Connect (only used in generic mode, when configured for it)

/// <summary>
/// Show prompt for remote connect authentication event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userIdentifier">The user identifier that was passed to Xal when
/// the user was added.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="url">The url to show in the prompt.</param>
/// <param name="code">The code to show in the prompt.</param>
/// <returns></returns>
/// <remarks>
/// This event is raised when Xal needs to prompt the user to perform the
/// remote connect authentication process.
/// The prompt ui should be displayed until
/// XalPlatformRemoteConnectClosePromptEventHandler is called or it is dismissed
/// by the user.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformRemoteConnectShowPromptEventHandler2)(
    _In_opt_ void* context,
    _In_ uint32_t userIdentifier,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* url,
    _In_z_ char const* code
);

/// <summary>
/// Close prompt for remote authentication event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userIdentifier">The user identifier that was passed to Xal when
/// the user was added.</param>
/// <param name="operation">The handle for this operation.</param>
/// <returns></returns>
/// <remarks>
/// This event is raised when the remote connect authentication process has been
/// completed and the prompt is no longer necessary.
/// This event will always be called with the same operation as a previous
/// XalPlatformRemoteConnectShowPromptEventHandler event.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformRemoteConnectClosePromptEventHandler2)(
    _In_opt_ void* context,
    _In_ uint32_t userIdentifier,
    _In_ XalPlatformOperation operation
);

/// <summary>
/// Struct encapsulating the remote connect event handlers.
/// </summary>
/// <remarks>
/// Both handlers must be set at the same time.
/// </remarks>
typedef struct XalPlatformRemoteConnectEventHandlers2
{
    /// <summary>
    /// Show the prompt handler.
    /// </summary>
    XalPlatformRemoteConnectShowPromptEventHandler2* show;

    /// <summary>
    /// Close the prompt handler.
    /// </summary>
    XalPlatformRemoteConnectClosePromptEventHandler2* close;

    /// <summary>
    /// Optional pointer to data used by the event handlers.
    /// </summary>
    void* context;
} XalPlatformRemoteConnectEventHandlers2;

//------------------------------------------------------------------------------
// Crypto (only used in generic mode)

/// <summary>
/// Struct representing a UUID.
/// </summary>
/// <remarks>
/// UUIDs should conform to RFC4122 (https://tools.ietf.org/html/rfc4122).
/// </remarks>
typedef struct XalUuid
{
    /// <summary>
    /// Stores the time_low field.
    /// </summary>
    uint32_t data1;
    /// <summary>
    /// Stores the time_mid field.
    /// </summary>
    uint16_t data2;
    /// <summary>
    /// Stores the time_hi_and_version field.
    /// </summary>
    uint16_t data3;
    /// <summary>
    /// Stores the clock_seq_hi_and_reserved field [0], clock_seq_low field [1], and node fields [2-7].
    /// </summary>
    uint8_t data4[8];
} XalUuid;

/// <summary>
/// Generate UUID callback.
/// </summary>
/// <param name="context">Optional pointer to data used by the callback.
/// </param>
/// <param name="newUuid">The new UUID.</param>
/// <returns>The results of the client operation.</returns>
/// <remarks>
/// This callback is invoked when Xal needs a new UUID.
///
/// This callback will be invoked on a thread Xal is running on.
/// </remarks>
typedef XalPlatformOperationResult (XalPlatformCryptoGenerateUuidCallback)(
    _In_opt_ void* context,
    _Out_ XalUuid* newUuid
);

/// <summary>
/// Generate random bytes callback.
/// </summary>
/// <param name="context">Optional pointer to data used by the callback.
/// </param>
/// <param name="bufferSize">The number of random bytes needed.</param>
/// <param name="buffer">The buffer the random data should be written to.
/// </param>
/// <returns>The results of the client operation.</returns>
/// <remarks>
/// This callback is invoked when Xal needs random data, which should be
/// generated using the platform cryptographic RNG.
///
/// This callback will be invoked on a thread Xal is running on.
/// </remarks>
typedef XalPlatformOperationResult (XalPlatformCryptoGenerateRandomBytesCallback)(
    _In_opt_ void* context,
    _In_ size_t bufferSize,
    _Out_writes_bytes_(bufferSize) uint8_t* buffer
);

/// <summary>
/// Struct encapsulating the crypto callbacks.
/// </summary>
/// <remarks>
/// Both callbacks must be set at the same time.
/// </remarks>
typedef struct XalPlatformCryptoCallbacks
{
    /// <summary>
    /// The UUID callback to be invoked.
    /// </summary>
    XalPlatformCryptoGenerateUuidCallback* uuid;
    /// <summary>
    /// The random bytes callback to be invoked.
    /// </summary>
    XalPlatformCryptoGenerateRandomBytesCallback* random;
    /// <summary>
    /// Optional pointer to data used by the callback.
    /// </summary>
    void* context;
} XalPlatformCryptoCallbacks;

//------------------------------------------------------------------------------
// Date & Time (only used in generic mode)

/// <summary>
/// Generate Unix timestamp callback.
/// </summary>
/// <param name="context">Optional pointer to data used by the callback.
/// </param>
/// <param name="secondsFromUnixEpoch">Number of seconds from the Unix Epoch
/// (1970-01-01T00:00:00Z) in the UTC timezone.</param>
/// <param name="subsecondsMilliseconds">The fraction of second, in
/// milliseconds.</param>
/// <returns>The results of the client operation.</returns>
/// <remarks>
/// This callback is invoked when Xal needs a timestamp.
/// The subsecond value is optional and can be set to 0.
///
/// This callback will be invoked on a thread Xal is running on.
/// </remarks>
typedef XalPlatformOperationResult (XalPlatformDateTimeGetUtcTimestampCallback)(
    _In_opt_ void* context,
    _Out_ int64_t* secondsFromUnixEpoch,
    _Out_ uint32_t* subsecondMilliseconds
);

/// <summary>
/// Convert a Unix timestamp into date/time components.
/// </summary>
/// <param name="context">Optional pointer to data used by the callback.
/// </param>
/// <param name="secondsFromUnixEpoch">Number of seconds from the Unix Epoch
/// (1970-01-01T00:00:00Z) in the UTC timezone.</param>
/// <param name="components">The resulting date/time components.</param>
/// <returns>The results of the client operation.</returns>
/// <remarks>
/// This callback is invoked when Xal needs to convert a timestamp into a date,
/// the date produced should be in the UTC timezone.
///
/// This callback will be invoked on a thread Xal is running on.
/// </remarks>
typedef XalPlatformOperationResult (XalPlatformDateTimeTimestampToComponentsCallback)(
    _In_opt_ void* context,
    _In_ int64_t secondsFromUnixEpoch,
    _Out_ XalTimestampComponents* components
);

/// <summary>
/// Convert a date/time components into a Unix timestamp.
/// </summary>
/// <param name="context">Optional pointer to data used by the callback.
/// </param>
/// <param name="components">The date/time components.</param>
/// <param name="secondsFromUnixEpoch">Number of seconds from the Unix Epoch
/// (1970-01-01T00:00:00Z) in the UTC timezone.</param>
/// <returns>The results of the client operation.</returns>
/// <remarks>
/// This callback is invoked when Xal needs to convert a date into a timestamp,
/// the date is always in the UTC timezone.
///
/// This callback will be invoked on a thread Xal is running on.
/// </remarks>
typedef XalPlatformOperationResult(XalPlatformDateTimeComponentsToTimestampCallback)(
    _In_opt_ void* context,
    _In_ XalTimestampComponents const* components,
    _Out_ int64_t* secondsFromUnixEpoch
);

/// <summary>
/// Struct encapsulating the date/time callbacks.
/// </summary>
/// <remarks>
/// All 3 callbacks must be set at the same time.
/// </remarks>
struct XalPlatformDateTimeCallbacks
{
    /// <summary>
    /// The get utc timestamp callback to be invoked.
    /// </summary>
    XalPlatformDateTimeGetUtcTimestampCallback* timestamp;
    /// <summary>
    /// The timestamp to components callback to be invoked.
    /// </summary>
    XalPlatformDateTimeTimestampToComponentsCallback* timestampToComponents;
    /// <summary>
    /// The components to timestamp callback to be invoked.
    /// </summary>
    XalPlatformDateTimeComponentsToTimestampCallback* componentsToTimestamp;
    /// <summary>
    /// Optional pointer to data used by the callback.
    /// </summary>
    void* context;
};

}

// Back compat handlers
#if XAL_ENABLE_BACK_COMPAT_SHIMS
//-----------------------------------------------------------------------------
// Web view (ignored on OneCore platforms, optional on iOS and Android)

/// <summary>
/// Show url event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userContext">Always null.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="startUrl">The url to navigate to.</param>
/// <param name="finalUrl">The url that indicates the web flow is
/// complete.</param>
/// <param name="showUrlType">Enum indicating the type of flow occurring. This
/// flag dictates what correct behavior for the client is expected to be.
/// </param>
/// <returns></returns>
/// <remarks>
/// This version of the handler is deprecated, please switch to
/// XalPlatformWebShowUrlEventHandler2.
///
/// This event is raised when Xal needs to show a web flow to the user, the
/// client should navigate to startUrl and wait for a redirect to finalUrl. Once
/// the redirect to finalUrl occurs the client should close the web ui and invoke
/// XalPlatformWebShowUrlComplete passing the full redirect url.
///
/// This handler is optional for Android and iOS platforms. If it is not set on
/// these platforms, Xal will provide default browser behavior. On UWP, and XDK
/// platforms, this handler is ignored.
///
/// Depending on the value of showUrlType and the type of browser the client is
/// using, different behavior is expected. See the definition for
/// XalShowUrlType for more information.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformWebShowUrlEventHandler)(
    _In_opt_ void* context,
    _In_opt_ void* userContext,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* startUrl,
    _In_z_ char const* finalUrl,
    _In_ XalShowUrlType showUrlType
);

//-----------------------------------------------------------------------------
// Storage (ignored on OneCore platforms, optional on iOS and Android)

/// <summary>
/// Write to storage event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userContext">Always null.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="key">Identifies the data being written.</param>
/// <param name="dataSize">The size (in bytes) of the data.</param>
/// <param name="data">The data to write.</param>
/// <returns></returns>
/// <remarks>
/// This version of the handler is deprecated, please switch to
/// XalPlatformStorageWriteEventHandler2.
///
/// This event is raised when Xal needs to write data to storage, the client
/// should write the data and when done invoke XalPlatformStorageWriteComplete.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformStorageWriteEventHandler)(
    _In_opt_ void* context,
    _In_opt_ void* userContext,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key,
    _In_ size_t dataSize,
    _In_reads_bytes_(dataSize) void const* data
);

/// <summary>
/// Read from storage event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userContext">Always null.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="key">Identifies the data being read.</param>
/// <returns></returns>
/// <remarks>
/// This version of the handler is deprecated, please switch to
/// XalPlatformStorageReadEventHandler2.
///
/// This event is raised when Xal needs to read data from storage, the client
/// should read the data and when done invoke XalPlatformStorageReadComplete.
/// If the key is not found, the client should complete with
/// XalPlatformOperationResult_Success and no data.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformStorageReadEventHandler)(
    _In_opt_ void* context,
    _In_opt_ void* userContext,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key
);

/// <summary>
/// Clear from storage event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userContext">Always null.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="key">Identifies the data being cleared.</param>
/// <returns></returns>
/// <remarks>
/// This version of the handler is deprecated, please switch to
/// XalPlatformStorageClearEventHandler2.
///
/// This event is raised when Xal needs to clear data from storage, the client
/// should clear the data and when done invoke XalPlatformStorageClearComplete.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformStorageClearEventHandler)(
    _In_opt_ void* context,
    _In_opt_ void* userContext,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key
);

/// <summary>
/// Struct encapsulating the storage event handlers.
/// </summary>
/// <remarks>
/// This version of the api is deprecated, please switch to
/// XalPlatformStorageEventHandlers2.
///
/// All 3 handlers must be set at the same time.
/// </remarks>
typedef struct XalPlatformStorageEventHandlers
{
    /// <summary>
    /// Write to storage handler.
    /// </summary>
    XalPlatformStorageWriteEventHandler* write;

    /// <summary>
    /// Read from storage handler.
    /// </summary>
    XalPlatformStorageReadEventHandler* read;

    /// <summary>
    /// Clear from storage handler.
    /// </summary>
    XalPlatformStorageClearEventHandler* clear;

    /// <summary>
    /// Optional pointer to data used by the event handlers.
    /// </summary>
    void* context;
} XalPlatformStorageEventHandlers;

//-----------------------------------------------------------------------------
// Remote Connect (only used in generic mode, when configured for it)

/// <summary>
/// Show prompt for remote connect authentication event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userContext">Always null.</param>
/// <param name="operation">The handle for this operation.</param>
/// <param name="url">The url to show in the prompt.</param>
/// <param name="code">The code to show in the prompt.</param>
/// <returns></returns>
/// <remarks>
/// This version of the handler is deprecated, please switch to
/// XalPlatformRemoteConnectShowPromptEventHandler2.
///
/// This event is raised when Xal needs to prompt the user to perform the
/// remote connect authentication process.
/// The prompt ui should be displayed until
/// XalPlatformRemoteConnectClosePromptEventHandler is called or it is dismissed
/// by the user.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformRemoteConnectShowPromptEventHandler)(
    _In_opt_ void* context,
    _In_opt_ void* userContext,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* url,
    _In_z_ char const* code
);

/// <summary>
/// Close prompt for remote authentication event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userContext">Always null.</param>
/// <param name="operation">The handle for this operation.</param>
/// <returns></returns>
/// <remarks>
/// This version of the handler is deprecated, please switch to
/// XalPlatformRemoteConnectClosePromptEventHandler2.
///
/// This event is raised when the remote connect authentication process has been
/// completed and the prompt is no longer necessary.
/// This event will always be called with the same operation as a previous
/// XalPlatformRemoteConnectShowPromptEventHandler event.
///
/// All arguments are owned by the caller (except context).
/// </remarks>
typedef void (XalPlatformRemoteConnectClosePromptEventHandler)(
    _In_opt_ void* context,
    _In_opt_ void* userContext,
    _In_ XalPlatformOperation operation
);

/// <summary>
/// Struct encapsulating the remote connect event handlers.
/// </summary>
/// <remarks>
/// This version of the api is deprecated, please switch to
/// XalPlatformRemoteConnectEventHandlers2.
///
/// Both handlers must be set at the same time.
/// </remarks>
typedef struct XalPlatformRemoteConnectEventHandlers
{
    /// <summary>
    /// Show the prompt handler.
    /// </summary>
    XalPlatformRemoteConnectShowPromptEventHandler* show;

    /// <summary>
    /// Close the prompt handler.
    /// </summary>
    XalPlatformRemoteConnectClosePromptEventHandler* close;

    /// <summary>
    /// Optional pointer to data used by the event handlers.
    /// </summary>
    void* context;
} XalPlatformRemoteConnectEventHandlers;

#endif
