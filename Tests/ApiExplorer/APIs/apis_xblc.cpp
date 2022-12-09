// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

void StopSocialManagerDoWorkHelper();
void StopAchievementsManagerDoWorkHelper();
#if CPP_TESTS_ENABLED
void StopSocialManagerDoWorkHelperCpp();
#endif

int XblInitialize_Lua(lua_State *L)
{
    bool bSetQueue = GetBoolFromLua(L, 1, true);
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblInitialize
    XblInitArgs args = { };
    args.queue = Data()->queue;

    // CODE SKIP START
    if(!bSetQueue)
    {
        //Use a default task queue. If the global task queue has been initialized to null,
        //trying to use a default task queue for XblInitialize will return E_NO_TASK_QUEUE.
        args.queue = nullptr;
    }
    // CODE SKIP END
#if !(HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP)
    args.scid = "00000000-0000-0000-0000-000076029b4d";
    // Alternate SCID for XboxLiveE2E Stats 2017 config
    // args.scid = "00000000-0000-0000-0000-000078c0191b"
#endif
#if HC_PLATFORM == HC_PLATFORM_WIN32
    char pathArray[MAX_PATH + 1];
    GetCurrentDirectoryA(MAX_PATH + 1, pathArray);
    auto pathString = std::string{ pathArray } + '\\';
    args.localStoragePath = pathString.data();
#endif
#if HC_PLATFORM == HC_PLATFORM_ANDROID
    args.applicationContext = Data()->applicationContext;
    args.javaVM = Data()->javaVM;
#endif
    HRESULT hr = XblInitialize(&args);
    // CODE SNIPPET END

    if (SUCCEEDED(hr))
    {
        XblDisableAssertsForXboxLiveThrottlingInDevSandboxes(XblConfigSetting::ThisCodeNeedsToBeChanged);
    }

    LogToFile("XblInitialize: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblGetScid_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XblGetScid
    HRESULT hr = XblGetScid(&Data()->scid);
    // CODE SNIPPET END
    LogToFile("XblGetScid: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblDisableAssertsForXboxLiveThrottlingInDevSandboxes_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblDisableAssertsForXboxLiveThrottlingInDevSandboxes
    XblDisableAssertsForXboxLiveThrottlingInDevSandboxes(XblConfigSetting::ThisCodeNeedsToBeChanged);
    // CODE SNIPPET END
    LogToFile("XblDisableAssertsForXboxLiveThrottlingInDevSandboxes Complete.");
    return LuaReturnHR(L, S_OK);
}

_Ret_maybenull_ _Post_writable_byte_size_(size) void* STDAPIVCALLTYPE ApiRunnerMemAlloc(
    _In_ size_t size,
    _In_ HCMemoryType
    )
{
    return new (std::nothrow) char[size];
}

void STDAPIVCALLTYPE ApiRunnerMemFree(
    _In_ _Post_invalid_ void* pointer,
    _In_ HCMemoryType
    )
{
    delete[] pointer;
}

XblMemAllocFunction g_apiRunnerMemAllocFunc = nullptr;
XblMemFreeFunction g_apiRunnerMemFreeFunc = nullptr;


int XblMemSetFunctions_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMemSetFunctions
    HRESULT hr = XblMemSetFunctions(&ApiRunnerMemAlloc, &ApiRunnerMemFree);
    // CODE SNIPPET END

    LogToFile("XblMemSetFunctions: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMemGetFunctions_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblMemGetFunctions
    XblMemAllocFunction memAllocFunc = nullptr;
    XblMemFreeFunction memFreeFunc = nullptr;
    HRESULT hr = XblMemGetFunctions(&memAllocFunc, &memFreeFunc);
    // CODE SNIPPET END

    g_apiRunnerMemAllocFunc = memAllocFunc;
    g_apiRunnerMemFreeFunc = memFreeFunc;
    LogToFile("XblMemGetFunctions: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblCleanupAsync_Lua(lua_State *L)
{
#if HC_PLATFORM == HC_PLATFORM_UWP
    Sleep(1000);
#endif

    // CODE SNIPPET START: XblCleanupAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;

    HRESULT hr = XblCleanupAsync(asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // Synchronously wait for cleanup to complete
        hr = XAsyncGetStatus(asyncBlock.get(), true);
    }
    // CODE SNIPPET END
    LogToFile("XblCleanup: hr=%s", ConvertHR(hr).data());

    // Ignore not init'd error
    if (hr == E_XBL_NOT_INITIALIZED)
    {
        hr = S_OK;
    }
    return LuaReturnHR(L, S_OK);
}

int XblGetErrorCondition_Lua(lua_State *L)
{
    HRESULT hrIn = static_cast<HRESULT>(GetUint32FromLua(L, 1, static_cast<uint32_t>(E_FAIL)));
    XblErrorCondition errc = XblGetErrorCondition(hrIn);
    lua_pushinteger(L, (int)errc);
    return LuaReturnHR(L, S_OK, 1);
}

int XblContextCreateHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextCreateHandle
    XblContextHandle contextHandle{ nullptr };
    HRESULT hr = XblContextCreateHandle(Data()->xalUser, &contextHandle);
    // CODE SNIPPET END

    // Cache the created handle for use in other APIs if we don't have one already
    if (Data()->xboxLiveContext == nullptr)
    {
        Data()->xboxLiveContext = contextHandle;
    }
    else
    {
        LogToFile("XblContextCreateHandle called but an existing handle was cached");
    }

    lua_pushinteger(L, (int64_t)contextHandle);

    LogToFile("XblContextCreateHandle: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr, 1);
}

int XblContextDuplicateHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextCreateHandle
    XblContextHandle duplicate{};
    HRESULT hr = XblContextDuplicateHandle(Data()->xboxLiveContext, &duplicate);
    // CODE SNIPPET END

    if (SUCCEEDED(hr))
    {
        XblContextCloseHandle(Data()->xboxLiveContext);
        Data()->xboxLiveContext = duplicate;
    }

    LogToFile("XblContextDuplicateHandle: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblContextCloseHandle_Lua(lua_State *L)
{
    StopSocialManagerDoWorkHelper();
    StopAchievementsManagerDoWorkHelper();
#if CPP_TESTS_ENABLED
    StopSocialManagerDoWorkHelperCpp();
#endif

    // Get the XblContextHandle
    XblContextHandle handleToClose = (XblContextHandle)GetUint64FromLua(L, 1, (uint64_t)Data()->xboxLiveContext);

    // CODE SNIPPET START: XblContextCloseHandle
    if (handleToClose != nullptr)
    {
        XblContextCloseHandle(handleToClose);
    }
    // CODE SNIPPET END

    if (handleToClose == Data()->xboxLiveContext)
    {
        Data()->xboxLiveContext = nullptr;
    }

    LogToFile("OnXblContextCloseHandle called.");
    return LuaReturnHR(L, S_OK);
}

// CODE SNIPPET START: XblCallRoutedHandler
void Test_XblCallRoutedHandler(
    _In_ XblServiceCallRoutedArgs,
    _In_ void*)
{
}
// CODE SNIPPET START: XblCallRoutedHandler


int XblAddServiceCallRoutedHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextSettingsAddServiceCallRoutedHandler
    XblFunctionContext fn = XblAddServiceCallRoutedHandler(Test_XblCallRoutedHandler, nullptr);
    // CODE SNIPPET END
    Data()->fnAddServiceCallRoutedHandler = fn;

    LogToFile("XblContextSettingsAddServiceCallRoutedHandler_Lua");
    return LuaReturnHR(L, S_OK);
}

int XblSetOverrideLocale_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSetOverrideLocale
    XblSetOverrideLocale("fr-FR");
    // CODE SNIPPET END

    LogToFile("XblSetOverrideLocale_Lua");
    return LuaReturnHR(L, S_OK);
}

int XblRemoveServiceCallRoutedHandler_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextSettingsRemoveServiceCallRoutedHandler
    XblRemoveServiceCallRoutedHandler(Data()->fnAddServiceCallRoutedHandler);
    // CODE SNIPPET END
    Data()->fnAddServiceCallRoutedHandler = 0;

    LogToFile("XblContextSettingsRemoveServiceCallRoutedHandler_Lua");
    return LuaReturnHR(L, S_OK);
}

int XblContextSettingsGetLongHttpTimeout_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextSettingsGetLongHttpTimeout_Lua
    uint32_t timeoutInSeconds = 0;
    XblContextSettingsGetLongHttpTimeout(Data()->xboxLiveContext, &timeoutInSeconds);
    // CODE SNIPPET END
        
    LogToFile("XblContextSettingsGetLongHttpTimeout_Lua: %d", timeoutInSeconds);
    return LuaReturnHR(L, S_OK);
}

int XblContextSettingsSetLongHttpTimeout_Lua(lua_State *L)
{
    uint32_t timeoutInSeconds = GetUint32FromLua(L, 1, 500);
    // CODE SNIPPET START: XblContextSettingsSetLongHttpTimeout_Lua
    HRESULT hr = XblContextSettingsSetLongHttpTimeout(Data()->xboxLiveContext, timeoutInSeconds);
    // CODE SNIPPET END

    LogToFile("XblContextSettingsSetLongHttpTimeout_Lua");
    return LuaReturnHR(L, hr);
}

int XblContextSettingsGetHttpRetryDelay_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextSettingsGetHttpRetryDelay_Lua
    uint32_t delayInSeconds = 0;
    HRESULT hr = XblContextSettingsGetHttpRetryDelay(Data()->xboxLiveContext, &delayInSeconds);
    // CODE SNIPPET END

    UNREFERENCED_PARAMETER(hr);
    LogToFile("XblContextSettingsGetHttpRetryDelay_Lua: %ul", delayInSeconds);
    return LuaReturnHR(L, S_OK);
}

