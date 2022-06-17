// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "xsapi-c/social_manager_c.h"
#include "xsapi-cpp/social_manager.h"
#include "social_manager_internal.h"

using namespace xbox::services::presence;
using namespace xbox::services::real_time_activity;
using namespace xbox::services::social::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char defaultPeoplehubTemplate[] = R"(
{
    "xuid": "1",
    "isFavorite": false,
    "isFollowingCaller": true,
    "isFollowedByCaller": true,
    "isIdentityShared": false,
    "displayName": "TestGamerTag",
    "realName": "",
    "displayPicRaw": "http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png",
    "useAvatar": false,
    "gamertag": "TestGamerTag",
    "modernGamertag": "TestGamerTag",
    "modernGamertagSuffix": "",
    "uniqueModernGamertag": "TestGamerTag",
    "gamerScore": "9001",
    "presenceState": "Online",
    "presenceDevices": null,
    "isBroadcasting": false,
    "titleHistory": 
    {
        "titleName": "Forza Horizon 2",
        "titleId": "1234",
        "lastTimePlayed": "2015-01-26T22:54:54.6630000Z",
        "lastTimePlayedText": "8 months ago"
    },
    "suggestion": null,
    "multiplayerSummary": {
        "InMultiplayerSession": 0,
        "InParty": 0
    },
    "recentPlayer": null,
    "follower": null,
    "preferredColor": {
        "primaryColor": "193e91",
        "secondaryColor": "2458cf",
        "tertiaryColor": "122e6b"
    },
    "titlePresence": null,
    "titleSummaries": null,
    "presenceDetails": [{
        "IsBroadcasting": false,
        "Device": "PC",
        "State": "Active",
        "TitleId": "1234",
        "PresenceText": "Home"
    }]
    }
)";

const char onlinePresenceResponseTemplate[] = R"(
{
    "xuid": "1",
    "state": "Online",
    "devices": [
        {
            "type": "PC",
            "titles": [
                {
                    "id": "1234",
                    "name": "awesomeGame",
                    "lastModified": "2013-02-01T00:00:00Z",
                    "state": "active",
                    "placement": "Full",
                    "activity": {
                        "richPresence": "Home"
                    }
                }
            ]
        }
    ]
})";

const char offlinePresenceResponseTemplate[] = R"(
{
    "xuid": "1",
    "state": "Offline",
    "devices": []
}
)";

const char* presenceOnlineRtaMessageSubscribeComplete = R"({"xuid":"2814613569642996","state":"Online","devices":[{"type":"MCapensis","titles":[{"id":"1234","name":"Default Title","placement":"Full","state":"Active", "activity": {"richPresence":"Home"}, "lastModified":"2016-09-30T00:15:35.5994615Z"}]}]})";
const char* presenceOfflineRtaMessageSubscribeComplete = R"({"xuid":"2814613569642996","state":"Offline"})";

#define NUM_USERS 100

