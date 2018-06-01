// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"Social"
#include "UnitTestIncludes.h"

#include "SocialGroupConstants_WinRT.h"
#include "RtaTestHelper.h"
#include "MultiplayerSessionReference_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Social;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring socialRelationshipChangedResponse =
LR"(
{
    "NotificationType": "Removed",
    "Xuids": [
        "2814662829937614"
    ]
})";

DEFINE_TEST_CLASS(SocialTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(SocialTests)

    struct SocialTestValues
    {
        Platform::String^ xuid;
        bool isFavorite;
        bool isFollowingCaller;
        Platform::String^ socialNetwork1;
        Platform::String^ socialNetwork2;
    };

    struct SocialTestCaseInput
    {
        SocialRelationship inputApiPersonView;
        int inputApiStartIndex;
        int inputApiMaxItems;
    };

    SocialTestCaseInput GetDefaultSocialInputData()
    {
        SocialTestCaseInput input;
        input.inputApiPersonView = SocialRelationship::All;
        input.inputApiStartIndex = 0;
        input.inputApiMaxItems = 0;

        return input;
    }

    SocialTestValues CreatePerson(
        _In_ int tagNum,
        _In_ std::wstring strTag
        )
    {
        SocialTestValues p;
        p.socialNetwork1 = L"social_test1";
        p.isFavorite = ((tagNum % 2) == 0);
        p.isFollowingCaller = ((tagNum % 3) == 0);
        uint64 xuid = 0x20000 + tagNum;
        p.xuid = xuid.ToString();
        return p;
    }

    web::json::value BuildPersonJson(
        _In_ SocialTestValues* social
        )
    {
        web::json::value jsonObject = web::json::value::object();
        jsonObject[L"xuid"] = web::json::value::string(social->xuid->Data());
        jsonObject[L"isFavorite"] = web::json::value::boolean(social->isFavorite);
        jsonObject[L"isFollowingCaller"] = web::json::value::boolean(social->isFollowingCaller);

        if (social->socialNetwork1 != nullptr ||
            social->socialNetwork2 != nullptr)
        {
            web::json::value jsonSocialNetworkArray = web::json::value::array();
            int i = 0;
            if (social->socialNetwork1 != nullptr)
            {
                jsonSocialNetworkArray[i++] = web::json::value::string(social->socialNetwork1->Data());
            }
            if (social->socialNetwork2 != nullptr)
            {
                jsonSocialNetworkArray[i++] = web::json::value::string(social->socialNetwork2->Data());
            }

            jsonObject[L"socialNetworks"] = jsonSocialNetworkArray;
        }

        return jsonObject;
    }

    web::json::value BuildSocialJsonArray(
        _In_ std::vector<SocialTestValues> socialList
        )
    {
        auto jsonArray = web::json::value::array();
        int i = 0;
        for (SocialTestValues p : socialList)
        {
            auto jsonResponse = BuildPersonJson(&p);
            jsonArray[i++] = jsonResponse;
        }

        auto jsonObject = web::json::value::object();
        jsonObject[L"people"] = jsonArray;
        jsonObject[L"totalCount"] = web::json::value::number(static_cast<double>(jsonArray.size()));
        return jsonObject;
    }

    void VerifyPersonMatch(
        _In_ XboxSocialRelationship^ xboxPersonRelationship,
        _In_ const SocialTestValues* p
        )
    {
        VERIFY_ARE_EQUAL(p->isFavorite, xboxPersonRelationship->IsFavorite);
        VERIFY_ARE_EQUAL(p->isFollowingCaller, xboxPersonRelationship->IsFollowingCaller);
        VERIFY_ARE_EQUAL_STR(p->xuid->Data(), xboxPersonRelationship->XboxUserId->Data());
    }

    void TestGetSocialAsyncByTestCase(
        _In_ SocialTestCaseInput inputData
        )
    {
        std::vector<SocialTestValues> socialList;
        for (int tag = 1; tag<3; tag++)
        {
            SocialTestValues p = CreatePerson(tag, FormatString(L"%d", tag));
            socialList.push_back(p);
        }
        auto jsonResponse = BuildSocialJsonArray(socialList);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(jsonResponse);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        {
            auto async = xboxLiveContext->SocialService->GetSocialRelationshipsAsync();
            VERIFY_IS_NOT_NULL(async);
            VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
            VERIFY_ARE_EQUAL_STR(L"https://social.mockenv.xboxlive.com", httpCall->ServerName);
            VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people", httpCall->PathQueryFragment.to_string());

            auto result = create_task(async).get();
            IVectorView<XboxSocialRelationship^>^ list = result->Items;
            VERIFY_IS_NOT_NULL(list);

            int index = 0;
            for (XboxSocialRelationship^ xboxPersonRelationship : list)
            {
                VerifyPersonMatch(xboxPersonRelationship, &socialList[index++]);
            }
        }

       {
            auto async = xboxLiveContext->SocialService->GetSocialRelationshipsAsync(
                inputData.inputApiPersonView
                );
            VERIFY_IS_NOT_NULL(async);
            VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
            VERIFY_ARE_EQUAL_STR(L"https://social.mockenv.xboxlive.com", httpCall->ServerName);
            if (inputData.inputApiPersonView == SocialRelationship::All)
            {
                VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people", httpCall->PathQueryFragment.to_string());
            }
            else if(inputData.inputApiPersonView == SocialRelationship::Favorite)
            {
                VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people?view=Favorite", httpCall->PathQueryFragment.to_string());
            }
            else if (inputData.inputApiPersonView == SocialRelationship::LegacyXboxLiveFriends)
            {
                VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people?view=LegacyXboxLiveFriends", httpCall->PathQueryFragment.to_string());
            }
            else
            {
                throw ref new Platform::Exception(E_FAIL, "uncovered relationship");
            }


            auto result = create_task(async).get();
            IVectorView<XboxSocialRelationship^>^ list = result->Items;
            VERIFY_IS_NOT_NULL(list);

            int index = 0;
            for (XboxSocialRelationship^ xboxPersonRelationship : list)
            {
                VerifyPersonMatch(xboxPersonRelationship, &socialList[index++]);
            }
        }

        {
            auto async = xboxLiveContext->SocialService->GetSocialRelationshipsAsync(
                inputData.inputApiPersonView,
                inputData.inputApiStartIndex,
                inputData.inputApiMaxItems
                );
            VERIFY_IS_NOT_NULL(async);
            VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
            VERIFY_ARE_EQUAL_STR(L"https://social.mockenv.xboxlive.com", httpCall->ServerName);
            if (inputData.inputApiPersonView == SocialRelationship::All)
            {
                VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people?startIndex=20&maxItems=100", httpCall->PathQueryFragment.to_string());
            }
            else if (inputData.inputApiPersonView == SocialRelationship::Favorite)
            {
                VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people?view=Favorite&startIndex=20&maxItems=100", httpCall->PathQueryFragment.to_string());
            }
            else if (inputData.inputApiPersonView == SocialRelationship::LegacyXboxLiveFriends)
            {
                VERIFY_ARE_EQUAL_STR(L"/users/xuid(TestXboxUserId)/people?view=LegacyXboxLiveFriends&startIndex=20&maxItems=100", httpCall->PathQueryFragment.to_string());
            }
            else
            {
                throw ref new Platform::Exception(E_FAIL, "uncovered relationship");
            }

            auto result = create_task(async).get();
            IVectorView<XboxSocialRelationship^>^ list = result->Items;
            VERIFY_IS_NOT_NULL(list);

            int index = 0;
            for (XboxSocialRelationship^ xboxPersonRelationship : list)
            {
                VerifyPersonMatch(xboxPersonRelationship, &socialList[index++]);
            }
        }
    }

    DEFINE_TEST_CASE(TestGetSocialAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetSocialAsync);
        SocialTestCaseInput input = GetDefaultSocialInputData();
        input.inputApiMaxItems = 100;
        input.inputApiStartIndex = 20;

        // SocialRelationship::All
        TestGetSocialAsyncByTestCase(input);

        // SocialRelationship::Favorite
        input.inputApiPersonView = SocialRelationship::Favorite;
        TestGetSocialAsyncByTestCase(input);

        // SocialRelationship::LegacyXboxLiveFriends
        input.inputApiPersonView = SocialRelationship::LegacyXboxLiveFriends;
        TestGetSocialAsyncByTestCase(input);
    }

    DEFINE_TEST_CASE(TestRTASocialRelationshipChange)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRTASocialRelationshipChange);
        const int subId = 321;
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, web::json::value::null().serialize(), subId);
        
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xboxLiveContext->RealTimeActivityService->Activate();
        LOG_INFO("RTA activate");
        helper->connectedEvent.wait();

        bool didFire = false;

        concurrency::event fireEvent;
        auto sessionChangeEvent = xboxLiveContext->SocialService->SocialRelationshipChanged += 
        ref new Windows::Foundation::EventHandler<SocialRelationshipChangeEventArgs^>([this, &fireEvent, &didFire](Platform::Object^, SocialRelationshipChangeEventArgs^ args)
        {
            didFire = true;
            VERIFY_ARE_EQUAL(args->CallerXboxUserId->Data(), string_t(L"TestUser"));
            VERIFY_ARE_EQUAL(args->SocialNotification.ToString()->Data(), string_t(L"removed"));
            VERIFY_ARE_EQUAL(args->XboxUserIds->GetAt(0)->Data(), string_t(L"2814662829937614"));
            fireEvent.set();
        });

        auto socialRelationshipSubscription = xboxLiveContext->SocialService->SubscribeToSocialRelationshipChange(
            ref new Platform::String(_T("TestUser"))
            );

        string_t socialSubUri = socialRelationshipSubscription->ResourceUri->Data();
        VERIFY_ARE_EQUAL(socialSubUri, L"http://social.xboxlive.com/users/xuid(TestUser)/friends");

        TEST_LOG(L"Calling receive_rta_event");
        mockSocket->receive_rta_event(subId, web::json::value::parse(socialRelationshipChangedResponse).serialize());
        LOG_INFO("Fire event");
        fireEvent.wait();
        TEST_LOG(L"Done calling receive_rta_event");

        VERIFY_IS_TRUE(socialRelationshipSubscription->State == Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivitySubscriptionState::Subscribed);
        VERIFY_ARE_EQUAL_STR(socialRelationshipSubscription->ResourceUri->Data(), L"http://social.xboxlive.com/users/xuid(TestUser)/friends");
        VERIFY_ARE_EQUAL_STR(socialRelationshipSubscription->XboxUserId->Data(), L"TestUser");
        VERIFY_ARE_EQUAL_INT(socialRelationshipSubscription->SubscriptionId, 321);

        fireEvent.reset();

        TEST_LOG(L"Calling UnsubscribeFromSocialRelationshipChange");
        didFire = false;
        xboxLiveContext->SocialService->UnsubscribeFromSocialRelationshipChange(
            socialRelationshipSubscription
            );
        TEST_LOG(L"Done calling UnsubscribeFromSocialRelationshipChange");

        TEST_LOG(L"Calling receive_rta_event");
        mockSocket->receive_rta_event(subId, web::json::value::parse(socialRelationshipChangedResponse).serialize());
        TEST_LOG(L"Done calling receive_rta_event");
        VERIFY_IS_FALSE(didFire);
    }

    DEFINE_TEST_CASE(TestSubmitReputationFeedbackAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSubmitReputationFeedbackAsync);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        Platform::String^ xboxUserId = L"1";
        ReputationFeedbackType reputationFeedbackType = ReputationFeedbackType::FairPlayCheater;
        Platform::String^ sessionName = L"2";
        Platform::String^ reasonMessage = L"3";
        Platform::String^ evidenceResourceId = L"4";

        auto async = xboxLiveContext->ReputationService->SubmitReputationFeedbackAsync(
            xboxUserId,
            reputationFeedbackType,
            sessionName,
            reasonMessage,
            evidenceResourceId
            );

        VERIFY_IS_NOT_NULL(async);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://reputation.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(1)/feedback", httpCall->PathQueryFragment.to_string());
        string_t jsonString = LR"({"evidenceId":"4","feedbackType":"FairPlayCheater","sessionName":"2","textReason":"3"})";
        VERIFY_IS_EQUAL_JSON_FROM_STRINGS(jsonString, httpCall->request_body().request_message_string());
        create_task(async).get();
    }

    DEFINE_TEST_CASE(TestSubmitBatchReputationFeedbackAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSubmitBatchReputationFeedbackAsync);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        Platform::String^ xboxUserId = L"1";
        ReputationFeedbackType reputationFeedbackType = ReputationFeedbackType::FairPlayCheater;
        Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ sessionRef = nullptr;
        Platform::String^ reasonMessage = L"2";
        Platform::String^ evidenceResourceId = L"3";

        Platform::Collections::Vector<ReputationFeedbackItem^>^ feedbackItems = ref new Platform::Collections::Vector<ReputationFeedbackItem^>();
#pragma warning(suppress: 6387)
        ReputationFeedbackItem^ feedbackItem = ref new ReputationFeedbackItem(
            xboxUserId, 
            reputationFeedbackType, 
            sessionRef, 
            reasonMessage, 
            evidenceResourceId
            );
        feedbackItems->Append(feedbackItem);

        auto async = xboxLiveContext->ReputationService->SubmitBatchReputationFeedbackAsync(
            feedbackItems->GetView()
            );
        VERIFY_IS_NOT_NULL(async);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://reputation.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/batchtitlefeedback", httpCall->PathQueryFragment.to_string());

        string_t jsonString = LR"({"items":[{"evidenceId":"3","feedbackType":"FairPlayCheater","sessionRef":null,"targetXuid":"1","textReason":"2","titleId":null}]})";

        TEST_LOG(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()).c_str());
        VERIFY_IS_EQUAL_JSON_FROM_STRINGS(jsonString, httpCall->request_body().request_message_string());
        create_task(async).get();
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

