// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "internal_types.h"

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#endif
#if !HC_PLATFORM_IS_MICROSOFT && !XSAPI_NO_PPL
#include "pplx/pplxtasks.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
class XboxLiveContextSettings;

/// <summary>
/// Enumeration values that indicate the trace levels of debug output for service diagnostics.
///
/// Setting the debug trace level to error or higher reports the last HRESULT, the current
/// function, the source file, and the line number for many trace points in the Xbox live code.
/// </summary>
enum class xbox_services_diagnostics_trace_level
{
    /// <summary>
    /// Output no tracing and debugging messages.
    /// </summary>
    off,

    /// <summary>
    /// Output error-handling messages.
    /// </summary>
    error,

    /// <summary>
    /// Output warnings and error-handling messages.
    /// </summary>
    warning,

    /// <summary>
    /// Output informational messages, warnings, and error-handling messages.
    /// </summary>
    info,

    /// <summary>
    /// Output all debugging and tracing messages.
    /// </summary>
    verbose
};


#if !XSAPI_UNIT_TESTS && !XSAPI_NO_PPL
namespace events {
    class events_service;
}
#endif

namespace multiplayer {
    namespace manager {
        class multiplayer_client_manager;
    }
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
/// <summary>
/// Configuration information for Xbox Live service objects. 
/// </summary>
namespace system {

    // Forward declaration
    class sign_out_completed_event_args;
    class user_impl;
    class token_and_signature_result_internal;
    class user_factory;
    class xbox_live_server_impl;
    class auth_config;

    class xbox_live_wns_event_args
    {
    public:
        /// <summary>
        /// Returns the xbox user id for the WNS event
        /// </summary>
        _XSAPIIMP const string_t& xbox_user_id() const { return m_xbox_user_id; }

        /// <summary>
        /// Returns the notification type
        /// </summary>
        _XSAPIIMP const string_t& notification_type() const { return m_notification_type; }

        /// <summary>
        /// Returns the full notification content
        /// </summary>
        _XSAPIIMP const string_t& notification_content() const { return m_notification_content; }

        /// <summary>
        /// Internal function
        /// </summary>
        xbox_live_wns_event_args(
            _In_ string_t xbox_user_id,
            _In_ string_t notification_type,
            _In_ string_t notification_content
        ) :
            m_xbox_user_id(std::move(xbox_user_id)),
            m_notification_type(std::move(notification_type)),
            m_notification_content(std::move(notification_content))
        {}

    private:
        string_t m_xbox_user_id;
        string_t m_notification_type;
        string_t m_notification_content;
    };

    class xbox_live_services_settings : public std::enable_shared_from_this<xbox_live_services_settings>
    {
    public:
        /// <summary>
        /// Gets the singleton instance
        /// </summary>
        _XSAPIIMP static std::shared_ptr<xbox_live_services_settings> get_singleton_instance(_In_ bool createIfRequired = true);

        /// <summary>
        /// Used by titles to register memory allocation hooks that are used by XSAPI when it 
        /// needs to allocate a large block of memory such as SocialManager which uses a large block 
        /// of memory to keep track of the friends list.  
        /// 
        /// Note that not all memory that XSAPI uses goes through this allocator (for example std::string),
        /// but these allocations are typically small and transient.
        /// </summary>
        /// <param name="memAllocHandler">The title's allocation function.  Input is size of memory block that's being requested.  Return is pointer to the allocated memory block</param>
        /// <param name="memFreeHandler">The title's memory free function. Input is address of memory to free</param>
        /// <remarks>
        /// If titles choose not to provide their own allocation hooks, these system default allocators will be used instead. 
        /// To unwire your hooks, call the same routine with nullptr passed in for both parameters. 
        /// It is important to provide an implementation for both memAllocHandler and memFreeHandler if you hook them;
        /// hooking only one of them will be considered an error.
        /// </remarks>
        _XSAPIIMP static void set_memory_allocation_hooks(
            _In_ const std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void* (_In_ size_t dwSize)>& memAllocHandler,
            _In_ const std::function<void(_In_ void* pAddress)>& memFreeHandler
        );

        /// <summary>
        /// Registers to receive logging messages for levels that are enabled.  Event handlers will receive the level, category, and content of the message.
        /// </summary>
        /// <param name="handler">The event handler function to call.</param>
        /// <returns>
        /// A function_context object that can be used to unregister the event handler.
        /// </returns>
        _XSAPIIMP function_context add_logging_handler(_In_ std::function<void(xbox_services_diagnostics_trace_level, const std::string&, const std::string&)> handler);

        /// <summary>
        /// Unregisters from receiving logging messages.
        /// </summary>
        /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
        _XSAPIIMP void remove_logging_handler(_In_ function_context context);

        /// <summary>
        /// Indicates the level of debug messages to send to the debugger's Output window.
        /// </summary>
        _XSAPIIMP xbox_services_diagnostics_trace_level diagnostics_trace_level() const;

        /// <summary>
        /// Sets the level of debug messages to send to the debugger's Output window.
        /// </summary>
        _XSAPIIMP void set_diagnostics_trace_level(_In_ xbox_services_diagnostics_trace_level value);

        /// <summary>
        /// Registers to receive Windows Push Notification Service(WNS) events.  Event handlers will receive the xbox user id and notification type.
        /// </summary>
        /// <param name="handler">The event handler function to call.</param>
        /// <returns>
        /// A function_context object that can be used to unregister the event handler.
        /// </returns>
        _XSAPIIMP function_context add_wns_handler(_In_ const std::function<void(const xbox_live_wns_event_args&)>& handler);

        /// <summary>
        /// Unregisters from receiving Windows Push Notification Service(WNS) events.
        /// </summary>
        /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
        _XSAPIIMP void remove_wns_handler(_In_ function_context context);

        /// <summary>
        /// Internal function
        /// </summary>
        void _Raise_logging_event(_In_ xbox_services_diagnostics_trace_level level, _In_ const std::string& category, _In_ const std::string& message);

        /// <summary>
        /// Internal function
        /// </summary>
        void _Raise_wns_event(_In_ const string_t& xbox_user_id, _In_ const string_t& nofitication_type, _In_ const string_t& content);

        /// <summary>
        /// Internal function
        /// </summary>
        bool _Is_at_diagnostics_trace_level(_In_ xbox_services_diagnostics_trace_level level);

    private:
        xbox_live_services_settings();

        void set_log_level_from_diagnostics_trace_level();

        xbox_services_diagnostics_trace_level m_traceLevel;
        std::mutex m_loggingWriteLock;
        std::unordered_map<uint32_t, std::function<void(xbox_services_diagnostics_trace_level, const std::string&, const std::string&)>> m_loggingHandlers;
        uint32_t m_loggingHandlersCounter{ 1 };

        std::mutex m_wnsEventLock;
        std::unordered_map<uint32_t, std::function<void(const xbox_live_wns_event_args&)>> m_wnsHandlers;
        uint32_t m_wnsHandlersCounter{ 1 };

        friend class xsapi_memory;
        friend void* custom_mem_alloc_wrapper(_In_ size_t size, _In_ uint32_t memoryType);
        friend void custom_mem_free_wrapper(_In_ void* pointer, _In_ uint32_t memoryType);
    };
} // namespace system

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END