DEFINE_TEST_CLASS(SocialManagerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(SocialManagerTests);

private:
    // RAII class for basic SocialManager Test Setup/teardown including relevant mocks
    class SMTestEnvironment : public TestEnvironment
    {
    public:
        SMTestEnvironment() noexcept
        {
            FollowedXuids.reserve(NUM_USERS);
            for (uint64_t i = 0; i < NUM_USERS; ++i)
            {
                FollowedXuids.push_back(i + 1);
            }

            MockRtaService().SetSubscribeHandler(RTASubscribeHandler);

            // Setup default presence and peoplehub mocks
            SetPeoplehubMock();
            SetPresenceMock();
        }

        ~SMTestEnvironment() noexcept
        {
            // Remove all local users
            size_t userCount = XblSocialManagerGetLocalUserCount();
            auto users{ new XblUserHandle[userCount] };
            VERIFY_SUCCEEDED(XblSocialManagerGetLocalUsers(userCount, users));

            for (size_t i = 0; i < userCount; ++i)
            {
                VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(users[i]));
            }

#if ENABLE_PERF_PROFILING
            TEST_LOG(Utils::StringTFromUtf8(detail::PerfTester::Instance().FormatStats().data()).data());
#endif
        }

        // CPP helpers wrapping SocialManager APIs

        // Add local user and validate the expected events are received
        void AddLocalUser(const User& user) const noexcept
        {
            LOGS_DEBUG << "Adding User to SocialManager and awaiting LocalUserAdded and RTA subscriptions";

            // As a testing convenience, await sub handshakes to complete so that tests can fire
            // RTA events and be sure that the social graph is prepared to respond to them
            struct RTASubResponder
            {
                RTASubResponder(size_t followeeCount)
                {
                    auto& rtaService{ system::MockRealTimeActivityService::Instance() };
                    rtaService.SetSubscribeHandler([=](uint32_t n, xsapi_internal_string uri)
                    {
                        SMTestEnvironment::RTASubscribeHandler(n, uri);
                        if (uri.find("https://userpresence.xboxlive.com") != xsapi_internal_string::npos)
                        {
                            ++presenceSubsComplete;
                        }
                        else if (uri.find("http://social.xboxlive.com") != xsapi_internal_string::npos)
                        {
                            socialRelationshipSubComplete = true;
                        }

                        // Graph will create two presence subscriptions for each followed User (Device & title presence)
                        if (presenceSubsComplete >= followeeCount * 2 && socialRelationshipSubComplete)
                        {
                            complete.Set();
                        }
                    });
                }

                ~RTASubResponder()
                {
                    // Reset to default handler
                    system::MockRealTimeActivityService::Instance().SetSubscribeHandler(SMTestEnvironment::RTASubscribeHandler);
                }

                size_t presenceSubsComplete{ 0 };
                bool socialRelationshipSubComplete{ false };
                Event complete{};
            } rtaResponder{ FollowedXuids.size() };

            VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(user.Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));
            AwaitEvents({ {XblSocialManagerEventType::LocalUserAdded, 1} });
            rtaResponder.complete.Wait();
        }

        std::vector<const XblSocialManagerEvent*> DoWork() const noexcept
        {
            const XblSocialManagerEvent* events{ nullptr };
            size_t eventCount{ 0 };
            HRESULT hr = XblSocialManagerDoWork(&events, &eventCount);
            if (FAILED(hr))
            {
                VERIFY_FAIL(); // avoiding log spam
            }

            auto eventsVector = Utils::Transform<const XblSocialManagerEvent*>(events, eventCount, [](const XblSocialManagerEvent& e) { return &e; });
            LogEvents(eventsVector);
            return eventsVector;
        }

        // DoWork until the specified events happen.
        void AwaitEvents(std::map<XblSocialManagerEventType, size_t> expectedEvents) const noexcept
        {
            while (!expectedEvents.empty())
            {
                auto events{ DoWork() };
                for (auto event : events)
                {
                    auto iter{ expectedEvents.find(event->eventType) };
                    if (iter == expectedEvents.end())
                    {
                        LOGS_DEBUG << "Unexpected SocialManager Event";
                        VERIFY_FAIL();
                    }
                    else if(--iter->second == 0)
                    {
                        expectedEvents.erase(iter);
                    }
                }
            }
        }

        std::vector<const XblSocialManagerUser*> GetUsers(
            XblSocialManagerUserGroupHandle group
        ) const noexcept
        {
            const XblSocialManagerUser* const* users{ nullptr };
            size_t userCount{ 0 };
            VERIFY_SUCCEEDED(XblSocialManagerUserGroupGetUsers(group, &users, &userCount));
            return std::vector<const XblSocialManagerUser*>(users, users + userCount);
        }

        size_t GetUsersCount(
            XblSocialManagerUserGroupHandle group
        ) const noexcept
        {
            const XblSocialManagerUser* const* users{ nullptr };
            size_t userCount{ 0 };
            VERIFY_SUCCEEDED(XblSocialManagerUserGroupGetUsers(group, &users, &userCount));
            return userCount;
        }

        size_t GetTrackedUsersCount(
            XblSocialManagerUserGroupHandle group
        ) const noexcept
        {
            const uint64_t* trackedUsers{ nullptr };
            size_t userCount{ 0 };
            VERIFY_SUCCEEDED(XblSocialManagerUserGroupGetUsersTrackedByGroup(group, &trackedUsers, &userCount));
            return userCount;
        }

        // Configure Mocks used by SocialManager
        void SetPeoplehubMock(
            bool online = true,
            bool isFavorite = false,
            bool isFollowedByCaller = true
        ) noexcept
        {
            // No HTTP method so it matches both batch and get friends calls
            m_peoplehubMock = std::make_shared<HttpMock>("", "https://peoplehub.xboxlive.com");

            m_peoplehubMock->SetMockMatchedCallback(
                [
                    this,
                    online,
                    isFavorite,
                    isFollowedByCaller
                ]
            (HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
                {
                    std::vector<uint64_t> xuids;

                    auto jsonRequest = Utils::ParseJson(requestBody.data());
                    if (jsonRequest.is_null())
                    {
                        xuids = this->FollowedXuids;
                    }
                    else
                    {
                        auto xuidArray = jsonRequest[L"xuids"];
                        for (size_t i = 0; i < xuidArray.size(); ++i)
                        {
                            xuids.push_back(Utils::Uint64FromStringT(xuidArray[i].as_string()));
                        }
                    }

                    JsonDocument responseBody(rapidjson::kObjectType);
                    JsonDocument::AllocatorType& allocator = responseBody.GetAllocator();

                    JsonValue peopleArray(rapidjson::kArrayType);
                    for (auto& xuid : xuids)
                    {
                        JsonDocument jsonBlob(&allocator);
                        jsonBlob.Parse(defaultPeoplehubTemplate);
                        JsonUtils::SetMember(jsonBlob, "xuid", JsonValue(utils::uint64_to_internal_string(xuid).c_str(), allocator));
                        JsonUtils::SetMember(jsonBlob, "presenceState", JsonValue((online ? "Online" : "Offline"), allocator));
                        JsonUtils::SetMember(jsonBlob, "isFavorite", JsonValue(isFavorite));
                        JsonUtils::SetMember(jsonBlob, "isFollowedByCaller", JsonValue(isFollowedByCaller));
                        peopleArray.PushBack(jsonBlob, allocator);
                    }

                    
                    responseBody.AddMember("people", peopleArray, allocator);
                    mock->SetResponseBody(responseBody);
                });
        }

        // Presence service will respond that all users are online except for those specified in offlineXuids
        void SetPresenceMock(
            const std::vector<uint64_t>& offlineXuids = {},
            uint32_t titleId = MOCK_TITLEID
        ) noexcept
        {
            m_presenceMock = std::make_shared<HttpMock>("GET", "https://userpresence.xboxlive.com");

            m_presenceMock->SetMockMatchedCallback(
                [
                    offlineXuids = std::unordered_set<uint64_t>{ offlineXuids.begin(), offlineXuids.end() },
                    titleId
                ]
            (HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody) mutable
                {
                    SetPresenceResponse(mock, requestUrl, requestBody, std::move(offlineXuids), titleId);
                });
        }

        void FireDevicePresenceChangeRtaEvent(
            const std::vector<uint64_t>& userList,
            bool online = false
        ) const noexcept
        {
            for (auto xuid : userList)
            {
                xsapi_internal_stringstream uri;
                uri << "https://userpresence.xboxlive.com/users/xuid(" << xuid << ")/devices";

                MockRealTimeActivityService::Instance().RaiseEvent(uri.str(), online ? R"("PC:true")" : R"("PC:false")");
            }
        }

        void FireTitlePresenceChangeRtaEvent(
            const std::vector<uint64_t>& userList,
            bool ended = true
        ) const noexcept
        {
            for (auto xuid : userList)
            {
                xsapi_internal_stringstream uri;
                uri << "https://userpresence.xboxlive.com/users/xuid(" << xuid << ")/titles/1234";

                MockRealTimeActivityService::Instance().RaiseEvent(uri.str(), ended ? R"("ended")" : R"("started")");
            }
        }

        void FireSocialGraphChangedRtaEvent(
            const User& localUser,
            XblSocialNotificationType notificationType,
            uint64_t affectedXuid
        ) const noexcept
        {
            xsapi_internal_stringstream uri;
            uri << "http://social.xboxlive.com/users/xuid(" << localUser.Xuid() << ")/friends";

            rapidjson::Document eventData{ rapidjson::kObjectType };
            auto& a{ eventData.GetAllocator() };

            eventData.AddMember("NotificationType", rapidjson::Value{ EnumName(notificationType).data(), a }, a);
            rapidjson::Value eventDataXuids{ rapidjson::kArrayType };
            eventDataXuids.PushBack(rapidjson::Value{ Utils::StringFromUint64(affectedXuid).c_str(), a }.Move(), a);
            eventData.AddMember("Xuids", eventDataXuids.Move(), a);

            MockRealTimeActivityService::Instance().RaiseEvent(uri.str(), eventData);
        }

        // Xuids Local Users follow
        std::vector<uint64_t> FollowedXuids;

    private:
        static void RTASubscribeHandler(uint32_t n, xsapi_internal_string uri)
        {
            auto& rtaService{ system::MockRealTimeActivityService::Instance() };
            if (uri.find("https://userpresence.xboxlive.com") != xsapi_internal_string::npos)
            {
                rtaService.CompleteSubscribeHandshake(n, presenceOnlineRtaMessageSubscribeComplete);
            }
            else if (uri.find("http://social.xboxlive.com") != xsapi_internal_string::npos)
            {
                rtaService.CompleteSubscribeHandshake(n);
            }
        }

        static size_t SetPresenceResponse(
            HttpMock* mock,
            const xsapi_internal_string& requestUrl,
            const xsapi_internal_string& requestBody,
            std::unordered_set<uint64_t>&& offlineXuids = {},
            uint32_t titleId = MOCK_TITLEID
        )
        {
            JsonDocument userArr{};
            if (requestUrl.find("batch") != xsapi_internal_string::npos)
            {
                // for batch requests the user list is in the request body
                JsonDocument jsonRequest{ rapidjson::kObjectType };
                jsonRequest.Parse(requestBody.data());
                userArr.CopyFrom(jsonRequest["users"], userArr.GetAllocator());
                assert(userArr.IsArray());
            }
            else
            {
                assert(requestBody.empty());
                userArr.SetArray();
                auto beginIndex{ requestUrl.find("(") };
                auto endIndex{ requestUrl.find(")") };
                userArr.PushBack(JsonValue{ requestUrl.substr(beginIndex + 1, endIndex - beginIndex - 1).data(), userArr.GetAllocator() }, userArr.GetAllocator());
            }

            JsonDocument response(rapidjson::kArrayType);
            JsonDocument::AllocatorType& allocator = response.GetAllocator();
            for (uint32_t i = 0; i < userArr.Size(); ++i)
            {
                if (offlineXuids.find(utils::internal_string_to_uint64(userArr[i].GetString())) == offlineXuids.end())
                {
                    JsonDocument onlinePresenceResponseTemplateJson(&allocator);
                    onlinePresenceResponseTemplateJson.Parse(onlinePresenceResponseTemplate);
                    response.PushBack(onlinePresenceResponseTemplateJson, allocator);
                    JsonUtils::SetMember(response[i]["devices"][0]["titles"][0], allocator, "id", JsonValue(utils::uint32_to_internal_string(titleId).c_str(), allocator));
                }
                else
                {
                    JsonDocument offlinePresenceResponseTemplateJson(&allocator);
                    offlinePresenceResponseTemplateJson.Parse(offlinePresenceResponseTemplate);
                    response.PushBack(offlinePresenceResponseTemplateJson, allocator);
                }
                JsonUtils::SetMember(response[i], allocator, "xuid", userArr[i]);
            }

            mock->SetResponseBody(response);

            return userArr.Size();
        }

        void LogEvents(
            const std::vector<const XblSocialManagerEvent*>& events
        ) const noexcept
        {
            static std::unordered_map<XblSocialManagerEventType, xsapi_internal_string> eventTypesMap
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

            for (auto& event : events)
            {
                xsapi_internal_stringstream ss;
                ss << "SocialManager Event: " << eventTypesMap[event->eventType] << std::endl;
                for (uint32_t i = 0; i < XBL_SOCIAL_MANAGER_MAX_AFFECTED_USERS_PER_EVENT; i++)
                {
                    if (event->usersAffected[i] != nullptr)
                    {
                        if (i == 0)
                        {
                            ss << "Users affected: " << std::endl;
                        }
                        ss << "\t" << event->usersAffected[i]->xboxUserId << std::endl;
                    }
                }
                LOGS_DEBUG << ss.str();
            }
        }

        // Default mocks used by SocialManager
        std::shared_ptr<HttpMock> m_peoplehubMock;
        std::shared_ptr<HttpMock> m_presenceMock;
    };

