// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/achievements.h"
#include "achievements_internal.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_title_association::achievement_title_association()
{
}

achievement_title_association::achievement_title_association(
    _In_ string_t name,
    _In_ uint32_t id
    )
{
    m_internalObj = xsapi_allocate_shared<achievement_title_association_internal>(
                    utils::internal_string_from_string_t(std::move(name)),
                    std::move(id));
}

achievement_title_association::achievement_title_association(
    _In_ std::shared_ptr<achievement_title_association_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(achievement_title_association, name);
DEFINE_GET_UINT32(achievement_title_association, title_id);

achievement_title_association_internal::achievement_title_association_internal(
    _In_ xsapi_internal_string name,
    _In_ uint32_t id
    ) :
    m_name(std::move(name)),
    m_id(std::move(id))
{
}


const xsapi_internal_string& 
achievement_title_association_internal::name() const
{
    return m_name;
}

uint32_t
achievement_title_association_internal::title_id() const
{
    return m_id;
}

xbox_live_result<std::shared_ptr<achievement_title_association_internal>>
achievement_title_association_internal::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<achievement_title_association_internal>>();

    std::error_code errc = xbox_live_error_code::no_error;

    auto achievementTitleAssoication = xsapi_allocate_shared<achievement_title_association_internal>(
        utils::internal_string_from_string_t(utils::extract_json_string(json, _T("name"), errc, true)),
        utils::extract_json_int(json, _T("id"), errc, true)
        );

    return xbox_live_result<std::shared_ptr<achievement_title_association_internal>>(achievementTitleAssoication, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END