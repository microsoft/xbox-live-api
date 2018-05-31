// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-c/errors_c.h"

const size_t GamertagMaxBytes = 16;

#if !XDK_API

/// <summary>
/// Enumeration values that indicate the result status of sign in.
/// </summary>
typedef enum XblSignInStatus
{
    /// <summary>
    /// Signed in successfully.
    /// </summary>
    XblSignInStatus_Success = 0,

    /// <summary>
    /// Need to invoke the signin API (w/ UX) to let the user take necessary actions for the sign-in operation to continue.
    /// Can only be returned from silent sign in APIs.
    /// </summary>
    XblSignInStatus_UserInterationRequired,

    /// <summary>
    /// The user decided to cancel the sign-in operation.
    /// Can only by returned from non-silent sign in APIs.
    /// </summary>
    XblSignInStatus_UserCancel
} XblSignInStatus;

/// <summary>
/// Enum defining the various age groups.
/// </summary>
typedef enum XblAgeGroup
{
    XblAgeGroup_Unknown = 0,
    XblAgeGroup_Child = 1,
    XblAgeGroup_Teen = 2,
    XblAgeGroup_Adult = 3,
} XblAgeGroup;

/// <summary>
/// Represents the result of the sign in operation.
/// </summary>
typedef struct XblSignInResult
{
    /// <summary>
    /// The status of sign in operation.
    /// </summary>
    XblSignInStatus status;
} XblSignInResult;

/// <summary>
/// Creates an xbl_user_handle used for Xbox Live authentication/sign in.
/// </summary>
/// <param name="user">The returned Xbox Live user handle.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI 
XblUserCreateHandle(
    _Out_ xbl_user_handle* user
    ) XBL_NOEXCEPT;

#if UWP_API

/// <summary>
/// Creates an xbl_user_handle associated with a Windows System User.
/// </summary>
/// <param name="user">The returned Xbox Live user handle.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI
XblUserCreateHandleFromSystemUser(
    _In_opt_ Windows::System::User^ creationContext,
    _Out_ xbl_user_handle* user
    ) XBL_NOEXCEPT;

#endif

