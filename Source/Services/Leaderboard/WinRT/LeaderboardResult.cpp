// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "LeaderboardResult.h"
#include "xsapi/leaderboard.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::leaderboard;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

LeaderboardResult::LeaderboardResult(
    _In_ xbox::services::leaderboard::leaderboard_result cppObj) :
    m_cppObj(std::move(cppObj))
{
    m_rows = ref new Vector<LeaderboardRow^>();

    for (const auto& row : m_cppObj.rows())
    {
        m_rows->Append(ref new LeaderboardRow(row));
    }

    m_columns = ref new Vector<LeaderboardColumn^>();

    for (const auto& column : m_cppObj.columns())
    {
        m_columns->Append(ref new LeaderboardColumn(column));
    }
}

Windows::Foundation::Collections::IVectorView<LeaderboardColumn^>^
LeaderboardResult::Columns::get()
{
    return m_columns->GetView();
}

Windows::Foundation::Collections::IVectorView<LeaderboardRow^>^
LeaderboardResult::Rows::get()
{
    return m_rows->GetView();
}

Windows::Foundation::IAsyncOperation<LeaderboardResult^>^ 
LeaderboardResult::GetNextAsync(_In_ uint32 maxItems)
{
    auto task = m_cppObj.get_next(maxItems)
    .then([](xbox::services::xbox_live_result<leaderboard_result> lb)
    {
        return ref new LeaderboardResult(std::move(lb.payload()));
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END