// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include <atomic>

#if CPP_TESTS_ENABLED
static struct SocialManagerCppState
{
    SocialManagerCppState() = default;
    ~SocialManagerCppState()
    {
        // Validate that our tests cleaned up correctly
        assert(!doWork);
        assert(groups.empty());
    }

    std::map<uint64_t, std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> groups;
    std::thread doWorkThread{};
    std::atomic<bool> doWork{ false };
    std::atomic<bool> doWorkJoinWhenDone{ false };
} socialManagerCppState;

HRESULT SocialManagerDoWorkCpp()
{
    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();
    std::vector<xbox::services::social::manager::social_event> events = socialManager->do_work();

    for (auto& socialEvent : events)
    {
        static std::unordered_map<xbox::services::social::manager::social_event_type, std::string> eventTypesMap
        {
            { xbox::services::social::manager::social_event_type::users_added_to_social_graph, "users_added_to_social_graph" },
            { xbox::services::social::manager::social_event_type::users_removed_from_social_graph, "users_removed_from_social_graph" },
            { xbox::services::social::manager::social_event_type::presence_changed, "presence_changed" },
            { xbox::services::social::manager::social_event_type::profiles_changed, "profiles_changed" },
            { xbox::services::social::manager::social_event_type::social_relationships_changed, "social_relationships_changed" },
            { xbox::services::social::manager::social_event_type::local_user_added, "local_user_added" },
            { xbox::services::social::manager::social_event_type::social_user_group_loaded, "social_user_group_loaded" },
            { xbox::services::social::manager::social_event_type::social_user_group_updated, "social_user_group_updated" },
            { xbox::services::social::manager::social_event_type::unknown, "unknown" }
        };

        std::stringstream ss;
        ss << "social_manager::do_work: Event of type " << eventTypesMap[socialEvent.event_type()] << std::endl;
        ss << "Users affected: " << std::endl;
        for (auto userContainer : socialEvent.users_affected())
        {
            char xuid[17];
            xbox::services::Utils::Utf8FromCharT(userContainer.xbox_user_id(), xuid, sizeof(xuid));
            ss << "\t" << xuid << std::endl;
        }
        LogToFile(ss.str().c_str());

        switch (socialEvent.event_type())
        {
        case xbox::services::social::manager::social_event_type::users_added_to_social_graph:
            LogToFile("social_manager::do_work: users_added_to_social_graph event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_UsersAddedToSocialGraphEvent");
            break;
        case xbox::services::social::manager::social_event_type::users_removed_from_social_graph:
            LogToFile("social_manager::do_work: users_removed_from_social_graph event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_UsersRemovedFromSocialGraphEvent");
            break;
        case xbox::services::social::manager::social_event_type::presence_changed:
            LogToFile("social_manager::do_work: presence_changed event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_PresenceChangedEvent");
            break;
        case xbox::services::social::manager::social_event_type::profiles_changed:
            LogToFile("social_manager::do_work: profiles_changed event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_ProfilesChangedEvent");
            break;
        case xbox::services::social::manager::social_event_type::social_relationships_changed:
            LogToFile("social_manager::do_work: social_relationships_changed event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_SocialRelationshipsChangedEvent");
            break;
        case xbox::services::social::manager::social_event_type::local_user_added:
            LogToFile("social_manager::do_work: local_user_added event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_LocalUserAddedEvent");
            break;
        case xbox::services::social::manager::social_event_type::local_user_removed:
            LogToFile("social_manager::do_work: local_user_removed event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_LocalUserRemovedEvent");
            break;
        case xbox::services::social::manager::social_event_type::social_user_group_loaded:
            LogToFile("social_manager::do_work: social_user_group_loaded event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_SocialUserGroupLoadedEvent");
            break;
        case xbox::services::social::manager::social_event_type::social_user_group_updated:
            LogToFile("social_manager::do_work: social_user_group_updated event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_SocialUserGroupUpdatedEvent");
            break;
        case xbox::services::social::manager::social_event_type::unknown:
        default:
            LogToFile("social_manager::do_work: unknown event");
            CallLuaFunctionWithHr(S_OK, "OnSocialManagerDoWorkCpp_UnknownEvent");
            break;
        }
    }

    return S_OK;
}

