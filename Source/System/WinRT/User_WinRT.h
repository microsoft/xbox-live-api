// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "GetTokenAndSignatureResult_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"
#include "user_impl.h"
#include "SignInResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN
ref class SignInResponse;
ref class GetTokenAndSignatureResponse;
ref class XboxLiveUser;

/// <summary>
/// Event arguments for the SignOutCompleted event. 
/// </summary>
public ref class SignOutCompletedEventArgs sealed
{
public:
    /// <summary>
    /// The user that completed signing out.
    /// </summary>
    property XboxLiveUser^ User { XboxLiveUser^ get(); }

internal:
    SignOutCompletedEventArgs(
        _In_ std::shared_ptr<xbox::services::system::user_impl> user_impl
        );

private:
    XboxLiveUser^ m_user;

};

class UserEventBind
{
public:
    UserEventBind();

    void UserSignOutCompletedHandler(_In_ xbox::services::system::sign_out_completed_event_args args);

    ~UserEventBind();
    function_context m_functionContext;
};


/// <summary>
/// Represents a player that is associated with a device or a controller.
/// </summary>
public ref class XboxLiveUser sealed 
{
public:
    /// <summary>
    /// Creates a new instance of XboxLiveUser for Single User Application.
    /// </summary>
    XboxLiveUser();

    /// <summary>
    /// Creates a new instance of XboxLiveUser for Multi-User Application.
    /// </summary>
    XboxLiveUser(Windows::System::User^ systemUser);

    /// <summary>
    /// Attempt to sign a player into their Xbox Live account. This call may bring up
    /// a sign-in user interface.
    /// </summary>
    /// <param name="coreDispatcher">The Windows Runtime core event message dispatcher.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// </returns>
    /// <remarks>
    /// You should only call this method if silent sign-in indicates that user interaction is required.
    ///
    /// This is cast as Platform::Object^ instead of Windows::UI::Core::CoreDispatcher^ to avoid the
    /// [Windows::Foundation::Metadata::WebHostHidden] requirement.
    ///
    /// If you're calling this API from non-UI thread, parameter coreDispatcherObj is required, so that app UI
    /// can be rendered and locale can be generated.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<SignInResult^>^
    SignInAsync(
        _In_opt_ Platform::Object^ coreDispatcher
        );

    /// <summary>
    /// Attempt to silently sign a player into their Xbox Live account. This call can be used to prefetch xbox live
    /// data before the UI is ready.
    /// </summary>
    /// <param name="coreDispatcher">The Windows Runtime core event message dispatcher.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call.
    /// </returns>
    /// <remarks>
    /// If the app is unable to silently sign-in, the API return SignInResult with UserInteractionRequired Status .
    /// to sign-in, so the app should then call SignInAsync().
    ///
    /// If you're calling this API from non-UI thread, parameter coreDispatcherObj is required, so that app locale can be generated.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<SignInResult^>^
    SignInSilentlyAsync(
        _In_opt_ Platform::Object^ coreDispatcher
        );

    /// <summary>
    /// Retrieves an authorization token and digital signature for an HTTP request by this user,
    /// with no request body.
    /// </summary>
    /// <param name="httpMethod">The HTTP method (GET, PUT, POST, etc.) for this request.</param>
    /// <param name="url">The URL for which to retrieve the authorization token and digital signature.</param>
    /// <param name="headers">The headers to be included in the HTTP request.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call. The result is an object
    /// indicating the token and the digital signature of the entire request, including the token.
    /// </returns>
    /// <remarks>
    /// </remarks>
    Windows::Foundation::IAsyncOperation<GetTokenAndSignatureResult^>^
    GetTokenAndSignatureAsync(
        _In_ Platform::String^ httpMethod,
        _In_ Platform::String^ url,
        _In_ Platform::String^ headers
        );

    /// <summary>
    /// Retrieves an authorization token and digital signature for an HTTP request by this user,
    /// with a request body expressed as a string.
    /// </summary>
    /// <param name="httpMethod">The HTTP method (GET, PUT, POST, etc.) for this request.</param>
    /// <param name="url">The URL for which to retrieve the authorization token and digital signature.</param>
    /// <param name="headers">The headers to be included in the HTTP request.</param>
    /// <param name="body">The body of the request, expressed as a string.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call. The result is an object
    /// indicating the token and the digital signature of the entire request, including the token.
    /// </returns>
    Windows::Foundation::IAsyncOperation<GetTokenAndSignatureResult^>^
    GetTokenAndSignatureAsync(
        _In_ Platform::String^ httpMethod,
        _In_ Platform::String^ url,
        _In_ Platform::String^ headers,
        _In_ Platform::String^ body
        );

    /// <summary>
    /// Retrieves an authorization token and digital signature for an HTTP request by this user,
    /// with a request body expressed as an array of bytes.
    /// </summary>
    /// <param name="httpMethod">The HTTP method (GET, PUT, POST, etc.) for this request.</param>
    /// <param name="url">The URL for which to retrieve the authorization token and digital signature.</param>
    /// <param name="headers">The headers to be included in the HTTP request.</param>
    /// <param name="body">The body of the request, expressed as an array of bytes.</param>
    /// <returns>
    /// An interface for tracking the progress of the asynchronous call. The result is an object
    /// indicating the token and the digital signature of the entire request, including the token.
    /// </returns>
    Windows::Foundation::IAsyncOperation<GetTokenAndSignatureResult^>^
    GetTokenAndSignatureArrayAsync(
        _In_ Platform::String^ httpMethod,
        _In_ Platform::String^ url,
        _In_ Platform::String^ headers,
        _In_opt_ const Platform::Array<byte>^ requestBodyArray
        );

    /// <summary>
    /// Event signaling that sign out has completed for a user. 
    /// </summary>
    static event Windows::Foundation::EventHandler<SignOutCompletedEventArgs^>^ SignOutCompleted;

public:

    /// <summary>
    /// Gets a unique ID that is tied to the user's account which persists across multiple devices.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ_FROM_INTERNAL_STRING(XboxUserId, xbox_user_id)

    /// <summary>
    /// The Xbox Live public gamertag name associated with the user.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ_FROM_INTERNAL_STRING(Gamertag, gamertag)

    /// <summary>
    /// Gets the age group of the user.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ_FROM_INTERNAL_STRING(AgeGroup, age_group)

    /// <summary>
    /// Gets the privileges of the user.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ_FROM_INTERNAL_STRING(Privileges, privileges)

    /// <summary>
    /// Indicates if the Xbox Live user is currently signed in.
    /// </summary>
    DEFINE_PTR_PROP_GET(IsSignedIn, is_signed_in, bool)

    /// <summary>
    /// Gets id of WebAccount returned by the Xbox live WebAccountProvider.
    /// </summary>
    /// <remarks>
    /// Check https://msdn.microsoft.com/en-us/library/windows/apps/windows.security.credentials.webaccount.aspx
    /// for more information about WebAccount
    /// </remarks>
    DEFINE_PTR_PROP_GET_STR_OBJ_FROM_INTERNAL_STRING(WebAccountId, web_account_id)

    /// <summary>
    /// The Windows System NT user associated with the Xbox Live User, only available in Multi-User application.
    /// </summary>
    property Windows::System::User^ WindowsSystemUser { Windows::System::User^ get(); }

internal:
    XboxLiveUser(std::shared_ptr<xbox::services::system::user_impl> user_impl);

    std::shared_ptr<xbox::services::system::user_impl> _User_impl()
    {
        return m_cppObj;
    }

   static void RaiseSignOutCompleted(SignOutCompletedEventArgs^ args);

private:
    std::shared_ptr<xbox::services::system::user_impl> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END
