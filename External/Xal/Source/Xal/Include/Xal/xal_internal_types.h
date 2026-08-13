#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

// Attention: This file is intended for internal uses only.
// Its use is not recommended and not supported.

#include <Xal/xal_types.h>

extern "C"
{

//------------------------------------------------------------------------------
// Xal init flags
//------------------------------------------------------------------------------

/// <summary>
/// Flag that instructs Xal to attempt to use cached MSA refresh tokens from Xsapi to
/// silently sign existing users in. Do not use this flag unless you have been
/// instructed to do so.
/// </summary>
/// <remarks>
/// This flag is supported on Android and iOS device types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_MIGRATE_XSAPI_USER_TOKENS = 1u;

/// <summary>
/// Flag that forces Xal to use the remote auth flow.
/// </summary>
/// <remarks>
/// This flag is supported on Win32 device types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_USE_REMOTE_FLOW = 1u << 22;

/// <summary>
/// Flag that instructs Xal to allow the querying of consents without passing
/// them in the consent list. This is useful for scenarios where the app does
/// not want to prompt the user for a specific consent, but still wants to query
/// the consent.
/// </summary>
/// <remarks>
/// This flag is supported on Android and iOS device types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_ALLOW_MANAGE_CONSENT_WITHOUT_PROMPT = 1u << 23;

/// <summary>
/// Flag that instructs Xal that your app is set up for FOCI token sharing and
/// to attempt to use FOCI SSO. If your app is not set up for FOCI do not set
/// this flag.
/// </summary>
/// <remarks>
/// This flag is supported on Win32, Android, iOS, Mac, and generic device
/// types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_USE_FOCI = 1u << 24;

/// <summary>
/// Flag that provides a hint to Xal that your app is allowed to sign out the
/// user on the UWP platform. Note that setting this does not give your app
/// permission to sign out the user, but only acts as clue for Xal that you have
/// that permission.
/// </summary>
/// <remarks>
/// This flag is supported on UWP device types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_ALLOW_UWP_SIGNOUT = 1u << 25;

/// <summary>
/// Flag to disable SSO browser usage on platforms where a secure browser
/// exists. This flag is provided for testing purposes only.
/// </summary>
/// <remarks>
/// This flag is supported on Android and iOS device types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_USE_IN_PROC_BROWSER = 1u << 27;

/// <summary>
/// Flag to tell XAL to use file storage for token storage on Win32 platforms
/// instead of the Windows Credential Manager. Using this flag will break many
/// SSO scenarios.
/// </summary>
/// <remarks>
/// This flag is supported on Win32 device types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_WIN32_USE_FILE_STORAGE = 1u << 28;

/// <summary>
/// Flag to instruct Xal to use the beta Xbox service cloud.
/// </summary>
/// <remarks>
/// This flag is supported on Win32, Android, iOS, Mac, and generic device
/// types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_USE_BETA_SERVICES_FLAG = 1u << 29;

/// <summary>
/// Flag to instruct Xal to use the 1st party auth flow.
/// </summary>
/// <remarks>
/// This flag is supported on Win32, UWP, Android, iOS, Mac, and generic device
/// types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_TITLE_TYPE_FIRST_PARTY_FLAG = 1u << 31;

//------------------------------------------------------------------------------
// Age verification status
//------------------------------------------------------------------------------

/// <summary>
/// The possible states of the user in the age verification system
/// </summary>
typedef enum XalAgeVerificationStatus
{
    /// <summary>
    /// The user state is unknown to the age verification system
    /// </summary>
    /// <remarks>
    /// This status may be caused by a variety of situations (including errors)
    /// but usually means that AION has no information about the user. Call
    /// XalUserDoAgeVerificationWithUiResult so that they will be processed by
    /// the system
    /// </remarks>
    XalAgeVerificationStatus_NoData = 0,
 
    /// <summary>
    /// The user is not required to perform any age verification
    /// </summary>
    XalAgeVerificationStatus_NotApplicable = 1,

    /// <summary>
    /// The user does not meet the age requirements
    /// </summary>
    /// <remarks>
    /// This may also mean the user decided to skip verification in some
    /// circumstances
    /// </remarks>
    XalAgeVerificationStatus_DoesNotMeetPolicy = 2,

    /// <summary>
    /// The user has chosen to not participate in age verification
    /// </summary>
    XalAgeVerificationStatus_OptedOut = 3,

    /// <summary>
    /// The user is verified to meet the age requirements
    /// </summary>
    XalAgeVerificationStatus_VerifiedToMeetPolicy = 4,
} XalAgeVerificationStatus;

}