void StopSocialManagerDoWorkHelperCpp()
{
    if (socialManagerCppState.doWorkJoinWhenDone)
    {
        socialManagerCppState.doWork = false;
        socialManagerCppState.doWorkJoinWhenDone = false;
        socialManagerCppState.doWorkThread.join();
    }
}

xbox::services::social::manager::social_manager_extra_detail_level ConvertStringToCppSocialManagerExtraDetailLevel(const char* str)
{
    xbox::services::social::manager::social_manager_extra_detail_level detailLevel = xbox::services::social::manager::social_manager_extra_detail_level::no_extra_detail;

    if (pal::stricmp(str, "social_manager_extra_detail_level::title_history_leve_T(") == 0) detailLevel = xbox::services::social::manager::social_manager_extra_detail_level::title_history_level;
    else if (pal::stricmp(str, "social_manager_extra_detail_level::preferred_color_leve_T(") == 0) detailLevel = xbox::services::social::manager::social_manager_extra_detail_level::preferred_color_level;

    return detailLevel;
}

xbox::services::social::manager::presence_filter ConvertStringToCppPresenceFilter(const char* str)
{
    xbox::services::social::manager::presence_filter filter = xbox::services::social::manager::presence_filter::unknown;

    if (pal::stricmp(str, "presence_filter::title_online") == 0) filter = xbox::services::social::manager::presence_filter::title_online;
    else if (pal::stricmp(str, "presence_filter::title_offline") == 0) filter = xbox::services::social::manager::presence_filter::title_offline;
#if XSAPI_BUILT_FROM_SOURCE
    else if (pal::stricmp(str, "presence_filter::title_online_outside_title") == 0) filter = xbox::services::social::manager::presence_filter::title_online_outside_title;
#endif
    else if (pal::stricmp(str, "presence_filter::all_online") == 0) filter = xbox::services::social::manager::presence_filter::all_online;
    else if (pal::stricmp(str, "presence_filter::all_offline") == 0) filter = xbox::services::social::manager::presence_filter::all_offline;
    else if (pal::stricmp(str, "presence_filter::all_title") == 0) filter = xbox::services::social::manager::presence_filter::all_title;
    else if (pal::stricmp(str, "presence_filter::al_T(") == 0) filter = xbox::services::social::manager::presence_filter::all;

    return filter;
}

xbox::services::social::manager::relationship_filter ConvertStringToCppRelationshipFilter(const char* str)
{
    xbox::services::social::manager::relationship_filter filter = xbox::services::social::manager::relationship_filter::friends;

    if (pal::stricmp(str, "relationship_filter::friends") == 0) filter = xbox::services::social::manager::relationship_filter::friends;
    else if (pal::stricmp(str, "relationship_filter::favorite") == 0) filter = xbox::services::social::manager::relationship_filter::favorite;

    return filter;
}

