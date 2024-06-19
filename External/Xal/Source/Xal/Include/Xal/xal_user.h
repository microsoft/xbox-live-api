#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <httpClient/async.h>
#include <Xal/xal_types.h>

extern "C"
{

//-----------------------------------------------------------------------------
// User Api
//-----------------------------------------------------------------------------

/// <summary>
/// Increments the reference count on the user object.
/// </summary>
/// <param name="user">The user handle.</param>
/// <param name="duplicatedUser">The new user handle.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
STDAPI XalUserDuplicateHandle(
    _In_ XalUserHandle user,
    _Out_ XalUserHandle* duplicatedUser
) noexcept;

/// <summary>
/// Decrements the reference count on the user object.
/// </summary>
/// <param name="user">The user object</param>
/// <returns></returns>
STDAPI_(void) XalUserCloseHandle(
    _In_ XalUserHandle user
) noexcept;

/// <summary>
/// Compares 2 user handler.
/// </summary>
/// <param name="user1">The first user.</param>
/// <param name="user2">The second user.</param>
/// <returns>
/// 0 if the two handles refer to the same xbox live identity, -1 if user1
/// identity is "less" than user2, 1 if user1 identity "greater" than user2.
/// </returns>
/// <remarks>
/// User identity ordering is arbitrary, but sutiable for sorting.
/// </remarks>
STDAPI_(int32_t) XalCompareUsers(
    _In_ XalUserHandle user1,
    _In_ XalUserHandle user2
) noexcept;

//-----------------------------------------------------------------------------
// User properties

/// <summary>
/// Returns the Xbox Live User ID (XUID) of the user.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="id">The Xbox Live User ID (XUID) of the user.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
STDAPI XalUserGetId( // TODO any valid error scenarios? local users, consent
    _In_ XalUserHandle user,
    _Out_ uint64_t* id
) noexcept;

/// <summary>
/// Gets the local id of the user.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="localId">The local id of the user.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
STDAPI XalUserGetLocalId(
    _In_ XalUserHandle user,
    _Out_ XalUserLocalId* localId
) noexcept;

/// <summary>
/// Returns a boolean indicating if the user is the device user.
/// </summary>
/// <param name="user">The user object.</param>
/// <returns></returns>
STDAPI_(bool) XalUserIsDevice(
    _In_ XalUserHandle user
) noexcept;

/// <summary>
/// Returns a boolean indicating if the user is a guest.
/// </summary>
/// <param name="user">The user object.</param>
/// <returns></returns>
STDAPI_(bool) XalUserIsGuest(
    _In_ XalUserHandle user
) noexcept;

/// <summary>
/// Returns the sign-in state of the user.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="state">The sign-in state of the user</param>
/// <see cref="XalUserState" />.
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserGetState(
    _In_ XalUserHandle user,
    _Out_ XalUserState* state
) noexcept;

/// <summary>
/// Returns the size of the buffer needed to store the gamertag string.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="component">The component of the gamertag to get the size of.</param>
/// <returns>The size of the buffer needed to store the gamertag string</returns>
/// <remark>
/// If XalGamertagComponent_Modern or XalGamertagComponent_UniqueModern are
/// specified but not available on the platform, this function will execute for
/// XalGamertagComponent_Classic instead. XalGamertagComponent_Suffix will be
/// empty if the modern components are unavailable.
/// </remark>
STDAPI_(size_t) XalUserGetGamertagSize(
    _In_ XalUserHandle user,
    _In_ XalGamertagComponent component
) noexcept;

/// <summary>
/// Returns the gamertag of the user.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="component">The component of the gamertag to get.</param>
/// <param name="gamertagSize">The size in bytes of the gamertag buffer.
/// Should be the value returned by XalUserGetGamertagSize.</param>
/// <param name="gamertag">The buffer the gamertag will be written to.</param>
/// <param name="gamertagUsed">The number of bytes used in the buffer including
/// the null terminator.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
/// <remark>
/// If XalGamertagComponent_Modern or XalGamertagComponent_UniqueModern are
/// specified but not available on the platform, this function will execute for
/// XalGamertagComponent_Classic instead. XalGamertagComponent_Suffix will be
/// empty if the modern components are unavailable.
/// </remark>
STDAPI XalUserGetGamertag(
    _In_ XalUserHandle user,
    _In_ XalGamertagComponent component,
    _In_ size_t gamertagSize,
    _Out_writes_(gamertagSize) char* gamertag,
    _Out_opt_ size_t* gamertagUsed
) noexcept;

/// <summary>
/// Gets the gamer picture for the user as a png in memory buffer.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="pictureSize">The size wanted.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK or E_FAIL.</returns>
STDAPI XalUserGetGamerPictureAsync(
    _In_ XalUserHandle user,
    _In_ XalGamerPictureSize pictureSize,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Gets the size in bytes of gamer picture buffer.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size in bytes for the result buffer.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserGetGamerPictureResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept;

/// <summary>
/// Gets the results of a successful XalUserGetGamerPictureAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the gamer picture buffer.</param>
/// <param name="buffer">The gamer picture png data.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserGetGamerPictureResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_(bufferSize) void* buffer
) noexcept;

/// <summary>
/// Returns the age group of the user.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="ageGroup">The age group.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_INVALIDARG, or E_FAIL.</returns>
STDAPI XalUserGetAgeGroup(
    _In_ XalUserHandle user,
    _Out_ XalAgeGroup* ageGroup
) noexcept;

/// <summary>
/// Checks if the user has the given privilege.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="privilege">The privilege to check.</param>
/// <param name="hasPrivilege">true if the user has the privilege, false
/// otherwise.</param>
/// <param name="reasons">Bitmask of the various reasons why the user could be
/// denied a privilege. If the user has the privilege it will always be
/// XAL_PRIVILEGE_CHECK_DENY_REASON_NONE.</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_XAL_NOTINITIALIZED, or E_FAIL.</returns>
STDAPI XalUserCheckPrivilege(
    _In_ XalUserHandle user,
    _In_ XalPrivilege privilege,
    _Out_ bool* hasPrivilege,
    _Out_opt_ XalPrivilegeCheckDenyReasons* reasons
) noexcept;

/// <summary>
/// Checks if the current platform supports resolving missing privileges.
/// </summary>
/// <returns></returns>
STDAPI_(bool) XalUserResolvePrivilegeWithUiIsPresent() noexcept;

/// <summary>
/// Shows ui explaining why the user is missing the given privilege and
/// allows acquiring it.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="privilege">The privilege to check.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserResolveUserPrivilegeWithUiAsync(
    _In_ XalUserHandle user,
    _In_ XalPrivilege privilege,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the result of a given XalUserResolveUserPrivilegeWithUiAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserResolveUserPrivilegeWithUiResult(
    _In_ XAsyncBlock* async
) noexcept;

//-----------------------------------------------------------------------------
// Get token and signature

/// <summary>
/// Gets the appropriate token and signature for an HTTP request.
/// </summary>
/// <param name="user">The user the token and signature are for.</param>
/// <param name="args">The HTTP request details.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <see cref="XalUserGetTokenAndSignatureArgs" />.
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserGetTokenAndSignatureSilentlyAsync(
    _In_ XalUserHandle user,
    _In_ XalUserGetTokenAndSignatureArgs const* args,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Gets the size in bytes of the token and signature buffers.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size in bytes for the result buffer.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserGetTokenAndSignatureSilentlyResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept;

/// <summary>
/// Gets the results of a successful XalUserGetTokenAndSignatureSilentlyAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the buffer for the result object.</param>
/// <param name="buffer">Byte buffer used for result value and its fields.</param>
/// <param name="result">Pointer to the result object.</param>
/// <param name="bufferUsed">The number of bytes in the provided buffer that were used.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// result is a pointer within buffer and does not need to be freed separately.
/// </remarks>
STDAPI XalUserGetTokenAndSignatureSilentlyResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XalUserGetTokenAndSignatureData** result,
    _Out_opt_ size_t* bufferUsed
) noexcept;

/// <summary>
/// This function will show the ui required to resolve certain errors in
/// XalUserGetTokenAndSignatureSilentlyAsync.
/// </summary>
/// <param name="user">The user the token and signature are for.</param>
/// <param name="url">The url of the request that failed.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// This function should only be called after a call to
/// XalUserGetTokenAndSignatureSilentlyAsync fails with E_XAL_UIREQUIRED.
/// </remarks>
STDAPI XalUserResolveIssueWithUiAsync(
    _In_ XalUserHandle user,
    _In_opt_z_ char const* url,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the result of a given XalUserResolveIssueWithUiAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserResolveIssueWithUiResult(
    _In_ XAsyncBlock* async
) noexcept;

//-----------------------------------------------------------------------------
// UCS consent

/// <summary>
/// Checks the state of the given UCS consent for the user.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="consentModelName">The UCS consent model name.</param>
/// <param name="consentState">The state of the consent.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserCheckUcsConsent(
    _In_ XalUserHandle user,
    _In_z_ char const* consentModelName,
    _Out_ XalConsentState* consentState
) noexcept;

/// <summary>
/// Shows ui explaining why the user is missing the given privilege and
/// allows acquiring it.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="consentModelName">The UCS consent model name.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserManageUcsConsentWithUiAsync(
    _In_ XalUserHandle user,
    _In_z_ char const* consentModelName,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Get the result of a given XalUserManageUcsConsentWithUiAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="consentState">The state of the consent.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalUserManageUcsConsentWithUiResult(
    _In_ XAsyncBlock* async,
    _Out_ XalConsentState* consentState
) noexcept;

//-----------------------------------------------------------------------------
// Events

/// <summary>
/// User detail change event handler.
/// </summary>
/// <param name="context">Optional pointer to data used by the event handler.
/// </param>
/// <param name="userId">The local id of the user that changed.</param>
/// <param name="change">The type of change.</param>
/// <returns></returns>
#if !XAL_OS_IMPL
typedef void (XalUserChangeEventHandler)(
    _In_opt_ void* context,
    _In_ XalUserLocalId userId,
    _In_ XalUserChangeType change
);
#else
// XalUserChangeEventHandler is defined in the platform specific header.
// That header is included in xal_types.h
#endif

/// <summary>
/// Register the event handler for user detail changes.
/// </summary>
/// <param name="queue">The async queue the callback should be invoked on.</param>
/// <param name="context">Optional pointer to data used by the event handler.</param>
/// <param name="handler">The event handler, <see cref="XalUserChangeEventHandler"/>.</param>
/// <param name="token">The token for unregistering this callback</param>
/// <returns>Result code for this API operation.  Possible values are S_OK, E_XAL_NOTINITIALIZED, or E_FAIL.</returns>
STDAPI XalUserRegisterChangeEventHandler(
    _In_opt_ XTaskQueueHandle queue,
    _In_opt_ void* context,
    _In_ XalUserChangeEventHandler* handler,
    _Out_ XalRegistrationToken* token
) noexcept;

/// <summary>
/// Unregisters a previously registered callback.
/// </summary>
/// <param name="token">The token returned from
/// XalUserRegisterChangeEventHandler.</param>
/// <returns></returns>
STDAPI_(void) XalUserUnregisterChangeEventHandler(
    _In_ XalRegistrationToken token
) noexcept;

/// <summary>
/// Get a signout deferral.
/// </summary>
/// <param name="deferral">The deferral handle.</param>
/// <returns>Result code for this API operation.</returns>
/// <remarks>
/// May only be called from within a XalUserChangeEventHandler during a
/// XalUserChange_SigningOut event. The signout process will be halted until
/// the deferral handle is closed (or a timeout is reached).
///
/// May fail with E_XAL_DEFERRALNOTAVAILABLE.
/// </remarks>
STDAPI XalUserGetSignoutDeferral(
    _Out_ XalSignoutDeferralHandle* deferral
) noexcept;

/// <summary>
/// Closes a signout deferral.
/// </summary>
/// <param name="deferral">The deferral handle.</param>
/// <returns></returns>
STDAPI_(void) XalUserCloseSignoutDeferral(
    _In_ XalSignoutDeferralHandle deferral
) noexcept;

}
