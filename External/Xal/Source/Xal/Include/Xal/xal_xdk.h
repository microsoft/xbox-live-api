#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

extern "C"
{

//------------------------------------------------------------------------------
// Xdk types
//------------------------------------------------------------------------------

/// <summary>
/// Struct that encapsulates the Xdk specific arguments for Xal.
/// </summary>
typedef struct XalXdkArgs {} XalXdkArgs;

typedef XalXdkArgs XalInitArgs;

#define XAL_PLATFORM "XDK"

}
