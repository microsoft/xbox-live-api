// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

using namespace xbox::services::social;
using namespace xbox::services::multiplayer;

// response sample from http://xboxwiki/wiki/Reputation#Submit_Feedback
const char expectedBatchRequestBody[] = R"({
    "items" :
    [
        {
            "targetXuid": "33445566778899",
            "titleId" : null,
            "sessionRef":
            {
                "scid": "372D829B-FA8E-471F-B696-07B61F09EC20",
                "templateName": "CaptureFlag5",
                "name": "Halo556932"
            },
            "feedbackType": "FairPlayKillsTeammates",
            "textReason": "Title detected this player killing team members 19 times in this session",
            "evidenceId": null
        },
        {
            "targetXuid": "33445566778899",
            "titleId" : null,
            "sessionRef":
            {
                "scid": "372D829B-FA8E-471F-B696-07B61F09EC20",
                "templateName": "CaptureFlag5",
                "name": "Halo556932"
            },
            "feedbackType": "FairPlayQuitter",
            "textReason": "Title detected quitting before the session was over",
            "evidenceId": null
        }
    ]
})";

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(ReputationTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(ReputationTests);

    DEFINE_TEST_CASE(TestSubmitReputationFeedbackAsync)
    {
        TEST_LOG(L"Test starting: TestSubmitReputationFeedbackAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto reputationMock = std::make_shared<HttpMock>( "POST", "https://reputation.xboxlive.com", 202 );

        bool requestWellFormed{ true };
        reputationMock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string url, xsapi_internal_string requestBody)
            {
                requestWellFormed &= url == "https://reputation.xboxlive.com/users/xuid(1)/feedback";

                // response sample from http://xboxwiki/wiki/Reputation#Submit_Feedback
                auto expectedBody = R"({
                    "sessionRef":
                    {
                        "scid": "372D829B-FA8E-471F-B696-07B61F09EC20",
                        "templateName": "CaptureFlag5",
                        "name": "Halo556932"
                    },
                    "feedbackType": "CommsAbusiveVoice",
                    "textReason": "They called me a doodoo-head!",
                    "evidenceId": null
                })";

                JsonDocument expectedBodyJson;
                JsonDocument requestBodyJson;
                expectedBodyJson.Parse(expectedBody);
                requestBodyJson.Parse(requestBody.c_str());

                requestWellFormed &= expectedBodyJson == requestBodyJson;
            });

        XAsyncBlock async{};
        XblMultiplayerSessionReference sessionRef{ "372D829B-FA8E-471F-B696-07B61F09EC20", "CaptureFlag5", "Halo556932" };

        XblSocialSubmitReputationFeedbackAsync(
            xboxLiveContext.get(),
            1,
            XblReputationFeedbackType::CommunicationsAbusiveVoice,
            &sessionRef,
            "They called me a doodoo-head!",
            nullptr,
            &async
        );

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(CppTestSubmitReputationFeedback)
    {
        TEST_LOG(L"Test starting: CppTestSubmitReputationFeedback");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();

        auto reputationMock = std::make_shared<HttpMock>( "POST", "https://reputation.xboxlive.com", 202 );

        bool requestWellFormed{ true };

        reputationMock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string url, xsapi_internal_string requestBody)
            {
                requestWellFormed &= url == "https://reputation.xboxlive.com/users/xuid(1)/feedback";

                // response sample from http://xboxwiki/wiki/Reputation#Submit_Feedback
                auto expectedBody = R"({
                    "sessionRef": null,
                    "feedbackType": "CommsAbusiveVoice",
                    "textReason": "They called me a doodoo-head!",
                    "evidenceId": null
                })";

                JsonDocument expectedBodyJson;
                JsonDocument requestBodyJson;
                expectedBodyJson.Parse(expectedBody);
                requestBodyJson.Parse(requestBody.c_str());

                requestWellFormed &= expectedBodyJson == requestBodyJson;
            });

        auto task = xboxLiveContext->reputation_service().submit_reputation_feedback(
            _T("1"),
            reputation_feedback_type::communications_abusive_voice,
            string_t{},
            _T("They called me a doodoo-head!")
        );

        VERIFY_IS_TRUE(!task.get().err());
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(TestSubmitBatchReputationFeedbackAsync)
    {
        TEST_LOG(L"Test starting: TestSubmitBatchReputationFeedbackAsync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto reputationMock = std::make_shared<HttpMock>("POST", "https://reputation.xboxlive.com", 202 );

        bool requestWellFormed{ true };
        reputationMock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string url, xsapi_internal_string requestBody)
            {
                requestWellFormed &= url == "https://reputation.xboxlive.com/users/batchtitlefeedback";
                JsonDocument expectedBodyJson;
                JsonDocument requestBodyJson;
                expectedBodyJson.Parse(expectedBatchRequestBody);
                requestBodyJson.Parse(requestBody.c_str());

                requestWellFormed &= expectedBodyJson == requestBodyJson;
            });

        XAsyncBlock async{};
        XblMultiplayerSessionReference sessionRef{ "372D829B-FA8E-471F-B696-07B61F09EC20", "CaptureFlag5", "Halo556932" };

        std::vector<XblReputationFeedbackItem> feedbackItems;
        feedbackItems.push_back(XblReputationFeedbackItem{
            33445566778899,
            XblReputationFeedbackType::FairPlayKillsTeammates,
            &sessionRef,
            "Title detected this player killing team members 19 times in this session",
            nullptr
        });
        feedbackItems.push_back(XblReputationFeedbackItem{
            33445566778899,
            XblReputationFeedbackType::FairPlayQuitter,
            &sessionRef,
            "Title detected quitting before the session was over",
            nullptr
        });

        XblSocialSubmitBatchReputationFeedbackAsync(
            xboxLiveContext.get(),
            feedbackItems.data(),
            feedbackItems.size(),
            &async
        );

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(CppTestSubmitBatchReputationFeedback)
    {
        TEST_LOG(L"Test starting: CppTestSubmitBatchReputationFeedback");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();

        auto reputationMock = std::make_shared<HttpMock>("POST", "https://reputation.xboxlive.com", 202 );

        bool requestWellFormed{ true };
        reputationMock->SetMockMatchedCallback(
            [&](HttpMock*, xsapi_internal_string url, xsapi_internal_string requestBody)
            {
                requestWellFormed &= url == "https://reputation.xboxlive.com/users/batchtitlefeedback";
                JsonDocument expectedBodyJson;
                JsonDocument requestBodyJson;
                expectedBodyJson.Parse(expectedBatchRequestBody);
                requestBodyJson.Parse(requestBody.c_str());

                requestWellFormed &= expectedBodyJson == requestBodyJson;
            });

        multiplayer_session_reference sessionRef{ _T("372D829B-FA8E-471F-B696-07B61F09EC20"), _T("CaptureFlag5"), _T("Halo556932") };

        std::vector<reputation_feedback_item> feedbackItems;
        feedbackItems.push_back(reputation_feedback_item{
            _T("33445566778899"),
            reputation_feedback_type::fair_play_kills_teammates,
            sessionRef,
            _T("Title detected this player killing team members 19 times in this session"),
        });
        feedbackItems.push_back(reputation_feedback_item{
            _T("33445566778899"),
            reputation_feedback_type::fair_play_quitter,
            sessionRef,
            _T("Title detected quitting before the session was over"),
        });

        auto task = xboxLiveContext->reputation_service().submit_batch_reputation_feedback(feedbackItems);

        VERIFY_IS_TRUE(!task.get().err());
        VERIFY_IS_TRUE(requestWellFormed);
    }

    DEFINE_TEST_CASE(TestSubmitReputationFeedbackAsyncInvalidArgs)
    {
        TEST_LOG(L"Test starting: TestSubmitReputationFeedbackAsyncInvalidArgs");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        XAsyncBlock async{};
        auto hr = XblSocialSubmitReputationFeedbackAsync(
            xboxLiveContext.get(),
            1,
            XblReputationFeedbackType::FairPlayCheater,
            nullptr,
            nullptr, // Invalid arg
            nullptr,
            &async
        );
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);

        hr = XblSocialSubmitReputationFeedbackAsync(
            xboxLiveContext.get(),
            1,
            XblReputationFeedbackType::FairPlayCheater,
            nullptr,
            "Reason message",
            nullptr,
            nullptr // Invalid arg
        );
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);
    }
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
