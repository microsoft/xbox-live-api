// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once
#if HC_PLATFORM != HC_PLATFORM_ANDROID
#pragma warning(disable: 4265)
#pragma warning(disable: 4266)
#pragma warning(disable: 4062)
#endif

#if (!defined(HC_LINK_STATIC) || HC_LINK_STATIC == 0) && HC_PLATFORM_IS_APPLE
#include <HttpClient/XAsync.h>
#else
#include <XAsync.h>
#endif

extern "C"
{

/////////////////////////////////////////////////////////////////////////////////////////
// Memory APIs
//

/// <summary>
/// A callback invoked every time a new memory buffer must be dynamically allocated by the library.  
/// This callback is optionally installed by calling XblMemSetFunctions().
/// </summary>
/// <param name="size">The size of the allocation to be made.  
/// This value will never be zero.</param>
/// <param name="memoryType">An opaque identifier representing the 
/// internal category of memory being allocated.</param>
/// <returns>A pointer to an allocated block of memory of the specified size, 
/// or a null pointer if allocation failed.</returns>
/// <remarks>
/// The callback must allocate and return a pointer to a contiguous block of memory of the 
/// specified size that will remain valid until the app's corresponding XblMemFreeFunction 
/// callback is invoked to release it.  
/// Every non-null pointer returned by this method will be subsequently passed to the corresponding 
/// XblMemFreeFunction callback once the memory is no longer needed.
/// </remarks>
typedef _Ret_maybenull_ _Post_writable_byte_size_(size) void*
(STDAPIVCALLTYPE* XblMemAllocFunction)(
    _In_ size_t size,
    _In_ HCMemoryType memoryType
);

/// <summary>
/// A callback invoked every time a previously allocated memory buffer is no longer needed by 
/// the library and can be freed.  
/// This callback is optionally installed by calling XblMemSetFunctions().
/// </summary>
/// <param name="pointer">The pointer to the memory buffer previously allocated.  
/// This value will never be a null pointer.</param>
/// <param name="memoryType">An opaque identifier representing the internal category of 
/// memory being allocated.</param>
/// <returns></returns>
/// <remarks>
/// The callback is invoked whenever the library has finished using a memory buffer previously 
/// returned by the app's corresponding XblMemAllocFunction such that the application can free the
/// memory buffer.
/// </remarks>
typedef void (STDAPIVCALLTYPE* XblMemFreeFunction)(
    _In_ _Post_invalid_ void* pointer,
    _In_ HCMemoryType memoryType
);

/// <summary>
/// Optionally sets the memory hook functions to allow callers to control route memory 
/// allocations to their own memory manager.
/// </summary>
/// <param name="memAllocFunc">A pointer to the custom allocation callback to use, or a null 
/// pointer to restore the default.</param>
/// <param name="memFreeFunc">A pointer to the custom freeing callback to use, or a null 
/// pointer to restore the default.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This must be called before XblInitialize() and can not be called again until XblCleanup().  
/// This method allows the application to install custom memory allocation routines in order 
/// to service all requests for new memory buffers instead of using default allocation routines.  
/// The <paramref name="memAllocFunc"/> and <paramref name="memFreeFunc"/> parameters can be null
/// pointers to restore the default routines.  
/// Both callback pointers must be null or both must be non-null.  
/// Mixing custom and default routines is not permitted.
/// </remarks>
STDAPI XblMemSetFunctions(
    _In_opt_ XblMemAllocFunction memAllocFunc,
    _In_opt_ XblMemFreeFunction memFreeFunc
) XBL_NOEXCEPT;

/// <summary>
/// Gets the memory hook functions to allow callers to control route memory allocations to their 
/// own memory manager.
/// </summary>
/// <param name="memAllocFunc">Set to the current allocation callback.  
/// Returns the default routine if not previously set.</param>
/// <param name="memFreeFunc">Set to the current memory free callback.  
/// Returns the default routine if not previously set.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This method allows the application get the default memory allocation routines.  
/// This can be used along with XblMemSetFunctions() to monitor all memory allocations.
/// </remarks>
STDAPI XblMemGetFunctions(
    _Out_ XblMemAllocFunction* memAllocFunc,
    _Out_ XblMemFreeFunction* memFreeFunc
) XBL_NOEXCEPT;

/////////////////////////////////////////////////////////////////////////////////////////
// Global APIs
//

/// <summary>
/// Defines values representing the Xbox Live initialization arguments.
/// </summary>
typedef struct XblInitArgs
{
    /// <summary>
    /// Queue used for XSAPI internal asynchronous work (telemetry, rta, etc.).  
    /// This field if optional - if not provided, a threadpool based queue will be used.
    /// </summary>
    XTaskQueueHandle queue;

#if !(HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP)

    /// <summary>
    /// The Service Configuration ID (SCID) for the app.
    /// You can find it on Partner Center in the Game Setup page under Identity details.
    /// This string is considered case sensitive so paste it directly from the Partner Center
    /// </summary>
    _Field_z_ const char* scid;

#endif

#if HC_PLATFORM == HC_PLATFORM_ANDROID

    /// <summary>
    /// A required reference to the application's Java VM.
    /// </summary>
    JavaVM* javaVM;

    /// <summary>
    /// A required reference to an instance of the application's context 
    /// provided by JNI.
    /// </summary>
    jobject applicationContext;

#endif

#if HC_PLATFORM == HC_PLATFORM_IOS

    /// <summary>
    /// An optional reference to the iOS APNS environment.  
    /// This field is required if the app is integrating with notifications.
    /// </summary>
    _Field_z_ const char* apnsEnvironment;

#endif

#if HC_PLATFORM == HC_PLATFORM_WIN32

    /// <summary>
    /// Local storage location for XSAPI.  
    /// Used to cache data platform events in the event that they cannot be uploaded due to connectivity problems.  
    /// This field is required and if custom local storage hooks are not set with 
    /// XblLocalStorageSetHandlers, otherwise it will be ignored.
    /// </summary>
    _Field_z_ const char* localStoragePath;

#endif

#if HC_PLATFORM_IS_EXTERNAL

    /// <summary>
    /// The id of the app.
    /// </summary>
    _Field_z_ char const* appId;

    /// <summary>
    /// App version.
    /// </summary>
    _Field_z_ char const* appVer;

    /// <summary>
    /// The os the app is running on.
    /// </summary>
    _Field_z_ char const* osName;

    /// <summary>
    /// The version of the os.
    /// </summary>
    _Field_z_ char const* osVersion;

    /// <summary>
    /// The locale the os is using.
    /// </summary>
    _Field_z_ char const* osLocale;

    /// <summary>
    /// The device type the app is running on.
    /// </summary>
    _Field_z_ char const* deviceClass;

    /// <summary>
    /// The id of the device.
    /// </summary>
    _Field_z_ char const* deviceId;

#endif
} XblInitArgs;

/// <summary>
/// Initializes the library instance.
/// </summary>
/// <param name="args">Platform-specific args for XblInitialize.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This must be called before any other Xbl* method, except for XblMemSetFunctions() and XblMemGetFunctions().  
/// Should have a corresponding call to XblCleanup().
/// </remarks>
STDAPI XblInitialize(_In_ const XblInitArgs* args) XBL_NOEXCEPT;

/// <summary>
/// Immediately reclaims all resources associated with the library.
/// </summary>
/// <param name="async">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If you called XblMemSetFunctions(), call this before shutting down your app's memory manager.  
/// It is the responsibility of the game to wait for any outstanding Async calls to complete before calling XblCleanup.  
/// If there are background async tasks started by XSAPI pending, this API will wait for them to complete.
/// </remarks>
STDAPI XblCleanupAsync(
    XAsyncBlock* async
) XBL_NOEXCEPT;

/// <summary>
/// Gets the async queue that is used for XSAPI's internal asynchronous operations.
/// </summary>
/// <returns>Returns the async queue being used.</returns>
/// <remarks>
/// Note that this queue will be derived from the queue passed in during XblInitialize, not the exact same one.  
/// Xsapi will call XTaskQueueDuplicateHandle before returning the queue, so XTaskQueueCloseHandle must be called
/// later by callers.
/// </remarks>
STDAPI_(XTaskQueueHandle) XblGetAsyncQueue() XBL_NOEXCEPT;

/// <summary>
/// Get the service configuration Id for the application.  
/// This is set during XblInitialize.
/// </summary>
/// <param name="scid">The service configuration Id for the app.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This string will be valid until XblCleanup is called.
/// </remarks>
STDAPI XblGetScid(
    _Out_ const char** scid
) XBL_NOEXCEPT;

/// <summary>
/// Defines the config settings value that is passed to the below API's.
/// </summary>
enum class XblConfigSetting : uint32_t
{
    /// <summary>
    /// Only passed to the below API's to warn code reviewers that there's an outstanding Xbox Live calling 
    /// pattern issue that needs to be addressed.
    /// </summary>
    ThisCodeNeedsToBeChanged
};

/// <summary>
/// Disables asserts for Xbox Live throttling in dev sandboxes.
/// </summary>
/// <param name="setting">The config settings value to be passed down.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The asserts will not fire in RETAIL sandbox, and this setting has no affect in RETAIL sandboxes.  
/// It is best practice to not call this API, and instead adjust the calling pattern but this is provided 
/// as a temporary way to get unblocked while in early stages of game development.
/// </remarks>
STDAPI_(void) XblDisableAssertsForXboxLiveThrottlingInDevSandboxes(
    _In_ XblConfigSetting setting
) XBL_NOEXCEPT;

/// <summary>
/// For advanced scenarios where a common Service Configuration ID (SCID) and title Id are needed for cross platform experiences.
/// </summary>
/// <param name="overrideScid">Override Service Configuration ID (SCID) to be used by multiplayer manager. This SCID is considered case sensitive so paste it directly from the Partner Center</param>
/// <param name="overrideTitleId">Override title Id to be used by multiplayer manager.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Currently only used by multiplayer manager to enable cross platform multiplayer scenarios.
/// </remarks>
STDAPI XblSetOverrideConfiguration(
    _In_ const char* overrideScid,
    _In_ uint32_t overrideTitleId
) XBL_NOEXCEPT;

/// <summary>
/// To override the locale used across XSAPI. If not set, the default is to use the OS locale
/// </summary>
/// <param name="overrideLocale">Override locale to be used</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblSetOverrideLocale(
    _In_ char const* overrideLocale
) XBL_NOEXCEPT;

