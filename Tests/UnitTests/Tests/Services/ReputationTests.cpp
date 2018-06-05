// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"Reputation"
#include "UnitTestIncludes.h"

#include "Utils_WinRT.h"
#include "WinRT/ReputationFeedbackType_WinRT.h"
#include "xsapi\social.h"
#include "social_internal.h"

// TODO 718292: HTTP error handling
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::system;
using namespace xbox::services::social;
using namespace Microsoft::Xbox::Services::Social;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

struct ReputationTestValues
{
    Platform::String^ xboxUserId;
    Windows::Foundation::DateTime dateCreated;
    uint32 overallReputation;
    uint32 fairplayReputation;
    uint32 commsReputation;
    uint32 ugcReputation;
};

const std::wstring defaultUserReputationData = 
LR"(
{
    "items":
    [
    { 
        "xuid" : "98052",
        "dateCreated" : "2012-06-13T16:22:21Z", 
        "overallReputation" : 3, 
        "fairplayReputation" : 5, 
        "commsReputation" : 2,
        "ugcReputation" : 1
    },
    {
        "xuid" : "98052",
        "dateCreated" : "2012-06-15T21:15:07Z", 
        "overallReputation" : 9, 
        "fairplayReputation" : 8, 
        "commsReputation" : 0,
        "ugcReputation" : 1
    }
    ]

}
)";

DEFINE_TEST_CLASS(ReputationTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(ReputationTests)

    void TestSubmitReputationFeedback(
        ReputationFeedbackType reputationFeedback, 
        Platform::String^ sessionName, 
        Platform::String^ reasonMessage, 
        Platform::String^ evidenceResourceId
        )
    {
        auto responseJson = web::json::value::parse(defaultUserReputationData);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        create_task(xboxLiveContext->ReputationService->SubmitReputationFeedbackAsync(
            "98052",
            reputationFeedback,
            sessionName,
            reasonMessage,
            evidenceResourceId
            )
            ).wait();
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://reputation.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/users/xuid(98052)/feedback", httpCall->PathQueryFragment.to_string());

        auto requestJson = web::json::value::parse(utils::string_t_from_internal_string(httpCall->request_body().request_message_string()));
        auto feedbackRequest = reputation_feedback_request::convert_reputation_feedback_type_to_string(static_cast<reputation_feedback_type>(reputationFeedback));
        VERIFY_IS_TRUE(!feedbackRequest.err() && !feedbackRequest.payload().empty());
        if (!sessionName->IsEmpty())
        {
            VERIFY_ARE_EQUAL_STR(sessionName->Data(), requestJson[L"sessionName"].as_string());
        }
        if (!reasonMessage->IsEmpty())
        {
            VERIFY_ARE_EQUAL_STR(reasonMessage->Data(), requestJson[L"textReason"].as_string());
        }
        if (!evidenceResourceId->IsEmpty())
        {
            VERIFY_ARE_EQUAL_STR(evidenceResourceId->Data(), requestJson[L"evidenceId"].as_string());
        }
    }

    DEFINE_TEST_CASE(TestSubmitReputationFeedbackAsync)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSubmitReputationFeedbackAsync);
        Platform::String^ sessionName = L"SecretSession";
        Platform::String^ reasonMessage = L"Yelled Loudly";
        Platform::String^ evidenceResourceId= L"99991999";

        TestSubmitReputationFeedback(ReputationFeedbackType::CommunicationsAbusiveVoice, sessionName, reasonMessage, evidenceResourceId);
        TestSubmitReputationFeedback(ReputationFeedbackType::CommunicationsInappropriateVideo, nullptr, reasonMessage, evidenceResourceId);
        TestSubmitReputationFeedback(ReputationFeedbackType::FairPlayCheater, sessionName, nullptr, evidenceResourceId);
        TestSubmitReputationFeedback(ReputationFeedbackType::FairPlayKicked, sessionName, reasonMessage, nullptr);
        TestSubmitReputationFeedback(ReputationFeedbackType::FairPlayKillsTeammates, nullptr, nullptr, evidenceResourceId);
        TestSubmitReputationFeedback(ReputationFeedbackType::FairPlayQuitter, nullptr, reasonMessage, nullptr);
        TestSubmitReputationFeedback(ReputationFeedbackType::FairPlayTampering, sessionName, nullptr, nullptr);
        TestSubmitReputationFeedback(ReputationFeedbackType::InappropriateUserGeneratedContent, nullptr, nullptr, nullptr);
        TestSubmitReputationFeedback(ReputationFeedbackType::PositiveHelpfulPlayer, sessionName, reasonMessage, evidenceResourceId);
        TestSubmitReputationFeedback(ReputationFeedbackType::PositiveHighQualityUserGeneratedContent, sessionName, nullptr, evidenceResourceId);
        TestSubmitReputationFeedback(ReputationFeedbackType::PositiveSkilledPlayer, sessionName, reasonMessage, evidenceResourceId);
    }

    DEFINE_TEST_CASE(TestSubmitReputationFeedbackAsyncInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSubmitReputationFeedbackAsyncInvalidArgs);
        auto responseJson = web::json::value::parse(defaultUserReputationData);
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ReputationService->SubmitReputationFeedbackAsync(
                nullptr,    // invalid arg
                ReputationFeedbackType::FairPlayCheater,
                nullptr,
                nullptr,
                nullptr
            )).get(),
            E_INVALIDARG
        )

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ReputationService->SubmitReputationFeedbackAsync(
                L"",    // invalid arg
                ReputationFeedbackType::FairPlayCheater,
                nullptr,
                nullptr,
                nullptr
            )).get(),
            E_INVALIDARG
        )
        
        Platform::String^ emptyXboxUserId;
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ReputationService->SubmitReputationFeedbackAsync(
                emptyXboxUserId,    // invalid arg
                ReputationFeedbackType::FairPlayCheater,
                nullptr,
                nullptr,
                nullptr
            )).get(),
            E_INVALIDARG
        )

        Platform::String^ xuid = L"98052";

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ReputationService->SubmitReputationFeedbackAsync(
                xuid,
                static_cast<ReputationFeedbackType>((int)ReputationFeedbackType::FairPlayKillsTeammates - 1),   // invalid arg
                nullptr,
                nullptr,
                nullptr
            )).get(),
            E_INVALIDARG
        )

        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->ReputationService->SubmitReputationFeedbackAsync(
                xuid,
                static_cast<ReputationFeedbackType>((int)ReputationFeedbackType::FairPlayLeaderboardCheater + 1),  // invalid arg
                nullptr,
                nullptr,
                nullptr
            )).get(),
            E_INVALIDARG
        )
    }

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