public:
    DEFINE_TEST_CASE(TestAddLocalUser)
    {
        TEST_LOG(L"Test starting: TestAddLocalUser");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());
    }

    DEFINE_TEST_CASE(TestBasicCreateFilterGroup)
    {
        TEST_LOG(L"Test starting: TestBasicCreateFilterGroup");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle filterGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &filterGroup
        ));

        bool groupLoaded{ false };
        while (!groupLoaded)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == filterGroup);
                    groupLoaded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        auto users{ env.GetUsers(filterGroup) };
        VERIFY_ARE_EQUAL(env.GetTrackedUsersCount(filterGroup), users.size());

        for (auto user : users)
        {
            // profile tests
            LOGS_DEBUG << "Validating user " << user->xboxUserId;
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->displayName);
            VERIFY_ARE_EQUAL_STR("http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png", user->displayPicUrlRaw);
            VERIFY_IS_TRUE(user->isFollowedByCaller);
            VERIFY_IS_TRUE(user->isFollowingUser);
            VERIFY_ARE_EQUAL_STR("9001", user->gamerscore);
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->gamertag);
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->modernGamertag);
            VERIFY_ARE_EQUAL_STR("", user->modernGamertagSuffix);
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->uniqueModernGamertag);
            VERIFY_IS_FALSE(user->isFavorite);
            VERIFY_IS_FALSE(user->useAvatar);

            // preferred color tests
            VERIFY_ARE_EQUAL_STR("193e91", user->preferredColor.primaryColor);
            VERIFY_ARE_EQUAL_STR("2458cf", user->preferredColor.secondaryColor);
            VERIFY_ARE_EQUAL_STR("122e6b", user->preferredColor.tertiaryColor);

            // presence record tests
            VERIFY_IS_TRUE(user->presenceRecord.presenceTitleRecordCount == 1);
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
            VERIFY_IS_TRUE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, 1234));
            VERIFY_IS_TRUE(user->presenceRecord.presenceTitleRecords[0].isTitleActive);
            VERIFY_IS_TRUE(!user->presenceRecord.presenceTitleRecords[0].isBroadcasting);
            VERIFY_IS_TRUE(user->presenceRecord.presenceTitleRecords[0].deviceType == XblPresenceDeviceType::PC);
            VERIFY_ARE_EQUAL_STR("Home", user->presenceRecord.presenceTitleRecords[0].presenceText);

            // title history tests
            VERIFY_IS_TRUE(user->titleHistory.hasUserPlayed);
            VERIFY_IS_TRUE(VerifyTime(user->titleHistory.lastTimeUserPlayed, "2015-01-26T22:54:54.6630Z"));
            VERIFY_ARE_EQUAL_STR("8 months ago", user->titleHistory.lastTimeUserPlayedText);
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(filterGroup));
    }

    DEFINE_TEST_CASE(TestPresenceRtaUpdates)
    {
        TEST_LOG(L"Test starting: TestPresenceRtaUpdates");

        SMTestEnvironment env{};
        auto xboxLiveContext{ env.CreateMockXboxLiveContext() };
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });

        auto users{ env.GetUsers(group) };
        VERIFY_ARE_EQUAL_INT(users.size(), env.FollowedXuids.size());

        // Verify initial presence
        for (auto user : users)
        {
            VERIFY_IS_TRUE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, 1234));
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
        }

        // Test title presence changed event
        auto presenceChangeXuids{ env.FollowedXuids };
        env.SetPresenceMock({}, 4321);
        env.FireTitlePresenceChangeRtaEvent(presenceChangeXuids);

        size_t presenceChangedEvents{ 0 };
        while (presenceChangedEvents < presenceChangeXuids.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_FALSE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&affectedUser->presenceRecord, 1234));
                            presenceChangedEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }
        VERIFY_ARE_EQUAL(presenceChangedEvents, presenceChangeXuids.size());

        // Verify the group view is updated as well
        for (auto user : env.GetUsers(group))
        {
            VERIFY_IS_FALSE(XblSocialManagerPresenceRecordIsUserPlayingTitle(&user->presenceRecord, 1234));
        }

        // Test device presence changed event
        env.SetPresenceMock(presenceChangeXuids);
        env.FireDevicePresenceChangeRtaEvent(presenceChangeXuids);

        presenceChangedEvents = 0;
        while (presenceChangedEvents < presenceChangeXuids.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(affectedUser->presenceRecord.userState == XblPresenceUserState::Offline);
                            presenceChangedEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }
        VERIFY_ARE_EQUAL(presenceChangedEvents, presenceChangeXuids.size());

        // Verify the group view is updated as well
        for (auto user : env.GetUsers(group))
        {
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Offline);
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    DEFINE_TEST_CASE(TestMultipleLocalUsers)
    {
        TEST_LOG(L"Test starting: TestMultipleLocalUsers");

        SMTestEnvironment env{};
        auto xboxLiveContext1 = env.CreateMockXboxLiveContext();
        auto xboxLiveContext2 = env.CreateMockXboxLiveContext(202020202020, "MockLocalUser2");

        env.AddLocalUser(xboxLiveContext1->User());
        VERIFY_IS_TRUE(XblSocialManagerGetLocalUserCount() == 1);

        env.AddLocalUser(xboxLiveContext2->User());
        VERIFY_IS_TRUE(XblSocialManagerGetLocalUserCount() == 2);

        XblSocialManagerUserGroupHandle user1Group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext1->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &user1Group
        ));

        // Create a list group that is a subset of user1Group
        std::vector<uint64_t> group2TrackedUsers{ 1, 2, 3, 4, 5 };

        XblSocialManagerUserGroupHandle user2Group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext2->User().Handle(),
            group2TrackedUsers.data(),
            group2TrackedUsers.size(),
            &user2Group
        ));

        size_t groupsLoaded{ 0 };
        while (groupsLoaded < 2)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == user1Group || event->groupAffected == user2Group);
                    groupsLoaded++;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_IS_TRUE(groupsLoaded == 2);
        auto group1Users{ env.GetUsers(user1Group) };
        auto group2Users{ env.GetUsers(user2Group) };
        VERIFY_ARE_EQUAL(group1Users.size(), env.FollowedXuids.size());
        VERIFY_ARE_EQUAL(group2Users.size(), group2TrackedUsers.size());

        // Verify initial presence
        for (auto& user : group1Users)
        {
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
        }
        for (auto& user : group2Users)
        {
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
        }

        // Validate presence change propogates to both user's groups and make sure we get presence
        // changed events for each local user
        env.SetPresenceMock(group2TrackedUsers);
        env.FireDevicePresenceChangeRtaEvent(group2TrackedUsers);

        size_t presenceChangedEvents{ 0 };
        size_t expectedEvents{ group2TrackedUsers.size() * 2 };

        while (presenceChangedEvents < expectedEvents)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            LOGS_DEBUG << affectedUser->xboxUserId;
                            VERIFY_IS_TRUE(affectedUser->presenceRecord.userState == XblPresenceUserState::Offline);
                            presenceChangedEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }
        VERIFY_ARE_EQUAL(presenceChangedEvents, expectedEvents);

        std::set<uint64_t> updatedXuids{ group2TrackedUsers.begin(), group2TrackedUsers.end() };

        // Verify both group views are updated as well
        for (auto& user : env.GetUsers(user1Group))
        {
            if (updatedXuids.find(user->xboxUserId) == updatedXuids.end())
            {
                VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Online);
            }
            else
            {
                VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Offline);
            }
        }
        for (auto& user : env.GetUsers(user2Group))
        {
            VERIFY_IS_TRUE(user->presenceRecord.userState == XblPresenceUserState::Offline);
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(user1Group));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(user2Group));
    }

    DEFINE_TEST_CASE(TestAddRemoveLocalUser)
    {
        TEST_LOG(L"Test starting: TestAddRemoveLocalUser");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        // Tests race condition in adding then removing a local user before adding is complete
        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));
        VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(xboxLiveContext->User().Handle()));
        VERIFY_ARE_EQUAL(XblSocialManagerGetLocalUserCount(), 0u);
        env.AddLocalUser(xboxLiveContext->User());
        VERIFY_ARE_EQUAL(XblSocialManagerGetLocalUserCount(), 1u);
    }

    DEFINE_TEST_CASE(TestSocialRelationshipChangedRtaUpdate)
    {
        TEST_LOG(L"Test starting: TestSocialRelationshipChangedRtaUpdate");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));

        size_t expectedGroupSize{ env.FollowedXuids.size() };

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });
        VERIFY_ARE_EQUAL_INT(expectedGroupSize, env.GetUsersCount(group));

        // Fire social relationship added event. Expected group size should grow by 1.
        uint64_t addedXuid{ 101 };
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Added, addedXuid);
        expectedGroupSize++;

        bool userAddedToGraph{ false };
        while (!userAddedToGraph)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, addedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    userAddedToGraph = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetUsersCount(group));

        uint64_t changedXuid{ 1 };
        env.SetPeoplehubMock(true, true);
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Changed, changedXuid);

        auto relationshipChangedEventFound{ false };
        while (!relationshipChangedEventFound)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, changedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    relationshipChangedEventFound = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetUsersCount(group));

        // Fire social relationship removed event. Expected group size should decrease by 1.
        uint64_t removedXuid{ 2 };
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Removed, removedXuid);
        expectedGroupSize--;

        bool userRemovedFromGraph{ false };

        // Since the user is being removed from the graph, there will never be social relationship changed event in this case.
        // If the user was removed as a friend but was remaining in the graph (i.e. they are tracked by a list group), we would
        // receive the social relationship changed event instead.
        while (!userRemovedFromGraph)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
                {
                    // User should be removed from graph if they are removed as a friend
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, removedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    userRemovedFromGraph = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetUsersCount(group));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    DEFINE_TEST_CASE(TestListGroupWithSocialRelationshipChanged)
    {
        TEST_LOG(L"Test starting: TestListGroupWithSocialRelationshipChanged");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle friendsGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &friendsGroup
        ));

        // Create a list group with a user who we aren't friends with
        uint64_t listXuid{ 101 };
        env.SetPeoplehubMock(true, false, false);

        XblSocialManagerUserGroupHandle listGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            &listXuid,
            1,
            &listGroup
        ));

        size_t groupsLoaded{ 0 };
        while (groupsLoaded < 2)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == friendsGroup || event->groupAffected == listGroup);
                    groupsLoaded++;
                    break;
                }
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, listXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        auto expectedFriendsSize{ env.FollowedXuids.size() };

        VERIFY_ARE_EQUAL(2u, groupsLoaded);
        VERIFY_ARE_EQUAL(1u, env.GetUsersCount(listGroup));
        VERIFY_ARE_EQUAL(expectedFriendsSize, env.GetUsersCount(friendsGroup));

        // Fire social relationship added event. Friends group size should grow by 1.
        env.SetPeoplehubMock(true, false, true);
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Added, listXuid);
        expectedFriendsSize++;

        bool relationshipChanged{ false };
        while (!relationshipChanged)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, listXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->isFollowedByCaller == true);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    relationshipChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedFriendsSize, env.GetUsersCount(friendsGroup));

        // Fire social relationship removed event. Expected group size should decrease by 1.
        env.SetPeoplehubMock(true, false, false);
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Removed, listXuid);
        expectedFriendsSize--;

        relationshipChanged = false;
        while (!relationshipChanged)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, listXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->isFollowedByCaller == false);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    relationshipChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedFriendsSize, env.GetUsersCount(friendsGroup));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(friendsGroup));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(listGroup));
    }

    DEFINE_TEST_CASE(TestFilterGroupChanges)
    {
        TEST_LOG(L"Test starting: TestFilterGroupChanges");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::TitleOnline,
            XblRelationshipFilter::Friends,
            &group
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });

        auto expectedGroupSize{ env.FollowedXuids.size() };
        VERIFY_ARE_EQUAL_INT(expectedGroupSize, env.GetUsersCount(group));

        // Make half the users go offline
        std::vector<uint64_t> usersToGoOffline{ env.FollowedXuids.begin(), env.FollowedXuids.begin() + env.FollowedXuids.size() / 2 };
        env.SetPresenceMock(usersToGoOffline);

        env.FireDevicePresenceChangeRtaEvent(usersToGoOffline);
        expectedGroupSize -= usersToGoOffline.size();

        size_t presenceChangedEvents{ 0 };
        size_t expectedEvents{ usersToGoOffline.size() };

        while (presenceChangedEvents < expectedEvents)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(affectedUser->presenceRecord.userState == XblPresenceUserState::Offline);
                            presenceChangedEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(expectedEvents, presenceChangedEvents);
        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetUsersCount(group));
        VERIFY_ARE_EQUAL(expectedGroupSize, env.GetTrackedUsersCount(group));
    }

    DEFINE_TEST_CASE(TestMultipleListGroups)
    {
        TEST_LOG(L"Test starting: TestMultipleListGroups");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        // Set mock for non-friends
        env.SetPeoplehubMock(true, false, false);

        // Create a list group that doesn't overlap with our friends at all. Five new users should be added to the graph
        std::set<uint64_t> trackedXuids1{ 101, 102, 103, 104, 105 };

        XblSocialManagerUserGroupHandle group1{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            std::vector<uint64_t>{ trackedXuids1.begin(), trackedXuids1.end() }.data(),
            trackedXuids1.size(),
            &group1
        ));

        VERIFY_ARE_EQUAL(trackedXuids1.size(), env.GetTrackedUsersCount(group1));

        auto groupLoaded{ false };
        size_t usersAddedToGraph{ 0 };
        while (!groupLoaded || usersAddedToGraph < trackedXuids1.size())
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == group1);
                    groupLoaded = true;
                    break;
                }
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(trackedXuids1.find(affectedUser->xboxUserId) != trackedXuids1.end());
                            VERIFY_IS_TRUE(affectedUser->isFollowedByCaller == false);
                            usersAddedToGraph++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(trackedXuids1.size(), env.GetUsersCount(group1));

        // Create a second group with partial graph overlap. No new users should be added to graph.
        std::vector<uint64_t> trackedXuids2{ 1, 2, 3, 4, 5, 101, 102, 103, 104, 105 };

        XblSocialManagerUserGroupHandle group2{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            trackedXuids2.data(),
            trackedXuids2.size(),
            &group2
        ));

        groupLoaded = false;
        while (!groupLoaded)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == group2);
                    groupLoaded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(trackedXuids2.size(), env.GetUsersCount(group2));

        // Validate that users remain in graph after their social status changes, until the list group is also
        // destroyed (they are also part of list group1).

        // Fire social relationship removed event.
        uint64_t removedXuid{ 1 };
        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Removed, removedXuid);

        bool socialRelationshipChanged{ false };
        while (!socialRelationshipChanged)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_IS_TRUE(event->usersAffected[0]->xboxUserId == removedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->isFollowedByCaller == false);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    socialRelationshipChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        // Destroying group2 should cause 1 user to new be removed from the graph (the remainder are either
        // friends or part of group1 as well)
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group2));

        size_t usersRemoved{ 0 };
        while (usersRemoved < 1)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
                {
                    // User should be removed from graph if they are removed as a friend
                    VERIFY_ARE_EQUAL_INT(event->usersAffected[0]->xboxUserId, removedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    usersRemoved++;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        // Test updating group1. This should cause the previous tracked users to be removed from the graph and the new ones to be added.
        std::set<uint64_t> newTrackedXuids1{ 106, 107 };
        VERIFY_SUCCEEDED(XblSocialManagerUpdateSocialUserGroup(
            group1,
            std::vector<uint64_t>{ newTrackedXuids1.begin(), newTrackedXuids1.end() }.data(),
            newTrackedXuids1.size()
        ));
        VERIFY_ARE_EQUAL(newTrackedXuids1.size(), env.GetTrackedUsersCount(group1));

        auto groupUpdated{ false };
        size_t usersRemovedFromGraph{ 0 };
        usersAddedToGraph = 0;

        while (!groupUpdated || usersAddedToGraph < newTrackedXuids1.size() || usersRemovedFromGraph < trackedXuids1.size())
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupUpdated:
                {
                    VERIFY_IS_TRUE(event->groupAffected == group1);
                    groupUpdated = true;
                    break;
                }
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(newTrackedXuids1.find(affectedUser->xboxUserId) != newTrackedXuids1.end());
                            usersAddedToGraph++;
                        }
                    }
                    break;
                }
                case XblSocialManagerEventType::UsersRemovedFromSocialGraph:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(trackedXuids1.find(affectedUser->xboxUserId) != trackedXuids1.end());
                            usersRemovedFromGraph++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(newTrackedXuids1.size(), env.GetUsersCount(group1));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group1));
    }

    DEFINE_TEST_CASE(TestSocialUserGroupFromListLarge)
    {
        TEST_LOG(L"Test starting: TestSocialUserGroupFromListLarge");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        std::vector<uint64_t> trackedXuids;
        for (uint64_t xuid = 0; xuid < XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST; ++xuid)
        {
            trackedXuids.push_back(xuid + 101); // Offset to make unique from our friends
        }

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(
            xboxLiveContext->User().Handle(),
            trackedXuids.data(),
            trackedXuids.size(),
            &group
        ));

        VERIFY_ARE_EQUAL(trackedXuids.size(), env.GetTrackedUsersCount(group));

        std::set<uint64_t> trackedXuidsSet{ trackedXuids.begin(), trackedXuids.end() };

        auto groupLoaded{ false };
        size_t usersAddedToGraph{ 0 };
        while (!groupLoaded || usersAddedToGraph < trackedXuids.size())
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == group);
                    groupLoaded = true;
                    break;
                }
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(trackedXuidsSet.find(affectedUser->xboxUserId) != trackedXuidsSet.end());
                            usersAddedToGraph++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL(trackedXuids.size(), env.GetUsersCount(group));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    // Tests refresh for RTA resync
    DEFINE_TEST_CASE(TestRtaResync)
    {
        TEST_LOG(L"Test starting: TestRtaResync");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());
        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(group));

        // Make a slight change so we know that the users were refreshed
        env.SetPeoplehubMock(true, true);
        MockRealTimeActivityService::Instance().RaiseResync();
        size_t profileChangesEvents{ 0 };
        while (profileChangesEvents < env.FollowedXuids.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_ARE_EQUAL(affectedUser->isFavorite, true);
                            profileChangesEvents++;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

    DEFINE_TEST_CASE(TestSMInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestSMInvalidArgs");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblUserHandle dummyHandle{ (XblUserHandle)0xFFFFFFFFFFFFFFFF };
        XblSocialManagerUserGroupHandle groupHandle{ nullptr };

#pragma warning( push )
#pragma warning( disable : 6387 )
        VERIFY_FAILED(XblSocialManagerAddLocalUser(nullptr, XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));
        VERIFY_FAILED(XblSocialManagerRemoveLocalUser(nullptr));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromFilters(nullptr, XblPresenceFilter::All, XblRelationshipFilter::Friends, &groupHandle));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromFilters(dummyHandle, XblPresenceFilter::All, XblRelationshipFilter::Friends, nullptr));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(nullptr, std::vector<uint64_t>{ 0, 1, 2 }.data(), 3, & groupHandle));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(dummyHandle, nullptr, 0, &groupHandle));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(dummyHandle, std::vector<uint64_t>{ 0, 1, 2 }.data(), 3, nullptr));
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(dummyHandle, std::vector<uint64_t>(XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST + 1, 0).data(), XBL_SOCIAL_MANAGER_MAX_USERS_FROM_LIST + 1, &groupHandle));
        VERIFY_FAILED(XblSocialManagerDestroySocialUserGroup(nullptr));
        VERIFY_FAILED(XblSocialManagerDoWork(nullptr, nullptr));
