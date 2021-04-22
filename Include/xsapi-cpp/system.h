// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types.h"
#include "errors.h"
#include "xbox_live_context_settings.h"
#include "xbox_live_app_config.h"

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#endif
#if !HC_PLATFORM_IS_MICROSOFT && !XSAPI_NO_PPL
#include "pplx/pplxtasks.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
    class XboxLiveContextSettings;

#if !XSAPI_UNIT_TESTS && !XSAPI_NO_PPL
    namespace events {
        class events_service;
    }
#endif

    namespace multiplayer { namespace manager {
        class multiplayer_client_manager;
    }}
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
    /// <summary>
    /// Configuration information for Xbox Live service objects. 
    /// </summary>
    namespace system {

    class xbox_live_wns_event_args
    {
    public:
        /// <summary>
        /// Returns the xbox user id for the WNS event
        /// </summary>
        const string_t& xbox_user_id() const { return m_xbox_user_id; }

        /// <summary>
        /// Returns the notification type
        /// </summary>
        const string_t& notification_type() const { return m_notification_type; }

        /// <summary>
        /// Returns the full notification content
        /// </summary>
        const string_t& notification_content() const { return m_notification_content; }

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
        inline static std::shared_ptr<xbox_live_services_settings> get_singleton_instance(_In_ bool createIfRequired = true);

        /// <summary>
        /// Used by titles to register memory allocation hooks that are used by XSAPI when it 
        /// needs to allocate a large block of memory such as SocialManager which uses a large block 
        /// of memory to keep track of the friends list.  
        /// </summary>
        /// <param name="memAllocHandler">The title's allocation function.  Input is size of memory block that's being requested.  Return is pointer to the allocated memory block</param>
        /// <param name="memFreeHandler">The title's memory free function. Input is address of memory to free</param>
        /// <remarks>
        /// If titles choose not to provide their own allocation hooks, these system default allocators will be used instead. 
        /// To unwire your hooks, call the same routine with nullptr passed in for both parameters. 
        /// It is important to provide an implementation for both memAllocHandler and memFreeHandler if you hook them;
        /// hooking only one of them will be considered an error.
        /// </remarks>
        inline static void set_memory_allocation_hooks(
            _In_ const std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void*(_In_ size_t dwSize)>& memAllocHandler,
            _In_ const std::function<void(_In_ void* pAddress)>& memFreeHandler
        );

        /// <summary>
        /// Deprecated. XSAPI is using libHttpClient logging. A logging handler can be added using HCTraceSetClientCallback.
        /// </summary>
        _XSAPICPP_DEPRECATED inline function_context add_logging_handler(_In_ std::function<void(xbox_services_diagnostics_trace_level, const std::string&, const std::string&)> handler);

        /// <summary>
        /// Deprecated. See above.
        /// </summary>
        _XSAPICPP_DEPRECATED inline void remove_logging_handler(_In_ function_context context);

        /// <summary>
        /// Indicates the level of debug messages to send to the debugger's Output window.
        /// </summary>
        inline xbox_services_diagnostics_trace_level diagnostics_trace_level() const;

        /// <summary>
        /// Sets the level of debug messages to send to the debugger's Output window.
        /// </summary>
        inline void set_diagnostics_trace_level(_In_ xbox_services_diagnostics_trace_level value);

        /// <summary>
        /// Deprecated. Registering WNS callbacks though XSAPI is no longer supported.
        /// </summary>
        _XSAPICPP_DEPRECATED inline function_context add_wns_handler(_In_ const std::function<void(const xbox_live_wns_event_args&)>& handler);

        /// <summary>
        /// Deprecated. Registering WNS callbacks though XSAPI is no longer supported.
        /// </summary>
        _XSAPICPP_DEPRECATED inline void remove_wns_handler(_In_ function_context context);

    private:
        xbox_live_services_settings() = default;
    };
} // namespace system

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#if !XSAPI_NO_PPL
#include "impl/system.hpp"
#endif
