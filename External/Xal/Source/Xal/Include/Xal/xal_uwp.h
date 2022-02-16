#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

extern "C"
{

//------------------------------------------------------------------------------
// Uwp types
//------------------------------------------------------------------------------

/// <summary>
/// Struct that encapsulates the Uwp specific arguments for Xal.
/// </summary>
typedef struct XalUwpArgs
{
    /// <summary>
    /// Xbox Live title id.
    /// </summary>
    uint32_t titleId;

    /// <summary>
    /// The package family name.
    /// </summary>
    _Field_z_ char const* packageFamilyName;

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
    /// The user that launched the application, as provided by Application::OnLaunched()
    /// </summary>
    /// <remarks>
    /// If left null, Xal will bring up the User Picker on Xbox when signing in with UI.
    /// This field has no effect on the PC sign in flow.
    /// </remarks>
    Windows::System::User^ launchUser;

    /// <summary>
    /// The hwnd of the window that launched the sign in request
    /// </summary>
    /// <remarks>
    /// If a centennial build, this is required for identifying the main window that
    /// launched a sign in request. Otherwise this is unused.
    /// </remarks>
    HWND mainWindow;

    /// <summary>
    /// Optional Cobrand ID for MSA
    /// </summary>
    _Field_z_ char const* cobrandId;
} XalUwpArgs;

typedef XalUwpArgs XalInitArgs;

#define XAL_PLATFORM "UWP"

}
