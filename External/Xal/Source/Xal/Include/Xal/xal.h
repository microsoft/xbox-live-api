#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <httpClient/async.h>
#include <Xal/xal_types.h>
#include <Xal/xal_user.h>

extern "C"
{

//-----------------------------------------------------------------------------
// User model
//-----------------------------------------------------------------------------

/// <summary>
/// Initializes Xal, must be called before any other Xal functions (except
/// XalMemSetFunctions, XalMemGetFunctions and XalPlatform*SetHandler).
/// </summary>
/// <param name="args">The initialization arguments for Xal,
/// <see cref="XalInitArgs" />.</param>
/// <param name="internalWorkQueue">The async queue Xal should use for internal
/// only work such as telemetry and asynchronous init work. Only the work side
/// of the queue will be used.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// Depending on the platform, some of the platform event handlers defined in
/// xal_platform.h may be required, in which case they should be set before
/// calling this function. If any required event handlers are missing
/// XalInitialize will fail with E_XAL_MISSINGPLATFORMEVENTHANDLER.
///
/// It is never an error to set a platform event handler that is not required.
/// </remarks>
STDAPI XalInitialize(
    _In_ XalInitArgs const* args,
    _In_opt_ XTaskQueueHandle internalWorkQueue
) noexcept;

/// <summary>
/// Cleans up Xal, after this function returns Xal will be in the same state as
/// if XalInit was never called.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This functions should only be called when all user handles have been closed
/// and there are no outstanding asynchronous operations.
/// </remarks>
STDAPI XalCleanupAsync(
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the result of a successful XalCleanupAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalCleanupResult(
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the maximum number of users that can be added at the same time on the
/// current platform.
/// </summary>
/// <param name="maxUsers">The maximum number of concurrent users.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_XAL_NOTINITIALIZED, or E_FAIL.</returns>
STDAPI XalGetMaxUsers(
    _Out_ uint32_t* maxUsers
) noexcept;

/// <summary>
/// Get the Xbox Live title ID that Xal was initialized with. Must be called
/// after XalInitialize.
/// </summary>
/// <param name="titleId">The Xbox Live title ID.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_XAL_NOTINITIALIZED, or E_FAIL.</returns>
STDAPI XalGetTitleId(
    _Out_ uint32_t* titleId
) noexcept;

/// <summary>
/// Get the size of the buffer needed to store the Xbox Live sandbox string.
/// </summary>
/// <returns>The size of the sandbox string including the null terminator.</returns>
STDAPI_(size_t) XalGetSandboxSize() noexcept;

/// <summary>
/// Get the Xbox Live sandbox that Xal was initialized with. Must be called after
/// XalInitialize.
/// </summary>
/// <param name="sandboxSize">The size in bytes of the sandbox buffer.
/// Should be the value returned by XalGetSandboxSize.</param>
/// <param name="sandbox">The buffer the sandbox will be written to.</param>
/// <param name="sandboxUsed">The number of bytes used in the buffer including
/// the null terminator.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_XAL_NOTINITIALIZED, or E_FAIL.</returns>
STDAPI XalGetSandbox(
    _In_ size_t sandboxSize,
    _Out_writes_(sandboxSize) char* sandbox,
    _Out_opt_ size_t* sandboxUsed
) noexcept;

/// <summary>
/// Attempts to add a user without showing any ui.
/// </summary>
/// <param name="userIdentifier">Client provided identifier for the user.
/// </param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// userIdentifier will be stored in the user and passed to all XalPlatform*
/// callbacks related to this user.
/// </remarks>
STDAPI XalTryAddDefaultUserSilentlyAsync(
    _In_ uint32_t userIdentifier,
    _In_ XAsyncBlock* async
) noexcept;
/// <summary>
/// Get the result of a successful XalTryAddDefaultUserSilentlyAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="newUser">The user that was just added.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If the operations failed, newUser will be NULL.
/// </remarks>
STDAPI XalTryAddDefaultUserSilentlyResult(
    _In_ XAsyncBlock* async,
    _Out_ XalUserHandle* newUser
) noexcept;

/// <summary>
/// Attempts to add a user with the given Xbox user id.
/// </summary>
/// <param name="userIdentifier">Client provided identifier for the user.
/// </param>
/// <param name="xboxUserId">The Xbox user id.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This function may show ui to the user.
///
/// userIdentifier will be stored in the user and passed to all XalPlatform*
/// callbacks related to this user.
/// </remarks>
STDAPI XalTryAddUserByIdAsync(
    _In_ uint32_t userIdentifier,
    _In_ uint64_t xboxUserId,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the result of a successful XalTryAddUserByIdAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="newUser">The user that was just added.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If the operations failed, newUser will be NULL.
/// </remarks>
STDAPI XalTryAddUserByIdResult(
    _In_ XAsyncBlock* async,
    _Out_ XalUserHandle* newUser
) noexcept;

/// <summary>
/// Attempts to add a user.
/// </summary>
/// <param name="userIdentifier">Client provided identifier for the user.
/// </param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This function will show ui to the user.
///
/// userIdentifier will be stored in the user and passed to all XalPlatform*
/// callbacks related to this user.
/// </remarks>
STDAPI XalAddUserWithUiAsync(
    _In_ uint32_t userIdentifier,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the result of a successful XalAddUserWithUiAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="newUser">The user that was just added.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// If the operations failed, newUser will be NULL.
/// </remarks>
STDAPI XalAddUserWithUiResult(
    _In_ XAsyncBlock* async,
    _Out_ XalUserHandle* newUser
) noexcept;

/// <summary>
/// Checks if the current platform supports retrieving a device user handle.
/// </summary>
/// <returns>True if device user present, false if not.</returns>
STDAPI_(bool) XalGetDeviceUserIsPresent() noexcept;

/// <summary>
/// Returns a user which represents the device itself.
/// </summary>
/// <param name="deviceUser">The user object.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_XAL_NOTINITIALIZED, or E_FAIL.</returns>
/// <remarks>
/// This user handle will have had XalUserDuplicateHandle called on it. Be sure
/// to call XalUserCloseHandle once it is no longer needed.
/// </remarks>
STDAPI XalGetDeviceUser(
    _Out_ XalUserHandle* deviceUser
) noexcept;

/// <summary>
/// Checks if the user can be signed out on the current platform.
/// </summary>
/// <returns>True if user can be signed out, false if user can't.</returns>
STDAPI_(bool) XalSignOutUserAsyncIsPresent() noexcept;

/// <summary>
/// Signs out a user from the device.
/// </summary>
/// <param name="user">The user to remove.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// User sign out may not be present on all platforms, on platforms where it is
/// missing this function will return E_XAL_FEATURENOTPRESENT. To query if user
/// sign out is available see <see cref="XalSignOutUserAsyncIsPresent"/>.
///
/// On some platforms this call may fail to sign out the user if there is no
/// internet connectivity, if that happens the user will not be removed from the
/// user set and will still be fully signed in.
///
/// On platforms that use the web ui hooks
/// (see <see cref="XalPlatformWebShowUrlEventHandler"/>) this function will
/// invoke the hook.
/// </remarks>
STDAPI XalSignOutUserAsync(
    _In_ XalUserHandle user,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the result of a given XalSignOutUserAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalSignOutUserResult(
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Tries to find a user for the given local id.
/// </summary>
/// <param name="localId">The local id it to look up.</param>
/// <param name="user">The user object.</param>
/// <returns>Result code for this API operation. Possible values are S_OK, E_XAL_NOTINITIALIZED, or E_FAIL.</returns>
/// <remarks>
/// If no user can be found matching the local id, E_XAL_USERNOTFOUND is returned.
/// </remarks>
STDAPI XalFindUserByLocalId(
    _In_ XalUserLocalId localId,
    _Out_ XalUserHandle* user
) noexcept;

/// <summary>
/// Checks if the given consent is opted in by all the users in the user set.
/// </summary>
/// <param name="consentModelName">The UCS consent model name.</param>
/// <param name="canOptIn">True if all the users in the user set are opted in.</param>
/// <returns>Result code for this API operation. Possible values are S_OK,
/// E_XAL_NOTINITIALIZED, E_XAL_UNLISTEDCONSENT, or E_FAIL.</returns>
STDAPI XalCheckUcsConsentForAllUsers(
    _In_z_ char const* consentModelName,
    _Out_ bool* canOptIn
) noexcept;

}

// Back compat apis
#if XAL_ENABLE_BACK_COMPAT_SHIMS
/// <summary>
/// Attempts to add a user without showing any ui.
/// </summary>
/// <param name="userContext">Must be null.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This version of the api is deprecated, consider the variant taking a
/// uint32_t userIdentifier instead.
/// </remarks>
inline
HRESULT XalTryAddDefaultUserSilentlyAsync(
    _In_opt_ nullptr_t /*userContext*/,
    _In_ XAsyncBlock* async
) noexcept
{
    return XalTryAddDefaultUserSilentlyAsync(0u, async);
}

/// <summary>
/// Attempts to add a user.
/// </summary>
/// <param name="userContext">Must be null.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This version of the api is deprecated, consider the variant taking a
/// uint32_t userIdentifier instead.
///
/// This function will show ui to the user.
/// </remarks>
inline
HRESULT XalAddUserWithUiAsync(
    _In_opt_ nullptr_t /*userContext*/,
    _In_ XAsyncBlock* async
) noexcept
{
    return XalAddUserWithUiAsync(0u, async);
}
#endif

#if HC_PLATFORM == HC_PLATFORM_GDK
#include <Xal/xal_gsdk_impl.h>
#elif HC_PLATFORM == HC_PLATFORM_XDK
#include <Xal/xal_xdk_ext.h>
#endif