int XblContextSettingsSetHttpRetryDelay_Lua(lua_State *L)
{
    uint32_t delayInSeconds = GetUint32FromLua(L, 1, 300);
    // CODE SNIPPET START: XblContextSettingsSetHttpRetryDelay_Lua
    HRESULT hr = XblContextSettingsSetHttpRetryDelay(Data()->xboxLiveContext, delayInSeconds);
    // CODE SNIPPET END

    LogToFile("XblContextSettingsSetHttpRetryDelay_Lua");
    return LuaReturnHR(L, hr);
}

int XblContextSettingsGetHttpTimeoutWindow_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextSettingsGetHttpTimeoutWindow_Lua
    uint32_t delayInSeconds = 0;
    HRESULT hr = XblContextSettingsGetHttpTimeoutWindow(Data()->xboxLiveContext, &delayInSeconds);
    // CODE SNIPPET END

    LogToFile("XblContextSettingsGetHttpTimeoutWindow_Lua %d", delayInSeconds);
    return LuaReturnHR(L, hr);
}

int XblContextSettingsSetHttpTimeoutWindow_Lua(lua_State *L)
{
    uint32_t timeoutWindowInSeconds = GetUint32FromLua(L, 1, 200);
    // CODE SNIPPET START: XblContextSettingsSetHttpTimeoutWindow_Lua
    HRESULT hr = XblContextSettingsSetHttpTimeoutWindow(Data()->xboxLiveContext, timeoutWindowInSeconds);
    // CODE SNIPPET END

    LogToFile("XblContextSettingsSetHttpTimeoutWindow_Lua: %d", timeoutWindowInSeconds);
    return LuaReturnHR(L, hr);
}

