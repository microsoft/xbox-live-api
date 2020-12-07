// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include <atomic>

static struct SocialManagerState
{
    // TODO move doWork logic into this state class
    SocialManagerState() = default;
    ~SocialManagerState()
    {
        // Validate that our tests cleaned up correctly
        assert(!doWork);
        assert(groups.empty());
    }

    std::set<XblSocialManagerUserGroupHandle> groups;
    std::thread doWorkThread{};
    std::atomic<bool> doWork{ false };
    std::atomic<bool> doWorkJoinWhenDone{ false };
} state;

HRESULT SocialManagerDoWork()
{
    // CODE SNIPPET START: XblSocialManagerDoWork_C
    const XblSocialManagerEvent* events{ nullptr };
    size_t eventCount{ 0 };
    HRESULT hr = XblSocialManagerDoWork(&events, &eventCount);
    if (SUCCEEDED(hr))
    {
        for (size_t i = 0; i < eventCount; i++)
        {
            // Act on the event
            auto& socialEvent = events[i];
            // CODE SKIP START
            static std::unordered_map<XblSocialManagerEventType, std::string> eventTypesMap
            {
                { XblSocialManagerEventType::UsersAddedToSocialGraph, "UsersAddedToSocialGraph" },
                { XblSocialManagerEventType::UsersRemovedFromSocialGraph, "UsersRemovedFromSocialGraph" },
                { XblSocialManagerEventType::PresenceChanged, "PresenceChanged" },
                { XblSocialManagerEventType::ProfilesChanged, "ProfilesChanged" },
                { XblSocialManagerEventType::SocialRelationshipsChanged, "SocialRelationshipsChanged" },
                { XblSocialManagerEventType::LocalUserAdded, "LocalUserAdded" },
                { XblSocialManagerEventType::SocialUserGroupLoaded, "SocialUserGroupLoaded" },
                { XblSocialManagerEventType::SocialUserGroupUpdated, "SocialUserGroupUpdated" },
                { XblSocialManagerEventType::UnknownEvent, "UnknownEvent" }
            };
            // CODE SKIP END
            std::stringstream ss;
            ss << "XblSocialManagerDoWork: Event of type " << eventTypesMap[socialEvent.eventType] << std::endl;
            for (size_t j = 0; j < XBL_SOCIAL_MANAGER_MAX_AFFECTED_USERS_PER_EVENT; j++)
            {
                if (socialEvent.usersAffected[j] != nullptr)
                {
                    if (j == 0)
                    {
                        ss << "Users affected: " << std::endl;
                    }
                    ss << "\t" << socialEvent.usersAffected[j]->gamertag << std::endl;
                }
            }
            LogToFile(ss.str().c_str());
            // CODE SKIP START
            switch (socialEvent.eventType)
            {
            case XblSocialManagerEventType::UsersAddedToSocialGraph:
                LogToFile("XblSocialManagerDoWork: UsersAddedToSocialGraph event");
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_UsersAddedToSocialGraphEvent");
                break;
            case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
                LogToFile("XblSocialManagerDoWork: UsersRemovedFromSocialGraph event");
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_UsersRemovedFromSocialGraphEvent");
                break;
            case XblSocialManagerEventType::PresenceChanged:
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_PresenceChangedEvent");
                break;
            case XblSocialManagerEventType::ProfilesChanged:
                LogToFile("XblSocialManagerDoWork: ProfilesChanged event");
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_ProfilesChangedEvent");
                break;
            case XblSocialManagerEventType::SocialRelationshipsChanged:
                LogToFile("XblSocialManagerDoWork: SocialRelationshipsChanged event");
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_SocialRelationshipsChangedEvent");
                break;
            case XblSocialManagerEventType::LocalUserAdded:
                LogToFile("XblSocialManagerDoWork: LocalUserAdded event");
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_LocalUserAddedEvent");
                break;
            case XblSocialManagerEventType::SocialUserGroupLoaded:
                LogToFile("XblSocialManagerDoWork: SocialUserGroupLoaded event");
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_SocialUserGroupLoadedEvent");
                break;
            case XblSocialManagerEventType::SocialUserGroupUpdated:
                LogToFile("XblSocialManagerDoWork: SocialUserGroupUpdated event");
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_SocialUserGroupUpdatedEvent");
                break;
            case XblSocialManagerEventType::UnknownEvent:
            default:
                LogToFile("XblSocialManagerDoWork: Unknown event");
                CallLuaFunctionWithHr(hr, "OnXblSocialManagerDoWork_UnknownEvent");
                break;
            }
            // CODE SKIP END
        }
    }
    // CODE SNIPPET END
    return hr;
}

