#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

// Attention: This file is intended for internal uses only.
// Its use is not recommended and not supported.

#include <Xal/xal_types.h>

extern "C"
{
//-----------------------------------------------------------------------------
// Marketing State
//-----------------------------------------------------------------------------

/// <summary>
/// Enum defining the various marketing states.
/// </summary>
typedef enum XalMarketingState
{
    /// <summary>Existing user</summary>
    XalMarketingState_ExistingUser = 0,
    /// <summary>User went through account creation</summary>
    XalMarketingState_NewUser = 1,
    /// <summary>User went through account creation and saw the first party marketing notice</summary>
    XalMarketingState_NewUserFirstPartyNotice = 2,
} XalMarketingState;

STDAPI XalUserGetMarketingState(
    _In_ XalUserHandle user,
    _Out_ XalMarketingState* marketingState
) noexcept;

}
