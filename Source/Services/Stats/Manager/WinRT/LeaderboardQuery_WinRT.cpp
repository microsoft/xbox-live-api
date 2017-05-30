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
#include "LeaderboardQuery_WinRT.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::leaderboard;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

LeaderboardQuery::LeaderboardQuery()
{
    m_cppObj = leaderboard_query();
}

LeaderboardQuery::LeaderboardQuery(
    _In_ xbox::services::leaderboard::leaderboard_query cppObj) :
    m_cppObj(std::move(cppObj))
{
}

const xbox::services::leaderboard::leaderboard_query& LeaderboardQuery::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END