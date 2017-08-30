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
#include "xsapi/stats_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

const xbox_live_result<leaderboard::leaderboard_result>& leaderboard_result_event_args::result()
{
    return m_result;
}

leaderboard_result_event_args::leaderboard_result_event_args(
    const xbox_live_result<leaderboard::leaderboard_result>& result
    ) :
    m_result(result)
{
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END