int XblContextSettingsGetWebsocketTimeoutWindow_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextSettingsGetWebsocketTimeoutWindow_Lua
    uint32_t timeoutWindowInSeconds = 0;
    HRESULT hr = XblContextSettingsGetHttpTimeoutWindow(Data()->xboxLiveContext, &timeoutWindowInSeconds);
    // CODE SNIPPET END

    LogToFile("XblContextSettingsGetWebsocketTimeoutWindow_Lua: %d", timeoutWindowInSeconds);
    return LuaReturnHR(L, hr);
}

int XblContextSettingsSetWebsocketTimeoutWindow_Lua(lua_State *L)
{
    uint32_t timeoutWindowInSeconds = GetUint32FromLua(L, 1, 200);
    // CODE SNIPPET START: XblContextSettingsSetWebsocketTimeoutWindow_Lua
    HRESULT hr = XblContextSettingsSetWebsocketTimeoutWindow(Data()->xboxLiveContext, timeoutWindowInSeconds);
    // CODE SNIPPET END

    LogToFile("XblContextSettingsSetWebsocketTimeoutWindow_Lua");
    return LuaReturnHR(L, hr);
}

int XblContextSettingsGetUseCrossPlatformQosServers_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblContextSettingsGetUseCrossPlatformQosServers_Lua
    bool useQosServers = 0;
    HRESULT hr = XblContextSettingsGetUseCrossPlatformQosServers(Data()->xboxLiveContext, &useQosServers);
    // CODE SNIPPET END

    UNREFERENCED_PARAMETER(hr);
    LogToFile("XblContextSettingsGetUseCrossPlatformQosServers_Lua: %d", useQosServers);
    return LuaReturnHR(L, S_OK);
}

