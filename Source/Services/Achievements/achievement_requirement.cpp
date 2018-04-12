// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/achievements.h"
#include "achievements_internal.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_requirement::achievement_requirement()
{
}

achievement_requirement::achievement_requirement(
    _In_ std::shared_ptr<achievement_requirement_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(achievement_requirement, id);
DEFINE_GET_STRING(achievement_requirement, current_progress_value);
DEFINE_GET_STRING(achievement_requirement, target_progress_value);

achievement_requirement_internal::achievement_requirement_internal(
    _In_ xsapi_internal_string id,
    _In_ xsapi_internal_string currentProgressValue,
    _In_ xsapi_internal_string targetProgressValue
    ) :
    m_id(std::move(id)),
    m_currentProgressValue(std::move(currentProgressValue)),
    m_targetProgressValue(std::move(targetProgressValue))
{
}

const xsapi_internal_string&
achievement_requirement_internal::id() const
{
    return m_id;
}

const xsapi_internal_string&
achievement_requirement_internal::current_progress_value() const
{
    return m_currentProgressValue;
}

const xsapi_internal_string&
achievement_requirement_internal::target_progress_value() const
{
    return m_targetProgressValue;
}

xbox_live_result<std::shared_ptr<achievement_requirement_internal>>
achievement_requirement_internal::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<achievement_requirement_internal>>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementRequirement = xsapi_allocate_shared<achievement_requirement_internal>(
        utils::internal_string_from_string_t(utils::extract_json_string(json, _T("id"), errc, true)),
        utils::internal_string_from_string_t(utils::extract_json_string(json, _T("current"), errc, true)),
        utils::internal_string_from_string_t(utils::extract_json_string(json, _T("target"), errc, true))
        );

    return xbox_live_result<std::shared_ptr<achievement_requirement_internal>>(achievementRequirement, errc);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END