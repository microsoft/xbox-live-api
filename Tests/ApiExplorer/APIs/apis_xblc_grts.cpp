// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#if HC_PLATFORM == HC_PLATFORM_GDK
#include <XGameUI.h>
#endif

HRESULT XGameUiShowSendGameInviteAsyncHelper(
    const std::string& sessionTemplateName,
    const std::string& sessionId,
    const std::string& inviteText,
    const std::string& customActivationContext
)
{
#if HC_PLATFORM == HC_PLATFORM_GDK
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XGameUiShowSendGameInviteResult(asyncBlock);
        CallLuaFunctionWithHr(hr, "OnXGameUiShowSendGameInviteAsync");
    };

    HRESULT hr = XGameUiShowSendGameInviteAsync(asyncBlock.get(), 
        Data()->xalUser, 
        Data()->scid,
        sessionTemplateName.c_str(), 
        sessionId.c_str(),
        inviteText.c_str(),
        customActivationContext.c_str()
    );
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
#else 
    UNREFERENCED_PARAMETER(sessionTemplateName);
    UNREFERENCED_PARAMETER(sessionId);
    UNREFERENCED_PARAMETER(inviteText);
    UNREFERENCED_PARAMETER(customActivationContext);
    HRESULT hr = S_OK;
#endif
    return hr;
}

int XGameUiShowSendGameInviteAsyncToMPMLobby_Lua(lua_State* L)
{
    std::string inviteText = GetStringFromLua(L, 1, "//MPSD/custominvitestrings_JoinMyGame");
    std::string customActivationContext = GetStringFromLua(L, 2, "MyCustomActivationContext");

    XblMultiplayerSessionReference sessionReference{};
    HRESULT hr = XblMultiplayerManagerLobbySessionSessionReference(&sessionReference);

    hr = XGameUiShowSendGameInviteAsyncHelper(
        sessionReference.SessionTemplateName,
        sessionReference.SessionName, 
        inviteText, 
        customActivationContext);
    return LuaReturnHR(L, hr);
}

int XGameUiShowSendGameInviteAsync_Lua(lua_State* L)
{
    std::string sessionTemplateName = GetStringFromLua(L, 1, "MinGameSession");
    std::string sessionId = GetStringFromLua(L, 2, "GameSession-0");
    std::string inviteText = GetStringFromLua(L, 3, "Join Me In My Game!");
    std::string customActivationContext = GetStringFromLua(L, 4, "MyCustomActivationContext");

    HRESULT hr = XGameUiShowSendGameInviteAsyncHelper(sessionTemplateName, sessionId, inviteText, customActivationContext);
    return LuaReturnHR(L, hr);
}

int XGameUiShowMultiplayerActivityGameInviteAsync_Lua(lua_State* L)
{
#if HC_PLATFORM == HC_PLATFORM_GDK
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        //HRESULT hr = XGameUiShowMultiplayerActivityGameInviteResult(asyncBlock);
        HRESULT hr = E_NOTIMPL; // requires GDK 2203+
        CallLuaFunctionWithHr(hr, "OnXGameUiShowMultiplayerActivityGameInviteAsync");
    };

    //HRESULT hr = XGameUiShowMultiplayerActivityGameInviteAsync(asyncBlock.get(), Data()->xalUser);
    HRESULT hr = E_NOTIMPL; // requires GDK 2203+
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
#else 
    HRESULT hr = S_OK;
#endif
    return LuaReturnHR(L, hr);
}

void SetupAPIs_GRTS()
{
    lua_register(Data()->L, "XGameUiShowMultiplayerActivityGameInviteAsync", XGameUiShowMultiplayerActivityGameInviteAsync_Lua);
    lua_register(Data()->L, "XGameUiShowSendGameInviteAsync", XGameUiShowSendGameInviteAsync_Lua);
    lua_register(Data()->L, "XGameUiShowSendGameInviteAsyncToMPMLobby", XGameUiShowSendGameInviteAsyncToMPMLobby_Lua);
}

