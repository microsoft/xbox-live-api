// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/achievements.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_requirement::achievement_requirement()
{
}

achievement_requirement::achievement_requirement(
    _In_ string_t id,
    _In_ string_t currentProgressValue,
    _In_ string_t targetProgressValue
    ) :
    m_id(std::move(id)),
    m_currentProgressValue(std::move(currentProgressValue)),
    m_targetProgressValue(std::move(targetProgressValue))
{
}

const string_t&
achievement_requirement::id() const
{
    return m_id;
}

const string_t&
achievement_requirement::current_progress_value() const
{
    return m_currentProgressValue;
}

const string_t&
achievement_requirement::target_progress_value() const
{
    return m_targetProgressValue;
}

xbox_live_result<achievement_requirement>
achievement_requirement::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<achievement_requirement>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementRequirement = achievement_requirement(
        utils::extract_json_string(json, _T("id"), errc, true),
        utils::extract_json_string(json, _T("current"), errc, true),
        utils::extract_json_string(json, _T("target"), errc, true)
        );

    return xbox_live_result<achievement_requirement>(achievementRequirement, errc);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END