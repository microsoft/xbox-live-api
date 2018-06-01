// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

struct XblAppConfig;

/// <summary>
/// Creates an xbl_context_handle used to access Xbox Live services.
/// </summary>
/// <param name="user">
/// The Xbox Live user associated with this context. For XDK this should be an IInspectable pointer
/// to a Windows::Xbox::System::User.
/// </param>
/// <param name="context">The returned Xbox Live context handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextCreateHandle(
    _In_ xbl_user_handle user,
    _Out_ xbl_context_handle* context
    ) XBL_NOEXCEPT;

/// <summary>
/// Increments the reference count of an xbl_context_handle. Use this method rather than creating
/// a new context with the same user if the context is needed by multiple threads.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
/// <returns>Returns the duplicated handle.</returns>
STDAPI_(xbl_context_handle)
XblContextDuplicateHandle(
    _In_ xbl_context_handle xboxLiveContextHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Decrements the reference count on the context object.
/// When the ref count reaches 0, XblContextCloseHandle() will free the memory associated with the handle.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
STDAPI_(void)
XblContextCloseHandle(
    _In_ xbl_context_handle xboxLiveContextHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the Xbox Live user associated with this context.
/// </summary>
/// <param name="context">The Xbox Live context handle.</param>
/// <param name="user">The returned Xbox Live user handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI
XblContextGetUser(
    _In_ xbl_context_handle context,
    _Out_ xbl_user_handle* user
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the Xbox user ID of the user associated with the context. The returned value
/// remains valid until the associated context is destroyed.
/// </summary>
/// <param name="context">The Xbox Live context handle.</param>
/// <param name="xboxUserId">The returned xbox user ID.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI
XblContextGetXboxUserId(
    _In_ xbl_context_handle context,
    _Out_ uint64_t* xboxUserId
    ) XBL_NOEXCEPT;
