// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi\stats_manager_c.h"
#include "xsapi\stats_manager.h"

using namespace xbox::services::system;
using namespace xbox::services::stats::manager;

struct XSAPI_STATS_MANAGER_VARS
{
public:
    xbox_live_result<void> cppVoidResult;
    xbox_live_result<stat_value> cppStatValueResult;

    std::vector<XSAPI_STAT_EVENT *> cEvents;

    std::vector<utility::string_t> cppStatNameList;
    std::vector<std::string> cStatNameStringList;
    std::vector<PCSTR> cStatNameCharList;

    std::map<xbox_live_user_t, XSAPI_XBOX_LIVE_USER*> cUsersMapping;
};