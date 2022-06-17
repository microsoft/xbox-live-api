#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <httpClient/async.h>
#include <Xal/xal_platform_types.h>
#include <Xal/xal_types.h>

extern "C"
{

//-----------------------------------------------------------------------------
// Hooks for platform specific behaviour
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Memory (optional on all platforms)

/// <summary>
/// Sets the memory hook functions to allow callers to route memory allocations
/// to their own memory manager. This must be called before XalInitialize and
/// cannot be called again until after XalCleanup and all XAL_USER_HANDLEs have
/// been closed.
///
/// This method allows the application to install custom memory allocation
/// routines in order to service all requests for new memory buffers instead of
/// using default allocation routines.
///
/// The <paramref name="memAllocFunc" /> and <paramref name="memFreeFunc" />
/// parameters can be null pointers to restore the default routines. Both
/// callback pointers must be null or both must be non-null. Mixing custom and
/// default routines is not permitted and will cause the function to fail.
/// </summary>
/// <param name="memAllocFunc">A pointer to the custom allocation callback to
/// use, or a null pointer to restore the default.</param>
/// <param name="memFreeFunc">A pointer to the custom freeing callback to use,
/// or a null pointer to restore the default.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalMemSetFunctions(
    _In_opt_ XalMemAllocFunc* memAllocFunc,
    _In_opt_ XalMemFreeFunc* memFreeFunc
) noexcept;

/// <summary>
/// Gets the memory hook functions to allow callers to route memory allocations
/// to their own memory manager. This method allows the application get the
/// default memory allocation routines. This can be used along with
/// XalMemSetFunctions to monitor all memory allocations.
/// </summary>
/// <param name="memAllocFunc">Set to the current allocation callback. Returns
/// the default routine if not previously set.</param>
/// <param name="memFreeFunc">Set to the to the current memory free callback.
/// Returns the default routine if not previously set.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalMemGetFunctions(
    _Out_ XalMemAllocFunc** memAllocFunc,
    _Out_ XalMemFreeFunc** memFreeFunc
) noexcept;

//-----------------------------------------------------------------------------
// Web view (ignored on OneCore platforms, optional on iOS and Android)

/// <summary>
/// Registers the show url event handler.
/// </summary>
/// <param name="queue">The async queue the callback should be invoked on.
/// </param>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="handler">The event handler,
/// <see cref="XalPlatformWebShowUrlEventHandler"/>.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize.
/// </remarks>
STDAPI XalPlatformWebSetEventHandler(
    _In_opt_ XTaskQueueHandle queue,
    _In_opt_ void* context,
    _In_ XalPlatformWebShowUrlEventHandler2* handler
) noexcept;

/// <summary>
/// Clears the show url event handler.
/// </summary>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize or after XalCleanupAsync completes.
/// </remarks>
STDAPI XalPlatformWebClearEventHandler() noexcept;

/// <summary>
/// Completes a show url operation.
/// </summary>
/// <param name="operation">The handle for this operation.</param>
/// <param name="result">The result of the operation.</param>
/// <param name="url">The full url for the final redirection.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This should only be called in response to a show url event, once the web
/// view is redirected to the final url or if an error occurs and the operation
/// cannot be completed.
/// <see cref="XalPlatformWebShowUrlEventHandler"/>
/// </remarks>
STDAPI XalPlatformWebShowUrlComplete(
    _In_ XalPlatformOperation operation,
    _In_ XalPlatformOperationResult result,
    _In_opt_z_ char const* url
) noexcept;

//-----------------------------------------------------------------------------
// Storage (ignored on OneCore platforms, optional on iOS and Android)

/// <summary>
/// Sets the storage event handlers.
/// </summary>
/// <param name="queue">The async queue the callbacks should be invoked on.
/// </param>
/// <param name="handlers">The event handlers,
/// <see cref="XalPlatformStorageEventHandlers"/>.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize.
/// </remarks>
STDAPI XalPlatformStorageSetEventHandlers(
    _In_opt_ XTaskQueueHandle queue,
    _In_ XalPlatformStorageEventHandlers2* handlers
) noexcept;

/// <summary>
/// Clears the storage event handlers.
/// </summary>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize or after XalCleanupAsync completes.
/// </remarks>
STDAPI XalPlatformStorageClearEventHandlers() noexcept;

