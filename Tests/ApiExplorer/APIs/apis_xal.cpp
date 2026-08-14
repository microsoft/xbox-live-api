// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
int XalCleanupAsync_Lua(lua_State *L)
{
    if (!Data()->m_isXalInitialized)
        return LuaReturnHR(L, S_OK);

    if (Data()->nsalMockCall != nullptr)
    {
        HCMockRemoveMock(Data()->nsalMockCall);
        //Close the handle we're holding onto for API runner state
        HCMockCallCloseHandle(Data()->nsalMockCall);
        Data()->nsalMockCall = nullptr;

        if (Data()->libHttpClientInit) // Set on GDK where XAL is just a wrapper around XUser
        {
            HCCleanup();
            Data()->libHttpClientInit = false;
        }
    }

#if CPP_TESTS_ENABLED
    Data()->blobMetadataResultCpp = xbox::services::title_storage::title_storage_blob_metadata_result{};
    Data()->blobMetadataCpp = xbox::services::title_storage::title_storage_blob_metadata{};
#endif

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    HRESULT hr = XalCleanupAsync(asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // Synchronously wait for cleanup to complete
        hr = XAsyncGetStatus(asyncBlock.get(), true);
        Data()->m_isXalInitialized = false;
    }

    LogToFile("XalCleanup: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

void SetupXalNsalMock()
{
    // Mock the NSAL call so XAL doesn't 429 due to repeated fast XAL init/cleanup during testing

    // GET https://title.mgt.xboxlive.com/titles/current/endpoints HTTP/1.1
    // HTTP/1.1 200 OK
    // {"EndPoints":[{"Protocol":"http","Host":"*","HostType":"wildcard"},{"Protocol":"https","Host":"*","HostType":"wildcard"}]}

    if (Data()->nsalMockCall != nullptr)
    {
        HCMockRemoveMock(Data()->nsalMockCall);
        Data()->nsalMockCall = nullptr;
    }

    HRESULT hr = HCMockCallCreate(&(Data()->nsalMockCall));
    if (hr == E_HC_NOT_INITIALISED)
    {
        // This happens on GDK where XAL is just a wrapper around XUser
        LogToScreen("Calling HCInitialize()");

#if HC_PLATFORM == HC_PLATFORM_ANDROID
        hr = HCInitialize(&(Data()->initArgs));
#else
        hr = HCInitialize(nullptr);
#endif
        LogToScreen("HCInitialize done");
        assert(SUCCEEDED(hr));
        Data()->libHttpClientInit = true;
        hr = HCMockCallCreate(&(Data()->nsalMockCall));
    }
    assert(SUCCEEDED(hr));

    hr = HCMockAddMock(Data()->nsalMockCall, "GET", "https://title.mgt.xboxlive.com/titles/current/endpoints", nullptr, 0);
    assert(SUCCEEDED(hr));

    hr = HCMockResponseSetStatusCode(Data()->nsalMockCall, 200);
    assert(SUCCEEDED(hr));

    std::string responseBodyString = "{\"EndPoints\":[{\"Protocol\":\"http\",\"Host\":\"*\",\"HostType\":\"wildcard\"},{\"Protocol\":\"https\",\"Host\":\"*\",\"HostType\":\"wildcard\"}]}";
    std::vector<uint8_t> bodyBytes{ responseBodyString.begin(), responseBodyString.end() };
    hr = HCMockResponseSetResponseBodyBytes(Data()->nsalMockCall, bodyBytes.data(), static_cast<uint32_t>(bodyBytes.size()));
    assert(SUCCEEDED(hr));
}


int XalInitialize_Lua(lua_State *L)
{
    std::string clientId = GetStringFromLua(L, 1, "000000004C26FED0");
    uint32_t titleId = (uint32_t)GetUint64FromLua(L, 2, 1979882317);
    std::string sandbox = GetStringFromLua(L, 3, "XDKS.1");

    // Alternate config for XboxLiveE2E Stats 2017
    //std::string clientId = GetStringFromLua(L, 1, "0000000044296E10");
    //uint32_t titleId = (uint32_t)GetUint64FromLua(L, 2, 2025855259);

    Data()->titleId = titleId;

    CreateQueueIfNeeded();

    // CODE SNIPPET START: XalInitialize

    XalInitArgs xalInitArgs = {};
#if HC_PLATFORM == HC_PLATFORM_UWP

    xalInitArgs.titleId = titleId;
    xalInitArgs.packageFamilyName = u8"41336MicrosoftATG.XboxLiveE2E_dspnxghe87tn0";
    //xalInitArgs.correlationVector; // optional
    //xalInitArgs.flags; // optional
    //xalInitArgs.launchUser; // optional
    //xalInitArgs.mainWindow; // optional

#elif HC_PLATFORM == HC_PLATFORM_GDK || HC_PLATFORM == HC_PLATFORM_XDK
    // No args on GDK / XDK

#else

    // Args for iOS / Android / Win32 
    xalInitArgs.clientId = clientId.c_str();
    xalInitArgs.titleId = titleId;
    xalInitArgs.sandbox = sandbox.c_str();
#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID
    // Extra args on Mobile
    std::string redirectUri{ "ms-xal-" + clientId + "://auth" };
    xalInitArgs.redirectUri = redirectUri.data();
#if HC_PLATFORM == HC_PLATFORM_ANDROID
    // Extra args on Android
    xalInitArgs.disableDiagnosticTelemetry = false;

    JNIEnv* jniEnv = nullptr;
    jint result = Data()->javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6);
    if (result != JNI_OK)
    {
        LogToScreen("Failed to retrieve the JNIEnv from the JavaVM.");
    }

    assert(jniEnv != nullptr);

    jobject res = jniEnv->CallObjectMethod(Data()->m_mainActivityClassInstance, Data()->m_getApplicationContext);

    xalInitArgs.javaVM = Data()->javaVM;
    xalInitArgs.appContext = res;
#endif
#endif
#endif

    HCTraceSetTraceToDebugger(true);
    HCSettingsSetTraceLevel(HCTraceLevel::Verbose);

    HRESULT hr = XalInitialize(&xalInitArgs, Data()->queue);
    // CODE SNIPPET END
    Data()->m_isXalInitialized = true;

    LogToScreen("XalInitialize: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalInitialize: hr=%s", ConvertHR(hr).c_str());

    SetupXalNsalMock();

    return LuaReturnHR(L, hr);
}

int XalTryAddFirstUserSilentlyAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XalTryAddFirstUserSilentlyAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XalUserHandle newUser = nullptr;
        HRESULT hr = XalTryAddDefaultUserSilentlyResult(asyncBlock, &newUser);

        // TODO: Store and use newUser
        LogToScreen("XalTryAddFirstUserSilentlyResult: hr=%s user=0x%0.8x", ConvertHR(hr).c_str(), newUser);
        LogToFile("XalTryAddFirstUserSilentlyResult: hr=%s user=0x%0.8x", ConvertHR(hr).c_str(), newUser); // CODE SNIP SKIP
        if (Data()->xalUser != nullptr)
        {
            XalUserCloseHandle(Data()->xalUser);
            Data()->xalUser = nullptr;
        }
        Data()->xalUser = newUser; // CODE SNIP SKIP
        if (Data()->xalUser) // CODE SNIP SKIP
        { // CODE SNIP SKIP
            Data()->gotXalUser = true; // CODE SNIP SKIP
        } // CODE SNIP SKIP
        LuaStopTestIfFailed(hr);
        CallLuaString("common = require 'common'; common.OnXalTryAddFirstUserSilentlyAsync()"); // CODE SNIP SKIP
    };

    HRESULT hr = XalTryAddDefaultUserSilentlyAsync(0u, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END
    LogToScreen("XalTryAddFirstUserSilentlyAsync: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalTryAddFirstUserSilentlyAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalGetMaxUsers_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalGetMaxUsers
    uint32_t maxUsers = 0;
    HRESULT hr = XalGetMaxUsers(&maxUsers);
    // CODE SNIPPET END

    LogToScreen("XalGetMaxUsers: hr=%s. result=%d", ConvertHR(hr).c_str(), maxUsers);
    LogToFile("XalGetMaxUsers: hr=%s. result=%d", ConvertHR(hr).c_str(), maxUsers); // CODE SNIP SKIP
    return LuaReturnHR(L, hr);
}

int XalAddUserWithUiAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XalAddUserWithUiAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        XalUserHandle newUser = nullptr;
        HRESULT hr = XalAddUserWithUiResult(asyncBlock, &newUser);
        
        // TODO: Store and use newUser
        LogToScreen("XalAddUserWithUiResult: hr=%s user=0x%0.8x", ConvertHR(hr).c_str(), newUser);
        LogToFile("XalAddUserWithUiResult: hr=%s user=0x%0.8x", ConvertHR(hr).c_str(), newUser); // CODE SNIP SKIP
        if (Data()->xalUser != nullptr)
        {
            XalUserCloseHandle(Data()->xalUser);
            Data()->xalUser = nullptr;
        }
        Data()->xalUser = newUser; // CODE SNIP SKIP
        if (Data()->xalUser) // CODE SNIP SKIP
        { // CODE SNIP SKIP
            Data()->gotXalUser = true; // CODE SNIP SKIP
        } // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXalAddUserWithUiAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XalAddUserWithUiAsync(0u, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XalAddUserWithUiAsync: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalAddUserWithUiAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalGetDeviceUserIsPresent_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalGetDeviceUserIsPresent
    bool result = XalGetDeviceUserIsPresent();
    // CODE SNIPPET END

    LogToScreen("XalGetDeviceUserIsPresent: result=%d", result);
    LogToFile("XalGetDeviceUserIsPresent: result=%d", result);
    return LuaReturnHR(L, S_OK);
}

