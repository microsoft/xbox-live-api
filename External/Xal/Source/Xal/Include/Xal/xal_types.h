#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <stdint.h>

#include <httpClient/pal.h>

#if HC_PLATFORM == HC_PLATFORM_WIN32
#include <Xal/xal_win32.h>
#elif HC_PLATFORM == HC_PLATFORM_UWP
#include <Xal/xal_uwp.h>
#elif HC_PLATFORM == HC_PLATFORM_XDK
#include <Xal/xal_xdk.h>
#elif HC_PLATFORM == HC_PLATFORM_GDK
#include <Xal/xal_gsdk.h>
#elif HC_PLATFORM == HC_PLATFORM_GRTS
#include <Xal/xal_grts.h>
#elif HC_PLATFORM == HC_PLATFORM_ANDROID
#include <Xal/xal_android.h>
#elif HC_PLATFORM_IS_APPLE
#include <Xal/xal_apple.h>
#elif HC_PLATFORM_IS_EXTERNAL
#include <Xal/xal_generic.h>
#else
#error Xal does not recognize this platform, do you need to set HC_PLATFORM = HC_PLATFORM_GENERIC?
#endif

#ifndef XAL_OS_IMPL
#define XAL_OS_IMPL 0
#endif

#ifndef XAL_ENABLE_BACK_COMPAT_SHIMS
#define XAL_ENABLE_BACK_COMPAT_SHIMS 1
#endif

#if HC_PLATFORM == HC_PLATFORM_GRTS
#ifndef FEATURE_ASSERTS_ENABLED
#if defined(DBG)
#define FEATURE_ASSERTS_ENABLED 1
#else
#define FEATURE_ASSERTS_ENABLED 0
#endif
#endif
#endif

