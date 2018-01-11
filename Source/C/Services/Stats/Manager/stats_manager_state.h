// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

struct XSAPI_STATS_MANAGER_VARS
{
public:
    xbox::services::xbox_live_result<void> cppVoidResult;
    xbox::services::xbox_live_result<xbox::services::stats::manager::stat_value> cppStatValueResult;

    std::vector<XSAPI_STAT_EVENT *> cEvents;

    std::vector<utility::string_t> cppStatNameList;
    std::vector<std::string> cStatNameStringList;
    std::vector<PCSTR> cStatNameCharList;

    std::map<xbox_live_user_t, XSAPI_XBOX_LIVE_USER*> cUsersMapping;
};