int XalGetDeviceUser_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalGetDeviceUser
    XalUserHandle deviceUser = nullptr;
    HRESULT hr = XalGetDeviceUser(&deviceUser);
    // CODE SNIPPET END

    LogToScreen("XalGetDeviceUser: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalGetDeviceUser: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalSignOutUserAsyncIsPresent_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalSignOutUserAsyncIsPresent
    bool result = XalSignOutUserAsyncIsPresent();
    // CODE SNIPPET END

    LogToScreen("XalSignOutUserAsyncIsPresent: result=%d", result);
    LogToFile("XalSignOutUserAsyncIsPresent: result=%d", result);
    return LuaReturnHR(L, S_OK);
}

int XalSignOutUserAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XalSignOutUserAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);

        // TODO: Store and use newUser
        LogToScreen("OnXalSignOutUserAsync: hr=%s", ConvertHR(hr).c_str());
        LogToFile("OnXalSignOutUserAsync: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        XalUserCloseHandle(Data()->xalUser); // CODE SNIP SKIP
        Data()->xalUser = nullptr; // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXalSignOutUserAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XalSignOutUserAsync(Data()->xalUser, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XalSignOutUserAsync: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalSignOutUserAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalUserDuplicateHandle_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalUserDuplicateHandle
    XalUserHandle dupUser = nullptr;
    HRESULT hr = XalUserDuplicateHandle(Data()->xalUser, &dupUser);
    // CODE SNIPPET END

    LogToScreen("XalUserDuplicateHandle: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalUserDuplicateHandle: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalUserCloseHandle_Lua(lua_State *L)
{
    XalUserHandle xalUser = Data()->xalUser;
    if (xalUser != nullptr)
    {
        // CODE SNIPPET START: XalUserCloseHandle
        XalUserCloseHandle(xalUser);
        xalUser = nullptr;
        // CODE SNIPPET END
        Data()->xalUser = nullptr;

        LogToScreen("XalUserCloseHandle");
        LogToFile("XalUserCloseHandle");
    }
    return LuaReturnHR(L, S_OK);
}

int XalUserGetId_Lua(lua_State *L)
{
    if (Data()->xalUser == nullptr)
    {
        return LuaReturnHR(L, S_OK);
    }

    // CODE SNIPPET START: XalUserGetId
    uint64_t xboxUserId = 0;
    HRESULT hr = XalUserGetId(Data()->xalUser, &xboxUserId);
    // CODE SNIPPET END

    LogToScreen("XalUserGetId: hr=%s xboxUserId=%llu", ConvertHR(hr).c_str(), xboxUserId);
    LogToFile("XalUserGetId: hr=%s xboxUserId=%llu", ConvertHR(hr).c_str(), xboxUserId);
    Data()->xboxUserId = xboxUserId;
    return LuaReturnHR(L, hr);
}

int XalUserIsDevice_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalUserIsDevice
    bool result = XalUserIsDevice(Data()->xalUser);
    // CODE SNIPPET END

    LogToScreen("XalUserIsDevice: result=%d", result);
    LogToFile("XalUserIsDevice: result=%d", result);
    return LuaReturnHR(L, S_OK);
}

int XalUserIsGuest_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalUserIsGuest
    bool result = XalUserIsGuest(Data()->xalUser);
    // CODE SNIPPET END

    LogToScreen("XalUserIsGuest: result=%d", result);
    LogToFile("XalUserIsGuest: result=%d", result);
    return LuaReturnHR(L, S_OK);
}