int StartSocialManagerDoWorkLoop_Lua(lua_State* L)
{
    state.doWork = true;
    state.doWorkJoinWhenDone = true;
    std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
    state.doWorkThread = std::thread([]()
    {
        Data()->m_socialDoWorkDone = false;
        while (state.doWork && !Data()->m_quit)
        {
            {
                std::lock_guard<std::recursive_mutex> lock(Data()->m_luaLock);
                SocialManagerDoWork();
            }
            pal::Sleep(10);
        }
        Data()->m_socialDoWorkDone = true;
        LogToFile("Exiting do work thread");
    });
    return LuaReturnHR(L, S_OK);
}

int StopSocialManagerDoWorkLoop_Lua(lua_State* L)
{
    LogToFile("StopSocialManagerDoWorkLoop_Lua");
    state.doWorkJoinWhenDone = true;
    state.doWork = false;

#if ENABLE_PERF_PROFILING
    LogToFile(xbox::services::detail::PerfTester::Instance().FormatStats().data());
#endif
    return LuaReturnHR(L, S_OK);
}

void StopSocialManagerDoWorkHelper()
{
    if (state.doWorkJoinWhenDone)
    {
        state.doWork = false;
        state.doWorkJoinWhenDone = false;
        state.doWorkThread.join();
    }
}

XblSocialManagerExtraDetailLevel ConvertStringToXblSocialManagerExtraDetailLevel(const char* str)
{
    XblSocialManagerExtraDetailLevel detailLevel = XblSocialManagerExtraDetailLevel::NoExtraDetail;

    if (pal::stricmp(str, "XblSocialManagerExtraDetailLevel::TitleHistoryLevel") == 0) detailLevel = XblSocialManagerExtraDetailLevel::TitleHistoryLevel;
    else if (pal::stricmp(str, "XblSocialManagerExtraDetailLevel::PreferredColorLevel") == 0) detailLevel = XblSocialManagerExtraDetailLevel::PreferredColorLevel;
    else if (pal::stricmp(str, "XblSocialManagerExtraDetailLevel::All") == 0) detailLevel = XblSocialManagerExtraDetailLevel::All;

    return detailLevel;
}

XblPresenceFilter ConvertStringToXblPresenceFilter(const std::string& filterString)
{
    XblPresenceFilter filter = XblPresenceFilter::Unknown;
    auto str{ filterString.data() };

    if (pal::stricmp(str, "XblPresenceFilter::TitleOnline") == 0) filter = XblPresenceFilter::TitleOnline;
    else if (pal::stricmp(str, "XblPresenceFilter::TitleOffline") == 0) filter = XblPresenceFilter::TitleOffline;
#if XSAPI_BUILT_FROM_SOURCE
    else if (pal::stricmp(str, "XblPresenceFilter::TitleOnlineOutsideTitle") == 0) filter = XblPresenceFilter::TitleOnlineOutsideTitle;
#endif
    else if (pal::stricmp(str, "XblPresenceFilter::AllOnline") == 0) filter = XblPresenceFilter::AllOnline;
    else if (pal::stricmp(str, "XblPresenceFilter::AllOffline") == 0) filter = XblPresenceFilter::AllOffline;
    else if (pal::stricmp(str, "XblPresenceFilter::AllTitle") == 0) filter = XblPresenceFilter::AllTitle;
    else if (pal::stricmp(str, "XblPresenceFilter::All") == 0) filter = XblPresenceFilter::All;

    return filter;
}

XblRelationshipFilter ConvertStringToXblRelationshipFilter(const std::string& filterString)
{
    XblRelationshipFilter filter = XblRelationshipFilter::Friends;
    auto str{ filterString.data() };

    if (pal::stricmp(str, "XblRelationshipFilter::Friends") == 0) filter = XblRelationshipFilter::Friends;
    else if (pal::stricmp(str, "XblRelationshipFilter::Favorite") == 0) filter = XblRelationshipFilter::Favorite;

    return filter;
}

// commands

