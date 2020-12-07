#pragma once

#include <httpClient/config.h>

#if !defined(__cplusplus)
#error C++11 required
#endif

extern "C"
{

//------------------------------------------------------------------------------
// Apple types
//------------------------------------------------------------------------------

/// <summary>
/// Struct that encapsulates the Apple specific arguments for Xal.
/// </summary>
typedef struct XalAppleArgs
{
    /// <summary>
    /// MSA client id
    /// </summary>
    _Field_z_ char const* clientId;

    /// <summary>
    /// Xbox Live title id
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// Xbox Live sandbox
    /// </summary>
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
    /// The app custom redirect URI. (Optional on macOS).
    /// </summary>
    _Field_z_ char const* redirectUri;
} XalAppleArgs;

typedef XalAppleArgs XalInitArgs;

#if HC_PLATFORM == HC_PLATFORM_IOS
#define XAL_PLATFORM "iOS"
#elif HC_PLATFORM == HC_PLATFORM_MAC
#define XAL_PLATFORM "macOS"
#endif

}