int XalUserGetState_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalUserGetState
    XalUserState state = {};
    HRESULT hr = XalUserGetState(Data()->xalUser, &state);
    // CODE SNIPPET END

    LogToScreen("XalUserGetState: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalUserGetState: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalUserGetGamertag_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalUserGetGamertag
    size_t gamerTagSize = XalUserGetGamertagSize(Data()->xalUser, XalGamertagComponent_Classic);
    std::vector<char> gamerTag(gamerTagSize, '\0');

    size_t bufferUsed;
    HRESULT hr = XalUserGetGamertag(Data()->xalUser, XalGamertagComponent_Classic, gamerTagSize, gamerTag.data(), &bufferUsed);
    // CODE SNIPPET END

    LogToScreen("XalUserGetGamertag %s: hr=%s gamerTag=%s", gamerTag.data(), ConvertHR(hr).c_str(), gamerTag.data());
    LogToFile("XalUserGetGamertag %s: hr=%s gamerTag=%s", gamerTag.data(), ConvertHR(hr).c_str(), gamerTag.data());

    Data()->gamertag = std::string(gamerTag.data());
    return LuaReturnHR(L, hr);
}

int XalUserGetAgeGroup_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalUserGetAgeGroup
    XalAgeGroup ageGroup = {};
    HRESULT hr = XalUserGetAgeGroup(Data()->xalUser, &ageGroup);
    // CODE SNIPPET END

    LogToScreen("XalUserGetAgeGroup: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalUserGetAgeGroup: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalUserCheckPrivilege_Lua(lua_State *L)
{
    // CODE SNIPPET START: XalUserCheckPrivilege
    XalPrivilege privilege = XalPrivilege_Multiplayer;
    bool hasPrivilege = false;
    XalPrivilegeCheckDenyReasons reasons = { };
    HRESULT hr = XalUserCheckPrivilege(Data()->xalUser, privilege, &hasPrivilege, &reasons);
    // CODE SNIPPET END

    LogToScreen("XalUserCheckPrivilege: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalUserCheckPrivilege: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalUserGetGamerPictureAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XalUserGetGamerPictureAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t bufferSize;
        HRESULT hr = XalUserGetGamerPictureResultSize(asyncBlock, &bufferSize);
        LogToScreen("XalUserGetGamerPictureResultSize: hr=%s bufferSize=%d", ConvertHR(hr).c_str(), bufferSize);
        LogToFile("XalUserGetGamerPictureResultSize: hr=%s bufferSize=%d", ConvertHR(hr).c_str(), bufferSize); // CODE SNIP SKIP
        
        std::vector<byte> buffer(bufferSize);
        hr = XalUserGetGamerPictureResult(asyncBlock, bufferSize, buffer.data());
        LogToScreen("XalUserGetGamerPictureResult: hr=%s", ConvertHR(hr).c_str());
        LogToFile("XalUserGetGamerPictureResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXalUserGetGamerPictureAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XalUserGetGamerPictureAsync(Data()->xalUser, XalGamerPictureSize_Small, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XalUserGetGamerPictureAsync: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalUserGetGamerPictureAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalUserGetTokenAndSignatureSilentlyAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    std::string method = GetStringFromLua(L, 1, "");
    std::string url = GetStringFromLua(L, 2, "");

    // CODE SNIPPET START: XalUserGetTokenAndSignatureSilentlyAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        size_t bufferSize;
        HRESULT hr = XalUserGetTokenAndSignatureSilentlyResultSize(asyncBlock, &bufferSize);
        LogToScreen("XalUserGetTokenAndSignatureSilentlyResultSize: hr=%s bufferSize=%d", ConvertHR(hr).c_str(), bufferSize);
        LogToFile("XalUserGetTokenAndSignatureSilentlyResultSize: hr=%s bufferSize=%d", ConvertHR(hr).c_str(), bufferSize); // CODE SNIP SKIP
        
        std::vector<byte> buffer(bufferSize);
        XalUserGetTokenAndSignatureData* result;
        hr = XalUserGetTokenAndSignatureSilentlyResult(asyncBlock, bufferSize, buffer.data(), &result, nullptr);
        LogToScreen("XalUserGetTokenAndSignatureSilentlyResult: hr=%s", ConvertHR(hr).c_str());
        LogToFile("XalUserGetTokenAndSignatureSilentlyResult: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXalUserGetTokenAndSignatureSilentlyAsync"); // CODE SNIP SKIP
    };

    XalUserGetTokenAndSignatureArgs args{};
    args.method = method.data();
    args.url = url.data();

    HRESULT hr = XalUserGetTokenAndSignatureSilentlyAsync(Data()->xalUser, &args, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XalUserCheckPrivilegesWithUiAsync: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalUserCheckPrivilegesWithUiAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalUserResolveIssueWithUiAsync_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    // CODE SNIPPET START: XalUserResolveIssueWithUiAsync
    auto asyncBlock = std::make_unique<XAsyncBlock>(); 
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        LogToScreen("XAsyncGetStatus: hr=%s", ConvertHR(hr).c_str());
        LogToFile("XAsyncGetStatus: hr=%s", ConvertHR(hr).c_str()); // CODE SNIP SKIP
        CallLuaFunctionWithHr(hr, "OnXalUserResolveIssueWithUiAsync"); // CODE SNIP SKIP
    };

    HRESULT hr = XalUserResolveIssueWithUiAsync(Data()->xalUser, "https://www.xboxlive.com", asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    // CODE SNIPPET END

    LogToScreen("XalUserResolveIssueWithUiAsync: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalUserResolveIssueWithUiAsync: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

// CODE SNIPPET START: XalUserRegisterChangeEventHandler
void OnXalUserChangeEventHandler(
    _In_opt_ void* context,
    _In_ XalUserLocalId userId,
    _In_ XalUserChangeType change)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(userId);
    UNREFERENCED_PARAMETER(change);
}

#if HC_PLATFORM == HC_PLATFORM_GDK
void CALLBACK OnXalUserChangeEventHandler_GDK(
    _In_opt_ void* context,
    _In_ XUserLocalId userLocalId,
    _In_ XUserChangeEvent event)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(userLocalId);
    UNREFERENCED_PARAMETER(event);
}
#endif
// CODE SNIPPET END

int XalUserRegisterChangeEventHandler_Lua(lua_State *L)
{
    CreateQueueIfNeeded();

    void* context = nullptr;
    XalRegistrationToken token = { 0 };

    // CODE SNIPPET START: XalUserRegisterChangeEventHandler
#if HC_PLATFORM == HC_PLATFORM_GDK
    HRESULT hr = XalUserRegisterChangeEventHandler(Data()->queue, context, OnXalUserChangeEventHandler_GDK, &token);
#else
    HRESULT hr = XalUserRegisterChangeEventHandler(Data()->queue, context, OnXalUserChangeEventHandler, &token);
#endif
    // CODE SNIPPET END

    LogToScreen("XalUserRegisterChangeEventHandler: hr=%s", ConvertHR(hr).c_str());
    LogToFile("XalUserRegisterChangeEventHandler: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XalUserUnregisterChangeEventHandler_Lua(lua_State *L)
{
    XalRegistrationToken token{0};

    // CODE SNIPPET START: XalUserUnregisterChangeEventHandler
    XalUserUnregisterChangeEventHandler(token);
    // CODE SNIPPET END

    LogToScreen("XalUserUnregisterChangeEventHandler");
    LogToFile("XalUserUnregisterChangeEventHandler");
    return LuaReturnHR(L, S_OK);
}


#if HC_PLATFORM == HC_PLATFORM_GDK || HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_ANDROID
int XalPlatformWebSetEventHandler_Lua(lua_State *L)
{
     return LuaReturnHR(L, S_OK);
}

int XalPlatformStorageSetEventHandlers_Lua(lua_State *L)
{
    return LuaReturnHR(L, S_OK);
}
#endif

void SetupAPIs_Xal()
{
    lua_register(Data()->L, "XalInitialize", XalInitialize_Lua);
    lua_register(Data()->L, "XalCleanupAsync", XalCleanupAsync_Lua);

    lua_register(Data()->L, "XalPlatformWebSetEventHandler", XalPlatformWebSetEventHandler_Lua);
    lua_register(Data()->L, "XalPlatformStorageSetEventHandlers", XalPlatformStorageSetEventHandlers_Lua);
    
    lua_register(Data()->L, "XalGetMaxUsers", XalGetMaxUsers_Lua);

    lua_register(Data()->L, "XalTryAddFirstUserSilentlyAsync", XalTryAddFirstUserSilentlyAsync_Lua);
    lua_register(Data()->L, "XalAddUserWithUiAsync", XalAddUserWithUiAsync_Lua);
    lua_register(Data()->L, "XalGetDeviceUserIsPresent", XalGetDeviceUserIsPresent_Lua);
    lua_register(Data()->L, "XalGetDeviceUser", XalGetDeviceUser_Lua);
    lua_register(Data()->L, "XalSignOutUserAsyncIsPresent", XalSignOutUserAsyncIsPresent_Lua);
    lua_register(Data()->L, "XalSignOutUserAsync", XalSignOutUserAsync_Lua);

    lua_register(Data()->L, "XalUserDuplicateHandle", XalUserDuplicateHandle_Lua);
    lua_register(Data()->L, "XalUserCloseHandle", XalUserCloseHandle_Lua);
    lua_register(Data()->L, "XalUserGetId", XalUserGetId_Lua);
    lua_register(Data()->L, "XalUserIsDevice", XalUserIsDevice_Lua);
    lua_register(Data()->L, "XalUserIsGuest", XalUserIsGuest_Lua);
    lua_register(Data()->L, "XalUserGetState", XalUserGetState_Lua);
    lua_register(Data()->L, "XalUserGetGamertag", XalUserGetGamertag_Lua);
    lua_register(Data()->L, "XalUserGetGamerPictureAsync", XalUserGetGamerPictureAsync_Lua);
    lua_register(Data()->L, "XalUserGetAgeGroup", XalUserGetAgeGroup_Lua);
    lua_register(Data()->L, "XalUserCheckPrivilege", XalUserCheckPrivilege_Lua);
    lua_register(Data()->L, "XalUserGetTokenAndSignatureSilentlyAsync", XalUserGetTokenAndSignatureSilentlyAsync_Lua);
    lua_register(Data()->L, "XalUserResolveIssueWithUiAsync", XalUserResolveIssueWithUiAsync_Lua);
    lua_register(Data()->L, "XalUserRegisterChangeEventHandler", XalUserRegisterChangeEventHandler_Lua);
    lua_register(Data()->L, "XalUserUnregisterChangeEventHandler", XalUserUnregisterChangeEventHandler_Lua);
}
                                                          