/// <summary>
/// Completes write to storage operation.
/// </summary>
/// <param name="operation">The handle for this operation.</param>
/// <param name="result">The result of the operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This should only be called in response to a write to storage event, once
/// the write is completed or if an error occurs and the operation cannot be
/// completed <see cref="XalPlatformStorageWriteEventHandler"/>.
/// </remarks>
STDAPI XalPlatformStorageWriteComplete(
    _In_ XalPlatformOperation operation,
    _In_ XalPlatformOperationResult result
) noexcept;

/// <summary>
/// Completes read from storage operation.
/// </summary>
/// <param name="operation">The handle for this operation.</param>
/// <param name="result">The result of the operation.</param>
/// <param name="dataSize">The size (in bytes) of the data.</param>
/// <param name="data">The data read.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This should only be called in response to a read from storage event, once
/// the read is completed or if an error occurs and the operation cannot be
/// completed <see cref="XalPlatformStorageReadEventHandler"/>.
///
/// If the requested key cannot be found, the operation should be completed
/// with XalClientOperationResult_Success, data = nullptr and dataSize = 0.
/// </remarks>
STDAPI XalPlatformStorageReadComplete(
    _In_ XalPlatformOperation operation,
    _In_ XalPlatformOperationResult result,
    _In_ size_t dataSize,
    _In_reads_bytes_opt_(dataSize) void const* data
) noexcept;

/// <summary>
/// Completes clear from storage operation.
/// </summary>
/// <param name="operation">The handle for this operation.</param>
/// <param name="result">The result of the operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This should only be called in response to a clear from storage event, once
/// the data is cleared or if an error occurs and the operation cannot be
/// completed <see cref="XalPlatformStorageClearEventHandler"/>.
/// </remarks>
STDAPI XalPlatformStorageClearComplete(
    _In_ XalPlatformOperation operation,
    _In_ XalPlatformOperationResult result
) noexcept;

//-----------------------------------------------------------------------------
// Remote Connect (only used in generic mode, when configured for it)

/// <summary>
/// Sets the remote connect event handlers.
/// </summary>
/// <param name="queue">The async queue the callbacks should be invoked on.
/// </param>
/// <param name="handlers">The event handlers,
/// <see cref="XalPlatformRemoteConnectEventHandlers"/>.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize.
/// </remarks>
STDAPI XalPlatformRemoteConnectSetEventHandlers(
    _In_opt_ XTaskQueueHandle queue,
    _In_ XalPlatformRemoteConnectEventHandlers2* handlers
) noexcept;

/// <summary>
/// Clears the remote connect event handlers.
/// </summary>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize or after XalCleanupAsync completes.
/// </remarks>
STDAPI XalPlatformRemoteConnectClearEventHandlers() noexcept;

/// <summary>
/// Signal to Xal that the remote connect prompt has been dismissed by the user.
/// </summary>
/// <param name="operation">The handle for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This should be called after a XalPlatformRemoteConnectShowPromptEventHandler
/// if the user dismisses the prompt. The whole remote connect process will be
/// cancelled and the starting AddUserAsync operation will complete with
/// E_ABORT. The XalPlatformRemoteConnectClosePromptEventHandler will be called
/// as normal.
/// </remarks>
STDAPI XalPlatformRemoteConnectCancelPrompt(
    _In_ XalPlatformOperation operation
) noexcept;

//------------------------------------------------------------------------------
// Crypto (only used in generic mode)

/// <summary>
/// Sets the crypto callbacks.
/// </summary>
/// <param name="callbacks">The callbacks,
/// <see cref="XalPlatformCryptoCallbacks"/>.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize.
/// </remarks>
STDAPI XalPlatformCryptoSetCallbacks(
    _In_ XalPlatformCryptoCallbacks* callbacks
) noexcept;

//------------------------------------------------------------------------------
// Date & Time (only used in generic mode)

/// <summary>
/// Sets the date/time callbacks.
/// </summary>
/// <param name="callbacks">The callbacks,
/// <see cref="XalPlatformDateTimeCallbacks"/>.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize.
/// </remarks>
STDAPI XalPlatformDateTimeSetCallbacks(
    _In_ XalPlatformDateTimeCallbacks* callbacks
) noexcept;

}

// Back compat hooks
#if XAL_ENABLE_BACK_COMPAT_SHIMS
//-----------------------------------------------------------------------------
// Web view (ignored on OneCore platforms, optional on iOS and Android)

