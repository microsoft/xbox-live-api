// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XBL_WARNING_DISABLE_DEPRECATED

static XblFunctionContext s_connectionStateHandlerContext{ 0 };
static XblFunctionContext s_subscriptionErrorHandlerContext{ 0 };
static XblFunctionContext s_resyncHandlerContext{ 0 };

int XblRealTimeActivityActivate_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblRealTimeActivityActivate
    HRESULT hr = XblRealTimeActivityActivate(Data()->xboxLiveContext);
    // CODE SNIPPET END
    LogToFile("XblRealTimeActivityActivate: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblRealTimeActivityDeactivate_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblRealTimeActivityDeactivate
    HRESULT hr = XblRealTimeActivityDeactivate(Data()->xboxLiveContext);
    // CODE SNIPPET END
    LogToFile("XblRealTimeActivityDeactivate: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblRealTimeActivityAddConnectionStateChangeHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblRealTimeActivityAddConnectionStateChangeHandler
    s_connectionStateHandlerContext = XblRealTimeActivityAddConnectionStateChangeHandler(Data()->xboxLiveContext,
        [](void *context, XblRealTimeActivityConnectionState connectionState)
    {
        UNREFERENCED_PARAMETER(context); // CODE SNIP SKIP
        LogToFile("XblRealTimeActivityConnectionState changed to %d", connectionState);

        // Handle connection state change
        switch (connectionState)
        {
        case XblRealTimeActivityConnectionState::Connected:
            // Handle connected state
            LogToFile("XblRealTimeActivityAddConnectionStateChangeHandler: Connected\n"); // CODE SNIP SKIP
            CallLuaFunction("OnXblRealTimeActivityAddConnectionStateChangeHandler_Connected"); // CODE SNIP SKIP
            break;
        case XblRealTimeActivityConnectionState::Connecting:
            // Handle connecting state
            LogToFile("XblRealTimeActivityAddConnectionStateChangeHandler: Connecting\n"); // CODE SNIP SKIP
            CallLuaFunction("OnXblRealTimeActivityAddConnectionStateChangeHandler_Connecting"); // CODE SNIP SKIP
            break;
        case XblRealTimeActivityConnectionState::Disconnected:
            // Handle disconnected state
            LogToFile("XblRealTimeActivityAddConnectionStateChangeHandler: Disconnected\n"); // CODE SNIP SKIP
            CallLuaFunction("OnXblRealTimeActivityAddConnectionStateChangeHandler_Disconnected"); // CODE SNIP SKIP
            break;
        }

    }, nullptr);
    // CODE SNIPPET END

    LogToFile("XblRealTimeActivityAddConnectionStateChangeHandler complete");
    return LuaReturnHR(L, S_OK);
}

int XblRealTimeActivityRemoveConnectionStateChangeHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblRealTimeActivityAddConnectionStateChangeHandler
    if (s_connectionStateHandlerContext != 0)
    {
        XblRealTimeActivityRemoveConnectionStateChangeHandler(Data()->xboxLiveContext, s_connectionStateHandlerContext);
    }
    // CODE SNIPPET END

    LogToFile("XblRealTimeActivityRemoveConnectionStateChangeHandler complete");
    return LuaReturnHR(L, S_OK);
}

int XblRealTimeActivityAddSubscriptionErrorHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblRealTimeActivityAddSubscriptionErrorHandler
    s_subscriptionErrorHandlerContext = XblRealTimeActivityAddSubscriptionErrorHandler(Data()->xboxLiveContext,
        [](void* context, _In_ XblRealTimeActivitySubscriptionHandle subscription, HRESULT subscriptionError)
    {
        UNREFERENCED_PARAMETER(context); // CODE SNIP SKIP
        UNREFERENCED_PARAMETER(subscription); // CODE SNIP SKIP
        // Handle subscription error
        LogToFile("Rta subscription error %s", ConvertHR(subscriptionError).c_str());
    }, nullptr);
    // CODE SNIPPET END

    LogToFile("XblRealTimeActivityAddSubscriptionErrorHandler complete");
    return LuaReturnHR(L, S_OK);
}

int XblRealTimeActivityRemoveSubscriptionErrorHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblRealTimeActivityRemoveConnectionStateChangeHandler
    if (s_subscriptionErrorHandlerContext != 0)
    {
        XblRealTimeActivityRemoveConnectionStateChangeHandler(Data()->xboxLiveContext, s_subscriptionErrorHandlerContext);
    }
    // CODE SNIPPET END

    LogToFile("XblRealTimeActivityRemoveConnectionStateChangeHandler complete");
    return LuaReturnHR(L, S_OK);
}

int XblRealTimeActivityAddResyncHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblRealTimeActivityAddResyncHandler
    s_resyncHandlerContext = XblRealTimeActivityAddResyncHandler(Data()->xboxLiveContext,
        [](void* context)
    {
        UNREFERENCED_PARAMETER(context); // CODE SNIP SKIP
        // Handle resync
        LogToFile("XblResyncHandler called");
    }, nullptr);
    // CODE SNIPPET END

    LogToFile("XblRealTimeActivityAddResyncHandler complete");
    return LuaReturnHR(L, S_OK);
}

int XblRealTimeActivityRemoveResyncHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblRealTimeActivityRemoveConnectionStateChangeHandler
    if (s_resyncHandlerContext != 0)
    {
        XblRealTimeActivityRemoveResyncHandler(Data()->xboxLiveContext, s_resyncHandlerContext);
    }
    // CODE SNIPPET END

    LogToFile("XblRealTimeActivityRemoveResyncHandler complete");
    return LuaReturnHR(L, S_OK);
}

int XblRealTimeActivitySubscriptionGetState_Lua(lua_State *L)
{
    XblRealTimeActivitySubscriptionHandle rtaSubscriptionHandle{ nullptr };
    if (Data()->statisticChangeSubscriptionHandle)
    {
        rtaSubscriptionHandle = Data()->statisticChangeSubscriptionHandle;
    }

    if (rtaSubscriptionHandle)
    {
        // CODE SNIPPET START: XblRealTimeActivitySubscriptionGetState
        XblRealTimeActivitySubscriptionState state;
        XblRealTimeActivitySubscriptionGetState(Data()->statisticChangeSubscriptionHandle, &state);
        // CODE SNIPPET END
    }

    LogToFile("XblRealTimeActivitySubscriptionGetState complete");
    return LuaReturnHR(L, S_OK);
}

int XblRealTimeActivitySubscriptionGetId_Lua(lua_State *L)
{
    XblRealTimeActivitySubscriptionHandle rtaSubscriptionHandle{ nullptr };
    if (Data()->statisticChangeSubscriptionHandle)
    {
        rtaSubscriptionHandle = Data()->statisticChangeSubscriptionHandle;
    }

    if (rtaSubscriptionHandle)
    {
        // CODE SNIPPET START: XblRealTimeActivitySubscriptionGetId
        uint32_t subscriptionId;
        XblRealTimeActivitySubscriptionGetId(rtaSubscriptionHandle, &subscriptionId);
        // CODE SNIPPET END
    }

    LogToFile("XblRealTimeActivitySubscriptionGetId complete");
    return LuaReturnHR(L, S_OK);
}

// Declare test hook
HRESULT XblTestHooksTriggerRTAResync();

int XblTestHooksTriggerRTAResync_Lua(lua_State *L)
{
    HRESULT hr = S_OK;
#if HC_PLATFORM != HC_PLATFORM_IOS
    // For some reason XCode complaining about link errors with this test hook. Disabling on iOS until that can be investigated
    hr = XblTestHooksTriggerRTAResync();
#endif
    return LuaReturnHR(L, hr);
}

#if !XSAPI_NO_PPL && HC_PLATFORM_IS_MICROSOFT && HC_PLATFORM != HC_PLATFORM_GDK
#include "combaseapi.h"
#include "xsapi-cpp/services.h"

struct RealTimeActivityState
{
    std::shared_ptr<xbox::services::xbox_live_context> xblContext;
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> session;
};
std::unique_ptr<RealTimeActivityState> g_multiplayerState;

RealTimeActivityState* RTAState()
{
    if (g_multiplayerState == nullptr)
    {
        g_multiplayerState = std::make_unique<RealTimeActivityState>();
    }
    return g_multiplayerState.get();
}

