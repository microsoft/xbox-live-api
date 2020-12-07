// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#if HC_PLATFORM == HC_PLATFORM_GDK
#include "XGameInvite.h"

XTaskQueueRegistrationToken g_token = { 0 };

void CALLBACK MyXGameInviteEventCallback(
    _In_opt_ void* context,
    _In_ const char* inviteUri)
{
    UNREFERENCED_PARAMETER(context);
    if (inviteUri != nullptr)
    {
        LogToScreen("GRTS: Game: MyXGameInviteEventCallback inviteUri:%s\n", inviteUri);
        std::string inviteString(inviteUri);
        auto pos = inviteString.find("handle=");
        auto handleString = inviteString.substr(pos + 7, 36);
        Data()->inviteHandle = handleString;
        CallLuaFunctionWithHr(S_OK, "OnXGameInviteRegisterForEvent");
    }
    else
    {
        LogToScreen("GRTS: Game: MyXGameInviteEventCallback inviteUri:nullptr\n");
    }
}

int XGameInviteRegisterForEvent_Lua(lua_State* L)
{
    CreateQueueIfNeeded();

    HRESULT hr = XGameInviteRegisterForEvent(
        Data()->queue,
        nullptr,
        MyXGameInviteEventCallback,
        &g_token);
    LogToFile("GRTS: Game: XGameInviteRegisterForEvent hr=%s", ConvertHR(hr).c_str()); 

    return LuaReturnHR(L, hr);
}

int XGameInviteUnregisterForEvent_Lua(lua_State* L)
{
    bool result = XGameInviteUnregisterForEvent(g_token, true);
    LogToFile("GRTS: Game: XGameInviteUnregisterForEvent result=%d", result);

    return LuaReturnHR(L, S_OK);
}
#endif

void SetupAPIs_GrtsGameInvite()
{
#if HC_PLATFORM == HC_PLATFORM_GDK
    lua_register(Data()->L, "XGameInviteRegisterForEvent", XGameInviteRegisterForEvent_Lua);
    lua_register(Data()->L, "XGameInviteUnregisterForEvent", XGameInviteUnregisterForEvent_Lua);
#endif
}

