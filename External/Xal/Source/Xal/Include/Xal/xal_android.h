#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

#include <jni.h>

extern "C"
{

//------------------------------------------------------------------------------
// Important Information About Xal For Android
//------------------------------------------------------------------------------
//
// Xal on Android has several hard requirements for how it is used with JNI.
// Xal functions may fail if these requirements are not met.
//
// First and foremost, XalInitialize must be called from a thread which 
// originated from Java. If this does not happen, Xal will not be able to load
// any of the java classes its Android implementation depends on.
//
// Xal also expects the client to be responsible for attaching and detaching
// threads to the Java Virtual Machine. Any thread Xal is running on for 
// Android should be attached to the Java VM. This is done to save the cost of 
// repeatedly attaching and detaching to the Java VM. 
// The recommended pattern for threads that may need to do JNI calls, including
// the ones that Xal will be run on, is to attach when the thread starts and 
// detach just before the thread terminates.
//
// Xal is configured to work with JNI_VERSION_1_6. Therefore this should be the 
// return value of the JNI_OnLoad value exported by the native library which
// consumes Xal. More information about JNI_OnLoad can be found in the JNI 
// invocation documentation.

//------------------------------------------------------------------------------
// Android types
//------------------------------------------------------------------------------

/// <summary>
/// Struct that encapsulates the Android specific settings for Xal.
/// </summary>
typedef struct XalAndroidArgs
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
    /// The JavaVM for the application consuming Xal.
    /// </summary>
    JavaVM* javaVM;

    /// <summary>
    /// The Android App context for the application consuming Xal.
    /// </summary>
    jobject appContext;

    /// <summary>
    /// The number of consents present in the ThirdPartyConsents array.
    /// </summary>
    uint32_t thirdPartyConsentCount;

    /// <summary>
    /// An optional list of consent requests to access Xbox Live services.
    /// </summary>
    _Field_size_(thirdPartyConsentCount) char const** thirdPartyConsents;

    /// <summary>
    /// The Android App custom redirect URI.
    /// </summary>
    _Field_z_ char const* redirectUri;

} XalAndroidArgs;

typedef XalAndroidArgs XalInitArgs;

#define XAL_PLATFORM "Android"

}
