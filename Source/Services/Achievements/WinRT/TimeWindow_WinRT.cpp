// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TimeWindow_winrt.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

AchievementTimeWindow::AchievementTimeWindow(
    _In_ xbox::services::achievements::achievement_time_window cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END