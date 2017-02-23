// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN

/// <summary>
/// Detail level controls how much information is exposed in each xbox_live_social_graph_user
/// Detail level can only be set on construction of social_manager
///</summary>
[Platform::Metadata::Flags]
public enum class SocialManagerExtraDetailLevel : unsigned int
{
    /// <summary>Only get default PeopleHub information (presence, profile)</summary>
    NoExtraDetail = xbox::services::social::manager::social_manager_extra_detail_level::no_extra_detail,

    /// <summary>Add extra detail for the title history for the users</summary>
    TitleHistoryLevel = xbox::services::social::manager::social_manager_extra_detail_level::title_history_level,

    /// <summary>Add extra detail for the preferred color for the users</summary>
    PreferredColorLevel = xbox::services::social::manager::social_manager_extra_detail_level::preferred_color_level,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END
