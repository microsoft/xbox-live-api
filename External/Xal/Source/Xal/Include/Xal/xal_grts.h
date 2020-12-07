#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <xgameerr.h> // for the HRESULTS so we can define the Xal ones to match (in xal_types.h)

extern "C"
{

//------------------------------------------------------------------------------
// Grts types
//------------------------------------------------------------------------------

struct XalPlatformHooks;

/// <summary>
/// Struct that encapsulates the extra information Xal needs.
/// </summary>
typedef struct XalGrtsArgs
{
    /// <summary>
    /// MSA client id.
    /// </summary>
    _Field_z_ char const* clientId;

    /// <summary>
    /// Xbox Live title id.
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// The MsaFullTrust value from microsoftgame.config
    /// </summary>
    bool msaFullTrust;

    XalPlatformHooks* hooks;
} XalGrtsArgs;

typedef XalGrtsArgs XalInitArgs;

#define XAL_PLATFORM "GRTS"

/// <summary>
/// Enum defining the MSA WAM UI modes.
/// </summary>
typedef enum MsaWamUiMode
{
    /// <summary>Sign In</summary>
    MsaWamUiMode_SignIn = 0,
    /// <summary>Resolve Issue</summary>
    MsaWamUiMode_ResolveIssue = 1
} MsaWamUiMode;

}
