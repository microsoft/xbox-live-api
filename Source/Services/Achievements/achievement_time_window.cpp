// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/achievements.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN

achievement_time_window::achievement_time_window()
{
}

achievement_time_window::achievement_time_window(
    _In_ utility::datetime startDate,
    _In_ utility::datetime endDate
    ):
    m_startDate(std::move(startDate)),
    m_endDate(std::move(endDate))
{
}

const utility::datetime&
achievement_time_window::start_date() const
{
    return m_startDate;
}

const utility::datetime& 
achievement_time_window::end_date() const
{
    return m_endDate;
}

xbox_live_result<achievement_time_window>
achievement_time_window::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<achievement_time_window>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto achievementTimeWindow = achievement_time_window(
        utils::extract_json_time(json, _T("startDate"), errc, true),
        utils::extract_json_time(json, _T("endDate"), errc, true)
        );

    return xbox_live_result<achievement_time_window>(achievementTimeWindow, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END