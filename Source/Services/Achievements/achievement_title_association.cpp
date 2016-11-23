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
#include "xsapi/achievements.h"
#include "utils.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_title_association::achievement_title_association()
{
}

achievement_title_association::achievement_title_association(
    _In_ string_t name,
    _In_ uint32_t id
    ) :
    m_name(std::move(name)),
    m_id(id)
{
}

const string_t&
achievement_title_association::name() const
{
    return m_name;
}

uint32_t
achievement_title_association::title_id() const
{
    return m_id;
}

xbox_live_result<achievement_title_association>
achievement_title_association::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<achievement_title_association>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementTitleAssoication = achievement_title_association(
        utils::extract_json_string(json, _T("name"), errc, true),
        utils::extract_json_int(json, _T("id"), errc, true)
        );

    return xbox_live_result<achievement_title_association>(achievementTitleAssoication, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END