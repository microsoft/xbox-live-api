// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/social.h"
#include "shared_macros.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN
/// <summary>
/// Represents the relationship the user has with another Xbox Live user.
/// </summary>

public ref class XboxSocialRelationship sealed
{
public:
    /// <summary>
    /// The person's Xbox user identifier
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// Indicates whether the person is one that the user cares about more. 
    /// Users can have a very large number of people in their people list, 
    /// favorite people should be prioritized first in experiences and shown before others that are not favorites. 
    /// </summary>        
    DEFINE_PROP_GET_OBJ(IsFavorite, is_favorite, bool);

    /// <summary>
    /// Indicates whether the person is following the person that requested the information.
    /// </summary>        
    DEFINE_PROP_GET_OBJ(IsFollowingCaller, is_following_caller, bool);

    /// <summary>
    /// A collection of strings indicating which social networks this person has a relationship with. 
    /// </summary>        
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ SocialNetworks
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

internal:
    XboxSocialRelationship(
        _In_ xbox::services::social::xbox_social_relationship cppObj
        );

private:
    xbox::services::social::xbox_social_relationship m_cppObj;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_socialNetworksView;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END
