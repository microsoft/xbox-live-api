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
#ifndef _WIN32
#include "pplx/pplxtasks.h"
#endif


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
    class http_call_impl;
    class xbox_live_context_impl;

    namespace events {
        class events_service;
    }
    namespace events {
        class events_service;
    }

    namespace multiplayer { namespace manager {
        class multiplayer_client_manager;
        class multiplayer_local_user;
    }}
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
    _XSAPIIMP void set_memory_allocation_hooks(
        _In_ const std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void*(_In_ size_t dwSize)>& memAllocHandler,
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

    std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void*(_In_ size_t dwSize)> m_pCustomMemAllocHook;
    std::function<void(_In_ void* pAddress)> m_pCustomMemFreeHook;

    void set_log_level_from_diagnostics_trace_level();

    xbox_services_diagnostics_trace_level m_traceLevel;
    std::mutex m_loggingWriteLock;
    std::unordered_map<function_context, std::function<void(xbox_services_diagnostics_trace_level, const std::string&, const std::string&)>> m_loggingHandlers;
    function_context m_loggingHandlersCounter;

    std::mutex m_wnsEventLock;
    std::unordered_map<function_context, std::function<void(const xbox_live_wns_event_args&)>> m_wnsHandlers;
    function_context m_wnsHandlersCounter;

    friend class xsapi_memory;
    friend void *custom_mem_alloc_wrapper(_In_ size_t size, _In_ uint32_t memoryType);
    friend void custom_mem_free_wrapper(_In_ void *pointer, _In_ uint32_t memoryType);
};


#if XSAPI_NONXDK_CPP_AUTH || XSAPI_NONXDK_WINRT_AUTH
/// <summary>
/// Contains information about the authorization token and digital signature for an HTTP request by a user.
/// This class is returned as the result of a call to xbox_live_user.get_token_and_signature().
/// </summary>
class token_and_signature_result
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    token_and_signature_result();

    /// <summary>
    /// Internal function
    /// </summary>
    token_and_signature_result(
        _In_ std::shared_ptr<token_and_signature_result_internal> internalObj
        );

    /// <summary>
    /// The authorization token for the HTTP request.
    /// </summary>
    _XSAPIIMP string_t token() const;

    /// <summary>
    /// The digital signature for the HTTP request.
    /// </summary>
    _XSAPIIMP string_t signature() const;

    /// <summary>
    /// The unique ID tied to the Xbox user's account.
    /// </summary>
    _XSAPIIMP string_t xbox_user_id() const;

    /// <summary>
    /// The gamertag name associated with the Xbox user's account.
    /// </summary>
    _XSAPIIMP string_t gamertag() const;

    /// <summary>
    /// The hashcode that identifies the user. This value is used for HTTP calls.
    /// </summary>
    _XSAPIIMP string_t xbox_user_hash() const;

    /// <summary>
    /// Internal function
    /// </summary>
    _XSAPIIMP string_t reserved() const;

    /// <summary>
    /// The age group
    /// </summary>
    _XSAPIIMP string_t age_group() const;
    
    /// <summary>
    /// The privileges
    /// </summary>
    _XSAPIIMP string_t privileges() const;
    
#if XSAPI_U
    /// <summary>
    /// The settings related user restrictions.
    /// </summary>
    _XSAPIIMP string_t user_settings_restrictions() const;
    
    /// <summary>
    /// The enforcement related user restrictions.
    /// </summary>
    _XSAPIIMP string_t user_enforcement_restrictions() const;
    
    /// <summary>
    /// The title related user restrictions.
    /// </summary>
    _XSAPIIMP string_t user_title_restrictions() const;
#endif

    /// <summary>
    /// The web account id
    /// </summary>
    _XSAPIIMP string_t web_account_id() const;

#if UWP_API
    Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ token_request_result() const;
#endif

private:
    std::shared_ptr<token_and_signature_result_internal> m_internalObj;
};

/// <summary>
/// Enumeration values that indicate the result status of sign in.
/// </summary>
enum sign_in_status
{
    /// <summary>
    /// Signed in successfully.
    /// </summary>
    success = 0,

    /// <summary>
    /// Need to invoke the signin API (w/ UX) to let the user take necessary actions for the sign-in operation to continue.
    /// Can only be returned from signin_silently().
    /// </summary>
    user_interaction_required,

    /// <summary>
    /// The user decided to cancel the sign-in operation.
    /// Can only be returned from signin().
    /// </summary>
    user_cancel
};


