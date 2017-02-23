// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/social_manager.h"
#include "XboxSocialUser_WinRT.h"
#include "SocialUserGroupType_WinRT.h"
#include "PresenceFilter_WinRT.h"
#include "RelationshipFilter_WinRT.h"
#include "user_context.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

/// <summary>
/// A subset snapshot of the users social graph
///</summary>
public ref class XboxSocialUserGroup sealed
{
public:
    ///<summary>
    /// Gets an up to date list of users from the social graph
    ///</summary>
    ///<returns>Returns a list of users</returns>
    property Windows::Foundation::Collections::IVectorView<XboxSocialUser^>^ Users
    {
        Windows::Foundation::Collections::IVectorView<XboxSocialUser^>^ get();
    };

    /// <summary>
    /// The type of social user group
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(SocialUserGroupType, social_user_group_type, Microsoft::Xbox::Services::Social::Manager::SocialUserGroupType)

    /// <summary>
    /// Users who are contained in this user group currently
    /// For list this is static, for filter this is dynamic and will change on do_work
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ UsersTrackedBySocialUserGroup
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    };

    /// <summary>
    /// The local user who the user group is related to
    /// </summary>
    property XboxLiveUser_t LocalUser
    {
        XboxLiveUser_t get();
    };

    /// <summary>
    /// Returns the presence filter used if group type is filter type
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(PresenceFilterOfGroup, presence_filter_of_group, PresenceFilter);

    /// <summary>
    /// Returns the relationship filter used if group type is filter type
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(RelationshipFilterOfGroup, relationship_filter_of_group, RelationshipFilter);

    /// <summary>
    /// Returns users from xuids
    /// </summary>
    _XSAPIIMP Windows::Foundation::Collections::IVectorView<XboxSocialUser^>^ GetUsersFromXboxUserIds(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds
        );

internal:
    XboxSocialUserGroup(
        _In_ std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> cppObj
        );

    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> m_cppObj;
    XboxLiveUser_t m_user;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END