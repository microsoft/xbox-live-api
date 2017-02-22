// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/game_server_platform.h"
#include "GameVariantSchema_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN
/// <summary>
/// Represents a set of game server images.
/// </summary>

public ref class GameServerImageSet sealed
{
    // Example:
    //        {
    //            "gsiSetId":"7efdf3a7-9ce4-4d28-b889-4aeea98727c1",
    //            "minRequiredPlayers":"2",
    //            "maxAllowedPlayers":"50",
    //            "selectionOrder":"1",
    //            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806"
    //        }

public:
    /// <summary>
    /// The minimum number of players for the variant.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MinPlayers, min_players, uint64);

    /// <summary>
    /// The maximum number of players for the variant.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MaxPlayers, max_players, uint64);

    /// <summary>
    /// The set ID of the variant.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>
    /// The set Name of the variant.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// Selection order for this set.
    /// </summary>
    DEFINE_PROP_GET_OBJ(SelectionOrder, selection_order, uint64);

    /// <summary>
    /// Content of this set's schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaContent, schema_content);

    /// <summary>
    /// Name of this set's schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaName, schema_name);

    /// <summary>
    /// ID of this set's schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaId, schema_id);

    /// <summary>
    /// Tags are key value metadata on game variant that can be set at the time of ingestion and then use them as filters or additional metadata.
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ Tags
    {
        Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ get();
    }

internal:
    GameServerImageSet(
        _In_ xbox::services::game_server_platform::game_server_image_set cppObj
        );

private:
    xbox::services::game_server_platform::game_server_image_set m_cppObj;
    Platform::Collections::Map<Platform::String^, Platform::String^>^ m_tagsMap;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
