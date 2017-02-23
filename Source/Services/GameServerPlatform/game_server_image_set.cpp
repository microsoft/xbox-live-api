// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/game_server_platform.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN

game_server_image_set::game_server_image_set() :
    m_minPlayers(0),
    m_maxPlayers(0),
    m_selectionOrder(0)
{
}

game_server_image_set::game_server_image_set(
    _In_ uint64_t minPlayers,
    _In_ uint64_t maxPlayers,
    _In_ string_t id,
    _In_ string_t name,
    _In_ uint64_t selectionOrder,
    _In_ string_t schemaId,
    _In_ std::map<string_t, string_t> tags
    ) :
    m_minPlayers(minPlayers),
    m_maxPlayers(maxPlayers),
    m_id(std::move(id)),
    m_name(std::move(name)),
    m_selectionOrder(selectionOrder),
    m_schemaId(std::move(schemaId)),
    m_tags(std::move(tags))
{
}

const uint64_t game_server_image_set::min_players() const
{
    return m_minPlayers;
}

uint64_t game_server_image_set::max_players() const
{
    return m_maxPlayers;
}

const string_t& game_server_image_set::id() const
{
    return m_id;
}

const string_t& game_server_image_set::name() const
{
    return m_name;
}

uint64_t game_server_image_set::selection_order() const
{
    return m_selectionOrder;
}

const string_t& game_server_image_set::schema_content() const
{
    return m_schema.schema_content();
}

const string_t& game_server_image_set::schema_name() const
{
    return m_schema.schema_name();
}

const string_t& game_server_image_set::schema_id() const
{
    return m_schemaId;
}

const std::map<string_t, string_t>& game_server_image_set::tags() const
{
    return m_tags;
}

xbox_live_result<game_server_image_set>
game_server_image_set::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<game_server_image_set>();

    std::error_code errc = xbox_live_error_code::no_error;

    std::map<string_t, string_t> tags;
    web::json::value tagField(utils::extract_json_field(json, _T("tags"), errc, false));
    if (tagField.is_array())
    {
        const web::json::array& arr(tagField.as_array());
        for (const auto& element : arr)
        {
            string_t name = utils::extract_json_string(element, _T("name"), errc);
            string_t value = utils::extract_json_string(element, _T("value"), errc);
            tags[name] = value;
        }
    }

    auto result = game_server_image_set(
        utils::extract_json_string_to_uint64(json, _T("minRequiredPlayers"), errc),
        utils::extract_json_string_to_uint64(json, _T("maxAllowedPlayers"), errc),
        utils::extract_json_string(json, _T("gsiSetId"), errc),
        utils::extract_json_string(json, _T("gsiSetName"), errc),
        utils::extract_json_string_to_uint64(json, _T("selectionOrder"), errc),
        utils::extract_json_string(json, _T("variantSchemaId"), errc),
        tags
        );

    return xbox_live_result<game_server_image_set>(result, errc);
}

void game_server_image_set::_Set_schema(_In_ game_variant_schema schema)
{
    m_schema = std::move(schema);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