/// <summary>
/// Represents the result of the sign in operation.
/// </summary>
class sign_in_result
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    sign_in_result() :
        m_status(success)
    {}

    /// <summary>
    /// Internal function
    /// </summary>
    sign_in_result(_In_ sign_in_status status) :
        m_status(status),
        m_newAccount(false)
    {}

    /// <summary>
    /// Internal function
    /// </summary>
    sign_in_result(_In_ sign_in_status status, _In_ bool newAccount) :
        m_status(status),
        m_newAccount(newAccount)
    {}

    /// <summary>
    /// The status of sign in operation.
    /// </summary>
    _XSAPIIMP sign_in_status status() const { return m_status; }

#ifdef XSAPI_U
    /// <summary>
    /// Tells if signed in to a new or existing account.
    /// Only relevant when status() == sign_in_status::success.
    /// </summary>
    _XSAPIIMP bool is_new_account() const { return m_newAccount; }
#endif

private:
    sign_in_status m_status;
    bool m_newAccount;
};

#if XSAPI_U
/// <summary>
/// Optional configuration for sign in process
/// </summary>
class xbox_sign_in_options
{
public:
    void setLogInButtonText(const string_t& buttonText) { m_logInButtonText = buttonText; }
    const string_t& getLogInButtonText() { return m_logInButtonText; }

private:
    string_t m_logInButtonText;
};
#endif

/// <summary>
/// Represents a player that is associated with a device or a controller.
/// </summary>
class xbox_live_user : public std::enable_shared_from_this<xbox_live_user>
{
public:
    /// <summary>
    /// Attempt to sign a player into their Xbox Live account. This call may bring up
    /// a sign-in user interface.
    /// </summary>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// </returns>
    /// <remarks>
    /// You should only call this method if silent sign-in indicates that user interaction is required.
    /// For UWA, this API is to be called from UI thread, if you're calling from non-UI thread or not sure, please use 
    /// signin_silently(Platform::Object^ coreDispatcherObj) version instead.
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<sign_in_result>> signin();

#if XSAPI_U
    /// <summary>
    /// Attempt to sign a player into their Xbox Live account. This call may bring up
    /// a sign-in user interface that will use customization overrides from the supplied xbox_sign_in_options object.
    /// </summary>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// </returns>
    /// <remarks>
    /// You should only call this method if silent sign-in indicates that user interaction is required.
    /// For UWA, this API is to be called from UI thread, if you're calling from non-UI thread or not sure, please use 
    /// signin_silently(Platform::Object^ coreDispatcherObj) version instead.
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<sign_in_result>> signin(_In_ std::shared_ptr<xbox_sign_in_options> options);
#endif

    /// <summary>
    /// Attempt to silently sign a player into their Xbox Live account.
    /// </summary>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// If silent sign-in is not successful, result.err() indicates the error.
    /// </returns>
    /// <remarks>
    /// If the app is unable to silently sign-in, the API return sign_in_result with user_interaction_required status .
    /// to sign-in, so the app should then call signin().
    /// For UWA, this API is to be called from UI thread, if you're not calling from non-UI thread or not sure, please use 
    /// signin_silently(Platform::Object^ coreDispatcherObj) version instead.
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<sign_in_result>> signin_silently();
    
    /// <summary>
    /// Set the session id to be used in telemetry events.
    /// </summary>
    /// <param name="sessionId">The titleSessionId to be set in telemetery events</param>
    _XSAPIIMP void set_title_telemetry_session_id(_In_ const string_t& sessionId);
    
    /// <summary>
    /// Internal function
    /// </summary>
    _XSAPIIMP string_t _Title_telemetry_session_id();
    
#if XSAPI_U
    _XSAPIIMP static std::shared_ptr<xbox_live_user> get_last_signed_in_user();
    _XSAPIIMP pplx::task<xbox_live_result<void>> signout();
    _XSAPIIMP void clear_token_cache();
#endif

#if WINAPI_FAMILY && WINAPI_FAMILY==WINAPI_FAMILY_APP
    /// <summary>
    /// Attempt to sign a player into their Xbox Live account. This call may bring up
    /// a sign-in user interface.
    /// </summary>
    /// <param name="coreDispatcherObj">The Windows Runtime core event message dispatcher.</param>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// </returns>
    /// <remarks>
    /// You should only call this method if silent sign-in indicates that user interaction is required.
    /// If you're calling this API from non-UI thread, parameter coreDispatcherObj is required, so that app UI
    /// can be rendered and locale can be generated.
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<sign_in_result>> signin(_In_opt_ Platform::Object^ coreDispatcherObj);

