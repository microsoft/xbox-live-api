// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/achievements.h"
#include "achievements_internal.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_progression::achievement_progression()
{
}

achievement_progression::achievement_progression(
    _In_ std::shared_ptr<achievement_progression_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_VECTOR_INTERNAL_TYPE(achievement_progression, achievement_requirement, requirements);
DEFINE_GET_OBJECT_REF(achievement_progression, utility::datetime, time_unlocked);

achievement_progression_internal::achievement_progression_internal(
    _In_ xsapi_internal_vector<std::shared_ptr<achievement_requirement_internal>> requirements,
    _In_ utility::datetime timeUnlocked
    ) :
    m_requirements(std::move(requirements)),
    m_timeUnlocked(std::move(timeUnlocked))
{
}

const xsapi_internal_vector<std::shared_ptr<achievement_requirement_internal>>&
achievement_progression_internal::requirements() const
{
    return m_requirements;
}

const utility::datetime& 
achievement_progression_internal::time_unlocked() const
{
    return m_timeUnlocked;
}

xbox_live_result<std::shared_ptr<achievement_progression_internal>>
achievement_progression_internal::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<std::shared_ptr<achievement_progression_internal>>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementProgression = xsapi_allocate_shared<achievement_progression_internal>(
        utils::extract_json_vector<std::shared_ptr<achievement_requirement_internal>>(achievement_requirement_internal::_Deserialize, json, "requirements", errc, true),
        utils::extract_json_time(json, _T("timeUnlocked"), errc, true)
        );

    return xbox_live_result<std::shared_ptr<achievement_progression_internal>>(achievementProgression, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END