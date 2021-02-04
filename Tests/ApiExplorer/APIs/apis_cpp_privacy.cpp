// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

#if CPP_TESTS_ENABLED
string_t ConvertXblPermissionToStringT(XblPermission permissionType)
{
    switch (permissionType)
    {
    case XblPermission::BroadcastWithTwitch:
        return xbox::services::privacy::permission_id_constants::broadcast_with_twitch();
    case XblPermission::CommunicateUsingText:
        return xbox::services::privacy::permission_id_constants::communicate_using_text();
    case XblPermission::CommunicateUsingVideo:
        return xbox::services::privacy::permission_id_constants::communicate_using_video();
    case XblPermission::CommunicateUsingVoice:
        return xbox::services::privacy::permission_id_constants::communicate_using_voice();
    case XblPermission::PlayMultiplayer:
        return xbox::services::privacy::permission_id_constants::play_multiplayer();
    //case XblPermission::ShareItem:
        //unsupported
    //case XblPermission::ShareTargetContentToExternalNetworks:
        //unsupported
    case XblPermission::ViewTargetExerciseInfo:
        return xbox::services::privacy::permission_id_constants::view_target_exercise_info();
    case XblPermission::ViewTargetGameHistory:
        return xbox::services::privacy::permission_id_constants::view_target_game_history();
    case XblPermission::ViewTargetMusicHistory:
        return xbox::services::privacy::permission_id_constants::view_target_music_history();
    case XblPermission::ViewTargetMusicStatus:
        return xbox::services::privacy::permission_id_constants::view_target_music_status();
    case XblPermission::ViewTargetPresence:
        return xbox::services::privacy::permission_id_constants::view_target_presence();
    case XblPermission::ViewTargetProfile:
        return xbox::services::privacy::permission_id_constants::view_target_profile();
    case XblPermission::ViewTargetUserCreatedContent:
        return xbox::services::privacy::permission_id_constants::view_target_user_created_content();
    case XblPermission::ViewTargetVideoHistory:
        return xbox::services::privacy::permission_id_constants::view_target_video_history();
    case XblPermission::ViewTargetVideoStatus:
        return xbox::services::privacy::permission_id_constants::view_target_video_status();
    case XblPermission::WriteComment:
    case XblPermission::ShareItem:
    case XblPermission::ShareTargetContentToExternalNetworks:
    case XblPermission::Unknown:
    default:
        return _T("Unknown");
    }
}
#endif
int PrivacyServiceGetAvoidList_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->privacy_service().get_avoid_list().then(
        [](xbox::services::xbox_live_result<std::vector<string_t>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PrivacyServiceGetAvoidList: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<string_t> avoidList = result.payload();
                LogToFile("PrivacyServiceGetAvoidList avoided xuids count=%d", avoidList.size());
            }

            CallLuaFunctionWithHr(hr, "OnPrivacyServiceGetAvoidList");
        });
#else
    LogToFile("PrivacyServiceGetAvoidList is disabled on this platform");
    CallLuaFunctionWithHr(S_OK, "OnPrivacyServiceGetAvoidList");
#endif

    return LuaReturnHR(L, S_OK);
}

int PrivacyServiceCheckPermissionWithTargetUser_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    string_t permissionToCheck = ConvertXblPermissionToStringT(static_cast<XblPermission>(GetUint32FromLua(L, 1, (uint32_t)XblPermission::ViewTargetProfile)));
    string_t targetXuid = xbox::services::Utils::StringTFromUint64(GetUint64FromLua(L, 2, 2743710844428572));

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->privacy_service().check_permission_with_target_user(
        permissionToCheck,
        targetXuid
    ).then(
        [](xbox::services::xbox_live_result<xbox::services::privacy::permission_check_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PrivacyServiceCheckPermissionWithTargetUser: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::privacy::permission_check_result permissionResult = result.payload();
                LogToFile("PrivacyServiceCheckPermissionWithTargetUser: isAllowed=%d", permissionResult.is_allowed());
            }

            CallLuaFunctionWithHr(hr, "OnPrivacyServiceCheckPermissionWithTargetUser");
        });
#else
    LogToFile("PrivacyServiceCheckPermissionWithTargetUser is disabled for this platform.");
    CallLuaFunctionWithHr(S_OK, "OnPrivacyServiceCheckPermissionWithTargetUser");
#endif
    return LuaReturnHR(L, S_OK);
}

int PrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::vector<string_t> permissionsToCheck{
        xbox::services::privacy::permission_id_constants::communicate_using_text(),
        xbox::services::privacy::permission_id_constants::communicate_using_video(),
        xbox::services::privacy::permission_id_constants::communicate_using_voice(),
        xbox::services::privacy::permission_id_constants::view_target_profile(),
        xbox::services::privacy::permission_id_constants::view_target_game_history(),
        xbox::services::privacy::permission_id_constants::view_target_video_history(),
        xbox::services::privacy::permission_id_constants::view_target_music_history(),
        xbox::services::privacy::permission_id_constants::view_target_exercise_info(),
        xbox::services::privacy::permission_id_constants::view_target_presence(),
        xbox::services::privacy::permission_id_constants::view_target_video_status(),
        xbox::services::privacy::permission_id_constants::view_target_music_status(),
        xbox::services::privacy::permission_id_constants::play_multiplayer(),
        xbox::services::privacy::permission_id_constants::view_target_user_created_content(),
        xbox::services::privacy::permission_id_constants::broadcast_with_twitch()
    };
    std::vector<string_t> targetXuids{
        _T("2743710844428572"),
        _T("2533274819720636"),
        xbox::services::privacy::anonymous_user_type_constants::cross_network_user(),
        xbox::services::privacy::anonymous_user_type_constants::crost_network_friend()
    };

    //the multiple_permissions_check_result for an anonymous type returns results for both anonymous types, is this expected?
    size_t expectedResultCount{ permissionsToCheck.size() * (targetXuids.size() + 2) };

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->privacy_service().check_multiple_permissions_with_multiple_target_users(
        permissionsToCheck,
        targetXuids
    ).then(
        [expectedResultCount](xbox::services::xbox_live_result<std::vector<xbox::services::privacy::multiple_permissions_check_result>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::privacy::multiple_permissions_check_result> multiplePermissionsResults = result.payload();
                size_t resultCount = 0;
                for (xbox::services::privacy::multiple_permissions_check_result multiplePermissionsResult : multiplePermissionsResults)
                {
                    resultCount += multiplePermissionsResult.items().size();
                }

                LogToFile("PrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers: expectedResultCount=%d resultCount=%d", expectedResultCount, resultCount);
                assert(resultCount == expectedResultCount);
            }

            CallLuaFunctionWithHr(hr, "OnPrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers");
        });
#else
    LogToFile("PrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers is disabled for this platform");
    CallLuaFunctionWithHr(S_OK, "OnPrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers");
#endif
    return LuaReturnHR(L, S_OK);
}

int PrivacyServiceGetMuteList_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->privacy_service().get_mute_list().then(
        [](xbox::services::xbox_live_result<std::vector<string_t>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PrivacyServiceGetMuteList: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<string_t> muteList = result.payload();
                LogToFile("PrivacyServiceGetMuteList muted xuids count=%d", muteList.size());
            }

            CallLuaFunctionWithHr(hr, "OnPrivacyServiceGetMuteList");
        });
#else
    LogToFile("PrivacyServiceGetMuteList is disabled on this platform");
    CallLuaFunctionWithHr(S_OK, "OnPrivacyServiceGetMuteList");
#endif

    return LuaReturnHR(L, S_OK);
}

int PrivacyServiceGetAvoidOrMuteList_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->privacy_service().get_avoid_or_mute_list(
        _T("avoid")
    ).then(
        [](xbox::services::xbox_live_result<std::vector<string_t>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("PrivacyServiceGetAvoidOrMuteList: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<string_t> avoidList = result.payload();
                LogToFile("PrivacyServiceGetAvoidOrMuteList xuids count=%d", avoidList.size());
            }

            CallLuaFunctionWithHr(hr, "OnPrivacyServiceGetAvoidOrMuteList");
        });
#else
    LogToFile("PrivacyServiceGetAvoidOrMuteList is disabled on this platform");
    CallLuaFunctionWithHr(S_OK, "OnPrivacyServiceGetAvoidOrMuteList");
#endif

    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppPrivacy()
{
    lua_register(Data()->L, "PrivacyServiceGetAvoidList", PrivacyServiceGetAvoidList_Lua);
    lua_register(Data()->L, "PrivacyServiceCheckPermissionWithTargetUser", PrivacyServiceCheckPermissionWithTargetUser_Lua);
    lua_register(Data()->L, "PrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers", PrivacyServiceCheckMultiplePermissionsWithMultipleTargetUsers_Lua);
    lua_register(Data()->L, "PrivacyServiceGetMuteList", PrivacyServiceGetMuteList_Lua);
    lua_register(Data()->L, "PrivacyServiceGetAvoidOrMuteList", PrivacyServiceGetAvoidOrMuteList_Lua);
}
