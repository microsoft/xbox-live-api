// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "LeaderboardRow.h"
#include "xsapi/leaderboard.h"

using namespace pplx;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN

LeaderboardRow::LeaderboardRow(
    _In_ xbox::services::leaderboard::leaderboard_row cppObj) :
    m_cppObj(std::move(cppObj))
{
    m_values = ref new Vector<String^>();

    for (const auto& value : m_cppObj.column_values())
    {
        m_values->Append(ref new String(value.c_str()));
    }
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^
LeaderboardRow::Values::get()
{
    return m_values->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END