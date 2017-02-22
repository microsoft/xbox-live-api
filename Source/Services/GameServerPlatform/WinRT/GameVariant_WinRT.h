// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/game_server_platform.h"
#include "GameVariantSchema_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN
/// <summary>
/// Represents a game variant.
/// </summary>

public ref class GameVariant sealed
{
    // Example:
    //        {
    //            "gamevariantId":"041aae97-d359-4244-9b93-2d23ba27cd19",
    //            "name":"Example Variant",
    //            "rank":"100",
    //            "isPublisher":"False"
    //            "gameVariantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
    //        }

public:
    /// <summary>
    /// ID of a variant.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>
    /// Name of variant.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// A boolean field to indicate whether the variant was user-created or publisher-created.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsPublisher, is_publisher, bool);

    /// <summary>
    /// A boolean field to indicate whether the variant was user-created or publisher-created.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Rank, rank, uint64);

    /// <summary>
    /// Content of the variant's schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaContent, schema_content);

    /// <summary>
    /// Name of the variant's schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaName, schema_name);

    /// <summary>
    /// ID of the variant's schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaId, schema_id);

internal:
    GameVariant(
        _In_ xbox::services::game_server_platform::game_variant cppObj
        );

private:
    xbox::services::game_server_platform::game_variant m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
