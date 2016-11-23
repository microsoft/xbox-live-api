//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/social_manager.h"
#include "SocialEventType_WinRT.h"
#include "XboxSocialUser_WinRT.h"
#include "user_context.h"
#include "SocialEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

/// <summary>
/// An event that something in the social graph has changed
///</summary>
public ref class SocialEvent sealed
{
public:
    /// <summary>
    /// The Xbox Live user this event is for
    /// </summary>
    property XboxLiveUser_t User { XboxLiveUser_t get(); }

    /// <summary>
    /// The type of event this is 
    /// Tells the caller what to cast the event_args to
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(EventType, event_type, SocialEventType);

    /// <summary>
    /// List of users this event affects
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ UsersAffected { Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); }

    /// <summary>
    /// The error code indicating the result of the operation.
    /// </summary>
    property int ErrorCode
    {
        int get();
    };

    /// <summary>
    /// Returns call specific debug information if join fails
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    property Platform::String^ ErrorMessage
    {
        Platform::String^ get();
    };

    property SocialEventArgs^ EventArgs
    {
        Microsoft::Xbox::Services::Social::Manager::SocialEventArgs^ get();
    };

internal:
    SocialEvent(
        _In_ xbox::services::social::manager::social_event cppObj
        );

private:
    XboxLiveUser_t m_user;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_usersAffected;
    xbox::services::social::manager::social_event m_cppObj;
    Platform::Exception^ m_errorCode;
    Platform::String^ m_errorMessage;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END