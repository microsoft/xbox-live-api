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
#if XSAPI_I
#include "errors.h"

namespace xbox { namespace services {
    
    enum class xbox_live_notification_type
    {
        unknown = 2,
        game_invite = 1,
        achievement_unlocked = 0
    };
    
    struct xbox_live_notification
    {
        xbox_live_notification_type type;
        string_t title;
        string_t body;
        string_t data;
    };
    

    xbox_live_result<xbox_live_notification> parse_notification(NSDictionary* notificationInfo);

}}
#endif