int XblContextSettingsSetUseCrossPlatformQosServers_Lua(lua_State *L)
{
    bool useQosServers = GetBoolFromLua(L, 1, true);
    // CODE SNIPPET START: XblContextSettingsSetUseCrossPlatformQosServers_Lua
    HRESULT hr = XblContextSettingsGetUseCrossPlatformQosServers(Data()->xboxLiveContext, &useQosServers);
    // CODE SNIPPET END

    UNREFERENCED_PARAMETER(hr);
    LogToFile("XblContextSettingsSetUseCrossPlatformQosServers_Lua");
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_Xbl()
{
    // xbox_live_global_c.h
    lua_register(Data()->L, "XblInitialize", XblInitialize_Lua);
    lua_register(Data()->L, "XblGetScid", XblGetScid_Lua);
    lua_register(Data()->L, "XblCleanupAsync", XblCleanupAsync_Lua);
    lua_register(Data()->L, "XblMemSetFunctions", XblMemSetFunctions_Lua);
    lua_register(Data()->L, "XblMemGetFunctions", XblMemGetFunctions_Lua);
    lua_register(Data()->L, "XblDisableAssertsForXboxLiveThrottlingInDevSandboxes", XblDisableAssertsForXboxLiveThrottlingInDevSandboxes_Lua);
    lua_register(Data()->L, "XblAddServiceCallRoutedHandler", XblAddServiceCallRoutedHandler_Lua);
    lua_register(Data()->L, "XblRemoveServiceCallRoutedHandler", XblRemoveServiceCallRoutedHandler_Lua);
    lua_register(Data()->L, "XblSetOverrideLocale", XblSetOverrideLocale_Lua);

    // errors_c.h
    lua_register(Data()->L, "XblGetErrorCondition", XblGetErrorCondition_Lua);

    // xbox_live_context_c.h
    lua_register(Data()->L, "XblContextCreateHandle", XblContextCreateHandle_Lua);
    lua_register(Data()->L, "XblContextDuplicateHandle", XblContextDuplicateHandle_Lua);
    lua_register(Data()->L, "XblContextCloseHandle", XblContextCloseHandle_Lua);
    // TBD: XblContextGetUser
    // TBD: XblContextGetXboxUserId

    // xbox_live_context_settings_c.h
    lua_register(Data()->L, "XblContextSettingsGetLongHttpTimeout", XblContextSettingsGetLongHttpTimeout_Lua);
    lua_register(Data()->L, "XblContextSettingsSetLongHttpTimeout", XblContextSettingsSetLongHttpTimeout_Lua);
    lua_register(Data()->L, "XblContextSettingsGetHttpRetryDelay", XblContextSettingsGetHttpRetryDelay_Lua);
    lua_register(Data()->L, "XblContextSettingsSetHttpRetryDelay", XblContextSettingsSetHttpRetryDelay_Lua);
    lua_register(Data()->L, "XblContextSettingsGetHttpTimeoutWindow", XblContextSettingsGetHttpTimeoutWindow_Lua);
    lua_register(Data()->L, "XblContextSettingsSetHttpTimeoutWindow", XblContextSettingsSetHttpTimeoutWindow_Lua);
    lua_register(Data()->L, "XblContextSettingsGetWebsocketTimeoutWindow", XblContextSettingsGetWebsocketTimeoutWindow_Lua);
    lua_register(Data()->L, "XblContextSettingsSetWebsocketTimeoutWindow", XblContextSettingsSetWebsocketTimeoutWindow_Lua);
    lua_register(Data()->L, "XblContextSettingsGetUseCrossPlatformQosServers", XblContextSettingsGetUseCrossPlatformQosServers_Lua);
    lua_register(Data()->L, "XblContextSettingsSetUseCrossPlatformQosServers", XblContextSettingsSetUseCrossPlatformQosServers_Lua);
}
