// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

#if CPP_TESTS_ENABLED
static struct PresenceStateCpp
{
    PresenceStateCpp() = default;
    ~PresenceStateCpp()
    {
        //assert(!presenceRecord);
        assert(!devicePresenceChangeSubscription);
        assert(!titlePresenceChangeSubscription);
        assert(!devicePresenceChangedHandlerContext);
        assert(!titlePresenceChangedHandlerContext);
    }

    std::shared_ptr<xbox::services::presence::presence_record> presenceRecord{ nullptr };
    std::shared_ptr<xbox::services::presence::device_presence_change_subscription> devicePresenceChangeSubscription{ nullptr };
    std::shared_ptr<xbox::services::presence::title_presence_change_subscription> titlePresenceChangeSubscription{ nullptr };
    function_context devicePresenceChangedHandlerContext{ nullptr };
    function_context titlePresenceChangedHandlerContext{ nullptr };
} presenceStateCpp;

#endif

int PresenceRecordGetXuidCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    string_t xuid = presenceStateCpp.presenceRecord->xbox_user_id();
    LogToScreen("PresenceRecordGetXuidCpp xuid=%s", xbox::services::Utils::StringFromStringT(xuid).c_str());
#else
    LogToFile("PresenceRecordGetXuidCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceRecordGetUserStateCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    xbox::services::presence::user_presence_state userState = presenceStateCpp.presenceRecord->user_state();
    LogToScreen("PresenceRecordGetUserStateCpp state=%u", userState);
#else
    LogToFile("PresenceRecordGetUserStateCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceRecordGetDeviceRecordsCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::vector<xbox::services::presence::presence_device_record> deviceRecords = presenceStateCpp.presenceRecord->presence_device_records();
    for (auto deviceRecord : deviceRecords)
    {
        LogToScreen("Got presence_device_record with device type %u and %u title records", deviceRecord.device_type(), deviceRecord.presence_title_records().size());
        for (auto titleRecord : deviceRecord.presence_title_records())
        {
            LogToScreen("Title record titleId %u", titleRecord.title_id());
        }

    }
    LogToFile("PresenceRecordGetDeviceRecordsCpp");
#else
    LogToFile("PresenceRecordGetDeviceRecordsCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceRecordCloseHandleCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    presenceStateCpp.presenceRecord = nullptr;
    LogToFile("PresenceRecordCloseHandleCpp");
#else
    LogToFile("PresenceRecordCloseHandleCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceServiceSetPresence_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    bool isActiveInTitle = GetUint64FromLua(L, 1, 1);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->presence_service().set_presence(isActiveInTitle).then(
        [](xbox::services::xbox_live_result<void> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PresenceServiceSetPresence: hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {

            }
            if (hr == HTTP_E_STATUS_429_TOO_MANY_REQUESTS)
            {
                LogToFile("PresenceServiceSetPresence returned 429.  Ignoring failure");
                hr = S_OK;
            }

            CallLuaFunctionWithHr(hr, "OnPresenceServiceSetPresence");
        });
#else
    CallLuaFunctionWithHr(S_OK, "OnPresenceServiceSetPresence");
    LogToFile("PresenceServiceSetPresence is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceServiceGetPresence_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    uint64_t xuid = Data()->xboxUserId;
    if (Data()->m_multiDeviceManager->GetRemoteXuid() != 0)
    {
        xuid = Data()->m_multiDeviceManager->GetRemoteXuid();
    }
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 1, xuid));

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->presence_service().get_presence(xboxUserId).then(
        [](xbox::services::xbox_live_result<xbox::services::presence::presence_record> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PresenceServiceGetPresence: hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                presenceStateCpp.presenceRecord = std::make_shared<xbox::services::presence::presence_record>(result.payload());
            }

            CallLuaFunctionWithHr(hr, "OnPresenceServiceGetPresence");
        });
#else
    LogToFile("PresenceServiceGetPresence is disabled for this platform");
    CallLuaFunctionWithHr(S_OK, "OnPresenceServiceGetPresence");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceServiceGetPresenceForMultipleUsers_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::vector<string_t> xuids{ _T("2814639011617876"), _T("2814641789541994") };
    std::vector<xbox::services::presence::presence_device_type> deviceTypes;
    std::vector<uint32_t> titleIds;
    xbox::services::presence::presence_detail_level detailLevel = xbox::services::presence::presence_detail_level::all;
    bool onlineOnly = true;
    bool broadcastingOnly = false;

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->presence_service().get_presence_for_multiple_users(
        xuids,
        deviceTypes,
        titleIds,
        detailLevel,
        onlineOnly,
        broadcastingOnly
    ).then([](xbox::services::xbox_live_result<std::vector<xbox::services::presence::presence_record>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PresenceServiceGetPresenceForMultipleUsers: hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                LogToFile("Got %u presence records", result.payload().size());
            }

            CallLuaFunctionWithHr(hr, "OnPresenceServiceGetPresenceForMultipleUsers");
        });
