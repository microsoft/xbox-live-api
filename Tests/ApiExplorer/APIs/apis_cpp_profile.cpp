// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int ProfileServiceGetUserProfile_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->profile_service().get_user_profile(
        xboxUserId
    ).then(
        [](xbox::services::xbox_live_result<xbox::services::social::xbox_user_profile> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("ProfileServiceGetUserProfile: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::social::xbox_user_profile userProfile = result.payload();
                LogToFile("ProfileServiceGetUserProfile: gamertag=%s", ConvertHR(hr).c_str(), xbox::services::Utils::StringFromStringT(userProfile.gamertag()).c_str());
            }

            CallLuaFunctionWithHr(hr, "OnProfileServiceGetUserProfile");
        });
#else
    LogToFile("ProfileServiceGetUserProfile is disabled on this platform.");
    CallLuaFunctionWithHr(S_OK, "OnProfileServiceGetUserProfile");
#endif
    return LuaReturnHR(L, S_OK);
}

int ProfileServiceGetUserProfiles_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::vector<string_t> userIds;
    string_t xboxUserId = xbox::services::Utils::StringTFromUint64(Data()->xboxUserId);
    userIds.push_back(xboxUserId);

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->profile_service().get_user_profiles(
        userIds
    ).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::social::xbox_user_profile>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("ProfileServiceGetUserProfiles: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::social::xbox_user_profile> profiles = result.payload();
                LogToFile("ProfileServiceGetUserProfiles: gamertag=%s", ConvertHR(hr).c_str(), xbox::services::Utils::StringFromStringT(profiles[0].gamertag()).c_str());
            }

            CallLuaFunctionWithHr(hr, "OnProfileServiceGetUserProfiles");
        });
#else
    LogToFile("ProfileServiceGetUserProfiles is disabled on this platform.");
    CallLuaFunctionWithHr(S_OK, "OnProfileServiceGetUserProfiles");
#endif
    return LuaReturnHR(L, S_OK);
}

int ProfileServiceGetUserProfilesForSocialGroup_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    string_t socialGroup = xbox::services::Utils::StringTFromUtf8(GetStringFromLua(L, 1, "People").c_str());

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->profile_service().get_user_profiles_for_social_group(
        socialGroup
    ).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::social::xbox_user_profile>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("ProfileServiceGetUserProfilesForSocialGroup: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::social::xbox_user_profile> profiles = result.payload();
                LogToFile("ProfileServiceGetUserProfilesForSocialGroup: profilesCount=%d", profiles.size());
                for (auto profile : profiles)
                {
                     LogToFile("\tgamertag=%s", xbox::services::Utils::StringFromStringT(profile.gamertag()).c_str());
                }
            }

            CallLuaFunctionWithHr(hr, "OnProfileServiceGetUserProfilesForSocialGroup");
        });
#else
    LogToFile("ProfileServiceGetUserProfiles is disabled on this platform.");
    CallLuaFunctionWithHr(S_OK, "OnProfileServiceGetUserProfilesForSocialGroup");
#endif
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppProfile()
{
    lua_register(Data()->L, "ProfileServiceGetUserProfile", ProfileServiceGetUserProfile_Lua);
    lua_register(Data()->L, "ProfileServiceGetUserProfiles", ProfileServiceGetUserProfiles_Lua);
    lua_register(Data()->L, "ProfileServiceGetUserProfilesForSocialGroup", ProfileServiceGetUserProfilesForSocialGroup_Lua);
}