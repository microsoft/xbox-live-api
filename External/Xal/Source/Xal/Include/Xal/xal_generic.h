#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

extern "C"
{

//------------------------------------------------------------------------------
// Generic types
//------------------------------------------------------------------------------

typedef struct XalGenericDeviceInfo
{
    _Field_z_ char const* deviceType;
    _Field_z_ char const* osVersion;
    _Field_z_ char const* deviceId;
} XalGenericDeviceInfo;

// TODO collapse the 2 structs? the only thing missing from this one is the
// device type, or remove dupes from this struct and ignore it unless telemetry
// is enabled?
typedef struct XalGenericTelemetryInfo
{
    _Field_z_ char const* appId; // TODO do we want to just use the titleId here? or the msa app id?
    _Field_z_ char const* appVer;
    _Field_z_ char const* osName; // TODO use the device type?
    _Field_z_ char const* osVersion; // TODO dupe
    _Field_z_ char const* osLocale;
    _Field_z_ char const* deviceClass; // TODO use the device type?
    _Field_z_ char const* deviceId; // TODO dupe
} XalGenericTelemetryInfo;

/// <summary>
/// Struct that encapsulates the extra information Xal needs.
/// </summary>
typedef struct XalGenericArgs
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
    /// Reserved for future use.
    /// </summary>
    uint32_t flags;

    /// <summary>
    /// The maximum number of users that can be signed in at the same time.
    /// </summary>
    uint32_t maxSignedInUsers;

    /// <summary>
    /// The number of consents present in the ThirdPartyConsents array.
    /// </summary>
    uint32_t thirdPartyConsentCount;

    /// <summary>
    /// An optional list of consent requests to access Xbox Live services.
    /// </summary>
    _Field_size_(thirdPartyConsentCount) char const** thirdPartyConsents;

    /// <summary>
    /// Custom redirect URI.
    /// </summary>
    _Field_z_ char const* redirectUri;

    bool useRemoteAuth;

    XalGenericDeviceInfo deviceInfo;
    XalGenericTelemetryInfo telemetryInfo;
} XalGenericArgs;

typedef XalGenericArgs XalInitArgs;

#define XAL_PLATFORM "Generic"

}
