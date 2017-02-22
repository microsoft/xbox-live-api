// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social_manager.h"
#include "XboxSocialUserGroup_WinRT.h"
#if !TV_API
#include "User_WinRT.h"
#endif
#include "RelationshipFilter_WinRT.h"
#include "SocialManagerExtraDetailLevel_WinRT.h"
#include "PresenceFilter_WinRT.h"
#include "SocialEvent_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

public ref class SocialManager sealed
{
public:
    /// <summary>
    /// Gets the social_manager singleton instance
    /// </summary>
    static property SocialManager^ SingletonInstance
    {
        SocialManager^ get();
    }

    /// <summary>
    /// Create a social graph for the specified local user
    ///</summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="extraDetailLevel">A bitmask of extra detail to include in the xbox_social_user</param>
    void AddLocalUser(
        _In_ XboxLiveUser_t user,
        _In_ SocialManagerExtraDetailLevel extraDetailLevel
        );

    /// <summary>
    /// Removes a social graph for the specified local user
    ///</summary>
    /// <param name="user">Xbox Live User</param>
    void RemoveLocalUser(
        _In_ XboxLiveUser_t user
        );

    /// <summary>
    /// Called whenever the title wants to update the social graph and get list of change events
    /// Events that are returned will only be for the page that is is currently selected
    /// Must be called every frame for data to be up to date
    ///</summary>
    /// <returns> The list of what has changed in between social graph updates</returns>
    Windows::Foundation::Collections::IVectorView<SocialEvent^>^ DoWork();

    /// <summary>
    /// Constructs a social social user group, which is a collection of users with social information
    ///</summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="presenceFilter">The restriction of users based on their presence and title activity</param>
    /// <param name="relationshipFilter">The restriction of users based on their relationship to the calling user</param>
    /// <returns>The created view</returns>
    XboxSocialUserGroup^ CreateSocialUserGroupFromFilters(
        _In_ XboxLiveUser_t user,
        _In_ PresenceFilter presenceFilter,
        _In_ RelationshipFilter relationshipFilter
        );

    /// <summary>
    /// Constructs a social social user group, which is a collection of users with social information
    ///</summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="xboxUserIdList">List of users to populate the view with</param>
    /// <returns>The created view</returns>
    XboxSocialUserGroup^ CreateSocialUserGroupFromList(
        _In_ XboxLiveUser_t user,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIdList
        );

    /// <summary>
    /// Destroys a created social social user group
    /// This will stop updating the social user group and remove tracking for any users the social user group holds
    ///</summary>
    /// <param name="xboxSocialUserGroup">The social social user group to destroy and stop tracking</param>
    void DestroySocialUserGroup(_In_ XboxSocialUserGroup^ xboxSocialUserGroup);

    /// <summary>
    /// Returns all local users who have been added to the social manager
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<XboxLiveUser_t>^ LocalUsers
    {
        Windows::Foundation::Collections::IVectorView<XboxLiveUser_t>^ get();
    };

    /// <summary>
    /// Updates specified social user group to new group of users
    /// Does a diff to see which users have been added or removed from 
    /// </summary>
    /// <param name="socialGroup">The xbox social user group to add users to</param>
    /// <param name="users">List of users to add to the xbox social user group</param>
    void UpdateSocialUserGroup(
        _In_ XboxSocialUserGroup^ socialGroup,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ users
        );

    /// <summary>
    /// Whether to enable social manager to poll every 30 seconds from the presence service 
    /// </summary>
    /// <param name="user">Xbox Live User</param>
    /// <param name="shouldEnablePolling">Whether or not polling should enabled</param>
    /// <returns>An xbox_live_result representing the success enabling polling</returns>
    void SetRichPresencePollingState(
        _In_ XboxLiveUser_t user,
        _In_ bool shouldEnablePolling
        );

internal:
    SocialManager();
    std::shared_ptr<xbox::services::social::manager::social_manager> GetCppObj() const;
    void LogState();

private:
    Windows::Foundation::Collections::IVectorView<XboxLiveUser_t>^ m_localUserVector;
    size_t m_previousSize;
    std::shared_ptr<xbox::services::social::manager::social_manager> m_cppObj;
    bool m_localUserChanged;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END