/// <summary>
/// Registers the show url event handler.
/// </summary>
/// <param name="queue">The async queue the callback should be invoked on.
/// </param>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="handler">The event handler,
/// <see cref="XalPlatformWebShowUrlEventHandler"/>.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize.
/// </remarks>
inline
HRESULT XalPlatformWebSetEventHandler(
    _In_opt_ XTaskQueueHandle queue,
    _In_opt_ void* context,
    _In_ XalPlatformWebShowUrlEventHandler* handler
) noexcept
{
    static struct WebShowUrlHandler
    {
        XalPlatformWebShowUrlEventHandler* handler;
        void* context;
    } s_handlers{};

    s_handlers.handler = handler;
    s_handlers.context = context;

    XalPlatformWebShowUrlEventHandler2* trampoline = [](
        void* ctx,
        uint32_t /*cuid*/,
        XalPlatformOperation op,
        char const* sUrl,
        char const* fUrl,
        XalShowUrlType t,
        uint32_t /*requestHeaderCount*/,
        XalHttpHeader const* /*requestHeaders*/
    )
    {
        auto handler = static_cast<WebShowUrlHandler*>(ctx);
        handler->handler(handler->context, nullptr, op, sUrl, fUrl, t);
    };

    return XalPlatformWebSetEventHandler(queue, &s_handlers, trampoline);
}

//-----------------------------------------------------------------------------
// Storage (ignored on OneCore platforms, optional on iOS and Android)

/// <summary>
/// Sets the storage event handlers.
/// </summary>
/// <param name="queue">The async queue the callbacks should be invoked on.
/// </param>
/// <param name="handlers">The event handlers,
/// <see cref="XalPlatformStorageEventHandlers"/>.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize.
/// </remarks>
inline
HRESULT XalPlatformStorageSetEventHandlers(
    _In_opt_ XTaskQueueHandle queue,
    _In_ XalPlatformStorageEventHandlers* handlers
) noexcept
{
    static XalPlatformStorageEventHandlers s_handlers{};

    s_handlers = *handlers;

    XalPlatformStorageEventHandlers2 trampolines = {};
    trampolines.write = [](void* ctx, uint32_t /*cuid*/, XalPlatformOperation op, char const* key, size_t size, void const* data)
    {
        auto handlers = static_cast<XalPlatformStorageEventHandlers*>(ctx);
        handlers->write(handlers->context, nullptr, op, key, size, data);
    };
    trampolines.read = [](void* ctx, uint32_t /*cuid*/, XalPlatformOperation op, char const* key)
    {
        auto handlers = static_cast<XalPlatformStorageEventHandlers*>(ctx);
        handlers->read(handlers->context, nullptr, op, key);
    };
    trampolines.clear = [](void* ctx, uint32_t /*cuid*/, XalPlatformOperation op, char const* key)
    {
        auto handlers = static_cast<XalPlatformStorageEventHandlers*>(ctx);
        handlers->clear(handlers->context, nullptr, op, key);
    };
    trampolines.context = &s_handlers;

    return XalPlatformStorageSetEventHandlers(queue, &trampolines);
}

//-----------------------------------------------------------------------------
// Remote Connect (only used in generic mode, when configured for it)

/// <summary>
/// Sets the remote connect event handlers.
/// </summary>
/// <param name="queue">The async queue the callbacks should be invoked on.
/// </param>
/// <param name="handlers">The event handlers,
/// <see cref="XalPlatformRemoteConnectEventHandlers"/>.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Must be called before XalInitialize.
/// </remarks>
inline
HRESULT XalPlatformRemoteConnectSetEventHandlers(
    _In_opt_ XTaskQueueHandle queue,
    _In_ XalPlatformRemoteConnectEventHandlers* handlers
) noexcept
{
    static XalPlatformRemoteConnectEventHandlers s_handlers{};

    s_handlers = *handlers;

    XalPlatformRemoteConnectEventHandlers2 trampolines = {};
    trampolines.show = [](void* ctx, uint32_t /*cuid*/, XalPlatformOperation op, char const* url, char const* code)
    {
        auto handlers = static_cast<XalPlatformRemoteConnectEventHandlers*>(ctx);
        handlers->show(ctx, nullptr, op, url, code);
    };
    trampolines.close = [](void* ctx, uint32_t /*cuid*/, XalPlatformOperation op)
    {
        auto handlers = static_cast<XalPlatformRemoteConnectEventHandlers*>(ctx);
        handlers->close(ctx, nullptr, op);
    };
    trampolines.context = &s_handlers;

    return XalPlatformRemoteConnectSetEventHandlers(queue, &trampolines);
}
#endif
