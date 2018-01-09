// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/achievements_c.h"

struct XSAPI_ACHIEVEMENTS_STATE
{
public:
    std::recursive_mutex m_lock;

    std::set<XSAPI_ACHIEVEMENTS_RESULT*> m_achievementResults;
    std::set<XSAPI_ACHIEVEMENT*> m_achievements;
};