    /// <summary>
    /// Attempt to silently sign a player into their Xbox Live account.
    /// </summary>
    /// <param name="coreDispatcherObj">The Windows Runtime core event message dispatcher.</param>
    /// <returns>
    /// Returns a pplx::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// If silent sign-in is not successful, result.err() indicates the error.
    /// </returns>
    /// <remarks>
    /// If the app is unable to silently sign-in, the API return sign_in_result with user_interaction_required status .
    /// to sign-in, so the app should then call signin().
    /// If you're calling this API from non-UI thread, parameter coreDispatcherObj is required, so that app locale can be generated.
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<sign_in_result>> signin_silently(_In_opt_ Platform::Object^ coreDispatcherObj);
#endif 

    /// <summary>
    /// Creates a new instance of xbox_live_user.
    /// </summary>
    _XSAPIIMP xbox_live_user();

#if WINAPI_FAMILY && WINAPI_FAMILY==WINAPI_FAMILY_APP
    /// <summary>
    /// Creates a new instance of XboxLiveUser for Multi-User Application.
    /// </summary>
    _XSAPIIMP xbox_live_user(Windows::System::User^ systemUser);
#endif

    /// <summary>
    /// Gets a unique ID that is tied to the user's account which persists across multiple devices.
    /// </summary>
    _XSAPIIMP string_t xbox_user_id() const;

    /// <summary>
    /// The Xbox Live public gamertag name associated with the user.
    /// </summary>
    _XSAPIIMP string_t gamertag() const;

    /// <summary>
    /// Gets the age group of the user.
    /// </summary>
    _XSAPIIMP string_t age_group() const;
    
    /// <summary>
    /// Gets the privileges of the user.
    /// </summary>
    _XSAPIIMP string_t privileges() const;
    
#if XSAPI_U
    /// <summary>
    /// Gets the settings related user restrictions.
    /// </summary>
    _XSAPIIMP const string_t& user_settings_restrictions() const;
    
    /// <summary>
    /// Gets the enforcement related user restrictions.
    /// </summary>
    _XSAPIIMP const string_t& user_enforcement_restrictions() const;
    
    /// <summary>
    /// Gets the title related user restrictions.
    /// </summary>
    _XSAPIIMP const string_t& user_title_restrictions() const;
#endif
    
    std::shared_ptr<auth_config> auth_config();

    /// <summary>
    /// Indicates if the Xbox Live user is currently signed in.
    /// </summary>
    _XSAPIIMP bool is_signed_in() const;

#if WINAPI_FAMILY && WINAPI_FAMILY==WINAPI_FAMILY_APP
    /// <summary>
    /// Gets id of WebAccount returned by the Xbox live WebAccountProvider.
    /// </summary>
    /// <remarks>
    /// Check https://msdn.microsoft.com/en-us/library/windows/apps/windows.security.credentials.webaccount.aspx
    /// for more information about WebAccount
    /// </remarks>
    _XSAPIIMP string_t web_account_id() const;

    /// <summary>
    /// The Windows System NT user associated with the Xbox Live User, only available in Multi-User application.
    /// </summary>
    _XSAPIIMP Windows::System::User^ windows_system_user() const;
#endif

    /// <summary>
    /// Retrieves an authorization token and digital signature for an HTTP request by this user.
    /// </summary>
    /// <param name="httpMethod">The HTTP method (GET, PUT, POST, etc.) for this request.</param>
    /// <param name="url">The URL for which to retrieve the authorization token and digital signature.</param>
    /// <param name="headers">The headers to be included in the HTTP request.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call. The result is an object
    /// indicating the token and the digital signature of the entire request, including the token.
    /// </returns>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
    get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers
        );

    /// <summary>
    /// Retrieves an authorization token and digital signature for an HTTP request by this user,
    /// with a request body expressed as an array of bytes.
    /// </summary>
    /// <param name="httpMethod">The HTTP method (GET, PUT, POST, etc.) for this request.</param>
    /// <param name="url">The URL for which to retrieve the authorization token and digital signature.</param>
    /// <param name="headers">The headers to be included in the HTTP request.</param>
    /// <param name="requestBodyString">The body of the request, expressed as a string.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call. The result is an object
    /// indicating the token and the digital signature of the entire request, including the token.
    /// </returns>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
    get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_opt_ const string_t& requestBodyString
        );

    /// <summary>
    /// Retrieves an authorization token and digital signature for an HTTP request by this user,
    /// with a request body expressed as an array of bytes.
    /// </summary>
    /// <param name="httpMethod">The HTTP method (GET, PUT, POST, etc.) for this request.</param>
    /// <param name="url">The URL for which to retrieve the authorization token and digital signature.</param>
    /// <param name="headers">The headers to be included in the HTTP request.</param>
    /// <param name="requestBodyArray">The body of the request, expressed as an array of bytes.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call. The result is an object
    /// indicating the token and the digital signature of the entire request, including the token.
    /// </returns>
    _XSAPIIMP pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
    get_token_and_signature_array(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& requestBodyArray 
        );

    xbox_live_user(_In_ std::shared_ptr<user_impl> userImpl) : m_user_impl(std::move(userImpl)) { }

    /// <summary>
    /// Registers an event handler for when user sign out completes.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    _XSAPIIMP static function_context add_sign_out_completed_handler(_In_ std::function<void(const sign_out_completed_event_args&)> handler);

    /// <summary>
    /// Unregisters an event handler for sign-out completion notifications.
    /// </summary>
    /// <param name="context">The function_context object that was returned when the event handler was registered. </param>
    _XSAPIIMP static void remove_sign_out_completed_handler(_In_ function_context context);

    std::shared_ptr<user_impl> _User_impl() { return m_user_impl; }

