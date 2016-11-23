//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#pragma warning(disable: 4265)
#pragma warning(disable: 4266)
#pragma warning(disable: 4062)

#include <cpprest/http_msg.h>
#include "xsapi/types.h"
#include "xsapi/errors.h"
#include "xsapi/xbox_live_context_settings.h"
#include "xsapi/system.h"
#include "xsapi/profile.h"
#include "xsapi/social.h"
#include "xsapi/achievements.h"
#include "xsapi/leaderboard.h"
#include "xsapi/user_statistics.h"
#include "xsapi/multiplayer.h"
#include "xsapi/matchmaking.h"
#include "xsapi/real_time_activity.h"
#include "xsapi/presence.h"
#include "xsapi/game_server_platform.h"
#include "xsapi/title_storage.h"
#include "xsapi/contextual_search_service.h"
#include "xsapi/privacy.h"
#include "xsapi/marketplace.h"
#include "xsapi/events.h"
#include "xsapi/title_callable_ui.h"
#include "xsapi/xbox_live_context.h"
#include "xsapi/http_call.h"
#include "xsapi/social_manager.h"
#include "xsapi/multiplayer_manager.h"
#include "xsapi/service_call_logging_config.h"
#include "xsapi/entertainment_profile.h"
#include "xsapi/simple_stats.h"

#ifdef U 
    #undef U // clean up cpprest's global define in case it's used by app
#endif
