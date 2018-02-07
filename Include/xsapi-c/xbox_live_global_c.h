// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#pragma warning(disable: 4265)
#pragma warning(disable: 4266)
#pragma warning(disable: 4062)


#if defined(__cplusplus)
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// Task APIs
//

/// <summary>
/// The task event type
/// </summary>
typedef enum XBL_TASK_EVENT_TYPE
{
    /// <summary>
    /// The task is in a pending task and has not started executing
    /// </summary>
    XBL_TASK_EVENT_PENDING,

    /// <summary>
    /// The task has started executing but has not yet completed execution
    /// </summary>
    XBL_TASK_EVENT_EXECUTE_STARTED,

    /// <summary>
    /// The task has completed executing
    /// </summary>
    XBL_TASK_EVENT_EXECUTE_COMPLETED
} XBL_TASK_EVENT_TYPE;

/// <summary>
/// The callback definition used by XblAddTaskEventHandler that's raised when a task changes state (pending, executing, completed).
/// </summary>
/// <param name="context">The context passed to this callback</param>
/// <param name="eventType">The event type for this callback</param>
/// <param name="taskHandle">The handle to the task</param>
typedef void
(XBL_CALLING_CONV* XBL_TASK_EVENT_FUNC)(
    _In_opt_ void* context,
    _In_ XBL_TASK_EVENT_TYPE eventType,
    _In_ XBL_TASK_HANDLE taskHandle
    );

/// <summary>
/// Sets the callback that is called when when task changes state (pending, executing, completed)
/// </summary>
/// <param name="context">The context passed to the event handler whenever it is called</param>
/// <param name="taskEventFunc">The function pointer for the event handler.  Set it to nullptr to disable</param>
/// <param name="eventHandle">Handle to the event handler.  Use this to remove the handler using XblRemoveTaskEventHandler</param>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblAddTaskEventHandler(
    _In_opt_ void* context,
    _In_opt_ XBL_TASK_EVENT_FUNC taskEventFunc,
    _Out_opt_ XBL_TASK_EVENT_HANDLE* eventHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Removes the callback that is called when when task changes state (pending, executing, completed)
/// </summary>
/// <param name="eventHandle">Handle to the event handler.  Use this to remove the handler using HCRemoveTaskEventHandler</param>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblRemoveTaskEventHandler(
    _In_ XBL_TASK_EVENT_HANDLE eventHandle
    ) XBL_NOEXCEPT;

/// <summary>
/// Calls the executionRoutine callback for the next pending Xbox Live task. It is recommended
/// the app calls XblTaskProcessNextPendingTask() in a background thread.
/// </summary>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblTaskProcessNextPendingTask() XBL_NOEXCEPT;

/// <summary>
/// Calls the completionRoutine callback for the next task that is completed.
/// This enables the caller to execute the callback on a specific thread to
/// avoid the need to marshal data to a app thread from a background thread.
///
/// XblTaskProcessNextCompletedTask will only process completed tasks that have a
/// matching taskGroupId.  This enables the caller to split the where results are
/// returned between between a set of app threads.  If this isn't needed, just pass in 0.
/// </summary>
/// <param name="taskGroupId">
/// HCTaskProcessNextCompletedTask will only process completed tasks that have a
/// matching taskGroupId.  This enables the caller to split the where results are
/// returned between between a set of app threads.  If this isn't needed, just pass in 0.
/// </param>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblTaskProcessNextCompletedTask(_In_ uint64_t taskGroupId) XBL_NOEXCEPT;

/// <summary>
/// Returns the size of the pending task queue for a specific task group ID
/// </summary>
/// <returns>Returns the size of the pending task queue.</returns>
XBL_API uint64_t XBL_CALLING_CONV
XblTaskGetPendingTaskQueueSize() XBL_NOEXCEPT;

/// <summary>
/// Returns the size of the completed task queue for a specific task group ID
/// </summary>
/// <param name="taskGroupId">
/// This enables the caller to split the where results are returned between between a set of app threads.
/// If this isn't needed, just pass in 0.
/// </param>
/// <returns>Returns the size of the completed task queue for a specific task group ID</returns>
XBL_API uint64_t XBL_CALLING_CONV
XblTaskGetCompletedTaskQueueSize(
    _In_ uint64_t taskGroupId
    ) XBL_NOEXCEPT;

/////////////////////////////////////////////////////////////////////////////////////////
// Memory APIs
//

