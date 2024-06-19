#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

extern "C"
{

//------------------------------------------------------------------------------
// Win32 types
//------------------------------------------------------------------------------

/// <summary>
/// Struct that encapsulates the Win32 specific arguments for Xal.
/// </summary>
typedef struct XalWin32Args
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
    /// Xbox Live sandbox.
    /// </summary>
    /// <remarks>
    /// If Xal detects a sandbox key in the registry it will use that value and
    /// ignore the value passed in here.
    /// </remarks>
    _Field_z_ char const* sandbox;

    /// <summary>
    /// A bool indicating whether Xal can send diagnostic telemetry.
    /// Setting this to true indicates to Xal that it does not have user consent
    /// to report data about any crashes or errors it encounters during use.
    /// If this variable is set to false, Xal assumes it can report this data.
    /// </summary>
    bool disableDiagnosticTelemetry;

    /// <summary>
    /// A correlation vector string for XAL to use as a base. XAL will extend
    /// this prior to using it. This argument is optional.
    /// </summary>
    _Field_z_ char const* correlationVector;

    /// <summary>
    /// Xal configuration flags.
    /// </summary>
    uint32_t flags;

    /// <summary>
    /// The number of consents present in the ThirdPartyConsents array.
    /// </summary>
    uint32_t thirdPartyConsentCount;

    /// <summary>
    /// An optional list of consent requests to access Xbox Live services.
    /// </summary>
    _Field_size_(thirdPartyConsentCount) char const** thirdPartyConsents;

    /// <summary>
    /// Win32 optional custom redirect URI.
    /// </summary>
    _Field_z_ char const* redirectUri;

    /// <summary>
    /// The number of consents present in the ucsConsents array
    /// </summary>
    uint32_t ucsConsentCount;

    /// <summary>
    /// An optional list of consent requests to UCS
    /// </summary>
    _Field_size_(ucsConsentCount) char const** ucsConsents;
} XalWin32Args;

typedef XalWin32Args XalInitArgs;

#define XAL_PLATFORM "Win32"

}
