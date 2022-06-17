// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

#pragma warning(disable : 4996)

using namespace xbox::services::social;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(SocialTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(SocialTests);

private:
    struct SocialRelationship
    {
        uint64_t xuid{ 1 };
        bool isFollowingCaller{ true };
        bool isFavorite{ false };
        std::vector<std::wstring> socialNetworks;
    };

    std::shared_ptr<HttpMock> CreateSocialMock(
        const std::vector<SocialRelationship>& people
    )
    {
        auto mock = std::make_shared<HttpMock>("GET", "https://social.xboxlive.com");

        mock->SetMockMatchedCallback(
            [
                &people
            ]
        (HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                assert(requestBody.empty());

                xbox::services::uri url(requestUrl.data());
                auto queryParams = url.split_query(url.query());

                size_t startIndex{ 0 };
                size_t maxItems{ people.size() };
                xsapi_internal_string view{ "All" };

                if (queryParams.find("startIndex") != queryParams.end())
                {
                    startIndex = _atoi64(queryParams["startIndex"].c_str());
                }
                if (queryParams.find("maxItems") != queryParams.end())
                {
                    maxItems = _atoi64(queryParams["maxItems"].c_str());
                }
                if (queryParams.find("view") != queryParams.end())
                {
                    view = queryParams["view"];
                }

                std::vector<SocialRelationship> filteredPeople;
                std::copy_if(people.begin(), people.end(), std::back_inserter(filteredPeople),
                    [&](const SocialRelationship& person)
                    {
                        if (_stricmp("favorite", view.c_str()) == 0)
                        {
                            return person.isFavorite;
                        }
                        else if (_stricmp("LegacyXboxLiveFriends", view.c_str()) == 0)
                        {
                            auto iter = std::find_if(person.socialNetworks.begin(), person.socialNetworks.end(),
                                [](const std::wstring& s)
                                {
                                    return _stricmp(convert::utf16_to_utf8(s).c_str(), "LegacyXboxLive") == 0;
                                });
                            return iter != person.socialNetworks.end();
                        }
                        else
                        {
                            assert(_stricmp("All", view.c_str()) == 0);
                            return true;
                        }
                    });
                JsonDocument responseJson(rapidjson::kObjectType);
                JsonDocument::AllocatorType& allocator = responseJson.GetAllocator();
                JsonValue peopleJson(rapidjson::kArrayType);
                for (size_t i = startIndex; i < startIndex + maxItems && i < filteredPeople.size(); ++i)
                {
                    auto& person{ filteredPeople[i] };
                    JsonValue personJson(rapidjson::kObjectType);

                    personJson.AddMember("xuid", JsonValue(utils::uint64_to_internal_string(person.xuid).c_str(), allocator).Move(), allocator);
                    personJson.AddMember("isFavorite", person.isFavorite, allocator);
                    personJson.AddMember("isFollowingCaller", person.isFollowingCaller, allocator);

                    JsonValue socialNetworksJson(rapidjson::kArrayType);
                    for (auto& socialNetwork : person.socialNetworks)
                    {
                        socialNetworksJson.PushBack(JsonValue(utils::internal_string_from_string_t(socialNetwork).c_str(), allocator).Move(), allocator);
                    }
                    if (socialNetworksJson.Size() > 0)
                    {
                        personJson.AddMember("socialNetworks", socialNetworksJson, allocator);
                    }

                    peopleJson.PushBack(personJson, allocator);
                }

                responseJson.AddMember("people", peopleJson, allocator);
                responseJson.AddMember("totalCount", static_cast<uint64_t>(filteredPeople.size()), allocator);
                mock->SetResponseBody(responseJson);
            }
        );

        return mock;
    }

    void ValidateSocialRelationshipResult(
        XblSocialRelationshipResultHandle resultHandle,
        size_t expectedTotalCount,
        bool expectedHasNext,
        const std::vector<SocialRelationship>& expectedSocialRelationships
    )
    {
        size_t totalCount{ 0 };
        VERIFY_SUCCEEDED(XblSocialRelationshipResultGetTotalCount(resultHandle, &totalCount));
        VERIFY_ARE_EQUAL_INT(totalCount, expectedTotalCount);

        bool hasNext{ false };
        VERIFY_SUCCEEDED(XblSocialRelationshipResultHasNext(resultHandle, &hasNext));
        VERIFY_ARE_EQUAL(hasNext, expectedHasNext);

        const XblSocialRelationship* socialRelationships{ nullptr };
        size_t socialRelationshipsCount{ 0 };
        VERIFY_SUCCEEDED(XblSocialRelationshipResultGetRelationships(resultHandle, &socialRelationships, &socialRelationshipsCount));
        VERIFY_ARE_EQUAL_INT(socialRelationshipsCount, expectedSocialRelationships.size());

        for (size_t i = 0; i < socialRelationshipsCount; ++i)
        {
            auto& r{ socialRelationships[i] };

            auto iter = std::find_if(expectedSocialRelationships.begin(), expectedSocialRelationships.end(), 
                [&r](const SocialRelationship& expectedRelationship)
                {
                    return expectedRelationship.xuid == r.xboxUserId;
                });
            VERIFY_IS_TRUE(iter != expectedSocialRelationships.end());
            VERIFY_ARE_EQUAL(iter->isFavorite, r.isFavorite);
            VERIFY_ARE_EQUAL(iter->isFollowingCaller, r.isFollowingCaller);
            VERIFY_ARE_EQUAL(iter->socialNetworks.size(), r.socialNetworksCount);

            for (size_t j = 0; j < r.socialNetworksCount; ++j)
            {
                VERIFY_IS_TRUE(Utils::Stricmp(Utils::StringTFromUtf8(r.socialNetworks[j]), iter->socialNetworks[j]) == 0);
            }
        }
    }

    DEFINE_TEST_CASE(TestGetSocialRelationshipsAsync)
    {
        TEST_LOG(L"Test starting: TestGetSocialRelationshipsAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        std::vector<SocialRelationship> socialRelationships(100);
        uint64_t friendXuid{ 1 };
        for (auto& person : socialRelationships)
        {
            person.xuid = friendXuid++;
        }

        // Add a couple of favorites
        socialRelationships[0].isFavorite = true;
        socialRelationships[1].isFavorite = true;

        // Add a LegacyFriend
        socialRelationships[2].socialNetworks.push_back(L"LegacyXboxLive");

        auto socialMock{ CreateSocialMock(socialRelationships) };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            XblSocialRelationshipFilter::All,
            0,
            0,
            &async
        ));

        XblSocialRelationshipResultHandle resultHandle{ nullptr };
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsResult(&async, &resultHandle));
        ValidateSocialRelationshipResult(
            resultHandle,
            socialRelationships.size(),
            false,
            socialRelationships
        );
        XblSocialRelationshipResultCloseHandle(resultHandle);

        ZeroMemory(async.internal, sizeof(async.internal));
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            XblSocialRelationshipFilter::Favorite,
            0,
            0,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsResult(&async, &resultHandle));
        ValidateSocialRelationshipResult(
            resultHandle,
            2,
            false,
            std::vector<SocialRelationship>(socialRelationships.begin(), socialRelationships.begin() + 2)
        );
        XblSocialRelationshipResultCloseHandle(resultHandle);

        ZeroMemory(async.internal, sizeof(async.internal));
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            XblSocialRelationshipFilter::LegacyXboxLiveFriends,
            0,
            0,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsResult(&async, &resultHandle));
        ValidateSocialRelationshipResult(
            resultHandle,
            1,
            false,
            std::vector<SocialRelationship>(1, socialRelationships[2])
        );
        XblSocialRelationshipResultCloseHandle(resultHandle);
    }

    DEFINE_TEST_CASE(TestGetSocialRelationshipsResultPaging)
    {
        TEST_LOG(L"Test starting: TestGetSocialRelationshipsResultPaging");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        std::vector<SocialRelationship> socialRelationships(15);
        uint64_t friendXuid{ 1 };
        for (auto& person : socialRelationships)
        {
            person.xuid = friendXuid++;
        }

        auto socialMock{ CreateSocialMock(socialRelationships) };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            XblSocialRelationshipFilter::All,
            0,
            10,
            &async
        ));

        XblSocialRelationshipResultHandle resultHandle{ nullptr };
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsResult(&async, &resultHandle));
        ValidateSocialRelationshipResult(
            resultHandle,
            15,
            true,
            std::vector<SocialRelationship>(socialRelationships.begin(), socialRelationships.begin() + 10)
        );

        VERIFY_SUCCEEDED(XblSocialRelationshipResultGetNextAsync(
            xboxLiveContext.get(),
            resultHandle,
            10,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        XblSocialRelationshipResultCloseHandle(resultHandle);
        VERIFY_SUCCEEDED(XblSocialGetSocialRelationshipsResult(&async, &resultHandle));
        ValidateSocialRelationshipResult(
            resultHandle,
            15,
            false,
            std::vector<SocialRelationship>(socialRelationships.begin() + 10, socialRelationships.end())
        );
        XblSocialRelationshipResultCloseHandle(resultHandle);
    }

    DEFINE_TEST_CASE(TestRTASocialRelationshipChange)
    {
        TEST_LOG(L"Test starting: TestRTASocialRelationshipChange");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };

        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            xsapi_internal_stringstream expectedUri;
            expectedUri << "http://social.xboxlive.com/users/xuid(" << xboxLiveContext->Xuid() << ")/friends";
            VERIFY_ARE_EQUAL_STR(uri, expectedUri.str());

            mockRtaService.CompleteSubscribeHandshake(n);

            // Immediately raise an event
            mockRtaService.RaiseEvent(uri, R"({ "NotificationType": "Removed", "Xuids": [ "2" ] })");
        });

        struct HandlerContext
        {
            Event notificationReceived;
            uint64_t xuid{ 0 };
            XblSocialNotificationType notificationType{ XblSocialNotificationType::Unknown };
            std::vector<uint64_t> affectedXuids;
        } context;

        auto handlerToken = XblSocialAddSocialRelationshipChangedHandler(xboxLiveContext.get(),
            [](const XblSocialRelationshipChangeEventArgs* args, void* context)
            {
                auto c{ static_cast<HandlerContext*>(context) };
                
                c->xuid = args->callerXboxUserId;
                c->notificationType = args->socialNotification;
                c->affectedXuids = std::vector<uint64_t>(args->xboxUserIds, args->xboxUserIds + args->xboxUserIdsCount);
                c->notificationReceived.Set();
            },
            &context
        );

        context.notificationReceived.Wait();

        VERIFY_ARE_EQUAL_INT(xboxLiveContext->Xuid(), context.xuid);
        VERIFY_IS_TRUE(context.notificationType == XblSocialNotificationType::Removed);
        VERIFY_ARE_EQUAL_INT(1u, context.affectedXuids.size());
        VERIFY_ARE_EQUAL_INT(2, context.affectedXuids[0]);

        VERIFY_SUCCEEDED(XblSocialRemoveSocialRelationshipChangedHandler(xboxLiveContext.get(), handlerToken));
    }

    DEFINE_TEST_CASE(CppTestGetSocialRelationships)
    {
        TEST_LOG(L"Test starting: CppTestGetSocialRelationships");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();

        std::vector<SocialRelationship> socialRelationships(100);
        uint64_t friendXuid{ 1 };
        for (auto& person : socialRelationships)
        {
            person.xuid = friendXuid++;
        }

        auto socialMock{ CreateSocialMock(socialRelationships) };

        auto task = xboxLiveContext->social_service().get_social_relationships();

        auto result{ task.get() };
        VERIFY_IS_TRUE(!result.err());

        auto& payload{ result.payload() };
        VERIFY_IS_TRUE(payload.has_next() == false);
        VERIFY_IS_TRUE(payload.total_count() == socialRelationships.size());
        auto items{ payload.items() };
        VERIFY_IS_TRUE(items.size() == socialRelationships.size());

        for (auto& item : items)
        {
            auto iter = std::find_if(socialRelationships.begin(), socialRelationships.end(),
                [&item](const SocialRelationship& expectedRelationship)
                {
                    return expectedRelationship.xuid == Utils::Uint64FromStringT(item.xbox_user_id());
                });
            VERIFY_IS_TRUE(iter != socialRelationships.end());
            VERIFY_ARE_EQUAL(iter->isFavorite, item.is_favorite());
            VERIFY_ARE_EQUAL(iter->isFollowingCaller, item.is_following_caller());
            VERIFY_ARE_EQUAL(iter->socialNetworks.size(), item.social_networks().size());

            for (size_t i = 0; i < item.social_networks().size(); ++i)
            {
                VERIFY_IS_TRUE(Utils::Stricmp(item.social_networks()[i], iter->socialNetworks[i]) == 0);
            }
        }
    }

    DEFINE_TEST_CASE(CppTestRTASocialRelationshipChange)
    {
        TEST_LOG(L"Test starting: CppTestRTASocialRelationshipChange");

        TestEnvironment env{};
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext(1);

        xboxLiveContext->real_time_activity_service()->activate();

        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            mockRtaService.CompleteSubscribeHandshake(n);

            // Immediately raise an event
            mockRtaService.RaiseEvent(uri, R"({ "NotificationType": "Removed", "Xuids": [ "2" ] })");
        });

        auto subscriptionResult = xboxLiveContext->social_service().subscribe_to_social_relationship_change(xboxLiveContext->xbox_live_user_id());
        VERIFY_IS_TRUE(!subscriptionResult.err());

        Event rtaMessageReceived;

        auto handlerToken = xboxLiveContext->social_service().add_social_relationship_changed_handler(
            [&rtaMessageReceived](social_relationship_change_event_args args)
            {
                VERIFY_IS_TRUE(args.caller_xbox_user_id() == _T("1"));
                VERIFY_IS_TRUE(args.social_notification() == social_notification_type::removed);
                VERIFY_ARE_EQUAL_INT(args.xbox_user_ids().size(), 1);
                VERIFY_IS_TRUE(args.xbox_user_ids()[0] == _T("2"));

                rtaMessageReceived.Set();
            });

        rtaMessageReceived.Wait();

        xboxLiveContext->social_service().remove_social_relationship_changed_handler(handlerToken);
        auto unsubscribeResult = xboxLiveContext->social_service().unsubscribe_from_social_relationship_change(subscriptionResult.payload());
        VERIFY_IS_TRUE(!unsubscribeResult.err());
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