/// <summary>
/// Contains information about a service call.
/// </summary>
typedef struct XblServiceCallRoutedArgs
{
    /// <summary>
    /// Handle for the service call.
    /// </summary>
    HCCallHandle call;

    /// <summary>
    /// The number of responses in this session.
    /// </summary>
    uint64_t responseCount;

    /// <summary>
    /// Returns the a full response log formatted message of all the properties in XblServiceCallRoutedArgs.
    /// </summary>
    const char* fullResponseFormatted;
} XblServiceCallRoutedArgs;

/// <summary>
/// A callback that will be synchronously invoked each time an HTTP call fails but will be automatically be retried.
/// </summary>
/// <param name="args">Contains information about the HTTP call that failed.  
/// The fields are only valid until the callback returns.</param>
/// <param name="context">Client context pass when the handler was added.</param>
/// <returns></returns>
/// <remarks>
/// Can be used to track intermittent failures similar to fiddler.
/// </remarks>
typedef void
(STDAPIVCALLTYPE* XblCallRoutedHandler)(
    _In_ XblServiceCallRoutedArgs args,
    _In_opt_ void* context
);

/// <summary>
/// Registers for all service call notifications.
/// </summary>
/// <param name="handler">The event handler function to call.</param>
/// <param name="context">Caller context to be passed back to the handler.</param>
/// <returns>A XblFunctionContext that can be used to unregister the event handler.</returns>
STDAPI_(XblFunctionContext) XblAddServiceCallRoutedHandler(
    _In_ XblCallRoutedHandler handler,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Unregisters from all service call notifications.
/// </summary>
/// <param name="token">The XblFunctionContext object that was returned when the event handler was registered.</param>
/// <returns></returns>
STDAPI_(void) XblRemoveServiceCallRoutedHandler(
    _In_ XblFunctionContext token
) XBL_NOEXCEPT;

/// <summary>
/// Internal use only.
/// </summary>
enum class XblApiType
{
    /// <summary>
    /// Using C API.
    /// </summary>
    XblCApi,

    /// <summary>
    /// Using C++ API.
    /// </summary>
    XblCPPApi
};

/// <summary>
/// Internal method.
/// </summary>
/// <param name="apiType">The internal API type.</param>
/// <returns></returns>
void XblSetApiType(
    _In_ XblApiType apiType
) XBL_NOEXCEPT;

}
