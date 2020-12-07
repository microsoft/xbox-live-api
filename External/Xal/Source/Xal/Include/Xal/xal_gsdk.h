#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <stdint.h>
#include <XGameErr.h>
#include <XUser.h>

extern "C"
{

//------------------------------------------------------------------------------
// GDK types
//------------------------------------------------------------------------------

#define XAL_OS_IMPL 1

typedef XUserHandle XalUserHandle;

typedef XUserLocalId XalUserLocalId;

/// <summary>
/// Struct that encapsulates the GDK specific arguments for Xal.
/// </summary>
typedef struct XalGsdkArgs {} XalGsdkArgs;

typedef XalGsdkArgs XalInitArgs;

typedef XUserChangeEvent XalUserChangeType;

XUserChangeEvent const XalUserChange_SignedInAgain = XUserChangeEvent::SignedInAgain;
XUserChangeEvent const XalUserChange_SigningOut = XUserChangeEvent::SigningOut;
XUserChangeEvent const XalUserChange_SignedOut = XUserChangeEvent::SignedOut;
XUserChangeEvent const XalUserChange_Gamertag = XUserChangeEvent::Gamertag;
XUserChangeEvent const XalUserChange_GamerPicture = XUserChangeEvent::GamerPicture;
XUserChangeEvent const XalUserChange_Privileges = XUserChangeEvent::Privileges;

typedef XUserChangeEventCallback XalUserChangeEventHandler;

typedef XUserSignOutDeferralHandle XalSignoutDeferralHandle;

}
