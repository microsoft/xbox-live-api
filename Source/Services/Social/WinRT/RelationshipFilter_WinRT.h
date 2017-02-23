// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

/// <summary>
/// Possible relationship types to filter by
///</summary>
public enum class RelationshipFilter
{
    /// <summary>Friends of the user (user is following)</summary>
    Friends = xbox::services::social::manager::relationship_filter::friends,

    /// <summary>Favorites of the user</summary>
    Favorite = xbox::services::social::manager::relationship_filter::favorite
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END