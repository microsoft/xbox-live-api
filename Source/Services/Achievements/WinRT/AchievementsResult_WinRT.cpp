// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "AchievementsResult_winrt.h"
#include "AchievementService_winrt.h"
#include "Utils_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace xbox::services::achievements;
using namespace concurrency;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

AchievementsResult::AchievementsResult(
    _In_ xbox::services::achievements::achievements_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_items = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<Achievement, achievement>(m_cppObj.items());
}

IVectorView<Achievement^>^
AchievementsResult::Items::get()
{
    return m_items->GetView();
}

IAsyncOperation<AchievementsResult^>^
AchievementsResult::GetNextAsync(
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_next(maxItems)
    .then([](xbox_live_result<achievements_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new AchievementsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END