extern "C"
{

//------------------------------------------------------------------------------
// Results
//------------------------------------------------------------------------------

#define E_XAL_NOTINITIALIZED                MAKE_E_HC(0x5100L) // 0x89235100
#define E_XAL_ALREADYINITIALIZED            MAKE_E_HC(0x5101L) // 0x89235101
#define E_XAL_USERSETNOTEMPTY               MAKE_E_HC(0x5102L) // 0x89235102
#define E_XAL_USERSETFULL                   MAKE_E_HC(0x5103L) // 0x89235103
#define E_XAL_USERSIGNEDOUT                 MAKE_E_HC(0x5104L) // 0x89235104
#define E_XAL_DUPLICATEDUSER                MAKE_E_HC(0x5105L) // 0x89235105
#define E_XAL_NETWORK                       MAKE_E_HC(0x5106L) // 0x89235106
#define E_XAL_CLIENTERROR                   MAKE_E_HC(0x5107L) // 0x89235107
#define E_XAL_UIREQUIRED                    MAKE_E_HC(0x5108L) // 0x89235108
#define E_XAL_HANDLERALREADYREGISTERED      MAKE_E_HC(0x5109L) // 0x89235109
#define E_XAL_UNEXPECTEDUSERSIGNEDIN        MAKE_E_HC(0x510AL) // 0x8923510A
#define E_XAL_NOTATTACHEDTOJVM              MAKE_E_HC(0x510BL) // 0x8923510B
#define E_XAL_DEVICEUSER                    MAKE_E_HC(0x510CL) // 0x8923510C
#define E_XAL_DEFERRALNOTAVAILABLE          MAKE_E_HC(0x510DL) // 0x8923510D
#define E_XAL_MISSINGPLATFORMEVENTHANDLER   MAKE_E_HC(0x510EL) // 0x8923510E
#define E_XAL_USERNOTFOUND                  MAKE_E_HC(0x510FL) // 0x8923510F
#define E_XAL_NOTOKENREQUIRED               MAKE_E_HC(0x5110L) // 0x89235110
#define E_XAL_NODEFAULTUSER                 MAKE_E_HC(0x5111L) // 0x89235111
#define E_XAL_FAILEDTORESOLVE               MAKE_E_HC(0x5112L) // 0x89235112
#define E_XAL_NOACCOUNTPROVIDER             MAKE_E_HC(0x5113L) // 0x89235113
#define E_XAL_MISMATCHEDTITLEANDCLIENTIDS   MAKE_E_HC(0x5114L) // 0x89235114
#define E_XAL_INVALIDAPPCONFIGURATION       MAKE_E_HC(0x5115L) // 0x89235115
#define E_XAL_MALFORMEDCLIENTID             MAKE_E_HC(0x5116L) // 0x89235116
#define E_XAL_MISSINGCLIENTID               MAKE_E_HC(0x5117L) // 0x89235117
#define E_XAL_MISSINGTITLEID                MAKE_E_HC(0x5118L) // 0x89235118

// E_XAL_INTERNAL_* values should never be returned to callers of XAL.
#define E_XAL_INTERNAL_SWITCHUSER           MAKE_E_HC(0x5171L) // 0x89235171
#define E_XAL_INTERNAL_NOUSERFOUND          MAKE_E_HC(0x5172L) // 0x89235172
#define E_XAL_INTERNAL_TOOMANYCACHEDUSERS   MAKE_E_HC(0x5173L) // 0x89235173
#define E_XAL_INTERNAL_BADUSERTOKEN         MAKE_E_HC(0x5174L) // 0x89235174
#define E_XAL_INTERNAL_BADDEVICEIDENTITY    MAKE_E_HC(0x5175L) // 0x89235175
#define E_XAL_INTERNAL_UNAUTHORIZED         MAKE_E_HC(0x5176L) // 0x89235176
#define E_XAL_INTERNAL_NODISPLAYCLAIMSFOUND MAKE_E_HC(0x5177L) // 0x89235177

// GDK has system definitions for some error values
#if HC_PLATFORM == HC_PLATFORM_GDK || HC_PLATFORM == HC_PLATFORM_GRTS
#undef E_XAL_USERSETFULL
#undef E_XAL_USERSIGNEDOUT
#undef E_XAL_UIREQUIRED
#undef E_XAL_DEFERRALNOTAVAILABLE
#undef E_XAL_USERNOTFOUND
#undef E_XAL_NOTOKENREQUIRED
#undef E_XAL_NODEFAULTUSER
#undef E_XAL_FAILEDTORESOLVE
#undef E_XAL_MISMATCHEDTITLEANDCLIENTIDS
#undef E_XAL_INVALIDAPPCONFIGURATION
#undef E_XAL_MALFORMEDCLIENTID
#undef E_XAL_MISSINGCLIENTID
#undef E_XAL_MISSINGTITLEID

#define E_XAL_USERSETFULL                   E_GAMEUSER_MAX_USERS_ADDED                      // 0x89245100
#define E_XAL_USERSIGNEDOUT                 E_GAMEUSER_SIGNED_OUT                           // 0x89245101
#define E_XAL_UIREQUIRED                    E_GAMEUSER_RESOLVE_USER_ISSUE_REQUIRED          // 0x89245102
#define E_XAL_DEFERRALNOTAVAILABLE          E_GAMEUSER_DEFERRAL_NOT_AVAILABLE               // 0x89245103
#define E_XAL_USERNOTFOUND                  E_GAMEUSER_USER_NOT_FOUND                       // 0x89245104
#define E_XAL_NOTOKENREQUIRED               E_GAMEUSER_NO_TOKEN_REQUIRED                    // 0x89245105
#define E_XAL_NODEFAULTUSER                 E_GAMEUSER_NO_DEFAULT_USER                      // 0x89245106
#define E_XAL_FAILEDTORESOLVE               E_GAMEUSER_FAILED_TO_RESOLVE                    // 0x89245107
#define E_XAL_MISSINGTITLEID                E_GAMEUSER_NO_TITLE_ID                          // 0x89245108
#define E_XAL_INVALIDAPPCONFIGURATION       E_GAMEUSER_INVALID_APP_CONFIGURATION            // 0x89245112
#define E_XAL_MALFORMEDCLIENTID             E_GAMEUSER_MALFORMED_MSAAPPID                   // 0x89245113
#define E_XAL_MISMATCHEDTITLEANDCLIENTIDS   E_GAMEUSER_INCONSISTENT_MSAAPPID_AND_TITLEID    // 0x89245114
#define E_XAL_MISSINGCLIENTID               E_GAMEUSER_NO_MSAAPPID                          // 0x89245115
#endif

//------------------------------------------------------------------------------
// Xal init flags
//------------------------------------------------------------------------------

/// <summary>
/// Flag to instruct Xal to use MPOP behavior when adding users. MPOP behavior
/// allows Xal to rely on cached Xtokens when adding users so refreshing is not
/// required. This lowers the time it takes for add user calls to finish
/// because less network traffic is needed, but it implies that SPOP vetoes
/// will not be checked prior to returning a user.
/// </summary>
/// <remarks>
/// This flag is supported on Win32, Android, iOS, Mac, and generic device types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_USE_MPOP_BEHAVIOR = 1u << 2;

/// <summary>
/// Flag to instruct Xal to use the modern Gamertag features during sign up.
/// </summary>
/// <remarks>
/// This flag is supported on Win32, UWP, Android, iOS, Mac, and generic device types.
/// </remarks>
uint32_t const XAL_INIT_OPTION_REQUEST_MODERN_GAMERTAG_FLOW = 1u << 26;

//------------------------------------------------------------------------------
// Privileges
//------------------------------------------------------------------------------

/// <summary>
/// Enum defining the values for Xbox Live privileges
/// </summary>
typedef enum XalPrivilege
{
    /// <summary>The user can play with people outside of Xbox Live</summary>
    XalPrivilege_CrossPlay              = 185,
    /// <summary>Create/join/participate in Clubs</summary>
    XalPrivilege_Clubs                  = 188,
    /// <summary>Create/join non interactive multiplayer sessions</summary>
    XalPrivilege_Sessions               = 189,
    /// <summary>Broadcast live gameplay</summary>
    XalPrivilege_BroadCast              = 190,
    /// <summary>Change settings to show real name</summary>
    XalPrivilege_ManageProfilePrivacy   = 196,
    /// <summary>Upload GameDVR</summary>
    XalPrivilege_GameDvr                = 198,
    /// <summary>Join parties</summary>
    XalPrivilege_MultiplayerParties     = 203,
    /// <summary>Use Voice Chat in game or in parties</summary>
    XalPrivilege_CommsInGameVoice       = 205,
    /// <summary>Allocate cloud compute resources for their session</summary>
    XalPrivilege_CloudManageSession     = 207,
    /// <summary>Join cloud compute sessions</summary>
    XalPrivilege_CloudJoinSession       = 208,
    /// <summary>Save games on the cloud</summary>
    XalPrivilege_CloudSavedGames        = 209,
    /// <summary>Share progress to social networks</summary>
    XalPrivilege_SocialNetworkSharing   = 220,
    /// <summary>Access user generated content in game</summary>
    XalPrivilege_Ugc                    = 247,
    /// <summary>Use real time voice and text communication with users in their friends list</summary>
    XalPrivilege_CommsFriendsOnly       = 251,
    /// <summary>Use real time voice and text communication with all users</summary>
    XalPrivilege_Comms                  = 252,
    /// <summary>Join multiplayer sessions</summary>
    XalPrivilege_Multiplayer            = 254,
    /// <summary>Add friends / people to follow</summary>
    XalPrivilege_AddFriends             = 255,
} XalPrivilege;

//------------------------------------------------------------------------------
// User api types
//------------------------------------------------------------------------------

/// <summary>
/// Handle to a user object. All operations on a user object are threadsafe.
/// </summary>
/// <remarks>
/// User objects returned by Xal as out parameters already have had their
/// reference count incremented, so XalUserRelease should be called when the
/// caller is done with them.
/// User objects passed as arguments to callbacks did not have their reference
/// count incremented, the callback should call XalUserDuplicateHandle if they
/// wish to hold onto the object (and XalUserCloseHandle when they are done).
/// </remarks>
#if !XAL_OS_IMPL
typedef struct XalUser* XalUserHandle;
#else
// XalUserHandle is defined in the platform specific header.
// That header is included at the top of this file
#endif

/// <summary>
/// Struct holding local user ID data.
/// </summary>
#if !XAL_OS_IMPL
typedef struct XalUserLocalId
{
    /// <summary>The local user ID</summary>
    uint64_t value;
} XalUserLocalId;
#else
// XalUserLocalId is defined in the platform specific header.
// That header is included at the top of this file
#endif

/// <summary>
/// Enum defining the possible states for a user object.
/// </summary>
typedef enum XalUserState
{
    /// <summary>XAL signed in state</summary>
    XalUserState_SignedIn = 0,
    /// <summary>XAL signing out state</summary>
    XalUserState_SigningOut = 1,
    /// <summary>XAL signed out state</summary>
    XalUserState_SignedOut = 2,
} XalUserState;

/// <summary>
/// Enum defining the possible gamer picture sizes.
/// </summary>
typedef enum XalGamerPictureSize
{
    /// <summary>64x64</summary>
    XalGamerPictureSize_Small = 0,
    /// <summary>208x208</summary>
    XalGamerPictureSize_Medium = 1,
    /// <summary>424x424</summary>
    XalGamerPictureSize_Large = 2,
    /// <summary>1080x1080</summary>
    XalGamerPictureSize_ExtraLarge = 3,
} XalGamerPictureSize;

/// <summary>
/// Enum defining the various gamertag components.
/// </summary>
typedef enum XalGamertagComponent
{
    /// <summary>The classic gamertag</summary>
    XalGamertagComponent_Classic = 0,
    /// <summary>The modern gamertag without the suffix</summary>
    XalGamertagComponent_Modern = 1,
    /// <summary>The modern gamertag suffix if the user has one (otherwise empty)</summary>
    XalGamertagComponent_ModernSuffix = 2,
    /// <summary>The combined modern gamertag with the suffix (if the suffix exists)</summary>
    XalGamertagComponent_UniqueModern = 3,
} XalGamertagComponent;

/// <summary>
/// Enum defining the various age groups.
/// </summary>
typedef enum XalAgeGroup
{
    /// <summary>Unknown age group</summary>
    XalAgeGroup_Unknown = 0,
    /// <summary>Child age group</summary>
    XalAgeGroup_Child = 1,
    /// <summary>Teen age group</summary>
    XalAgeGroup_Teen = 2,
    /// <summary>Adult age group</summary>
    XalAgeGroup_Adult = 3,
} XalAgeGroup;

/// <summary>
/// Enum defining the various reasons for a privilege being denied.
/// </summary>
typedef enum XalPrivilegeCheckDenyReasons
{
    /// <summary>None</summary>
    XalPrivilegeCheckDenyReasons_None = 0,
    /// <summary>Purchase required</summary>
    XalPrivilegeCheckDenyReasons_PurchaseRequired = 1,
    /// <summary>Restricted</summary>
    XalPrivilegeCheckDenyReasons_Restricted = 2,
    /// <summary>Banned</summary>
    XalPrivilegeCheckDenyReasons_Banned = 3,

    /// <summary>Unknown</summary>
    XalPrivilegeCheckDenyReasons_Unknown = 0xFFFFFFFF
} XalPrivilegeCheckDenyReasons;

//-----------------------------------------------------------------------------
// Get token and signature

/// <summary>
/// Struct that represents an HTTP header.
/// </summary>
typedef struct XalHttpHeader
{
    /// <summary>HTTP header name</summary>
    _Field_z_ char const* name;

    /// <summary>HTTP header value</summary>
    _Field_z_ char const* value;
} XalHttpHeader;

/// <summary>
/// Struct that encapsulates the arguments for
/// XalUserGetTokenAndSignatureSilentlyAsync.
/// </summary>
/// <remarks>
/// Xal will copy the data before XalUserGetTokenAndSignatureSilentlyAsync
/// returns.
/// </remarks>
typedef struct XalUserGetTokenAndSignatureArgs
{
    /// <summary>
    /// The method for the request
    /// </summary>
    _Field_z_ char const* method;

    /// <summary>
    /// The url to get the token and to signature for (fully escaped).
    /// </summary>
    _Field_z_ char const* url;

    /// <summary>
    /// The number of headers that will be added to the HTTP request.
    /// </summary>
    uint32_t headerCount;

    /// <summary>
    /// The array of headers that will be added to the HTTP request.
    /// </summary>
    _Field_size_(headerCount) XalHttpHeader const* headers;

    /// <summary>
    /// The size of the request body in bytes.
    /// </summary>
    size_t bodySize;

    /// <summary>
    /// The request body.
    /// </summary>
    _Field_size_bytes_(bodySize) uint8_t const* body;

    /// <summary>
    /// Ignore cached tokens.
    /// </summary>
    /// <remarks>
    /// This flag should only be set if an http request using a token and
    /// signature failed with a 401 error. In that case the entire call should
    /// be retried after getting a new token and signature using this flag.
    /// </remarks>
    bool forceRefresh;

    /// <summary>
    /// Get a token for all users.
    /// </summary>
    bool allUsers;
} XalUserGetTokenAndSignatureArgs;

/// <summary>
/// Struct that encapsulates the results for
/// XalUserGetTokenAndSignatureSilentlyAsync.
/// </summary>
typedef struct XalUserGetTokenAndSignatureData
{
    /// <summary>
    /// The size of the Token string in bytes including the null terminator.
    /// </summary>
    size_t tokenSize;

    /// <summary>
    /// The size of the Signature string in bytes including the null terminator.
    /// </summary>
    size_t signatureSize;

    /// <summary>
    /// The token for the request, if necessary, as a null terminated string.
    /// </summary>
    _Field_size_opt_(tokenSize) _Null_terminated_ char const* token;

    /// <summary>
    /// The signature for the request, if necessary, as a null terminated
    /// string.
    /// </summary>
    _Field_size_opt_(signatureSize) _Null_terminated_ char const* signature;
} XalUserGetTokenAndSignatureData;

//-----------------------------------------------------------------------------
// Events

/// <summary>
/// Enum describing the possible types of changes to a user's details.
/// </summary>
#if !XAL_OS_IMPL
typedef enum XalUserChangeType
{
    /// <summary>Changed to signed in</summary>
    XalUserChange_SignedInAgain = 0,
    /// <summary>Changed to signing out</summary>
    XalUserChange_SigningOut = 1,
    /// <summary>Changed to signed out</summary>
    XalUserChange_SignedOut = 2,
    /// <summary>Changed gamertag</summary>
    XalUserChange_Gamertag = 3,
    /// <summary>Changed gamer picture</summary>
    XalUserChange_GamerPicture = 4,
    /// <summary>Changed privileges</summary>
    XalUserChange_Privileges = 5,
} XalUserChangeType;
#else
// XalUserChangeType is defined in the platform specific header.
// That header is included at the top of this file
#endif

/// <summary>
/// A token returned when registering a callback to identify the registration. This token
/// is later used to unregister the callback.
/// </summary>
struct XalRegistrationToken
{
    /// <summary>The registration token</summary>
    uint64_t token;
};

/// <summary>
/// Handle to a deferral object.
/// </summary>
#if !XAL_OS_IMPL
typedef struct XalSignoutDeferral* XalSignoutDeferralHandle;
#else
// XalSignoutDeferralHandle is defined in the platform specific header.
// That header is included at the top of this file
#endif

//------------------------------------------------------------------------------
// Date & time

/// <summary>
/// This struct represents a date.
/// </summary>
/// <remarks>
/// The date is always in the Gregorian calendar and in the UTC timezone.
/// </remarks>
typedef struct XalTimestampComponents
{
    /// <summary>
    /// The year.
    /// </summary>
    uint16_t year;

    /// <summary>
    /// The month [1, 12].
    /// </summary>
    uint8_t month;

    /// <summary>
    /// The day of the month [1, 31].
    /// </summary>
    uint8_t day;

    /// <summary>
    /// The hour in the day [0, 24).
    /// </summary>
    uint8_t hour;

    /// <summary>
    /// The minute in the hour [0, 60).
    /// </summary>
    uint8_t minute;

    /// <summary>
    /// The second in the minute [0, 60).
    /// </summary>
    uint8_t second;
} XalTimestampComponents;

}
