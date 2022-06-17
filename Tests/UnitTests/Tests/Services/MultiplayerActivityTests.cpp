// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char getActivityResponse[] = R"(
{
  "userActivities": [
    {
      "userId": "1",
      "activities": [
        {
          "sequenceNumber": 0,
          "titleId": 1234,
          "connectionString": "connectionString",
          "joinRestriction": "Public",
          "maxPlayers": 10,
          "currentPlayers": 2,
          "groupId": "groupId",
          "platform": "XboxOne"
        },
        {
          "sequenceNumber": 0,
          "titleId": 0,
          "connectionString": "connectionString2",
          "joinRestriction": "Public",
          "maxPlayers": 0,
          "currentPlayers": 0,
          "groupId": "string",
          "platform": "XboxOne"
        }
      ]
    },
    {
      "userId": "2",
      "activities": [
        {
          "sequenceNumber": 0,
          "titleId": 1234,
          "joinRestriction": "Public",
          "maxPlayers": 2,
          "currentPlayers": 1,
          "groupId": "groupId",
          "platform": "IOS"
        }
      ]
    }
  ]
})";

DEFINE_TEST_CLASS(MultiplayerActivityTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(MultiplayerActivityTests);

    DEFINE_TEST_CASE(TestUpdateRecentPlayers)
    {
        TEST_LOG(L"Test starting: TestUpdateRecentPlayers");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        Stringstream url;
        url << "https://multiplayeractivity.xboxlive.com/titles/" << MOCK_TITLEID << "/recentplayers";

        bool requestWellFormed{ true };
        const XblMultiplayerActivityRecentPlayerUpdate updates[] = { {1}, {2}, {3} };

        auto mock = std::make_shared<HttpMock>( "POST", url.str(), 204 );
        mock->SetMockMatchedCallback(
            [&](HttpMock* /*mock*/, xsapi_internal_string /*uri*/, xsapi_internal_string body)
        {
            rapidjson::Document d;
            d.Parse(body.data());

            if (d.HasParseError() || !d.HasMember("recentPlayers"))
            {
                requestWellFormed = false;
            }
            else
            {
                const auto& recentPlayers{ d["recentPlayers"].GetArray() };
                requestWellFormed = recentPlayers.Size() == _countof(updates);
            }
        });

        VERIFY_SUCCEEDED(XblMultiplayerActivityUpdateRecentPlayers(xboxLiveContext.get(), updates, _countof(updates)));

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerActivityFlushRecentPlayersAsync(xboxLiveContext.get(), &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(TestUpdateRecentPlayersNoFlush)
    {
        TEST_LOG(L"Test starting: TestUpdateRecentPlayersNoFlush");

        auto env = std::make_unique<TestEnvironment>();
        auto xboxLiveContext = env->CreateMockXboxLiveContext();

        Stringstream url;
        url << "https://multiplayeractivity.xboxlive.com/titles/" << MOCK_TITLEID << "/recentplayers";

        std::unordered_set<uint64_t> updatesSent{};
        bool requestsWellFormed{ true };
        size_t httpRequestCount{ 0 };


        auto mock = std::make_shared<HttpMock>( "POST", url.str(), 204 );
        mock->SetMockMatchedCallback(
            [&](HttpMock* /*mock*/, xsapi_internal_string /*uri*/, xsapi_internal_string body)
        {
            httpRequestCount++;

            JsonDocument d;
            d.Parse(body.data());

            if (d.HasParseError() || !d.HasMember("recentPlayers"))
            {
                requestsWellFormed = false;
            }
            else
            {
                const auto& recentPlayers{ d["recentPlayers"].GetArray() };
                for (const auto& p : recentPlayers)
                {
                    updatesSent.insert(strtoull(p["id"].GetString(), nullptr, 0));
                }
            }
        });

        std::vector<XblMultiplayerActivityRecentPlayerUpdate> updatesRequested{};

        constexpr size_t updateCalls{ 10 };
        std::array<XAsyncBlock, updateCalls> asyncOps{};

        constexpr size_t xuidsPerUpdate{ 10 };
        for (size_t update = 0; update < updateCalls; ++update)
        {
            std::array<XblMultiplayerActivityRecentPlayerUpdate, xuidsPerUpdate> updates;
            for (size_t i = 0; i < xuidsPerUpdate; ++i)
            {
                updates[i] = XblMultiplayerActivityRecentPlayerUpdate{ static_cast<uint64_t>(std::rand() % 10) };
            }
            updatesRequested.insert(updatesRequested.end(), updates.begin(), updates.end());

            VERIFY_SUCCEEDED(XblMultiplayerActivityUpdateRecentPlayers(xboxLiveContext.get(), updates.data(), updates.size()));

            // Space out the update requests a random amount
            Sleep(rand() % 2000);
        }

        // Cleanup XSAPI and then make sure all updates are flushed
        xboxLiveContext.reset();
        env.reset();

        VERIFY_IS_TRUE(requestsWellFormed);

        // Make sure each update eventually made it into a request
        for (auto update : updatesRequested)
        {
            VERIFY_IS_TRUE(updatesSent.find(update.xuid) != updatesSent.end());
        }

        std::wstringstream ss;
        ss << updatesRequested.size() << L" requested updates resulted in " << httpRequestCount << L" service requests";
        TEST_LOG(ss.str().data());
    }

    DEFINE_TEST_CASE(TestSetActivity)
    {
        TEST_LOG(L"Test starting: TestSetActivity");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        const char* expectedRequestBody = R"(
        {
            "connectionString": "mockConnectionString",
            "joinRestriction": "Public",
            "maxPlayers": 10
        })";

        Stringstream url;
        url << "https://multiplayeractivity.xboxlive.com/titles/" << MOCK_TITLEID << "/users/" << xboxLiveContext->Xuid() << "/activities";
        auto mock = std::make_shared<HttpMock>( "PUT", url.str(), 204 );

        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* /*mock*/, xsapi_internal_string /*uri*/, xsapi_internal_string body)
            {
                JsonDocument d;
                d.Parse(body.data());

                // Don't validate sequence number since that is a timestamp
                d.RemoveMember("sequenceNumber");
                requestWellFormed = VerifyJson(d, expectedRequestBody);
            });

        XblMultiplayerActivityInfo info
        {
            xboxLiveContext->Xuid(),
            "mockConnectionString",
            XblMultiplayerActivityJoinRestriction::Public,
            10,
            0,
            nullptr
        };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerActivitySetActivityAsync(
            xboxLiveContext.get(),
            &info,
            true,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
    }

    void VerifyActivityInfo(
        const char* expected,
        const XblMultiplayerActivityInfo* actual,
        size_t actualCount
    ) noexcept
    {
        JsonDocument e;
        e.Parse(expected);
        VERIFY_IS_FALSE(e.HasParseError());

        size_t i{ 0 };
        const auto& userActivitiesJson{ e["userActivities"].GetArray() };
        for (auto& user : userActivitiesJson)
        {
            auto xuid{ strtoull(user["userId"].GetString(), nullptr, 0) };
            const auto& activitesArray{ user["activities"].GetArray() };
            for (auto& activity : activitesArray)
            {
                auto titleId{ activity["titleId"].GetUint() };
                if (titleId == MOCK_TITLEID)
                {
                    VERIFY_IS_TRUE(i < actualCount);
                    auto& actualActivity{ actual[i++] };

                    VERIFY_ARE_EQUAL_UINT(xuid, actualActivity.xuid);
                    // Connection string may be missing if the caller doesn't have permission to join an activity
                    if (activity.HasMember("connectionString"))
                    {
                        VERIFY_ARE_EQUAL_STR(activity["connectionString"].GetString(), actualActivity.connectionString);
                    }
                    else
                    {
                        VERIFY_IS_TRUE(nullptr == actualActivity.connectionString);
                    }
                    VERIFY_IS_TRUE(EnumValue<XblMultiplayerActivityJoinRestriction>(activity["joinRestriction"].GetString()) == actualActivity.joinRestriction);
                    VERIFY_ARE_EQUAL_UINT(activity["maxPlayers"].GetUint64(), actualActivity.maxPlayers);
                    VERIFY_ARE_EQUAL_UINT(activity["currentPlayers"].GetUint64(), actualActivity.currentPlayers);
                    VERIFY_ARE_EQUAL_STR(activity["groupId"].GetString(), actualActivity.groupId);
                    VERIFY_IS_TRUE(EnumValue<XblMultiplayerActivityPlatform>(activity["platform"].GetString()) == actualActivity.platform);
                }
            }
        }
    }

    DEFINE_TEST_CASE(TestGetActivities)
    {
        TEST_LOG(L"Test starting: TestGetActivities");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>( "POST", "https://multiplayeractivity.xboxlive.com" );
        mock->SetResponseBody(getActivityResponse);

        uint64_t xuids[] = { 1, 2 };
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* /*mock*/, xsapi_internal_string /*uri*/, xsapi_internal_string body)
            {
                JsonDocument b;
                b.Parse(body.data());

                const auto& xuidsArray{ b["users"].GetArray() };
                requestWellFormed &= (xuidsArray.Size() == _countof(xuids));

                size_t i{ 0 };
                for (const auto& xuidString : xuidsArray)
                {
                    requestWellFormed &= (strtoull(xuidString.GetString(), nullptr, 0) == xuids[i++]);
                }
            });

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerActivityGetActivityAsync(
            xboxLiveContext.get(),
            xuids,
            _countof(xuids),
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        size_t bufferSize{};
        VERIFY_SUCCEEDED(XblMultiplayerActivityGetActivityResultSize(&async, &bufferSize));

        std::vector<uint8_t> buffer(bufferSize);
        XblMultiplayerActivityInfo* activities{ nullptr };
        size_t activitiesCount{};
        VERIFY_SUCCEEDED(XblMultiplayerActivityGetActivityResult(&async, bufferSize, buffer.data(), &activities, &activitiesCount, nullptr));

        VerifyActivityInfo(getActivityResponse, activities, activitiesCount);
    }

    DEFINE_TEST_CASE(TestGetActivitiesWithLargeBuffer)
    {
        TEST_LOG(L"Test starting: TestGetActivitiesWithLargeBuffer");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>( "POST", "https://multiplayeractivity.xboxlive.com" );
        mock->SetResponseBody(getActivityResponse);

        uint64_t xuids[] = { 1, 2 };
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&](HttpMock* /*mock*/, xsapi_internal_string /*uri*/, xsapi_internal_string body)
        {
            JsonDocument b;
            b.Parse(body.data());

            const auto& xuidsArray{ b["users"].GetArray() };
            requestWellFormed &= (xuidsArray.Size() == _countof(xuids));

            size_t i{ 0 };
            for (const auto& xuidString : xuidsArray)
            {
                requestWellFormed &= (strtoull(xuidString.GetString(), nullptr, 0) == xuids[i++]);
            }
        });

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerActivityGetActivityAsync(
            xboxLiveContext.get(),
            xuids,
            _countof(xuids),
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);

        size_t bufferSize{};
        VERIFY_SUCCEEDED(XblMultiplayerActivityGetActivityResultSize(&async, &bufferSize));

        size_t bufferUsed{};
        std::vector<uint8_t> buffer(bufferSize * 2);
        XblMultiplayerActivityInfo* activities{ nullptr };
        size_t activitiesCount{};
        VERIFY_SUCCEEDED(XblMultiplayerActivityGetActivityResult(&async, bufferSize * 2, buffer.data(), &activities, &activitiesCount, &bufferUsed));

        VERIFY_ARE_EQUAL_UINT(bufferSize, bufferUsed);

        VerifyActivityInfo(getActivityResponse, activities, activitiesCount);
    }

    DEFINE_TEST_CASE(TestDeleteActivity)
    {
        TEST_LOG(L"Test starting: TestDeleteActivity");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        Stringstream url;
        url << "https://multiplayeractivity.xboxlive.com/titles/" << MOCK_TITLEID << "/users/" << xboxLiveContext->Xuid() << "/activities";
        HttpMock mock{ "DELETE", url.str(), 204 };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerActivityDeleteActivityAsync(xboxLiveContext.get(), &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
    }

    DEFINE_TEST_CASE(TestSendInvites)
    {
        TEST_LOG(L"Test starting: TestSendInvites");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        const char* expectedRequestBody = R"(
        {
          "invitedUsers": [
            "1",
            "2",
            "3"
          ],
          "platform": "Win32",
          "connectionString": "mockConnectionString"
        })";

        Stringstream url;
        url << "https://multiplayeractivity.xboxlive.com/titles/" << MOCK_TITLEID << "/invites";

        auto mock = std::make_shared<HttpMock>( "POST", url.str(), 204 );

        bool requestWellFormed{ false };
        mock->SetMockMatchedCallback(
            [&](HttpMock* /*mock*/, xsapi_internal_string /*url*/, xsapi_internal_string body)
            {
                requestWellFormed = VerifyJson(expectedRequestBody, body.data());
            });

        const uint64_t xuids[] = { 1, 2, 3 };
        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblMultiplayerActivitySendInvitesAsync(
            xboxLiveContext.get(),
            xuids,
            _countof(xuids),
            false,
            "mockConnectionString",
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END