#else
    LogToFile("PresenceServiceGetPresenceForMultipleUsers is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnPresenceServiceGetPresenceForMultipleUsers");
#endif

    return LuaReturnHR(L, S_OK);
}

int PresenceServiceGetPresenceForSocialGroup_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    string_t socialGroup = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "Favorites").c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->presence_service().get_presence_for_social_group(socialGroup).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::presence::presence_record>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PresenceServiceGetPresenceForSocialGroup: hr=%s", ConvertHR(hr).data());

            if (SUCCEEDED(hr))
            {
                LogToFile("Got %u presence records", result.payload().size());
            }

            CallLuaFunctionWithHr(hr, "OnPresenceServiceGetPresenceForMultipleUsers");
        });
#else
    LogToFile("PresenceServiceGetPresenceForSocialGroup is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnPresenceServiceGetPresenceForMultipleUsers");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceServiceSubscribeToDevicePresenceChange_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    string_t xuid{ _T("2814639011617876") };
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xbox::services::xbox_live_result<std::shared_ptr<xbox::services::presence::device_presence_change_subscription>> result = xblc->presence_service().subscribe_to_device_presence_change(xuid);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    LogToFile("PresenceServiceSubscribeToDevicePresenceChange: hr=%s", ConvertHR(hr).c_str());

    if (SUCCEEDED(hr))
    {
        presenceStateCpp.devicePresenceChangeSubscription = result.payload();
    }

    return LuaReturnHR(L, hr);
#else
    LogToFile("PresenceServiceSubscribeToDevicePresenceChange is disabled for this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int PresenceServiceUnsubscribeFromDevicePresenceChange_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xbox::services::xbox_live_result<void> result = xblc->presence_service().unsubscribe_from_device_presence_change(presenceStateCpp.devicePresenceChangeSubscription);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    LogToFile("PresenceServiceUnsubscribeFromDevicePresenceChange: hr=%s", ConvertHR(hr).c_str());

    if (SUCCEEDED(hr))
    {
        presenceStateCpp.devicePresenceChangeSubscription = nullptr;
    }
    return LuaReturnHR(L, hr);
#else
    LogToFile("PresenceServiceUnsubscribeFromDevicePresenceChange is disabled for this platform");
    return LuaReturnHR(L, S_OK);
#endif
}

int PresenceServiceSubscribeToTitlePresenceChange_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    string_t xuid{ _T("2814639011617876") };
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xbox::services::xbox_live_result<std::shared_ptr<xbox::services::presence::title_presence_change_subscription>> result = xblc->presence_service().subscribe_to_title_presence_change(xuid, Data()->titleId);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    LogToFile("PresenceServiceSubscribeToTitlePresenceChange: hr=%s", ConvertHR(hr).c_str());

    if (SUCCEEDED(hr))
    {
        presenceStateCpp.titlePresenceChangeSubscription = result.payload();
    }

    return LuaReturnHR(L, hr);
#else
    LogToFile("PresenceServiceSubscribeToTitlePresenceChange is disabled for this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int PresenceServiceUnsubscribeFromTitlePresenceChange_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xbox::services::xbox_live_result<void> result = xblc->presence_service().unsubscribe_from_title_presence_change(presenceStateCpp.titlePresenceChangeSubscription);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    LogToFile("PresenceServiceUnsubscribeFromTitlePresenceChange: hr=%s", ConvertHR(hr).c_str());

    if (SUCCEEDED(hr))
    {
        presenceStateCpp.titlePresenceChangeSubscription = nullptr;
    }
    return LuaReturnHR(L, hr);
#else
    LogToFile("PresenceServiceUnsubscribeFromTitlePresenceChange is disabled for this platform");
    return LuaReturnHR(L, S_OK);
#endif
}

