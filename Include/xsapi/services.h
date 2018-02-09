// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#pragma warning(disable: 4265)
#pragma warning(disable: 4266)
#pragma warning(disable: 4062)

#include "cpprest/http_msg.h"
#include "xsapi/types.h"
#include "xsapi/errors.h"
#include "xsapi/mem.h"
#include "xsapi/system.h"
#include "xsapi/service_call_logging_config.h"

#include "xsapi/leaderboard.h"
#include "xsapi/social_manager.h"
#include "xsapi/stats_manager.h"
#include "xsapi/http_call.h"
#include "xsapi/xbox_live_context_settings.h"
#include "xsapi/title_storage.h"
#include "xsapi/privacy.h"
#include "xsapi/profile.h"
#include "xsapi/clubs.h"

#if !defined(XBOX_LIVE_CREATORS_SDK)
#include "xsapi/events.h"
#include "xsapi/social.h"
#include "xsapi/achievements.h"
#include "xsapi/user_statistics.h"
#include "xsapi/multiplayer.h"
#include "xsapi/matchmaking.h"
#include "xsapi/tournaments.h"
#include "xsapi/real_time_activity.h"
#include "xsapi/presence.h"
#include "xsapi/game_server_platform.h"
#include "xsapi/contextual_search_service.h"
#include "xsapi/marketplace.h"
#include "xsapi/multiplayer_manager.h"
#include "xsapi/entertainment_profile.h"
#endif // !defined(XBOX_LIVE_CREATORS_SDK)

#include "xsapi/title_callable_ui.h"
#include "xsapi/xbox_live_context.h"

#ifdef U 
    #undef U // clean up cpprest's global define in case it's used by app
#endif
