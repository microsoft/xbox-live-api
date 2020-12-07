// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#if HC_PLATFORM != HC_PLATFORM_ANDROID
#pragma warning(disable: 4265)
#pragma warning(disable: 4266)
#pragma warning(disable: 4062)
#endif

#ifndef _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#endif

// cpprest assumed to be part of XSAPI static lib
#ifndef _ASYNCRT_IMPORT // define this if cpprest is actually a dll
#ifndef _ASYNCRT_EXPORT
#ifndef _NO_ASYNCRTIMP
#define _NO_ASYNCRTIMP // by default disable cpprest dll fn decorations
#endif
#endif
#endif

#include "xsapi-c/services_c.h"

#include "xsapi-cpp/types.h"
#include "xsapi-cpp/errors.h"
#include "xsapi-cpp/mem.h"
#include "cpprest/http_msg.h"
#include "xsapi-cpp/system.h"
#include "xsapi-cpp/service_call_logging_config.h"

#if !XSAPI_NO_PPL
#include "xsapi-cpp/leaderboard.h"
#include "xsapi-cpp/title_storage.h"
#include "xsapi-cpp/privacy.h"
#include "xsapi-cpp/profile.h"
#endif // !XSAPI_NO_PPL

#include "xsapi-cpp/social_manager.h"
#include "xsapi-cpp/http_call.h"
#include "xsapi-cpp/xbox_live_context_settings.h"

#if !defined(XBOX_LIVE_CREATORS_SDK)
#include "xsapi-cpp/social.h"
#include "xsapi-cpp/achievements.h"
#include "xsapi-cpp/real_time_activity.h"
#include "xsapi-cpp/presence.h"
    #if !XSAPI_NO_PPL
        #include "xsapi-cpp/events.h"
        #include "xsapi-cpp/user_statistics.h"
        #include "xsapi-cpp/multiplayer.h"
        #include "xsapi-cpp/matchmaking.h"
        #include "xsapi-cpp/multiplayer_manager.h"
        #include "xsapi-cpp/notification_service.h"
        #include "xsapi-cpp/string_verify.h"
    #endif // !XSAPI_NO_PPL
#endif // !defined(XBOX_LIVE_CREATORS_SDK)

#include "xsapi-cpp/title_callable_ui.h"
#include "xsapi-cpp/xbox_live_context.h"

#ifdef U 
    #undef U // clean up cpprest's global define in case it's used by app
#endif
