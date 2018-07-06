// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "leaderboard_query.h"
#include "xsapi/leaderboard.h"
#include "leaderboard_service_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

leaderboard_result::leaderboard_result() 
{
}

leaderboard_result::leaderboard_result(
    _In_ std::shared_ptr<leaderboard_result_internal> leaderboardResultInternal
    ) :
    m_leaderboardResultInternal(std::move(leaderboardResultInternal))
{
}

string_t leaderboard_result::display_name() const
{
    return utils::string_t_from_internal_string(m_leaderboardResultInternal->display_name());
}

uint32_t leaderboard_result::total_row_count() const
{
    return m_leaderboardResultInternal->total_row_count();
}

const std::vector<leaderboard_column>& leaderboard_result::columns() const
{
    return m_leaderboardResultInternal->columns();
}

const std::vector<leaderboard_row>& leaderboard_result::rows() const
{
    return m_leaderboardResultInternal->rows();
}

bool leaderboard_result::has_next() const
{
    return m_leaderboardResultInternal->has_next();
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_result::get_next(_In_ uint32_t maxItems) const
{
    pplx::task_completion_event<xbox_live_result<leaderboard_result>> tce;

    auto hr = m_leaderboardResultInternal->get_next(
        maxItems,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<leaderboard_result_internal>> result)
    {
        tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(leaderboard_result, result));
    });

    if (FAILED(hr))
    {
        std::error_code errorCode = std::make_error_code(static_cast<xbox_live_error_code>(hr));
        return pplx::task_from_result(xbox_live_result<leaderboard_result>(errorCode));
    }
    return pplx::task<xbox_live_result<leaderboard_result>>(tce);
}

xbox_live_result<leaderboard_query> leaderboard_result::get_next_query() const
{
    return m_leaderboardResultInternal->get_next_query();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END