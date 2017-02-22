// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

/// <summary>Enumeration values that indicate the media item type</summary>
public enum class MediaItemType
{
    /// <summary>Unknown media type.</summary>
    Unknown = xbox::services::marketplace::media_item_type::unknown,

    /// <summary>Game media type.</summary>
    Game = xbox::services::marketplace::media_item_type::game,

    /// <summary>GameV2 media type.</summary>
    GameV2 = xbox::services::marketplace::media_item_type::game_v2,

    /// <summary>Application media type.</summary>
    Application = xbox::services::marketplace::media_item_type::application,

    /// <summary>Game content media type.</summary>
    GameContent = xbox::services::marketplace::media_item_type::game_content,

    /// <summary>Game consumable media type.</summary>
    GameConsumable = xbox::services::marketplace::media_item_type::game_consumable,

    /// <summary>Subscription media type.</summary>
    Subscription = xbox::services::marketplace::media_item_type::subscription,

    /// <summary>Movie media type.</summary>
    Movie = xbox::services::marketplace::media_item_type::movie,

    /// <summary>Television Show media type.</summary>
    TelevisionShow = xbox::services::marketplace::media_item_type::television_show,

    /// <summary>Television Series media type.</summary>
    TelevisionSeries = xbox::services::marketplace::media_item_type::television_series,

    /// <summary>Television Season media type.</summary>
    TelevisionSeason = xbox::services::marketplace::media_item_type::television_season,

    /// <summary>Service detects if media type is an application or a game.</summary>
    ApplicationOrGameUnknown = xbox::services::marketplace::media_item_type::application_or_game_unknown,

    /// <summary>Game demo media type.</summary>
    GameDemo = xbox::services::marketplace::media_item_type::game_demo,

    /// <summary>WebVideo media type.</summary>
    WebVideo = xbox::services::marketplace::media_item_type::web_video,

    /// <summary>Request All media types for inventory calls.</summary>
    All = xbox::services::marketplace::media_item_type::all
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
