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
#include "shared_macros.h"
#include "xsapi/social.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

/// <summary> Defines values for the {view} field</summary>
public enum class SocialRelationship
{
    /// <summary>All the people on the user's people list</summary>
    All = xbox::services::social::xbox_social_relationship_filter::all,

    /// <summary>Filters to only the people on the user's people list that have the attribute "Favorite" associated with them.</summary>
    Favorite = xbox::services::social::xbox_social_relationship_filter::favorite,

    /// <summary>Filters to only the people on the user's people list that are also legacy Xbox Live friends</summary>
    LegacyXboxLiveFriends = xbox::services::social::xbox_social_relationship_filter::legacy_xbox_live_friends,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END
