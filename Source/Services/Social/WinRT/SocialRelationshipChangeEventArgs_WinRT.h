// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/social.h"
#include "SocialNotificationType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

public ref class SocialRelationshipChangeEventArgs sealed
{
public:
    /// <summary>
    /// The Xbox user ID for the user who's social graph changes are being listed for.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(CallerXboxUserId, caller_xbox_user_id);

    /// <summary>
    /// The type of notification change.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(SocialNotification, social_notification, Microsoft::Xbox::Services::Social::SocialNotificationType);

    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ XboxUserIds { Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); }

internal:
    SocialRelationshipChangeEventArgs(_In_ xbox::services::social::social_relationship_change_event_args cppObj);

private:
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_xboxUserIds;
    xbox::services::social::social_relationship_change_event_args m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END