int XblSocialManagerPresenceRecordIsUserPlayingTitle_Lua(lua_State *L)
{
    auto group{ reinterpret_cast<XblSocialManagerUserGroupHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(*state.groups.begin()))) };
    uint32_t titleId = GetUint32FromLua(L, 1, 174925616);

    if (group == nullptr)
    {
        LogToFile("XblSocialManagerPresenceRecordIsUserPlayingTitle: No XblSocialManagerUserGroup Loaded");
        return S_OK;
    }

    XblSocialManagerUserPtrArray users{ nullptr };
    size_t count{ 0 };
    HRESULT result = XblSocialManagerUserGroupGetUsers(group, &users, &count);

    bool playingTitle{ false };
    if (SUCCEEDED(result) && count > 0)
    {
        XblSocialManagerPresenceRecord presenceRecord = users[0]->presenceRecord;
        // CODE SNIPPET START: XblSocialManagerPresenceRecordIsUserPlayingTitle
        playingTitle = XblSocialManagerPresenceRecordIsUserPlayingTitle(&presenceRecord, Data()->titleId);
        // CODE SNIPPET END

        LogToFile("XblSocialManagerPresenceRecordIsUserPlayingTitle: TitleId: %d, playing: %u", titleId, playingTitle);
    }

    LogToFile("XblSocialManagerPresenceRecordIsUserPlayingTitle: hr=%s", ConvertHR(result).c_str());
    return LuaReturnHR(L, result);
}

int XblSocialManagerUserGroupGetType_Lua(lua_State* L)
{
    auto group{ reinterpret_cast<XblSocialManagerUserGroupHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(*state.groups.begin()))) };

    if (group == nullptr)
    {
        LogToFile("XblSocialManagerUserGroupGetUsers: No XblSocialManagerUserGroup Loaded");
        return S_OK;
    }

    // CODE SNIPPET START: XblSocialManagerUserGroupGetType
    XblSocialUserGroupType type{ XblSocialUserGroupType::FilterType };
    HRESULT hr = XblSocialManagerUserGroupGetType(group, &type);
    // CODE SNIPPET END

    LogToFile("XblSocialManagerUserGroupGetType: type=%u, hr=%s", type, ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialManagerUserGroupGetLocalUser_Lua(lua_State* L)
{
    auto group{ reinterpret_cast<XblSocialManagerUserGroupHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(*state.groups.begin()))) };

    // CODE SNIPPET START: XblSocialManagerUserGroupGetLocalUser
    XblUserHandle user{ nullptr };
    HRESULT hr = XblSocialManagerUserGroupGetLocalUser(group, &user);
    // CODE SNIPPET END

    LogToFile("XblSocialManagerUserGroupGetLocalUser: user=%llu, hr=%s", user, ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialManagerUserGroupGetFilters_Lua(lua_State* L)
{
    auto group{ reinterpret_cast<XblSocialManagerUserGroupHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(*state.groups.begin()))) };

    // CODE SNIPPET START: XblSocialManagerUserGroupGetFilters
    XblPresenceFilter presenceFilter{ XblPresenceFilter::Unknown };
    XblRelationshipFilter relationshipFilter{ XblRelationshipFilter::Unknown };
    HRESULT hr = XblSocialManagerUserGroupGetFilters(group, &presenceFilter, &relationshipFilter);
    // CODE SNIPPET END

    LogToFile("XblSocialManagerUserGroupGetFilters: presenceFilter=%u, relationshipFilter=%u, hr=%s", presenceFilter, relationshipFilter, ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblSocialManagerUserGroupGetUsers_Lua(lua_State *L)
{
    auto group{ reinterpret_cast<XblSocialManagerUserGroupHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(*state.groups.begin()))) };

    if (group == nullptr)
    {
        LogToFile("XblSocialManagerUserGroupGetUsers: No XblSocialManagerUserGroup Loaded");
        return S_OK;
    }

    // CODE SNIPPET START: XblSocialManagerUserGroupGetUsers_C
    XblSocialManagerUserPtrArray users{ nullptr };
    size_t userCount{ 0 };
    HRESULT hr = XblSocialManagerUserGroupGetUsers(group, &users, &userCount);
    LogToFile("XblSocialManagerUserGroupGetUsers: %s usersCount: %d", ConvertHR(hr).c_str(), userCount); // CODE SNIP SKIP

    for (size_t i = 0; i < userCount; ++i)
    {
        // Display user info etc.
        // CODE SKIP START
        LogToFile("\t%s", users[i]->gamertag);
        // CODE SKIP END
    }
    // CODE SNIPPET END

    return LuaReturnHR(L, hr);
}

