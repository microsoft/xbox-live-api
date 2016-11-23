/////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Macros_WinRT.h"
#include "shared_macros.h"
#include "user_impl.h"
#include "User_WinRT.h"
#include "user_context.h"
#include "MockXboxSystemFactory.h"
#include "RealTimeActivityService_WinRT.h"

void SetWebSocketRTAAutoResponser(std::shared_ptr<xbox::services::system::MockWebSocketClient> ws, string_t initData, int subId = -1, bool useAutoReply = true);
void SetMultipleClientWebSocketRTAAutoResponser(std::vector<std::shared_ptr<xbox::services::system::MockWebSocketClient>> ws, string_t initData, int subId = -1, bool useAutoReply = true);

struct StateChangeHelper
{
public:
    int disconnected = 0;
    int connecting = 0;
    int connected = 0;
    concurrency::event disconnectedEvent;
    concurrency::event connectedEvent;
    concurrency::event connectingEvent;

    void reset_events()
    {
        disconnected = 0;
        disconnectedEvent.reset();
        connecting = 0;
        connectedEvent.reset();
        connected = 0;
        connectingEvent.reset();
    }

};

std::shared_ptr<StateChangeHelper> SetupStateChangeHelper(Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivityService^ rtaService);