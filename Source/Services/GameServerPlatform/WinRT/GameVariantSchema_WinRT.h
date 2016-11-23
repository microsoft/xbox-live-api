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
#include "xsapi/game_server_platform.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN
/// <summary>
/// Represents a game variant schema.
/// </summary>

ref class GameVariantSchema sealed
{
    // Example:
    //        {
    //            "variantSchemaId":"e96582b8-d78d-49e4-afe8-8b7ea3d10806",
    //            "schemaContent":"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n <xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\r\n<xs:element name=\"testSchema\">\r\n </xs:element> \r\n </xs:schema> ",
    //            "name":"Example Variant"
    //        }

public:
    /// <summary>
    /// The ID of the schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaId, schema_id);

    /// <summary>
    /// The content of the schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaContent, schema_content);

    /// <summary>
    /// The name of the schema.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SchemaName, schema_name);

internal:
    GameVariantSchema(
        _In_ xbox::services::game_server_platform::game_variant_schema cppObj
        );

private:
    xbox::services::game_server_platform::game_variant_schema m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END
