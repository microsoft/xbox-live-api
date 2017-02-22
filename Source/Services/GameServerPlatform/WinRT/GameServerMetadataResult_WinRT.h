// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/game_server_platform.h"
#include "GameVariant_WinRT.h"
#include "GameVariantSchema_WinRT.h"
#include "GameServerImageSet_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN
/// <summary>
/// Represents information on game server image sets and game variants.
/// </summary>

public ref class GameServerMetadataResult sealed
{
    // Example:
    //{
    //    "variants" :
    //    [
    //        {
    //            "gamevariantId":"041aae97-d359-4244-9b93-2d23ba27cd19",
    //            "name":"Example Variant",
    //            "rank":"100",
    //            "isPublisher":"False"
    //            "gameVariantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
    //        }
    //    ],
    //
    //    "variantSchemas" :
    //    [
    //        {
    //            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
    //            "schemaContent":"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n <xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\r\n<xs:element name=\"testSchema\">\r\n </xs:element> \r\n </xs:schema> ",
    //            "name":"Example Variant"
    //        }
    //    ],
    //
    //    "gsiSets" :
    //    [
    //        {
    //            "gsiSetId":"7efdf3a7-9ce4-4d28-b889-4aeea98727c1",
    //            "minRequiredPlayers":"2",
    //            "maxAllowedPlayers":"50",
    //            "selectionOrder":"1",
    //            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806"
    //        }
    //    ]
    //}

public:
    /// <summary>
    /// The collection of game variants.
    /// </summary>        
    property Windows::Foundation::Collections::IVectorView<GameVariant^>^ GameVariants
    {
        Windows::Foundation::Collections::IVectorView<GameVariant^>^ get();
    }

    /// <summary>
    /// The collection of game server image sets.
    /// </summary>        
    property Windows::Foundation::Collections::IVectorView<GameServerImageSet^>^ GameServerImageSets
    {
        Windows::Foundation::Collections::IVectorView<GameServerImageSet^>^ get();
    }

internal:
    GameServerMetadataResult(
        _In_ xbox::services::game_server_platform::game_server_metadata_result cppObj
        );

private:
    xbox::services::game_server_platform::game_server_metadata_result m_cppObj;
    Windows::Foundation::Collections::IVector<GameVariant^>^ m_gameVariants;
    Windows::Foundation::Collections::IVector<GameServerImageSet^>^ m_gameServerImageSets;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