/// <summary>
/// Increments the reference count on the user object.
/// </summary>
/// <param name="user">The user handle.</param>
/// <returns>The duplicate handle.</returns>
STDAPI_(xbl_user_handle)
XblUserDuplicateHandle(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT;

/// <summary>
/// Decrements the reference count on the user object.
/// </summary>
/// <param name="user">The user handle.</param>
STDAPI
XblUserCloseHandle(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns the id of the user.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="xboxUserId">The returned Xbox User ID.</param>
STDAPI
XblUserGetXboxUserId(
    _In_ xbl_user_handle user,
    _Out_ uint64_t* xboxUserId
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns the gamertag of the user.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="gamertagBufferSize">Size of the provided output buffer.</param>
/// <param name="gamertagBuffer">Buffer to write gamertag.</param>
/// <param name="written">The actual number of bytes written to the buffer.</param>
STDAPI
XblUserGetGamertag(
    _In_ xbl_user_handle user,
    _In_ size_t gamertagBufferSize,
    _Out_writes_to_(gamertagBufferSize, *written) char* gamertagBuffer,
    _Out_opt_ size_t* written
    ) XBL_NOEXCEPT;

/// <summary>
/// Returns the age group of the user.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="ageGroup">The age group.</param>
STDAPI
XblUserGetAgeGroup(
    _In_ xbl_user_handle user,
    _Out_ XblAgeGroup* ageGroup
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the length of the privileges string for a user. 
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="size">The required buffer size for the privileges string.</param>
STDAPI
XblUserGetPrivilegesSize(
    _In_ xbl_user_handle user,
    _Out_ size_t* size
    ) XBL_NOEXCEPT;

/// <summary>
/// Get the privileges string for a user.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="gamertagBufferSize">Size of the provided output buffer.</param>
/// <param name="privileges">Buffer to write the users privileges.</param>
/// <param name="written">The actual number of bytes written to the buffer.</param>
STDAPI
XblUserGetPrivileges(
    _In_ xbl_user_handle user,
    _In_ size_t privilegesSize,
    _Out_writes_to_(privilegesSize, *written) char* privileges,
    _Out_opt_ size_t* written
    ) XBL_NOEXCEPT;

/// <summary>
/// Checks if a user is signed in.
/// </summary>
/// <param name="userHandle">The user handle.</param>
/// <param name="isSignedIn">Whether the user is currently signed in.</param>
STDAPI
XblUserIsSignedIn(
    _In_ xbl_user_handle user,
    _Out_ bool* isSignedIn
    ) XBL_NOEXCEPT;

/// <summary>
/// Attempt to sign a player into their Xbox Live account. This call may bring up
/// a sign-in user interface.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <remarks>
/// You should only call this method if silent sign-in indicates that user interaction is required.
/// For UWA, this API is to be called from UI thread, if you're calling from non-UI thread or not sure, please use 
/// XblUserSignInWithCoreDispatcher instead.
/// </remarks>
STDAPI
XblUserSignIn(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT;

/// <summary>
/// Attempt to silently sign a player into their Xbox Live account.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <remarks>
/// If the app is unable to silently sign-in, the result will be an XblSignInResult with a user interaction required status .
/// to sign-in, so the app should then call XblUserSignIn.
/// For UWA, this API is to be called from UI thread, if you're not calling from non-UI thread or not sure, please use
/// XblUserSignInSilentlyWithCoreDispatcher instead.
/// </remarks>
STDAPI
XblUserSignInSilently(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT;

#if UWP_API

/// <summary>
/// Attempt to sign a player into their Xbox Live account. This call may bring up
/// a sign-in user interface.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="coreDispatcherObj">The Windows Runtime core event message dispatcher.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <remarks>
/// You should only call this method if silent sign-in indicates that user interaction is required.
/// If you're calling this API from non-UI thread, parameter coreDispatcherObj is required, so that app UI
/// can be rendered and locale can be generated.
/// </remarks>
STDAPI
XblUserSignInWithCoreDispatcher(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user,
    _In_ Platform::Object^ coreDispatcherObj
    ) XBL_NOEXCEPT;

/// <summary>
/// Attempt to silently sign a player into their Xbox Live account.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="coreDispatcherObj">The Windows Runtime core event message dispatcher.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <remarks>
/// If the app is unable to silently sign-in, the API return sign_in_result with user_interaction_required status .
/// to sign-in, so the app should then call signin().
/// If you're calling this API from non-UI thread, parameter coreDispatcherObj is required, so that app locale can be generated.
/// </remarks>
STDAPI
XblUserSignInSilentlyWithCoreDispatcher(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user,
    _In_ Platform::Object^ coreDispatcherObj
    ) XBL_NOEXCEPT;

#endif

/// <summary>
/// Get the result of a completed sign in attempt.
/// </summary>
/// <param name="async">The async block passed to the sign in function.</param>
/// <param name="signInResult">Caller allocated object to write results into.</param>
STDAPI
XblUserGetSignInResult(
    _Inout_ AsyncBlock* async,
    _Out_ XblSignInResult* signInResult
    ) XBL_NOEXCEPT;

typedef void (STDAPIVCALLTYPE *XblGetTokenAndSignatureCallback)(
    _In_ void* context,
    _In_z_ const char* token,
    _In_z_ const char* signature
    );

/// <summary>
/// Retrieves an authorization token and digital signature for an HTTP request by this user,
/// with a request body expressed as an array of bytes.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="httpMethod">The HTTP method (GET, PUT, POST, etc.) for this request.</param>
/// <param name="url">The URL for which to retrieve the authorization token and digital signature.</param>
/// <param name="headers">The headers to be included in the HTTP request.</param>
/// <param name="requestBodyString">The body of the request, expressed as a string.</param>
/// <param name="async">Caller allocated AsyncBlock.</param>
/// <param name="context>A context to be passed back to the typed callback.</param>
/// <param name="callback">A strongly typed callback function that will be called when the operation completes.
/// Note that the token and signature parameters pass to the callback are only valid until the callback returns.
/// </param>
STDAPI
XblUserGetTokenAndSignature(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user,
    _In_z_ const char* httpMethod,
    _In_z_ const char* url,
    _In_z_ const char* headers,
    _In_z_ const char* requestBodyString,
    _In_ XblGetTokenAndSignatureCallback callback
    ) XBL_NOEXCEPT;

typedef void (STDAPIVCALLTYPE *XblSignOutCompletedHandler)(
    _In_ xbl_user_handle user
    );

/// <summary>
/// Registers a handler that will be called when user sign out completes.
/// </summary>
/// <param name="handler">The callback function that receives notifications.</param>
/// <returns>
/// A function_context object that can be used to unregister the event handler.
/// </returns>
STDAPI_(function_context)
XblUserAddSignOutCompletedHandler(
    _In_ XblSignOutCompletedHandler handler
    ) XBL_NOEXCEPT;

/// <summary>
/// Unregisters an event handler for sign-out completion notifications.
/// </summary>
/// <param name="context">The function_context that was returned when the event handler was registered. </param>
STDAPI_(void)
XblUserRemoveSignOutCompletedHandler(
    _In_ function_context context
    ) XBL_NOEXCEPT;

#endif //!XDK_API
