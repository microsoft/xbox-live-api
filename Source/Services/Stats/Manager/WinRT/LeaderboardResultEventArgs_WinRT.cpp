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
#include "LeaderboardresultEventArgs_WinRT.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::leaderboard;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN

LeaderboardResultEventArgs::LeaderboardResultEventArgs(
    _In_ std::shared_ptr<xbox::services::stats::manager::leaderboard_result_event_args> cppObj
) :
    m_cppObj(std::move(cppObj)
    )
{
    auto result = m_cppObj->result();
    THROW_IF_ERR(result);
    m_result = ref new Leaderboard::LeaderboardResult(result.payload());
}

Leaderboard::LeaderboardResult^ LeaderboardResultEventArgs::Result::get()
{
    return m_result;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END