#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

// Attention: This file is intended for internal uses only.
// Its use is not recommended and not supported.

#include <httpClient/async.h>
#include <Xal/xal_types.h>

extern "C"
{

//-----------------------------------------------------------------------------
// User properties

/// <summary>
/// Returns the size of the buffer needed to store the web account ID string.
/// </summary>
/// <param name="user">The user object.</param>
STDAPI_(size_t) XalUserGetWebAccountIdSize(
    _In_ XalUserHandle user
) noexcept;

/// <summary>
/// Returns the web account ID of the user.
/// </summary>
/// <param name="user">The user object.</param>
/// <param name="webAccountIdSize">The size in bytes of the webAccountId buffer.
/// Should be the value returned by XalUserGetWebAccountIdSize.</param>
/// <param name="webAccountId">The buffer the web account ID will be written to.
/// </param>
/// <param name="webAccountIdUsed">The number of bytes used in the buffer
/// including the null terminator.</param>
STDAPI XalUserGetWebAccountId(
    _In_ XalUserHandle user,
    _In_ size_t webAccountIdSize,
    _Out_writes_(webAccountIdSize) char* webAccountId,
    _Out_opt_ size_t* webAccountIdUsed
) noexcept;

//-----------------------------------------------------------------------------
// Get web account token

/// <summary>
/// Struct that represents a parameter that has a key and a value for web account APIs.
/// </summary>
typedef struct XalWebAccountParameter
{
    /// <summary>Parameter name.</summary>
    _Field_z_ char const* name;

    /// <summary>Parameter value.</summary>
    _Field_z_ char const* value;
} XalWebAccountParameter;

/// <summary>
/// Struct that encapsulates the arguments for XalUserGetWebAccountTokenSilentlyAsync.
/// </summary>
/// <remarks>
/// Xal will copy the data before XalUserGetWebAccountTokenSilentlyAsync returns.
/// </remarks>
typedef struct XalUserGetWebAccountTokenArgs
{
    /// <summary>
    /// The token scope string being requested.
    /// </summary>
    _Field_z_ char const* Scope;

    /// <summary>
    /// Ignore cached tokens.
    /// </summary>
    /// <remarks>
    /// This flag should only be set if an http request using a previously
    /// fetched token failed with a 401 error. In that case the entire call
    /// should be retried after getting a new token using this flag.
    /// </remarks>
    bool ForceRefresh;

    /// <summary>
    /// The number of request parameters that will be added to the token request.
    /// </summary>
    uint32_t parameterCount;

    /// <summary>
    /// The array of request parameters that will be added to the token request.
    /// </summary>
    _Field_size_(parameterCount) XalWebAccountParameter const* requestParameters;
} XalUserGetWebAccountTokenArgs;

/// <summary>
/// Gets a token with the specified scope for the user without showing UI.
/// </summary>
/// <param name="user">The user the token is for.</param>
/// <param name="args">The requested token details.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <see cref="XalUserGetWebAccountTokenArgs" />.
STDAPI XalUserGetWebAccountTokenSilentlyAsync(
    _In_ XalUserHandle user,
    _In_ XalUserGetWebAccountTokenArgs const* args,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Gets the size in bytes of the web account token result buffer.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size in bytes for the result buffer.</param>
STDAPI XalUserGetWebAccountTokenSilentlyResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept;

/// <summary>
/// Gets the results of a successful XalUserGetWebAccountTokenSilentlyAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the buffer for the result object.
/// </param>
/// <param name="result">The result token.</param>
STDAPI XalUserGetWebAccountTokenSilentlyResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_z_(bufferSize) char* result
) noexcept;

/// <summary>
/// Gets a token with the specified scope for the user showing UI if necessary.
/// </summary>
/// <param name="user">The user the token is for.</param>
/// <param name="args">The requested token details.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <see cref="XalUserGetWebAccountTokenArgs" />.
STDAPI XalUserGetWebAccountTokenWithUiAsync(
    _In_ XalUserHandle user,
    _In_ XalUserGetWebAccountTokenArgs const* args,
    _In_ XAsyncBlock* async
) noexcept;

/// <summary>
/// Gets the size in bytes of the web account token result buffer.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size in bytes for the result buffer.</param>
STDAPI XalUserGetWebAccountTokenWithUiResultSize(
    _In_ XAsyncBlock* async,
    _Out_ size_t* bufferSize
) noexcept;

/// <summary>
/// Gets the results of a successful XalUserGetWebAccountTokenWithUiAsync operation.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the buffer for the result object.
/// </param>
/// <param name="result">The result token.</param>
STDAPI XalUserGetWebAccountTokenWithUiResult(
    _In_ XAsyncBlock* async,
    _In_ size_t bufferSize,
    _Out_writes_z_(bufferSize) char* result
) noexcept;

}
