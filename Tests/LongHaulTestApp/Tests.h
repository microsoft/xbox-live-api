// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class TestsManager
{
public:
    void StartTests(xbl_context_handle xboxContext);

private:
    async_queue_handle_t m_queue;
    async_queue_handle_t m_queue2;
    uint32_t m_asyncQueueCallbackToken;

    struct ThreadContext
    {
        xbl_context_handle xblContext;
        async_queue_handle_t queue;
    };
    ThreadContext* m_context;

    static DWORD WINAPI TestsThreadProc(LPVOID lpParam);
    static DWORD WINAPI BackgroundWorkThreadProc(LPVOID lpParam);
};

class Tests : public std::enable_shared_from_this<Tests>
{
public:
    static uint32_t TestDelay;
    static uint64_t TestsRun;

    ///////////////////////////////////////
    //////       Test Framework      //////
    ///////////////////////////////////////

    // Methods
    Tests(xbl_context_handle xboxLiveContext, async_queue_handle_t queue, bool runSocialManagerTests);
    ~Tests();
    void HandleTests();

    void Log(std::wstring log, bool showOnUI = true);
    void Log(std::string log, bool showOnUI = true);

private:
    void RunTests();

    // Vars
    bool m_testsStarted;
    time_t m_time;

    xbl_user_handle m_user;
    uint64_t m_xuid;
    xbl_context_handle m_xboxLiveContext;
    async_queue_handle_t m_queue;
    const XblAppConfig* m_config;

    bool m_runSocialManagerTests;
    
    ///////////////////////////////////////
    //////        Achievements       //////
    ///////////////////////////////////////

    // Tests
    void TestAchievementsFlow();

    // Utils
    void GetAchievmentsForTitle();
    void AchievementResultsGetNext(xbl_achievements_result_handle result);
    void GetAchievement(PCSTR scid, PCSTR achievementId);
    void UpdateAchievement(PCSTR scid, PCSTR achievementId);

    // Vars
    uint32_t m_progress;

    ///////////////////////////////////////
    //////           Profile         //////
    ///////////////////////////////////////

    // Tests
    void TestProfileFlow();

    // Utils
    void TestGetUserProfile();
    void TestGetUserProfiles();
    void TestGetUserProfilesForSocialGroup();

    // Vars
    uint32_t m_test;

    ///////////////////////////////////////
    //////           Social          //////
    ///////////////////////////////////////

    // Tests
    void TestSocialFlow();

    // Utils
    void GetSocialRelationship();
    void SocialRelationshipGetNext(xbl_social_relationship_result_handle relationshipResult);
    void TestResputationFeedback();

    ///////////////////////////////////////
    //////       Social Manager      //////
    ///////////////////////////////////////

    // Tests
    void TestSocialManagerFlow();

    // Test Utils
    void AddLocalUserToSocialManager();
    void RemoveLocalUserFromSocialManager();
    void CreateSocialUserGroup();
    void TestSocialUserGroup();

    // Manager Utils
    void SocialManagerIntegrationUpdate();
    void NextSocialManagerTestConfig();

    void WaitForSocialEvent(XblSocialManagerEventType eventType, std::function<void(XblSocialManagerEvent)> callback);

    void PrintSocialManagerDoWork(XblSocialManagerEvent* events, uint32_t size);
    string GroupTypeToString();
    string SocialEventTypeToString(XblSocialManagerEventType eventType);
    string PresenceFilterToString();
    string RelationshipFilterToString();

    // Vars
    XblSocialUserGroupType m_testGroupType;

    XblPresenceFilter m_presenceFilter = XblPresenceFilter_TitleOnline;
    XblRelationshipFilter m_relationshipFilter;
    XblSocialManagerUserGroup* m_socialUserGroup;

    bool m_waitingForEvent;
    XblSocialManagerEventType m_eventType;
    std::function<void(XblSocialManagerEvent)> m_eventTypeCallback;
};