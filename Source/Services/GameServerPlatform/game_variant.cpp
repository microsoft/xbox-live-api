//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/game_server_platform.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN

game_variant::game_variant() :
    m_isPublisher(false),
    m_rank(0)
{
}

game_variant::game_variant(
    _In_ string_t id,
    _In_ string_t name,
    _In_ bool isPublisher,
    _In_ uint64_t rank,
    _In_ string_t schemaId
    ) :
    m_id(id),
    m_name(std::move(name)),
    m_isPublisher(isPublisher),
    m_rank(rank),
    m_schemaId(std::move(schemaId))
{
}

const string_t& 
game_variant::id() const
{
    return m_id;
}

const string_t& 
game_variant::name() const
{
    return m_name;
}

bool 
game_variant::is_publisher() const
{
    return m_isPublisher;
}

uint64_t 
game_variant::rank() const
{
    return m_rank;
}

const string_t& 
game_variant::schema_content() const
{
    return m_schema.schema_content();
}

const string_t& 
game_variant::schema_name() const
{
    return m_schema.schema_name();
}

const string_t& 
game_variant::schema_id() const
{
    return m_schemaId;
}

xbox_live_result<game_variant>
game_variant::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<game_variant>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto isPublisher = utils::extract_json_string(json, _T("isPublisher"), errc);

    auto result = game_variant(
        utils::extract_json_string(json, _T("gamevariantId"), errc),
        utils::extract_json_string(json, _T("name"), errc),
        utils::str_icmp(isPublisher, _T("True")) == 0,
        utils::extract_json_string_to_uint64(json, _T("rank"), errc),
        utils::extract_json_string(json, _T("gameVariantSchemaId"), errc)
        );

    return xbox_live_result<game_variant>(result, errc);
}

void game_variant::_Set_schema(_In_ game_variant_schema schema)
{
    m_schema = std::move(schema);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
