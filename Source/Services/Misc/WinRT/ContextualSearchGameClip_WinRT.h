// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/contextual_search_service.h"
#include "ContextualSearchGameClipThumbnail_WinRT.h"
#include "ContextualSearchGameClipUriInfo_WinRT.h"
#include "ContextualSearchGameClipStat_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN

/// <summary>Describes the type of game clip</summary>
public enum class ContextualSearchGameClipType
{
    /// <summary>Game clip type is unknown or not set. (not valid for upload or queries)</summary>
    None = xbox::services::contextual_search::contextual_search_game_clip_type::none,

    /// <summary>Game clip is initiated by a developer / title</summary>
    DeveloperInitiated = xbox::services::contextual_search::contextual_search_game_clip_type::developer_initiated,

    /// <summary>Game clip is an achievement type</summary>
    Achievement = xbox::services::contextual_search::contextual_search_game_clip_type::achievement,

    /// <summary>Game clip is a user-generated type</summary>
    UserGenerated = xbox::services::contextual_search::contextual_search_game_clip_type::user_generated
};

/// <summary>Contains information about a contextual search game clip.</summary>
public ref class ContextualSearchGameClip sealed
{
public:
    /// <summary>
    /// The localized version of the clip's name based on the input locale of the request as looked up from the title management system.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ClipName, clip_name);

    /// <summary>
    /// Specifies the duration of the game clip in seconds.
    /// </summary>
    DEFINE_PROP_GET_OBJ(DurationInSeconds, duration_in_seconds, uint64);

    /// <summary>
    /// The ID assigned to the game clip.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(GameClipId, game_clip_id);

    /// <summary>
    /// the locale the game clip was created under.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(GameClipLocale, game_clip_locale);

    /// <summary>
    /// Array of playback options for this clip.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipUriInfo^>^ GameClipUris { Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipUriInfo^>^ get(); }

    /// <summary>
    /// Array of thumbnails for this clip.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipThumbnail^>^ Thumbnails { Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipThumbnail^>^ get(); }

    /// <summary>
    /// The type of clip. Can be multiple values and if so will be comma delimited.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(GameClipType, game_clip_type, Microsoft::Xbox::Services::ContextualSearch::ContextualSearchGameClipType);

    /// <summary>
    /// The number of views associated with a game clip.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Views, views, uint64);

    /// <summary>
    /// The XUID of the owner of the game clip
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// Stats associated with the game clip. 
    /// This contains the initial values of stats at the start of the game clip and any new value a stat had during the recording of the game clip. 
    /// Note that these are only the stats configured for intermedia in the title configuration.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipStat^>^ Stats { Windows::Foundation::Collections::IVectorView<ContextualSearchGameClipStat^>^ get(); }

internal:
    ContextualSearchGameClip(
        _In_ xbox::services::contextual_search::contextual_search_game_clip cppObj
        );

private:
    xbox::services::contextual_search::contextual_search_game_clip m_cppObj;
    Windows::Foundation::Collections::IVector<ContextualSearchGameClipThumbnail^>^ m_thumbnails;
    Windows::Foundation::Collections::IVector<ContextualSearchGameClipUriInfo^>^ m_uriInfos;
    Windows::Foundation::Collections::IVector<ContextualSearchGameClipStat^>^ m_stats;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END