protected:
    std::shared_ptr<user_impl> m_user_impl;

private:
    pplx::task<xbox_live_result<sign_in_result>> signin_helper(bool showUI, bool forceRefresh);
};


/// <summary>
/// Arguments for the SignOutCompleted event. 
/// </summary>
class sign_out_completed_event_args
{
public:
    sign_out_completed_event_args(
        _In_ std::weak_ptr<system::xbox_live_user> weakUser, 
        _In_ std::shared_ptr<user_impl> user_impl
    );

    /// <summary>
    /// The user that completed signing out.
    /// </summary>
    _XSAPIIMP std::shared_ptr<system::xbox_live_user> user() const;

    /// <summary>
    /// Internal function
    /// </summary>
    std::shared_ptr<user_impl> _Internal_user();

private:
    std::shared_ptr<system::xbox_live_user> m_user;
    std::shared_ptr<user_impl> m_user_impl;
};

#endif // XSAPI_NONXDK_CPP_AUTH || XSAPI_NONXDK_WINRT_AUTH


/// <summary>Enumeration values that indicate the result code from string verification.
/// These values are defined on the service side and should not be modified.
/// </summary>
enum class verify_string_result_code
{
    /// <summary>No issues were found with the string.</summary>
    success = 0,

    /// <summary>The string contains offensive content.</summary>
    offensive = 1,

    /// <summary>The string is too long to verify.</summary>
    too_long = 2,

    /// <summary>An unknown error was encountered during string verification.</summary>
    unknown_error
};

/// <summary>
/// Contains information about the results of a string verification.
/// </summary>
class verify_string_result
{
public:
    /// <summary>
    /// The result code for the string verification.
    /// </summary>
    _XSAPIIMP verify_string_result_code result_code() const;

    /// <summary>
    /// first_offending_substring() contains the first offending substring if the
    /// result code is verify_string_result_code::offensive.
    /// </summary>
    _XSAPIIMP const string_t& first_offending_substring() const;

    /// <summary>
    /// Internal function
    /// </summary>
    verify_string_result();

    /// <summary>
    /// Internal function
    /// </summary>
    verify_string_result(
        verify_string_result_code resultCode,
        string_t firstOffendingSubstring
        );

    static xbox_live_result<verify_string_result> _Deserialize(_In_ const web::json::value& inputJson);

private:
    verify_string_result_code m_resultCode;
    string_t m_firstOffendingSubstring;
};

/// <summary>
/// Provides methods to validate a string for use with Xbox live.
/// </summary>
class string_service
{
public:

    /// <summary>
    /// Verifies if a string contains acceptable text for use with Xbox Live.
    /// </summary>
    /// <param name="stringToVerify">The string to verify.</param>
    /// <returns>
    /// A verify_string_result object which indicates if the string contains unacceptable text.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// 
    /// Calls V2 GET /system/strings/validate
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<verify_string_result>> verify_string(_In_ const string_t& stringToVerify);

    /// <summary>
    /// Verifies a collection of strings to see if each string contains acceptable text for use with Xbox Live.
    /// </summary>
    /// <param name="stringsToVerify">The collection of strings to verify.</param>
    /// <returns>
    /// A collection of verify_string_result objects which indicate if the strings contain unacceptable text.
    /// </returns>
    /// <remarks>
    /// Returns a concurrency::task&lt;T&gt; object that represents the state of the asynchronous operation.
    /// 
    /// Calls V2 GET /system/strings/validate
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<verify_string_result>>> verify_strings(_In_ const std::vector<string_t>& stringsToVerify);

    /// <summary>
    /// Internal function
    /// </summary>
    string_service();

    /// <summary>
    /// Internal function
    /// </summary>
    string_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

private:
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
};
} // namespace system
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
