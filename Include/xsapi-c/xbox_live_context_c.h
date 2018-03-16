// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"

// TODO: run and clean code analysis errors
#if defined(__cplusplus)
extern "C" {
#endif

struct XBL_XBOX_LIVE_APP_CONFIG;

/// <summary>
/// Creates an XBL_XBOX_LIVE_CONTEXT_HANDLE used to access Xbox Live services.
/// </summary>
/// <param name="user">The Xbox Live user associated with this context.</param>
/// <param name="xboxLiveContextHandle">The returned Xbox Live context handle.</param>
/// <returns>Result code for this API operation.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextCreateHandle(
    _In_ XBL_XBOX_LIVE_USER_PTR user,
    _Out_ XBL_XBOX_LIVE_CONTEXT_HANDLE* xboxLiveContextHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Increments the reference count of an XBL_XBOX_LIVE_CONTEXT_HANDLE. Use this method rather than creating
/// a new context with the same user if the conext is needed by multiple threads.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
/// <returns>Returns the duplicated handle.</returns>
XBL_API XBL_XBOX_LIVE_CONTEXT_HANDLE XBL_CALLING_CONV
XblXboxLiveContextDuplicateHandle(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Decrements the reference count on the context object.
/// When the ref count reaches 0, XblXboxLiveContextCloseHandle() will free the memory associated with the handle.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
/// <returns>Result code for this API operation.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextCloseHandle(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the Xbox Live user associated with this context.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
/// <param name="user">The returned Xbox Live user pointer.</param>
/// <returns>Result code for this API operation.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextGetUser(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle,
    _Out_ XBL_XBOX_LIVE_USER_PTR* user
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the Xbox user ID of the user associated with the context. The returned value
/// remains valid until the associated context is destroyed.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
/// <param name="xboxUserId">The returned xbox user ID string.</param>
/// <returns>Result code for this API operation.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextGetXboxUserId(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle,
    _Out_ PCSTR* xboxUserId
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the app configuration.  The returned value remains valid until XblGlobalCleanup is called.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
/// <param name="appConfig">The returned xbox live app config.</param>
/// <returns>Result code for this API operation.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblXboxLiveContextGetAppConfig(
    _In_ XBL_XBOX_LIVE_CONTEXT_HANDLE xboxLiveContextHandle,
    _Out_ CONST XBL_XBOX_LIVE_APP_CONFIG** appConfig
    ) XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)