#pragma warning( pop )

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::All,
            XblRelationshipFilter::Friends,
            &group
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });

        VERIFY_FAILED(XblSocialManagerUpdateSocialUserGroup(group, std::vector<uint64_t>{ 0, 1, 2 }.data(), 3));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));
    }

#if 0
    DEFINE_TEST_CASE(TestImproperCallingPattern)
    {
        TEST_LOG(L"Test starting: TestImproperCallingPattern");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromFilters(xboxLiveContext->User().Handle(), XblPresenceFilter::All, XblRelationshipFilter::Friends, &group));

        VERIFY_FAILED(XblSocialManagerCreateSocialUserGroupFromList(xboxLiveContext->User().Handle(), std::vector<uint64_t>{ 0, 1, 2 }.data(), 3, & group));

        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::All, nullptr));
        VERIFY_FAILED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::All, nullptr));

        VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(xboxLiveContext->User().Handle()));
        VERIFY_FAILED(XblSocialManagerRemoveLocalUser(xboxLiveContext->User().Handle()));
    }
#endif

    DEFINE_TEST_CASE(TestCreateGroupBeforeUserAddedCompletes)
    {
        TEST_LOG(L"Test starting: TestCreateGroupBeforeUserAddedCompletes");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        // Add user, and before local user is added, create and destroy a group
        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

        XblSocialManagerUserGroupHandle group{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromList(xboxLiveContext->User().Handle(), std::vector<uint64_t>{ 101, 102 }.data(), 2, & group));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(group));

        bool localUserAdded{ false };
        while (!localUserAdded)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::LocalUserAdded:
                {
                    localUserAdded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(xboxLiveContext->User().Handle()));
    }

    DEFINE_TEST_CASE(TestSocialRelationshipChangedDuringInitialization)
    {
        TEST_LOG(L"Test starting: TestSocialRelationshipChangedDuringInitialization");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        // Add user, and before local user is added, receive a social relationship changed notification
        uint64_t xuidAdded{ 101 };
        env.FollowedXuids.push_back(xuidAdded);

        VERIFY_SUCCEEDED(XblSocialManagerAddLocalUser(xboxLiveContext->User().Handle(), XblSocialManagerExtraDetailLevel::NoExtraDetail, nullptr));

        XblSocialManagerUserGroupHandle friendsGroup{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(xboxLiveContext->User().Handle(), XblPresenceFilter::All, XblRelationshipFilter::Friends, &friendsGroup));

        env.FireSocialGraphChangedRtaEvent(xboxLiveContext->User(), XblSocialNotificationType::Added, xuidAdded);

        bool localUserAdded{ false };
        bool userAddedToGraph{ false };
        while (!localUserAdded || !userAddedToGraph)
        {
            auto events{ env.DoWork() };
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::LocalUserAdded:
                {
                    localUserAdded = true;
                    break;
                }
                // There are two possible orderings in this scenario: if the rta event is processed before the
                // graph initialization completes, by design, we won't get a UsersAddedToSocialGraph event. However,
                // in that case, the new user should already be in the friends group when it is loaded.
                case XblSocialManagerEventType::UsersAddedToSocialGraph:
                {
                    VERIFY_IS_TRUE(event->usersAffected[0]->xboxUserId == xuidAdded);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    userAddedToGraph = true;
                    break;
                }
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    VERIFY_IS_TRUE(event->groupAffected == friendsGroup);
                    if (env.GetUsersCount(friendsGroup) == env.FollowedXuids.size())
                    {
                        userAddedToGraph = true;
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(friendsGroup));
    }

    DEFINE_TEST_CASE(TestFilters)
    {
        TEST_LOG(L"Test starting: TestFilters");
        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());
        auto user{ xboxLiveContext->User().Handle() };

        XblSocialManagerUserGroupHandle allFriends, titleOnlineFriends, allFavorites, allOfflineFriends, titleOfflineFriends, allOnlineFriends, allTitleFriends;

        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::All, XblRelationshipFilter::Friends, &allFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::TitleOnline, XblRelationshipFilter::Friends, &titleOnlineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::All, XblRelationshipFilter::Favorite, &allFavorites));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::AllOffline, XblRelationshipFilter::Friends, &allOfflineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::TitleOffline, XblRelationshipFilter::Friends, &titleOfflineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::AllOnline, XblRelationshipFilter::Friends, &allOnlineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(user, XblPresenceFilter::AllTitle, XblRelationshipFilter::Friends, &allTitleFriends));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 7} });

        // Verify initial user sizes
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(titleOnlineFriends));
        VERIFY_ARE_EQUAL_INT(0, env.GetUsersCount(allFavorites));
        VERIFY_ARE_EQUAL_INT(0, env.GetUsersCount(allOfflineFriends));
        VERIFY_ARE_EQUAL_INT(0, env.GetUsersCount(titleOfflineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allOnlineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allTitleFriends));

        // Setup a mock that will affect multiple groups: Online->Offline && Followed->Favorite
        env.SetPeoplehubMock(false, true);

        uint64_t changedXuid{ 1 };

        auto userResult = User::WrapHandle(user);
        env.FireSocialGraphChangedRtaEvent(userResult.ExtractPayload(), XblSocialNotificationType::Changed, changedXuid);

        bool profileChanged{ false };
        bool presenceChanged{ false };
        while (!profileChanged || !presenceChanged)
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialRelationshipsChanged:
                {
                    VERIFY_IS_TRUE(event->usersAffected[0]->xboxUserId == changedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->isFavorite == true);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    profileChanged = true;
                    break;
                }
                case XblSocialManagerEventType::PresenceChanged:
                {
                    VERIFY_IS_TRUE(event->usersAffected[0]->xboxUserId == changedXuid);
                    VERIFY_IS_TRUE(event->usersAffected[0]->presenceRecord.userState == XblPresenceUserState::Offline);
                    VERIFY_IS_TRUE(event->usersAffected[1] == nullptr);
                    presenceChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        // verify groups are affected as expected
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - 1, env.GetUsersCount(titleOnlineFriends));
        VERIFY_ARE_EQUAL_INT(1, env.GetUsersCount(allFavorites));
        VERIFY_ARE_EQUAL_INT(1, env.GetUsersCount(allOfflineFriends));
        VERIFY_ARE_EQUAL_INT(1, env.GetUsersCount(titleOfflineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - 1, env.GetUsersCount(allOnlineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allTitleFriends));

        std::vector<uint64_t> offlineXuids{ 1, 2, 3, 4, 5 };
        env.SetPresenceMock(offlineXuids);
        env.FireDevicePresenceChangeRtaEvent(offlineXuids);

        size_t presenceChangedEvents{ 0 };
        while (presenceChangedEvents < offlineXuids.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            VERIFY_IS_TRUE(affectedUser->xboxUserId > 0 && affectedUser->xboxUserId < 6);
                            ++presenceChangedEvents;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - offlineXuids.size(), env.GetUsersCount(titleOnlineFriends));
        VERIFY_ARE_EQUAL_INT(1, env.GetUsersCount(allFavorites));
        VERIFY_ARE_EQUAL_INT(offlineXuids.size(), env.GetUsersCount(allOfflineFriends));
        VERIFY_ARE_EQUAL_INT(offlineXuids.size(), env.GetUsersCount(titleOfflineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - offlineXuids.size(), env.GetUsersCount(allOnlineFriends));
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(allTitleFriends));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(titleOnlineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allFavorites));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allOfflineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(titleOfflineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allOnlineFriends));
        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(allTitleFriends));
    }

    DEFINE_TEST_CASE(TestRichPresencePolling)
    {
        TEST_LOG(L"Test starting: TestRichPresencePolling");
        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        env.AddLocalUser(xboxLiveContext->User());

        XblSocialManagerUserGroupHandle onlineFriends{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
            xboxLiveContext->User().Handle(),
            XblPresenceFilter::AllOnline,
            XblRelationshipFilter::Friends,
            &onlineFriends
        ));

        env.AwaitEvents({ {XblSocialManagerEventType::SocialUserGroupLoaded, 1} });
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size(), env.GetUsersCount(onlineFriends));

        // Change presence mock so we get a PresenceChanged event when it is polled
        std::vector<uint64_t> offlineUsers{ 1, 2, 3 };
        env.SetPresenceMock(offlineUsers);
        VERIFY_SUCCEEDED(XblSocialManagerSetRichPresencePollingStatus(xboxLiveContext->User().Handle(), true));

        size_t presenceChangedEvents{ 0 };
        while (presenceChangedEvents < offlineUsers.size())
        {
            auto events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    for (auto affectedUser : event->usersAffected)
                    {
                        if (affectedUser)
                        {
                            ++presenceChangedEvents;
                        }
                    }
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_ARE_EQUAL_INT(offlineUsers.size(), presenceChangedEvents);
        VERIFY_ARE_EQUAL_INT(env.FollowedXuids.size() - offlineUsers.size(), env.GetUsersCount(onlineFriends));

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(onlineFriends));
    }

    DEFINE_TEST_CASE(TestEventUserHandleLifetime)
    {
        TEST_LOG(L"Test starting: TestEventUserHandleLifetime");

        SMTestEnvironment env{};

        uint64_t const presenceChangedXuid{ 1 };
        std::vector<const XblSocialManagerEvent*> events{};

        XblSocialManagerUserGroupHandle groupHandle{ nullptr };

        {
            // Add a user to SocialManager and create a user group but don't hang on to the user handle. 
            // Ensure the user handles returned in all SM events remain valid until the next DoWork call
            User user{ CreateMockUser(MOCK_XUID) };
            env.AddLocalUser(user);

            VERIFY_SUCCEEDED(XblSocialManagerCreateSocialUserGroupFromFilters(
                user.Handle(),
                XblPresenceFilter::AllOnline,
                XblRelationshipFilter::Friends,
                &groupHandle
            ));
        }

        bool groupLoaded{ false };
        while (!groupLoaded)
        {
            events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::SocialUserGroupLoaded:
                {
                    groupLoaded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        VERIFY_SUCCEEDED(XblSocialManagerDestroySocialUserGroup(groupHandle));
        {
            // Ensure the user in the event is valid even after destroying the group
            auto userFromEventResult = User::WrapHandle(events[0]->user);
            VERIFY_ARE_EQUAL_UINT(MOCK_XUID, userFromEventResult.ExtractPayload().Xuid());
        }

        // Make a user go offline to trigger event
        env.SetPresenceMock({ presenceChangedXuid });
        env.FireDevicePresenceChangeRtaEvent({ presenceChangedXuid }, false);

        bool presenceChanged{ false };
        while (!presenceChanged)
        {
            events = env.DoWork();
            for (auto event : events)
            {
                switch (event->eventType)
                {
                case XblSocialManagerEventType::PresenceChanged:
                {
                    VERIFY_ARE_EQUAL_UINT(presenceChangedXuid, event->usersAffected[0]->xboxUserId);
                    presenceChanged = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        XblUserHandle userHandle{ nullptr };
        VERIFY_SUCCEEDED(XblSocialManagerGetLocalUsers(1, &userHandle));
        VERIFY_SUCCEEDED(XblSocialManagerRemoveLocalUser(userHandle));

        {
            // Ensure the user handle from the event is still valid after removing the user
            auto userFromEventResult = User::WrapHandle(events[0]->user);
            VERIFY_ARE_EQUAL_UINT(MOCK_XUID, userFromEventResult.ExtractPayload().Xuid());
        }
    }

    DEFINE_TEST_CASE(CppTestBasicCreateFilterGroup)
    {
        TEST_LOG(L"Test starting: CppTestBasicCreateFilterGroup");

        SMTestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();
        xbox_live_user_t userHandle = xboxLiveContext->user();

        auto socialManager{ social_manager::get_singleton_instance() };

        auto addUserResult = socialManager->add_local_user(
            userHandle,
            social_manager_extra_detail_level::no_extra_detail
        );
        VERIFY_IS_TRUE(!addUserResult.err());

        {
            // Wait until we get the local_user_added event
            bool localUserAdded{ false };
            while (!localUserAdded)
            {
                auto events{ socialManager->do_work() };
                for (auto event : events)
                {
                    switch (event.event_type())
                    {
                    case social_event_type::local_user_added:
                    {
                        localUserAdded = true;
                        break;
                    }
                    default:
                    {
                        LOGS_DEBUG << "Unexpected SocialManager Event";
                        VERIFY_FAIL();
                    }
                    }
                }
            }
        }

        auto createGroupResult = socialManager->create_social_user_group_from_filters(
            userHandle,
            presence_filter::all,
            relationship_filter::friends
        );

        VERIFY_IS_TRUE(!createGroupResult.err());
        auto group = createGroupResult.payload();

        bool groupLoaded{ false };
        while (!groupLoaded)
        {
            auto events{ socialManager->do_work() };
            for (auto event : events)
            {
                switch (event.event_type())
                {
                case social_event_type::social_user_group_loaded:
                {
                    auto groupLoadedArgs = static_cast<social_user_group_loaded_event_args*>(event.event_args().get());
                    VERIFY_IS_TRUE(group == groupLoadedArgs->social_user_group());
                    groupLoaded = true;
                    break;
                }
                default:
                {
                    LOGS_DEBUG << "Unexpected SocialManager Event";
                    VERIFY_FAIL();
                }
                }
            }
        }

        auto users{ group->users() };
        VERIFY_ARE_EQUAL(group->users_tracked_by_social_user_group().size(), users.size());

        for (auto user : users)
        {
            // profile tests
            LOGS_DEBUG << "Validating user " << user->xbox_user_id();
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->display_name());
            VERIFY_ARE_EQUAL_STR("http://images-eds.xboxlive.com/image?url=mHGRD8KXEf2sp2LC58XhBQKNl2IWRp.J.q8mSURKUUeiPPf0Y7Kl7zLN7rafayiPptVaX_XIUmNOPotNmNubbx4bHmf6It7Oj1ChU5UAo9k-&background=0xababab&mode=Padding&format=png", user->display_pic_url_raw());
            VERIFY_IS_TRUE(user->is_followed_by_caller());
            VERIFY_IS_TRUE(user->is_following_user());
            VERIFY_ARE_EQUAL_STR("9001", user->gamerscore());
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->gamertag());
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->modern_gamertag());
            VERIFY_ARE_EQUAL_STR("", user->modern_gamertag_suffix());
            VERIFY_ARE_EQUAL_STR("TestGamerTag", user->unique_modern_gamertag());
            VERIFY_IS_FALSE(user->is_favorite());
            VERIFY_IS_FALSE(user->use_avatar());

            // preferred color tests
            VERIFY_ARE_EQUAL_STR("193e91", user->preferred_color().primary_color());
            VERIFY_ARE_EQUAL_STR("2458cf", user->preferred_color().secondary_color());
            VERIFY_ARE_EQUAL_STR("122e6b", user->preferred_color().tertiary_color());

            // presence record tests
            VERIFY_IS_TRUE(user->presence_record().presence_title_records().size());
            VERIFY_IS_TRUE(user->presence_record().user_state() == user_presence_state::online);
            VERIFY_IS_TRUE(user->presence_record().is_user_playing_title(1234));
            VERIFY_IS_TRUE(user->presence_record().presence_title_records()[0].is_title_active());
            VERIFY_IS_TRUE(!user->presence_record().presence_title_records()[0].is_broadcasting());
            VERIFY_IS_TRUE(user->presence_record().presence_title_records()[0].device_type() == presence_device_type::pc);
            VERIFY_ARE_EQUAL_STR("Home", user->presence_record().presence_title_records()[0].presence_text());

            // title history tests
            VERIFY_IS_TRUE(user->title_history().has_user_played());
            VERIFY_IS_TRUE(Utils::TimeTFromDatetime(user->title_history().last_time_user_played()) == utils::TimeTFromDatetime(xbox::services::datetime::from_string("2015-01-26T22:54:54.6630Z", xbox::services::datetime::date_format::ISO_8601)));
        }

        auto destroyGroupResult = socialManager->destroy_social_user_group(group);
        VERIFY_IS_TRUE(!destroyGroupResult.err());

        auto removeUserResult = socialManager->remove_local_user(userHandle);
        VERIFY_IS_TRUE(!removeUserResult.err());

        auto events{ socialManager->do_work() };
        VERIFY_ARE_EQUAL_INT(events.size(), 1);
        VERIFY_IS_TRUE(events[0].event_type() == social_event_type::local_user_removed);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END