// Pool of XDKS.1 xuids to create social groups from
std::vector<string_t> listXuidStrings
{
    _T("2814639011617876"),_T("2814641789541994"),_T("2814644008675844"),_T("2814644210052185"),_T("2814645164579523"),_T("2814646075485729"),_T("2814649783195402"),_T("2814650260879943"),
    _T("2814652370182940"),_T("2814652714045777"),_T("2814654391560620"),_T("2814654975417728"),_T("2814656000993855"),_T("2814660006763195"),_T("2814666715930430"),_T("2814667316080600"),
    _T("2814669550092398"),_T("2814669684179632"),_T("2814669733667211"),_T("2814671180786692"),_T("2814679901432274"),_T("2814613501048225"),_T("2814614352529204"),_T("2814615856126401"),
    _T("2814616641363830"),_T("2814617883586813"),_T("2814618053453081"),_T("2814629752527080"),_T("2814631255161151"),_T("2814632477267887"),_T("2814633284389038"),_T("2814635732495522"),
    _T("2814635779785472"),_T("2814635974475208"),_T("2814636979708499"),_T("2814618092438397"),_T("2814618260480530"),_T("2814618319551907"),_T("2814619559360314"),_T("2814620368929739"),
    _T("2814620769042115"),_T("2814621007349381"),_T("2814623088399025"),_T("2814623825448960"),_T("2814624220291971"),_T("2814624961587858"),_T("2814626394212372"),_T("2814626639518570"),
    _T("2814628203722867"),_T("2814629143923154"),_T("2814614382301082"),_T("2814614959737919"),_T("2814615558140392"),_T("2814618401629514"),_T("2814618701087902"),_T("2814619300882392"),
    _T("2814623785189962"),_T("2814623956387698"),_T("2814625066090704"),_T("2814625471782204"),_T("2814626946705530"),_T("2814627006318591"),_T("2814628046127456"),_T("2814631487749991"),
    _T("2814631517599783"),_T("2814632798310691"),_T("2814633582140204"),_T("2814634204785789"),_T("2814634895412664"),_T("2814635439049207"),_T("2814638609354868"),_T("2814639589885754"),
    _T("2814641670947751"),_T("2814643512602566"),_T("2814646137630843"),_T("2814648499394446"),_T("2814651465227139"),_T("2814652150012664"),_T("2814653926747608"),_T("2814655098938516"),
    _T("2814655264861214"),_T("2814655417678099"),_T("2814655883565306"),_T("2814656031821923"),_T("2814656159501072"),_T("2814656780954834"),_T("2814660657970845"),_T("2814661604435490"),
    _T("2814663444319727"),_T("2814663818015575"),_T("2814665274839967"),_T("2814667273133504"),_T("2814670761542037"),_T("2814672762886609"),_T("2814673772488023"),_T("2814674096344056"),
    _T("2814674229538758"),_T("2814678943953289"),_T("2814680898042782")
};
#endif

//lua commands

int StartSocialManagerDoWorkLoopCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    socialManagerCppState.doWork = true;
    socialManagerCppState.doWorkJoinWhenDone = true;
    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
    socialManagerCppState.doWorkThread = std::thread([]()
        {
            Data()->m_socialDoWorkDone = false;
            while (socialManagerCppState.doWork && !Data()->m_quit)
            {
                {
                    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
                    SocialManagerDoWorkCpp();
                }
                pal::Sleep(10);
            }
            Data()->m_socialDoWorkDone = true;
            LogToFile("Exiting do work thread");
        });
#else
    LogToFile("StartSocialManagerDoWorkLoopCpp is disabled for this platform.");
    //Call a 'disabled' function so tests that are waiting on do work events can still terminate
    CallLuaFunctionWithHr(S_OK, "OnStartSocialManagerDoWorkLoopCppDisabled");
#endif
    return LuaReturnHR(L, S_OK);
}

int StopSocialManagerDoWorkLoopCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    LogToFile("StopSocialManagerDoWorkLoopCpp");
    socialManagerCppState.doWorkJoinWhenDone = true;
    socialManagerCppState.doWork = false;
