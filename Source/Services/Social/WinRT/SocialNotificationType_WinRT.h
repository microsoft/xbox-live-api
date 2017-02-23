// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN

public enum class SocialNotificationType
{
    /// <summary>
    /// unknown
    /// </summary>
    Unknown = xbox::services::social::social_notification_type::unknown,

    /// <summary>
    /// User(s) were added.
    /// </summary>
    added = xbox::services::social::social_notification_type::added,

    /// <summary>
    /// User(s) data changed.
    /// </summary>
    changed = xbox::services::social::social_notification_type::changed,

    /// <summary>
    /// User(s) were removed.
    /// </summary>
    removed = xbox::services::social::social_notification_type::removed
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END