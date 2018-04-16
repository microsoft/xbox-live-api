// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

struct XBL_ACHIEVEMENT;
struct XBL_ACHIEVEMENTS_RESULT;

struct XBL_ACHIEVEMENTS_STATE
{
public:
    std::recursive_mutex m_lock;

    std::set<XBL_ACHIEVEMENTS_RESULT*> m_achievementResults;
    std::set<XBL_ACHIEVEMENT*> m_achievements;
};