int PresenceServiceAddDevicePresenceChangedHandler_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    presenceStateCpp.devicePresenceChangedHandlerContext = xblc->presence_service().add_device_presence_changed_handler(
        [](xbox::services::presence::device_presence_change_event_args args) 
        {
            LogToFile("Device presence change notification received:");
            LogToFile("Xuid = %s, device_type = %u, is_user_logged_on_device = %u", xbox::services::Utils::StringFromStringT(args.xbox_user_id()).c_str(), args.device_type(), args.is_user_logged_on_device());
        });

    LogToFile("PresenceServiceAddDevicePresenceChangedHandler");
#else
    LogToFile("PresenceServiceAddDevicePresenceChangedHandler is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceServiceRemoveDevicePresenceChangedHandler_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->presence_service().remove_device_presence_changed_handler(presenceStateCpp.devicePresenceChangedHandlerContext);

    presenceStateCpp.devicePresenceChangedHandlerContext = nullptr;
    LogToFile("PresenceServiceRemoveDevicePresenceChangedHandler");
#else
    LogToFile("PresenceServiceRemoveDevicePresenceChangedHandler is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceServiceAddTitlePresenceChangedHandler_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    presenceStateCpp.titlePresenceChangedHandlerContext = xblc->presence_service().add_title_presence_changed_handler(
        [](xbox::services::presence::title_presence_change_event_args args)
        {
            LogToFile("Title presence change notification received:");
            LogToFile("Xuid = %s, title_id = %u, title_state = %u", xbox::services::Utils::StringFromStringT(args.xbox_user_id()).c_str(), args.title_id(), args.title_state());
        });

    LogToFile("PresenceServiceAddTitlePresenceChangedHandler");
#else
    LogToFile("PresenceServiceAddTitlePresenceChangedHandler is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int PresenceServiceRemoveTitlePresenceChangedHandler_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xboxLiveContext);
    xblc->presence_service().remove_title_presence_changed_handler(presenceStateCpp.titlePresenceChangedHandlerContext);

    presenceStateCpp.titlePresenceChangedHandlerContext = nullptr;
    LogToFile("PresenceServiceRemoveTitlePresenceChangedHandler");
#else
    LogToFile("PresenceServiceRemoveTitlePresenceChangedHandler is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppPresence()
{
    lua_register(Data()->L, "PresenceRecordGetXuidCpp", PresenceRecordGetXuidCpp_Lua);
    lua_register(Data()->L, "PresenceRecordGetUserStateCpp", PresenceRecordGetUserStateCpp_Lua);
    lua_register(Data()->L, "PresenceRecordGetDeviceRecordsCpp", PresenceRecordGetDeviceRecordsCpp_Lua);
    lua_register(Data()->L, "PresenceRecordCloseHandleCpp", PresenceRecordCloseHandleCpp_Lua);

    lua_register(Data()->L, "PresenceServiceSetPresence", PresenceServiceSetPresence_Lua);
    lua_register(Data()->L, "PresenceServiceGetPresence", PresenceServiceGetPresence_Lua);
    lua_register(Data()->L, "PresenceServiceGetPresenceForSocialGroup", PresenceServiceGetPresenceForSocialGroup_Lua);
    lua_register(Data()->L, "PresenceServiceGetPresenceForMultipleUsers", PresenceServiceGetPresenceForMultipleUsers_Lua);
    lua_register(Data()->L, "PresenceServiceSubscribeToDevicePresenceChange", PresenceServiceSubscribeToDevicePresenceChange_Lua);
    lua_register(Data()->L, "PresenceServiceUnsubscribeFromDevicePresenceChange", PresenceServiceUnsubscribeFromDevicePresenceChange_Lua);
    lua_register(Data()->L, "PresenceServiceSubscribeToTitlePresenceChange", PresenceServiceSubscribeToTitlePresenceChange_Lua);
    lua_register(Data()->L, "PresenceServiceUnsubscribeFromTitlePresenceChange", PresenceServiceUnsubscribeFromTitlePresenceChange_Lua);
    lua_register(Data()->L, "PresenceServiceAddDevicePresenceChangedHandler", PresenceServiceAddDevicePresenceChangedHandler_Lua);
    lua_register(Data()->L, "PresenceServiceRemoveDevicePresenceChangedHandler", PresenceServiceRemoveDevicePresenceChangedHandler_Lua);
    lua_register(Data()->L, "PresenceServiceAddTitlePresenceChangedHandler", PresenceServiceAddTitlePresenceChangedHandler_Lua);
    lua_register(Data()->L, "PresenceServiceRemoveTitlePresenceChangedHandler", PresenceServiceRemoveTitlePresenceChangedHandler_Lua);
}