/// <summary>
/// A callback invoked every time a new memory buffer must be dynamically allocated by the library.
/// This callback is optionally installed by calling XblMemSetFunctions()
/// 
/// The callback must allocate and return a pointer to a contiguous block of memory of the 
/// specified size that will remain valid until the app's corresponding XBL_MEM_FREE_FUNC 
/// callback is invoked to release it.
/// 
/// Every non-null pointer returned by this method will be subsequently passed to the corresponding
/// XBL_MEM_FREE_FUNC callback once the memory is no longer needed.
/// </summary>
/// <returns>A pointer to an allocated block of memory of the specified size, or a null 
/// pointer if allocation failed.</returns>
/// <param name="size">The size of the allocation to be made. This value will never be zero.</param>
/// <param name="memoryTypeId">An opaque identifier representing the internal category of 
/// memory being allocated.</param>
typedef _Ret_maybenull_ _Post_writable_byte_size_(size) void*
(XBL_CALLING_CONV* XBL_MEM_ALLOC_FUNC)(
    _In_ size_t size,
    _In_ XBL_MEMORY_TYPE memoryType
    );

/// <summary>
/// A callback invoked every time a previously allocated memory buffer is no longer needed by 
/// the library and can be freed. This callback is optionally installed by calling XblMemSetFunctions()
///
/// The callback is invoked whenever the library has finished using a memory buffer previously 
/// returned by the app's corresponding XBL_MEM_ALLOC_FUNC such that the application can free the
/// memory buffer.
/// </summary>
/// <param name="pointer">The pointer to the memory buffer previously allocated. This value will
/// never be a null pointer.</param>
/// <param name="memoryTypeId">An opaque identifier representing the internal category of 
/// memory being allocated.</param>
typedef void
(XBL_CALLING_CONV* XBL_MEM_FREE_FUNC)(
    _In_ _Post_invalid_ void* pointer,
    _In_ XBL_MEMORY_TYPE memoryType
    );

/// <summary>
/// Optionally sets the memory hook functions to allow callers to control route memory 
/// allocations to their own memory manager. This must be called before HCGlobalInitialize() 
/// and can not be called again until HCGlobalCleanup()
///
/// This method allows the application to install custom memory allocation routines in order 
/// to service all requests for new memory buffers instead of using default allocation routines.
///
/// The <paramref name="memAllocFunc" /> and <paramref name="memFreeFunc" /> parameters can be null
/// pointers to restore the default routines. Both callback pointers must be null or both must 
/// be non-null. Mixing custom and default routines is not permitted.
/// </summary>
/// <param name="memAllocFunc">A pointer to the custom allocation callback to use, or a null 
/// pointer to restore the default.</param>
/// <param name="memFreeFunc">A pointer to the custom freeing callback to use, or a null 
/// pointer to restore the default.</param>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblMemSetFunctions(
    _In_opt_ XBL_MEM_ALLOC_FUNC memAllocFunc,
    _In_opt_ XBL_MEM_FREE_FUNC memFreeFunc
    ) XBL_NOEXCEPT;

/// <summary>
/// Gets the memory hook functions to allow callers to control route memory allocations to their 
/// own memory manager.  This method allows the application get the default memory allocation routines.
/// This can be used along with XblMemSetFunctions() to monitor all memory allocations.
/// </summary>
/// <param name="memAllocFunc">Set to the current allocation callback.  Returns the default routine 
/// if not previously set</param>
/// <param name="memFreeFunc">Set to the to the current memory free callback.  Returns the default 
/// routine if not previously set</param>
/// <returns>Result code for this API operation.  Possible values are XBL_RESULT_OK, XBL_RESULT_E_HC_INVALIDARG, or XBL_RESULT_E_HC_FAIL.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblMemGetFunctions(
    _Out_ XBL_MEM_ALLOC_FUNC* memAllocFunc,
    _Out_ XBL_MEM_FREE_FUNC* memFreeFunc
    ) XBL_NOEXCEPT;

/////////////////////////////////////////////////////////////////////////////////////////
// Global APIs
// 

/// <summary>
/// Initializes the library instance.
/// This must be called before any other method, except for XblMemSetFunctions() and XblMemGetFunctions()
/// Should have a corresponding call to XblGlobalCleanup().
/// </summary>
/// <returns>Result code for this API operation.</returns>
XBL_API XBL_RESULT XBL_CALLING_CONV
XblGlobalInitialize() XBL_NOEXCEPT;

/// <summary>
/// Immediately reclaims all resources associated with the library.
/// If you called XblMemSetFunctions(), call this before shutting down your app's memory manager.
/// </summary>
XBL_API void XBL_CALLING_CONV
XblGlobalCleanup() XBL_NOEXCEPT;

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)

