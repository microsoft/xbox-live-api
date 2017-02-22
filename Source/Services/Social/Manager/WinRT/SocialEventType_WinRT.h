// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

public enum class SocialEventType
{
    /// <summary>Users added to social graph</summary>
    UsersAddedToSocialGraph = xbox::services::social::manager::social_event_type::users_added_to_social_graph,

    /// <summary>Users removed from</summary>
    UsersRemovedFromSocialGraph = xbox::services::social::manager::social_event_type::users_removed_from_social_graph,

    /// <summary>Users presence record has changed</summary>
    PresenceChanged = xbox::services::social::manager::social_event_type::presence_changed,

    /// <summary>Users profile information has changed</summary>
    ProfilesChanged = xbox::services::social::manager::social_event_type::profiles_changed,

    /// <summary>Relationship to users has changed</summary>
    SocialRelationshipsChanged = xbox::services::social::manager::social_event_type::social_relationships_changed,

    /// <summary>Social graph load complete from adding a local user</summary>
    LocalUserAdded = xbox::services::social::manager::social_event_type::local_user_added,

    /// <summary>Social graph removal complete</summary>
    LocalUserRemoved = xbox::services::social::manager::social_event_type::local_user_removed,

    /// <summary>Xbox Social User Group load complete (will only trigger for views that take a list of users)</summary>
    SocialUserGroupLoaded = xbox::services::social::manager::social_event_type::social_user_group_loaded,

    /// <summary>Social user group updated</summary>
    SocialUserGroupUpdated = xbox::services::social::manager::social_event_type::social_user_group_updated
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END