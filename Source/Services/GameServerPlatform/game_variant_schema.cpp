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

game_variant_schema::game_variant_schema()
{
}

game_variant_schema::game_variant_schema(
    _In_ string_t schemaId,
    _In_ string_t m_schemaContent,
    _In_ string_t schemaName
    ) :
    m_schemaId(std::move(schemaId)),
    m_schemaContent(std::move(m_schemaContent)),
    m_schemaName(std::move(schemaName))
{
}

const string_t& 
game_variant_schema::schema_id() const
{
    return m_schemaId;
}

const string_t& 
game_variant_schema::schema_content() const
{
    return m_schemaContent;
}

const string_t& 
game_variant_schema::schema_name() const
{
    return m_schemaName;
}

xbox_live_result<game_variant_schema>
game_variant_schema::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<game_variant_schema>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto result = game_variant_schema(
        utils::extract_json_string(json, _T("variantSchemaId"), errc),
        utils::extract_json_string(json, _T("schemaContent"), errc),
        utils::extract_json_string(json, _T("name"), errc)
        );

    return xbox_live_result<game_variant_schema>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END