int XblSocialManagerUserGroupGetUsersTrackedByGroup_Lua(lua_State *L)
{
    auto group{ reinterpret_cast<XblSocialManagerUserGroupHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(*state.groups.begin()))) };

    if (group == nullptr)
    {
        LogToFile("XblSocialManagerUserGroupGetUsersTrackedByGroup: No XblSocialManagerUserGroup Loaded");
        return S_OK;
    }

    // CODE SNIPPET START: XblSocialManagerUserGroupGetUsersTrackedByGroup
    const uint64_t* xuids{ nullptr };
    size_t count{ 0 };

    HRESULT hr = XblSocialManagerUserGroupGetUsersTrackedByGroup(
        group,
        &xuids,
        &count
    );
    // CODE SNIPPET END

    LogToFile("XblSocialManagerUserGroupGetUsersTrackedByGroup: %s trackedUsersCount: %d", ConvertHR(hr).c_str(), count);

    for (size_t i = 0; i < count; ++i)
    {
        LogToFile("\t%llu", static_cast<unsigned long long>(xuids[i]));
    }
    LogToFile("XblSocialManagerUserGroupGetUsersTrackedByGroup: %s", ConvertHR(hr).c_str());

    return LuaReturnHR(L, hr);
}

int XblSocialManagerAddLocalUser_Lua(lua_State *L)
{
    XblSocialManagerExtraDetailLevel extraLevelDetail = ConvertStringToXblSocialManagerExtraDetailLevel(
        GetStringFromLua(L, 1, "XblSocialManagerExtraDetailLevel::NoExtraDetail").c_str());
    LogToFile("XblSocialManagerAddLocalUser: ExtraLevelDetail: %d", extraLevelDetail);

    XalUserHandle user = Data()->xalUser;
    // CODE SNIPPET START: XblSocialManagerAddLocalUser
    HRESULT hr = XblSocialManagerAddLocalUser(user, extraLevelDetail, nullptr);
    // CODE SNIPPET END

    LogToFile("XblSocialManagerAddLocalUser: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialManagerRemoveLocalUser_Lua(lua_State *L)
{
    XalUserHandle user = Data()->xalUser;
    // CODE SNIPPET START: XblSocialManagerRemoveLocalUser_C
    HRESULT hr = XblSocialManagerRemoveLocalUser(user);
    // CODE SNIPPET END

    LogToFile("XblSocialManagerRemoveLocalUser: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, S_OK);
}

int XblSocialManagerCreateSocialUserGroupFromFilters_Lua(lua_State *L)
{
    XalUserHandle user = Data()->xalUser;

    // CODE SNIPPET START: XblSocialManagerCreateSocialUserGroupFromFilters_C
    XblPresenceFilter presenceFilter{ XblPresenceFilter::All };
    XblRelationshipFilter relationshipFilter{ XblRelationshipFilter::Friends };
    // CODE SKIP START
    XblPresenceFilter presenceFilterArg = ConvertStringToXblPresenceFilter(GetStringFromLua(L, 1, std::string{}));
    if (presenceFilter != XblPresenceFilter::Unknown)
    {
        presenceFilter = presenceFilterArg;
    }
    XblRelationshipFilter relationshipFilterArg = ConvertStringToXblRelationshipFilter(GetStringFromLua(L, 2, std::string{}));
    if (relationshipFilterArg != XblRelationshipFilter::Unknown)
    {
        relationshipFilter = relationshipFilterArg;
    }

    LogToFile("XblSocialManagerCreateSocialUserGroupFromFilters: PresenceFilter: %d", presenceFilter);
    LogToFile("XblSocialManagerCreateSocialUserGroupFromFilters: RelationshipFilter: %d", relationshipFilter);
    // CODE SKIP END

    XblSocialManagerUserGroupHandle groupHandle{ nullptr };
    HRESULT hr = XblSocialManagerCreateSocialUserGroupFromFilters(user, presenceFilter, relationshipFilter, &groupHandle);

    if (SUCCEEDED(hr))
    {
        state.groups.insert(groupHandle);
    }
    // CODE SNIPPET END

    lua_pushinteger(L, reinterpret_cast<lua_Integer>(groupHandle));

    LogToFile("XblSocialManagerCreateSocialUserGroupFromFilters: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr, 1);
}

int XblSocialManagerDestroySocialUserGroup_Lua(lua_State* L)
{
    auto groupHandle{ reinterpret_cast<XblSocialManagerUserGroupHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(*state.groups.begin()))) };

    // CODE SNIPPET START: XblSocialManagerDestroySocialUserGroup_C
    HRESULT hr = XblSocialManagerDestroySocialUserGroup(groupHandle);
    if (SUCCEEDED(hr))
    {
        state.groups.erase(groupHandle);
    }
    // CODE SNIPPET END

    LogToFile("XblSocialManagerDestroySocialUserGroup: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

// Pool of XDKS.1 xuids to create social groups from
std::vector<uint64_t> listXuids
{
    2814639011617876,2814641789541994,2814644008675844,2814644210052185,2814645164579523,2814646075485729,2814649783195402,2814650260879943,
    2814652370182940,2814652714045777,2814654391560620,2814654975417728,2814656000993855,2814660006763195,2814666715930430,2814667316080600,
    2814669550092398,2814669684179632,2814669733667211,2814671180786692,2814679901432274,2814613501048225,2814614352529204,2814615856126401,
    2814616641363830,2814617883586813,2814618053453081,2814629752527080,2814631255161151,2814632477267887,2814633284389038,2814635732495522,
    2814635779785472,2814635974475208,2814636979708499,2814618092438397,2814618260480530,2814618319551907,2814619559360314,2814620368929739,
    2814620769042115,2814621007349381,2814623088399025,2814623825448960,2814624220291971,2814624961587858,2814626394212372,2814626639518570,
    2814628203722867,2814629143923154,2814614382301082,2814614959737919,2814615558140392,2814618401629514,2814618701087902,2814619300882392,
    2814623785189962,2814623956387698,2814625066090704,2814625471782204,2814626946705530,2814627006318591,2814628046127456,2814631487749991,
    2814631517599783,2814632798310691,2814633582140204,2814634204785789,2814634895412664,2814635439049207,2814638609354868,2814639589885754,
    2814641670947751,2814643512602566,2814646137630843,2814648499394446,2814651465227139,2814652150012664,2814653926747608,2814655098938516,
    2814655264861214,2814655417678099,2814655883565306,2814656031821923,2814656159501072,2814656780954834,2814660657970845,2814661604435490,
    2814663444319727,2814663818015575,2814665274839967,2814667273133504,2814670761542037,2814672762886609,2814673772488023,2814674096344056,
    2814674229538758,2814678943953289,2814680898042782
};

int XblSocialManagerCreateSocialUserGroupFromList_Lua(lua_State *L)
{
    // Params:
    // 1) number of xuids to include in list
    // 2) offset in the above vector of first xuid
    auto count{ GetUint64FromLua(L, 1, 10) };
    auto offset{ GetUint64FromLua(L, 2, 0) };

    assert(offset + count <= listXuids.size());

    XalUserHandle user = Data()->xalUser;
    // CODE SNIPPET START: XblSocialManagerCreateSocialUserGroupFromList_C
    std::vector<uint64_t> xuids
    {
        listXuids.begin() + static_cast<int>(offset),
        listXuids.begin() + static_cast<int>(offset + count) 
    };

    XblSocialManagerUserGroupHandle groupHandle{ nullptr };
    HRESULT hr = XblSocialManagerCreateSocialUserGroupFromList(user, xuids.data(), xuids.size(), &groupHandle);

    if (SUCCEEDED(hr))
    {
        state.groups.insert(groupHandle);
    }
    // CODE SNIPPET END

    lua_pushinteger(L, reinterpret_cast<lua_Integer>(groupHandle));

    LogToFile("XblSocialManagerCreateSocialUserGroupFromList: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr, 1);
}

int XblSocialManagerGetLocalUsers_Lua(lua_State *L)
{
    // CODE SNIPPET START: XblSocialManagerGetLocalUsers
    size_t localUsersCount = XblSocialManagerGetLocalUserCount();
    std::vector<XblUserHandle> localUsers(localUsersCount, nullptr);
    HRESULT hr = XblSocialManagerGetLocalUsers(localUsersCount, &localUsers[0]);
    // CODE SNIPPET END

    LogToFile("XblSocialManagerGetLocalUsers: %s localUsersCount: %d", ConvertHR(hr).c_str(), localUsersCount);

    for (uint32_t i = 0; i < localUsersCount; i++)
    {
        size_t gamerTagSize = XalUserGetGamertagSize(localUsers[i], XalGamertagComponent_Classic);
        std::vector<char> gamerTag(gamerTagSize, '\0');

        size_t bufferUsed;
        hr = XalUserGetGamertag(localUsers[i], XalGamertagComponent_Classic, gamerTagSize, gamerTag.data(), &bufferUsed);
        if (SUCCEEDED(hr))
        {
            LogToFile("\t%s", gamerTag.data());
        }
    }

    LogToFile("XblSocialManagerGetLocalUsers: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialManagerUpdateSocialUserGroup_Lua(lua_State *L)
{
    // Params:
    // 1) group pointer
    // 1) number of xuids to include in list
    // 2) offset in the above vector of first xuid
    auto group{ reinterpret_cast<XblSocialManagerUserGroupHandle>(GetUint64FromLua(L, 1, reinterpret_cast<uint64_t>(*state.groups.begin()))) };
    auto count{ GetUint64FromLua(L, 2, 15) };
    auto offset{ GetUint64FromLua(L, 3, 0) };

    // CODE SNIPPET START: XblSocialManagerUpdateSocialUserGroup_C
    std::vector<uint64_t> xuids
    { 
        listXuids.begin() + static_cast<int>(offset),
        listXuids.begin() + static_cast<int>(offset + count)
    };

    HRESULT hr = XblSocialManagerUpdateSocialUserGroup(group, xuids.data(), xuids.size());
    // CODE SNIPPET END

    LogToFile("XblSocialManagerUpdateSocialUserGroup: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialManagerSetRichPresencePollingStatus_Lua(lua_State *L)
{
    bool shouldEnablePolling = GetBoolFromLua(L, 1, false);
    LogToFile("XblSocialManagerSetRichPresencePollingStatus: ShouldEnablePolling: %s", shouldEnablePolling ? "true" : "false");

    XalUserHandle user = Data()->xalUser;
    // CODE SNIPPET START: XblSocialManagerSetRichPresencePollingStatus
    HRESULT hr = XblSocialManagerSetRichPresencePollingStatus(user, shouldEnablePolling);
    // CODE SNIPPET END

    LogToFile("XblSocialManagerSetRichPresencePollingStatus: %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblSocialManagerDoWork_Lua(lua_State *L)
{
    HRESULT hr = SocialManagerDoWork();
    LogToFile("XblSocialManagerDoWork: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

void SetupAPIs_XblSocialManager()
{
    // Non XSAPI APIs
    lua_register(Data()->L, "StartSocialManagerDoWorkLoop", StartSocialManagerDoWorkLoop_Lua);
    lua_register(Data()->L, "StopSocialManagerDoWorkLoop", StopSocialManagerDoWorkLoop_Lua);

    // XSAPI APIs
    lua_register(Data()->L, "XblSocialManagerPresenceRecordIsUserPlayingTitle", XblSocialManagerPresenceRecordIsUserPlayingTitle_Lua);
    lua_register(Data()->L, "XblSocialManagerUserGroupGetType", XblSocialManagerUserGroupGetType_Lua);
    lua_register(Data()->L, "XblSocialManagerUserGroupGetLocalUser", XblSocialManagerUserGroupGetLocalUser_Lua);
    lua_register(Data()->L, "XblSocialManagerUserGroupGetFilters", XblSocialManagerUserGroupGetFilters_Lua);
    lua_register(Data()->L, "XblSocialManagerUserGroupGetUsers", XblSocialManagerUserGroupGetUsers_Lua);
    lua_register(Data()->L, "XblSocialManagerUserGroupGetUsersTrackedByGroup", XblSocialManagerUserGroupGetUsersTrackedByGroup_Lua);
    lua_register(Data()->L, "XblSocialManagerAddLocalUser", XblSocialManagerAddLocalUser_Lua);
    lua_register(Data()->L, "XblSocialManagerRemoveLocalUser", XblSocialManagerRemoveLocalUser_Lua);
    lua_register(Data()->L, "XblSocialManagerCreateSocialUserGroupFromFilters", XblSocialManagerCreateSocialUserGroupFromFilters_Lua);
    lua_register(Data()->L, "XblSocialManagerCreateSocialUserGroupFromList", XblSocialManagerCreateSocialUserGroupFromList_Lua);
    lua_register(Data()->L, "XblSocialManagerGetLocalUsers", XblSocialManagerGetLocalUsers_Lua);
    lua_register(Data()->L, "XblSocialManagerUpdateSocialUserGroup", XblSocialManagerUpdateSocialUserGroup_Lua);
    lua_register(Data()->L, "XblSocialManagerSetRichPresencePollingStatus", XblSocialManagerSetRichPresencePollingStatus_Lua);
    lua_register(Data()->L, "XblSocialManagerDoWork", XblSocialManagerDoWork_Lua);
    lua_register(Data()->L, "XblSocialManagerDestroySocialUserGroup", XblSocialManagerDestroySocialUserGroup_Lua);
}