#else
    LogToFile("StopSocialManagerDoWorkLoopCpp is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerPresenceRecordIsUserPlayingTitleCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED    
    using xbox::services::social::manager::xbox_social_user_group;
    using xbox::services::social::manager::xbox_social_user;

    std::shared_ptr<xbox_social_user_group> group{ nullptr };
    uint64_t luaGroup = GetUint64FromLua(L, 1, 0);
    if (luaGroup)
    {
        group = (*socialManagerCppState.groups.find(luaGroup)).second;
    }
    else
    {
        group = (*socialManagerCppState.groups.begin()).second;
    }
    if (group == nullptr)
    {
        LogToFile("SocialManagerPresenceRecordIsUserPlayingTitleCpp: No xbox_social_user_group Loaded");
        return S_OK;
    }

    uint32_t titleId = GetUint32FromLua(L, 1, 174925616);

    std::vector<xbox_social_user*> userVec = group->users();
    for (auto user : userVec)
    {
        xbox::services::social::manager::social_manager_presence_record presenceRecord = user->presence_record();
        bool playingTitle = presenceRecord.is_user_playing_title(titleId);
        LogToFile("SocialManagerPresenceRecordIsUserPlayingTitleCpp: TitleId: %d, playing: %u", titleId, playingTitle);
    }

#else
    LogToFile("SocialManagerPresenceRecordIsUserPlayingTitleCpp is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerUserGroupGetTypeCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group{ nullptr };
    uint64_t luaGroup = GetUint64FromLua(L, 1, 0);
    if (luaGroup)
    {
        group = (*socialManagerCppState.groups.find(luaGroup)).second;
    }
    else
    {
        group = (*socialManagerCppState.groups.begin()).second;
    }
    if (group == nullptr)
    {
        LogToFile("SocialManagerUserGroupGetTypeCpp: No xbox_social_user_group Loaded");
        return S_OK;
    }

    xbox::services::social::manager::social_user_group_type type = group->social_user_group_type();
    LogToFile("SocialManagerUserGroupGetTypeCpp: type=%u", type);
#else
    LogToFile("SocialManagerUserGroupGetTypeCpp is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerUserGroupGetLocalUserCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group{ nullptr };
    uint64_t luaGroup = GetUint64FromLua(L, 1, 0);
    if (luaGroup)
    {
        group = (*socialManagerCppState.groups.find(luaGroup)).second;
    }
    else
    {
        group = (*socialManagerCppState.groups.begin()).second;
    }
    if (group == nullptr)
    {
        LogToFile("SocialManagerUserGroupGetLocalUserCpp: No xbox_social_user_group Loaded");
        return S_OK;
    }

    xbox_live_user_t user = group->local_user();
    LogToFile("SocialManagerUserGroupGetLocalUserCpp: user=%llu", user);
#else
    LogToFile("SocialManagerUserGroupGetLocalUserCpp is disabled on this platform.");

#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerUserGroupGetFiltersCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group{ nullptr };
    uint64_t luaGroup = GetUint64FromLua(L, 1, 0);
    if (luaGroup)
    {
        group = (*socialManagerCppState.groups.find(luaGroup)).second;
    }
    else
    {
        group = (*socialManagerCppState.groups.begin()).second;
    }
    if (group == nullptr)
    {
        LogToFile("SocialManagerUserGroupGetFiltersCpp: No xbox_social_user_group Loaded");
        return S_OK;
    }

    xbox::services::social::manager::presence_filter presenceFilter = group->presence_filter_of_group();
    xbox::services::social::manager::relationship_filter relationshipFilter = group->relationship_filter_of_group();

    LogToFile("SocialManagerUserGroupGetFiltersCpp: presenceFilter=%u, relationshipFilter=%u", presenceFilter, relationshipFilter);
#else
    LogToFile("SocialManagerUserGroupGetFiltersCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerUserGroupGetUsersCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group{ nullptr };
    uint64_t luaGroup = GetUint64FromLua(L, 1, 0);
    if (luaGroup)
    {
        group = (*socialManagerCppState.groups.find(luaGroup)).second;
    }
    else
    {
        group = (*socialManagerCppState.groups.begin()).second;
    }
    if (group == nullptr)
    {
        LogToFile("SocialManagerUserGroupGetUsersCpp: No xbox_social_user_group Loaded");
        return S_OK;
    }

    LogToFile("SocialManagerUserGroupGetUsersCpp: usersCount: %d", group->users().size());
    for (auto user : group->users())
    {
        char buffer[17];
        xbox::services::Utils::Utf8FromCharT(user->gamertag(), buffer, sizeof(buffer));
        LogToFile("\t%s", buffer);
    }
#else
    LogToFile("SocialManagerUserGroupGetUsersCpp is not supported on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerUserGroupGetUsersTrackedByGroupCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group{ nullptr };
    uint64_t luaGroup = GetUint64FromLua(L, 1, 0);
    if (luaGroup)
    {
        group = (*socialManagerCppState.groups.find(luaGroup)).second;
    }
    else
    {
        group = (*socialManagerCppState.groups.begin()).second;
    }
    if (group == nullptr)
    {
        LogToFile("SocialManagerUserGroupGetUsersTrackedByGroupCpp: No xbox_social_user_group Loaded");
        return S_OK;
    }

    LogToFile("SocialManagerUserGroupGetUsersTrackedByGroupCpp trackedUsersCount: %d",  group->users_tracked_by_social_user_group().size());

    for (auto user : group->users_tracked_by_social_user_group())
    {
        LogToFile("\t%s", user.xbox_user_id());
    }
#else
    LogToFile("SocialManagerUserGroupGetUsersTrackByGroupCpp is disabled on this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerAddLocalUserCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    xbox::services::social::manager::social_manager_extra_detail_level extraDetailLevel = ConvertStringToCppSocialManagerExtraDetailLevel(
        GetStringFromLua(L, 1, "social_manager_extra_detail_level::no_extra_detai_T(").c_str());
    LogToFile("SocialManagerAddLocalUserCpp: social_manager_extra_detail_level: %d", extraDetailLevel);

    xbox_live_user_t user = Data()->xalUser;

    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();
    socialManager->add_local_user(user, extraDetailLevel);
    LogToFile("SocialManagerAddLocalUserCpp");
#else
    LogToFile("SocialManagerAddLocalUserCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerRemoveLocalUserCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();
    xbox_live_user_t user = Data()->xalUser;

    socialManager->remove_local_user(user);

    LogToFile("SocialManagerRemoveLocalUserCpp");
#else
    LogToFile("SocialManagerRemoveLocalUserCpp is disabled for this platform.");
#endif
    return LuaReturnHR(L, S_OK);
}

int SocialManagerCreateSocialUserGroupFromFiltersCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    xbox_live_user_t user = Data()->xalUser;
    xbox::services::social::manager::presence_filter presenceFilter = ConvertStringToCppPresenceFilter(GetStringFromLua(L, 1, "presence_filter::al_T(").c_str());
    xbox::services::social::manager::relationship_filter relationshipFilter = ConvertStringToCppRelationshipFilter(GetStringFromLua(L, 2, "relationship_filter::friends").c_str());

    LogToFile("SocialManagerCreateSocialUserGroupFromFiltersCpp: presence_filter: %d", presenceFilter);
    LogToFile("SocialManagerCreateSocialUserGroupFromFiltersCpp: relationship_filter: %d", relationshipFilter);

    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();
    xbox::services::xbox_live_result<std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> result = socialManager->create_social_user_group_from_filters(user, presenceFilter, relationshipFilter);

    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group{ nullptr };
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    if (SUCCEEDED(hr))
    {
        group = result.payload();
        socialManagerCppState.groups.insert(std::pair<uint64_t, std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>>(reinterpret_cast<uint64_t>(group.get()), group));
    }
    lua_pushinteger(L, reinterpret_cast<lua_Integer>(group.get()));

    LogToFile("SocialManagerCreateSocialUserGroupFromFiltersCpp: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr, 1);
#else
    LogToFile("SocialManagerCreateSocialUserGroupFromFiltersCpp is disabled for this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int SocialManagerDestroySocialUserGroupCpp_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group{ nullptr };
    uint64_t luaGroup = GetUint64FromLua(L, 1, 0);
    if (luaGroup)
    {
        group = (*socialManagerCppState.groups.find(luaGroup)).second;
    }
    else
    {
        group = (*socialManagerCppState.groups.begin()).second;
    }
    if (group == nullptr)
    {
        LogToFile("SocialManagerDestroySocialUserGroupCpp: No xbox_social_user_group Loaded");
        return S_OK;
    }

    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();
    xbox::services::xbox_live_result<void> result = socialManager->destroy_social_user_group(group);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    if (SUCCEEDED(hr))
    {
        socialManagerCppState.groups.erase(reinterpret_cast<uint64_t>(group.get()));
    }

    LogToFile("SocialManagerDestroySocialUserGroupCpp: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    LogToFile("SocialManagerDestroySocialUserGroupCpp is disabled for this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int SocialManagerCreateSocialUserGroupFromListCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    // Params:
    // 1) number of xuids to include in list
    // 2) offset in the above vector of first xuid
    uint64_t count{ GetUint64FromLua(L, 1, 10) };
    uint64_t offset{ GetUint64FromLua(L, 2, 0) };

    assert(offset + count <= listXuidStrings.size());

    xbox_live_user_t user = Data()->xalUser;
    std::vector<string_t> xuids
    {
        listXuidStrings.begin() + static_cast<int>(offset),
        listXuidStrings.begin() + static_cast<int>(offset + count)
    };

    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();
    xbox::services::xbox_live_result<std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> result = socialManager->create_social_user_group_from_list(user, xuids);

    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group;
    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
    if (SUCCEEDED(hr))
    {
        group = result.payload();
        socialManagerCppState.groups.insert(std::pair<uint64_t, std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>>(reinterpret_cast<uint64_t>(group.get()), group));
    }

    lua_pushinteger(L, reinterpret_cast<lua_Integer>(group.get()));

    LogToFile("SocialManagerCreateSocialUserGroupFromListCpp: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr, 1);
#else
    LogToFile("SocialManagerCreateSocialUserGroupFromListCpp is disabled on this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int SocialManagerGetLocalUsersCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();

    LogToFile("XblSocialManagerGetLocalUsers localUsersCount: %d", socialManager->local_users().size());

    HRESULT hr = S_OK;
    for (xbox_live_user_t user : socialManager->local_users())
    {
        size_t gamertagSize = XalUserGetGamertagSize(user, XalGamertagComponent_Classic);
        std::vector<char> gamertag(gamertagSize, '\0');

        size_t bufferUsed;
        hr = XalUserGetGamertag(user, XalGamertagComponent_Classic, gamertagSize, gamertag.data(), &bufferUsed);
        if (SUCCEEDED(hr))
        {
            LogToFile("\t%s", gamertag.data());
        }
    }

    LogToFile("SocialManagerGetLocalUsersCpp: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    LogToFile("SocialManagerGetLocalUsersCpp is disabled for this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int SocialManagerUpdateSocialUserGroupCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    // Params:
    // 1) group pointer
    // 1) number of xuids to include in list
    // 2) offset in the above vector of first xuid
    std::shared_ptr<xbox::services::social::manager::xbox_social_user_group> group{ nullptr };
    uint64_t luaGroup = GetUint64FromLua(L, 1, 0);
    if (luaGroup)
    {
        group = (*socialManagerCppState.groups.find(luaGroup)).second;
    }
    else
    {
        group = (*socialManagerCppState.groups.begin()).second;
    }
    if (group == nullptr)
    {
        LogToFile("SocialManagerUpdateSocialUserGroupCpp: No xbox_social_user_group Loaded");
        return S_OK;
    }

    auto count{ GetUint64FromLua(L, 2, 15) };
    auto offset{ GetUint64FromLua(L, 3, 0) };

    // CODE SNIPPET START: XblSocialManagerUpdateSocialUserGroup_C
    std::vector<string_t> xuids
    {
        listXuidStrings.begin() + static_cast<int>(offset),
        listXuidStrings.begin() + static_cast<int>(offset + count)
    };

    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();
    xbox::services::xbox_live_result<void> result = socialManager->update_social_user_group(group, xuids);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());

    LogToFile("SocialManagerUpdateSocialUserGroupCpp: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    LogToFile("SocialManagerUpdateSocialUserGroupCpp is disabled for this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int SocialManagerSetRichPresencePollingStatusCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    xbox_live_user_t user = Data()->xalUser;
    bool shouldEnablePolling = GetBoolFromLua(L, 1, false);
    LogToFile("SocialManagerSetRichPresencePollingStatusCpp: ShouldEnablePolling: %s", shouldEnablePolling ? "true" : "false");

    std::shared_ptr<xbox::services::social::manager::social_manager> socialManager = xbox::services::social::manager::social_manager::get_singleton_instance();
    xbox::services::xbox_live_result<void> result = socialManager->set_rich_presence_polling_status(user, shouldEnablePolling);

    HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());

    LogToFile("SocialManagerSetRichPresencePollingStatusCpp: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    LogToFile("SocialManagerSetRichPresencePollingStatusCpp is disabled for this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

int SocialManagerDoWorkCpp_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    HRESULT hr = SocialManagerDoWorkCpp();
    LogToFile("SocialManagerDoWorkCpp: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
#else
    LogToFile("SocialManagerDoWorkCpp is disabled on this platform.");
    return LuaReturnHR(L, S_OK);
#endif
}

void SetupAPIs_CppSocialManager()
{
    lua_register(Data()->L, "StartSocialManagerDoWorkLoopCpp", StartSocialManagerDoWorkLoopCpp_Lua);
    lua_register(Data()->L, "StopSocialManagerDoWorkLoopCpp", StopSocialManagerDoWorkLoopCpp_Lua);
    lua_register(Data()->L, "SocialManagerPresenceRecordIsUserPlayingTitleCpp", SocialManagerPresenceRecordIsUserPlayingTitleCpp_Lua);
    lua_register(Data()->L, "SocialManagerUserGroupGetTypeCpp", SocialManagerUserGroupGetTypeCpp_Lua);
    lua_register(Data()->L, "SocialManagerUserGroupGetLocalUserCpp", SocialManagerUserGroupGetLocalUserCpp_Lua);
    lua_register(Data()->L, "SocialManagerUserGroupGetFiltersCpp", SocialManagerUserGroupGetFiltersCpp_Lua);
    lua_register(Data()->L, "SocialManagerUserGroupGetUsersCpp", SocialManagerUserGroupGetUsersCpp_Lua);
    lua_register(Data()->L, "SocialManagerUserGroupGetUsersTrackedByGroupCpp", SocialManagerUserGroupGetUsersTrackedByGroupCpp_Lua);
    lua_register(Data()->L, "SocialManagerAddLocalUserCpp", SocialManagerAddLocalUserCpp_Lua);
    lua_register(Data()->L, "SocialManagerRemoveLocalUserCpp", SocialManagerRemoveLocalUserCpp_Lua);
    lua_register(Data()->L, "SocialManagerCreateSocialUserGroupFromFiltersCpp", SocialManagerCreateSocialUserGroupFromFiltersCpp_Lua);
    lua_register(Data()->L, "SocialManagerCreateSocialUserGroupFromListCpp", SocialManagerCreateSocialUserGroupFromListCpp_Lua);
    lua_register(Data()->L, "SocialManagerGetLocalUsersCpp", SocialManagerGetLocalUsersCpp_Lua);
    lua_register(Data()->L, "SocialManagerUpdateSocialUserGroupCpp", SocialManagerUpdateSocialUserGroupCpp_Lua);
    lua_register(Data()->L, "SocialManagerSetRichPresencePollingStatusCpp", SocialManagerSetRichPresencePollingStatusCpp_Lua);
    lua_register(Data()->L, "SocialManagerDoWorkCpp", SocialManagerDoWorkCpp_Lua);
    lua_register(Data()->L, "SocialManagerDestroySocialUserGroupCpp", SocialManagerDestroySocialUserGroupCpp_Lua);
}