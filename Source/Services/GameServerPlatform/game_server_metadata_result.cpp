// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/game_server_platform.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN

game_server_metadata_result::game_server_metadata_result()
{
}

game_server_metadata_result::game_server_metadata_result(
    _In_ std::vector< game_variant > gameVariants,
    _In_ std::vector< game_variant_schema > gameVariantSchemas,
    _In_ std::vector< game_server_image_set > gameServerImageSets
    ) :
    m_gameVariants(std::move(gameVariants)),
    m_gameVariantSchemas(std::move(gameVariantSchemas)),
    m_gameServerImageSets(std::move(gameServerImageSets))
{
}

const std::vector< game_variant >& 
game_server_metadata_result::game_variants()
{
    return m_gameVariants;
}

const std::vector< game_server_image_set >& 
game_server_metadata_result::game_server_image_sets() const
{
    return m_gameServerImageSets;
}

xbox_live_result<game_server_metadata_result>
game_server_metadata_result::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<game_server_metadata_result>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto variantSchemas = utils::extract_json_vector<game_variant_schema>(game_variant_schema::_Deserialize, json, _T("variantSchemas"), errc, false);
    auto variants = utils::extract_json_vector<game_variant>(game_variant::_Deserialize, json, _T("variants"), errc, false);
    auto gameServerImageSets = utils::extract_json_vector<game_server_image_set>(game_server_image_set::_Deserialize, json, _T("gsiSets"), errc, false);

    // Build a map of schemaIds -> schema
    std::map<string_t, game_variant_schema> schemaMap;
    for (const auto& schema : variantSchemas)
    {
        schemaMap[schema.schema_id()] = schema;
    }

    // For each variant and set, find its schema and set it.
    for (auto& variant : variants)
    {
        auto it = schemaMap.find(variant.schema_id());
        if (it != schemaMap.end())
        {
            variant._Set_schema(it->second);
        }
    }

    for (auto& imageSet : gameServerImageSets)
    {
        auto it = schemaMap.find(imageSet.schema_id());
        if (it != schemaMap.end())
        {
            imageSet._Set_schema(it->second);
        }
    }

    auto result = game_server_metadata_result(
        variants,
        variantSchemas,
        gameServerImageSets
        );

    return xbox_live_result<game_server_metadata_result>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
