// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

social_user_group_loaded_event_args::social_user_group_loaded_event_args(
    _In_ std::shared_ptr<xbox_social_user_group> socialUserGroup
    ) :
    m_socialUserGroup(socialUserGroup)
{
}

const std::shared_ptr<xbox_social_user_group>&
social_user_group_loaded_event_args::social_user_group() const
{
    return m_socialUserGroup;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END