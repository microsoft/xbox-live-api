// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once

#include "threadpool_c.h"
#include "xsapi-c/xbox_live_app_config_c.h"
#include "achievements_state.h"
#include "social_manager_state.h"
#include "stats_manager_state.h"
#include "title_storage_state.h"

struct XSAPI_XBOX_LIVE_USER;

struct xsapi_singleton_c
{
    xsapi_singleton_c();

    std::mutex m_singletonLock;

    std::unique_ptr<xsapi_thread_pool> m_threadPool;

    std::unique_ptr<title_storage_state> m_titleStorageState;

    std::mutex m_usersLock;
    std::map<std::string, XSAPI_XBOX_LIVE_USER*> m_signedInUsers;

    std::unique_ptr<XSAPI_XBOX_LIVE_APP_CONFIG> m_appConfigSingleton;
    std::string m_scid;
    std::string m_environment;
    std::string m_sandbox;

    std::unique_ptr<XSAPI_ACHIEVEMENTS_STATE> m_achievementsState;
    std::unique_ptr<XSAPI_SOCIAL_MANAGER_VARS> m_socialVars;
    std::unique_ptr<XSAPI_STATS_MANAGER_VARS> m_statsVars;
};

xsapi_singleton_c* get_xsapi_singleton_c(_In_ bool createIfRequired = false);
void cleanup_xsapi_singleton_c();

void verify_global_init();
