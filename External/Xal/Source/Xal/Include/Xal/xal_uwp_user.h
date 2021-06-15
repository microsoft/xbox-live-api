#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <Xal/xal_types.h>
#include <Xal/xal_user.h>

extern "C"
{

//------------------------------------------------------------------------------
// Uwp user functions
//------------------------------------------------------------------------------
STDAPI XalUserGetPlatformWebAccount(
    _In_ XalUserHandle user,
    _Out_ Windows::Security::Credentials::WebAccount^* webAccount
) noexcept;

/// <summary>
/// Adds the given system user.
/// </summary>
/// <param name="user">The system user.</param>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>Result code for this API operation.</returns>
STDAPI XalAddUwpSystemUserSilentAsync(
    _In_ Windows::System::User^ user,
    _In_ XAsyncBlock* async
) noexcept;


}
