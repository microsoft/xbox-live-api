// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once

#include <httpClient/httpClient.h>
#include <xsapi-c/types_c.h>
#include <xsapi-c/pal.h>
#include <xsapi-c/errors_c.h>
#include <xsapi-c/xbox_live_global_c.h>
#include <xsapi-c/platform_c.h>
#include <xsapi-c/xbox_live_context_settings_c.h>
#include <xsapi-c/xbox_live_context_c.h>
#include <xsapi-c/presence_c.h>
#include <xsapi-c/profile_c.h>
#include <xsapi-c/social_c.h>
#include <xsapi-c/social_manager_c.h>
#include <xsapi-c/string_verify_c.h>
#include <xsapi-c/achievements_c.h>
#include <xsapi-c/achievements_manager_c.h>
#include <xsapi-c/multiplayer_c.h>
#include <xsapi-c/multiplayer_activity_c.h>
#include <xsapi-c/multiplayer_manager_c.h>
#include <xsapi-c/matchmaking_c.h>
#include <xsapi-c/privacy_c.h>
#include <xsapi-c/title_managed_statistics_c.h>
#include <xsapi-c/user_statistics_c.h>
#include <xsapi-c/events_c.h>
#include <xsapi-c/real_time_activity_c.h>
#include <xsapi-c/leaderboard_c.h>
#include <xsapi-c/http_call_c.h>
#include <xsapi-c/title_storage_c.h>
#include <xsapi-c/game_invite_c.h>
#if XSAPI_NOTIFICATION_SERVICE
#include <xsapi-c/notification_c.h>
#endif