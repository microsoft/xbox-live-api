// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/achievements.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_progression::achievement_progression()
{
}

achievement_progression::achievement_progression(
    _In_ std::vector<achievement_requirement> requirements,
    _In_ utility::datetime timeUnlocked
    ):
    m_requirements(std::move(requirements)),
    m_timeUnlocked(std::move(timeUnlocked))
{
}

const std::vector<achievement_requirement>&
achievement_progression::requirements() const
{
    return m_requirements;
}

const utility::datetime& 
achievement_progression::time_unlocked() const
{
    return m_timeUnlocked;
}

xbox_live_result<achievement_progression>
achievement_progression::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<achievement_progression>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementProgression = achievement_progression(
        utils::extract_json_vector<achievement_requirement>(achievement_requirement::_Deserialize, json, _T("requirements"), errc, true),
        utils::extract_json_time(json, _T("timeUnlocked"), errc, true)
        );

    return xbox_live_result<achievement_progression>(achievementProgression, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END