int XblRtaMultiplayerInit_Lua(lua_State *L)
{
    using namespace xbox::services;
    using namespace xbox::services::multiplayer;

    stringstream_t stream;
    stream << Data()->scid;
    string_t SCID = stream.str();
    string_t SessionTemplate = L"GameSession";
    GUID guid;
    CoCreateGuid(&guid);
    OLECHAR* guidString;
    StringFromCLSID(guid, &guidString);
    string_t SessionName = guidString;
    SessionName = SessionName.substr(1, SessionName.length() - 2);

    stringstream_t xuidStream;
    xuidStream << Data()->xboxUserId;


    auto context = RTAState();

    std::shared_ptr<xbox_live_context> xblContext = std::make_shared<xbox_live_context>(Data()->xalUser);

    context->xblContext = xblContext;
    auto multiplayerSessionReference = std::make_shared<multiplayer_session_reference>(SCID, SessionTemplate, SessionName);
    context->session = std::make_shared<multiplayer_session>(xuidStream.str(), *multiplayerSessionReference);
    auto session = context->session;

    xblContext->multiplayer_service().enable_multiplayer_subscriptions();

    xblContext->multiplayer_service().add_multiplayer_connection_id_changed_handler([]() {
        LogToFile("add_multiplayer_connection_id_changed_handler");
        auto context = RTAState();
        auto xblContext = context->xblContext;
        auto session = context->session;

        LogToFile("updating mpsd connection id");
        xblContext->multiplayer_service().get_current_session(session->session_reference())
            .then([context](xbox_live_result<std::shared_ptr<multiplayer_session>> result) {

            if (result.err())
            {
                LogToFile("you've been kicked");
            }
            else if (auto session{ result.payload() })
            {
                context->session = session;
                context->session->set_current_user_status(multiplayer_session_member_status::active);
                context->xblContext->multiplayer_service().write_session(context->session, multiplayer_session_write_mode::update_or_create_new)
                    .then([context](xbox_live_result<std::shared_ptr<multiplayer_session>> result2) {

                    if (result2.err())
                    {
                        LogToFile("you've been kicked 2");
                    }
                    else
                    {
                        context->session = result2.payload();
                    }
                });
            }
        });
    });

    int count = 0;

    LogToFile("creating the session");
    session->join();
    session->set_current_user_member_custom_property_json(L"count", web::json::value(count));
    xblContext->multiplayer_service().write_session(session, multiplayer_session_write_mode::update_or_create_new)
        .then([context](xbox_live_result<std::shared_ptr<multiplayer_session>> result) {

        if (result.err())
        {
            LogToFile("get_current_session err");
        }
        else
        {
            context->session = result.payload();
        }
    });

    return LuaReturnHR(L, S_OK);
}

int XblRtaMultiplayerOnConnected_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}
#endif //!XSAPI_NO_PPL && HC_PLATFORM_IS_MICROSOFT

void SetupAPIs_XblRta()
{
    lua_register(Data()->L, "XblRealTimeActivityActivate", XblRealTimeActivityActivate_Lua);
    lua_register(Data()->L, "XblRealTimeActivityDeactivate", XblRealTimeActivityDeactivate_Lua);
    lua_register(Data()->L, "XblRealTimeActivityAddConnectionStateChangeHandler", XblRealTimeActivityAddConnectionStateChangeHandler_Lua);
    lua_register(Data()->L, "XblRealTimeActivityRemoveConnectionStateChangeHandler", XblRealTimeActivityRemoveConnectionStateChangeHandler_Lua);
    lua_register(Data()->L, "XblRealTimeActivityAddSubscriptionErrorHandler", XblRealTimeActivityAddSubscriptionErrorHandler_Lua);
    lua_register(Data()->L, "XblRealTimeActivityRemoveSubscriptionErrorHandler", XblRealTimeActivityRemoveSubscriptionErrorHandler_Lua);
    lua_register(Data()->L, "XblRealTimeActivityAddResyncHandler", XblRealTimeActivityAddResyncHandler_Lua);
    lua_register(Data()->L, "XblRealTimeActivityRemoveResyncHandler", XblRealTimeActivityRemoveResyncHandler_Lua);
    lua_register(Data()->L, "XblRealTimeActivitySubscriptionGetState", XblRealTimeActivitySubscriptionGetState_Lua);
    lua_register(Data()->L, "XblRealTimeActivitySubscriptionGetId", XblRealTimeActivitySubscriptionGetId_Lua);
    lua_register(Data()->L, "XblTestHooksTriggerRTAResync", XblTestHooksTriggerRTAResync_Lua);

#if !XSAPI_NO_PPL && HC_PLATFORM_IS_MICROSOFT && HC_PLATFORM != HC_PLATFORM_GDK
    lua_register(Data()->L, "XblRtaMultiplayerInit", XblRtaMultiplayerInit_Lua);
    lua_register(Data()->L, "XblRtaMultiplayerOnConnected", XblRtaMultiplayerOnConnected_Lua);
#endif
}
