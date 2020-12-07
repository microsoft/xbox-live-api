// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XBL_WARNING_DISABLE_DEPRECATED

#if CPP_TESTS_ENABLED
static function_context s_connectionStateHandlerContextCpp{ nullptr };
static function_context s_subscriptionErrorHandlerContextCpp{ nullptr };
static function_context s_resyncHandlerContextCpp{ nullptr };
#endif

int RealTimeActivityServiceActivate_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->real_time_activity_service()->activate();
    LogToFile("RealTimeActivityServiceActivate");
#else
    LogToFile("RealTimeActivityServiceActivate is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int RealTimeActivityServiceDeactivate_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->real_time_activity_service()->deactivate();
    LogToFile("RealTimeActivityServiceDeactivate");
#else
    LogToFile("RealTimeActivityServiceDeactivate is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int RealTimeActivityServiceAddConnectionStateChangeHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    s_connectionStateHandlerContextCpp = xblc->real_time_activity_service()->add_connection_state_change_handler(
        [](xbox::services::real_time_activity::real_time_activity_connection_state connectionState)
        {
            LogToFile("RealTimeActivityServiceConnectionState changed to %d", connectionState);

            switch (connectionState)
            {
            case xbox::services::real_time_activity::real_time_activity_connection_state::connecting:
                LogToFile("RealTimeActivityServiceAddConnectionStateChangeHandler: Connecting\n");
                CallLuaFunction("OnRealTimeActivityServiceAddConnectionStateChangeHandler_Connecting");
                break;
            case xbox::services::real_time_activity::real_time_activity_connection_state::connected:
                LogToFile("RealTimeActivityServiceAddConnectionStateChangeHandler: Connected\n");
                CallLuaFunction("OnRealTimeActivityServiceAddConnectionStateChangeHandler_Connected");
                break;
            case xbox::services::real_time_activity::real_time_activity_connection_state::disconnected:
                LogToFile("RealTimeActivityServiceAddConnectionStateChangeHandler: Connected\n");
                CallLuaFunction("OnRealTimeActivityServiceAddConnectionStateChangeHandler_Disconnected");
                break;
            }
        });

    LogToFile("RealTimeActivityServiceAddConnectionStateChangeHandler");
#else
    LogToFile("RealTimeActivityServiceAddConnectionStateChangeHandler is disabled on this platform.");
    CallLuaFunction("OnRealTimeActivityServiceAddConnectionStateChangeHandler_Disabled");
#endif
    return LuaReturnHR(L, S_OK);
}

int RealTimeActivityServiceRemoveConnectionStateChangeHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    if (s_connectionStateHandlerContextCpp)
    {
        std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
        xblc->real_time_activity_service()->remove_connection_state_change_handler(s_connectionStateHandlerContextCpp);
        s_connectionStateHandlerContextCpp = nullptr;
    }

    LogToFile("RealTimeActivityServiceRemoveConnectionStateChangeHandler");
#else
    LogToFile("RealTimeActivityServiceRemoveConnectionStateChangeHandler is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int RealTimeActivityServiceAddSubscriptionErrorHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    s_subscriptionErrorHandlerContextCpp = xblc->real_time_activity_service()->add_subscription_error_handler(
        [](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args subscriptionError)
        {
            LogToFile("Rta subscription error %s", ConvertHR(ConvertXboxLiveErrorCodeToHresult(subscriptionError.err())).c_str());
        });

    LogToFile("RealTimeActivityServiceAddSubscriptionErrorHandler");
#else
    LogToFile("RealTimeActivityServiceAddSubscriptionErrorHandler is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int RealTimeActivityServiceRemoveSubscriptionErrorHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    if (s_subscriptionErrorHandlerContextCpp)
    {
        std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
        xblc->real_time_activity_service()->remove_subscription_error_handler(s_subscriptionErrorHandlerContextCpp);
        s_subscriptionErrorHandlerContextCpp = nullptr;
    }

    LogToFile("RealTimeActivityServiceRemoveSubscriptionErrorHandler");
#else
    LogToFile("RealTimeActivityServiceRemoveSubscriptionErrorHandler is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int RealTimeActivityServiceAddResyncHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    s_resyncHandlerContextCpp = xblc->real_time_activity_service()->add_resync_handler(
        []()
        {
            LogToFile("RealTimeActivityServiceResyncHandler called");
        });

    LogToFile("RealTimeActivityServiceAddResyncHandler");
#else
    LogToFile("RealTimeActivityServiceAddResyncHandler is disabled for this platform");
#endif
    return LuaReturnHR(L, S_OK);
}

int RealTimeActivityServiceRemoveResyncHandler_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    if (s_resyncHandlerContextCpp)
    {
        std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
        xblc->real_time_activity_service()->remove_resync_handler(s_resyncHandlerContextCpp);
        s_resyncHandlerContextCpp = nullptr;
    }

    LogToFile("RealTimeActivityServiceRemoveResyncHandler");
#else
    LogToFile("RealTimeActivityServiceRemoveResyncHandler is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppRta()
{
    lua_register(Data()->L, "RealTimeActivityServiceActivate", RealTimeActivityServiceActivate_Lua);
    lua_register(Data()->L, "RealTimeActivityServiceDeactivate", RealTimeActivityServiceDeactivate_Lua);
    lua_register(Data()->L, "RealTimeActivityServiceAddConnectionStateChangeHandler", RealTimeActivityServiceAddConnectionStateChangeHandler_Lua);
    lua_register(Data()->L, "RealTimeActivityServiceRemoveConnectionStateChangeHandler", RealTimeActivityServiceRemoveConnectionStateChangeHandler_Lua);
    lua_register(Data()->L, "RealTimeActivityServiceAddSubscriptionErrorHandler", RealTimeActivityServiceAddSubscriptionErrorHandler_Lua);
    lua_register(Data()->L, "RealTimeActivityServiceRemoveSubscriptionErrorHandler", RealTimeActivityServiceRemoveSubscriptionErrorHandler_Lua);
    lua_register(Data()->L, "RealTimeActivityServiceAddResyncHandler", RealTimeActivityServiceAddResyncHandler_Lua);
    lua_register(Data()->L, "RealTimeActivityServiceRemoveResyncHandler", RealTimeActivityServiceRemoveResyncHandler_Lua);
}