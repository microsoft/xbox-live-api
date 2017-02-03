//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#define TEST_CLASS_OWNER L"blgross"
#define TEST_CLASS_AREA L"SimplfiedStatService"
#include "UnitTestIncludes.h"
#include "xsapi/stats_manager.h"
#include "Stats/Manager/stats_manager_internal.h"
#include "xbox_live_context_impl.h"
#include "StatsManagerHelper.h"

using namespace xbox::services;
using namespace xbox::services::experimental::stats::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

using namespace xbox::services::experimental::stats::manager;

static xbox::services::experimental::stats::manager::simplified_stats_service GetSimplifiedStatsService()
{
    auto xblContext = GetMockXboxLiveContext_WinRT();
    auto xblContextImpl = std::make_shared<xbox::services::xbox_live_context_impl>(xblContext->User);
    xblContextImpl->init();

    return xbox::services::experimental::stats::manager::simplified_stats_service(
        xblContextImpl->user_context(),
        xblContextImpl->settings(),
        xblContextImpl->application_config()
        );
}

DEFINE_TEST_CLASS(SimplifiedStatServiceTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(SimplifiedStatServiceTests)

    void VerifyStatAreEqual(
        const string_t& statName,
        const stat_value& stat,
        web::json::object& titleStatsList
        )
    {
        auto titleStat = titleStatsList[statName.c_str()];
        switch (stat.data_type())
        {
            case stat_data_type::number:
                VERIFY_IS_TRUE(titleStat[L"value"].as_double() == stat.as_number());
                break;
            case stat_data_type::string:
                VERIFY_IS_TRUE(titleStat[L"value"].as_string() == stat.as_string());
                break;
        }

        auto titleOp = titleStat[L"op"].as_string();
        VERIFY_IS_TRUE(titleOp == L"replace");
    }

    stats_value_document GetStatValueDocument(simplified_stats_service& simplifiedStatService, const std::shared_ptr<MockHttpCall>& httpCall, const string_t& jsonValue)
    {
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(statValueDocumentResponse));
        auto statValueDocumentResult = simplifiedStatService.get_stats_value_document().get();
        VERIFY_IS_TRUE(!statValueDocumentResult.err());
        return statValueDocumentResult.payload();
    }

    DEFINE_TEST_CASE(SimplifiedStatServiceGetSVD)
    {
        DEFINE_TEST_CASE_PROPERTIES(SimplifiedStatServiceGetSVD);
        auto simplifiedStatService = GetSimplifiedStatsService();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        auto statValueDocument = GetStatValueDocument(simplifiedStatService, httpCall, statValueDocumentResponse);
        auto statJSON = web::json::value::parse(statValueDocumentResponse);

        auto versionNumCompare = statJSON[L"ver"].as_integer();
        VERIFY_ARE_EQUAL_INT(statValueDocument.version(), versionNumCompare);

        auto statEnvelopedField = statJSON[L"envelope"];
        VERIFY_ARE_EQUAL_INT(statValueDocument.client_version(), statEnvelopedField[L"clientVersion"].as_integer());
        VERIFY_ARE_EQUAL_INT(statValueDocument.server_version(), statEnvelopedField[L"serverVersion"].as_integer());
        VERIFY_ARE_EQUAL_STR(statValueDocument.client_id().c_str(), statEnvelopedField[L"clientId"].as_string());
        auto statField = statJSON[L"stats"];
        auto titleStatsList = statField[L"title"].as_object();

        std::vector<string_t> statNames;
        statValueDocument.get_stat_names(statNames);
        VERIFY_IS_TRUE(titleStatsList.size() == statNames.size());
        for (auto& statName : statNames)
        {
            auto& statValueResult = statValueDocument.get_stat(statName.c_str());
            VERIFY_IS_TRUE(!statValueResult.err());
            VerifyStatAreEqual(statName, *statValueResult.payload(), titleStatsList);
        }

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://statsread.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/stats/users/TestXboxUserId/scids/MockScid",
            httpCall->PathQueryFragment.to_string()
            );
    }

    bool VerifyStatJSON(
        web::json::value& compareA,
        web::json::value& compareB
        )
    {
        bool result = true;
        result &= compareA[L"value"].serialize() == compareB[L"value"].serialize();
        result &= compareB[L"op"].as_string() == compareB[L"op"].as_string();

        return result;
    }

    DEFINE_TEST_CASE(SimplifiedStatServicePostSVD)
    {
        DEFINE_TEST_CASE_PROPERTIES(SimplifiedStatServicePostSVD);
        auto simplifiedStatService = GetSimplifiedStatsService();
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();

        auto statsValueDoc = GetStatValueDocument(simplifiedStatService, httpCall, statValueDocumentResponseWithContext);
        statsValueDoc.set_stat(L"headshots", 8.f);
        statsValueDoc.do_work();
        auto updateStatResult = simplifiedStatService.update_stats_value_document(statsValueDoc).get();
        VERIFY_IS_TRUE(!updateStatResult.err());
        auto& serializedRequest = web::json::value::parse(httpCall->request_body().request_message_string());
        auto& compareValue = web::json::value::parse(statValueDocumentResponse);
        
        auto& versionField = serializedRequest[_T("ver")];
        auto& compareVersion = compareValue[_T("ver")];

        VERIFY_IS_TRUE(versionField.as_integer() == compareVersion.as_integer());

        VERIFY_IS_TRUE(serializedRequest.has_field(_T("timestamp")));
        auto& envelopeField = serializedRequest[_T("envelope")];
        auto& envelopeFieldCompare = compareValue[_T("envelope")];

        VERIFY_IS_TRUE(envelopeField[L"serverVersion"].as_integer() == envelopeFieldCompare[L"serverVersion"].as_integer());
        VERIFY_IS_TRUE(envelopeField[L"clientVersion"].as_integer() == envelopeFieldCompare[L"clientVersion"].as_integer());
        VERIFY_IS_TRUE(envelopeField[L"clientId"].as_string() == envelopeFieldCompare[L"clientId"].as_string());

        auto& statsField = serializedRequest[L"stats"];
        auto& statsFieldCompare = compareValue[L"stats"];
        auto& contextualKeysField = statsField[L"contextualKeys"].as_object();
        auto& contextualKeysFieldCompare = statsFieldCompare[L"contextualKeys"].as_object();
        for (auto& contextualKey : contextualKeysField)
        {
            bool isValid = false;
            for (auto& contextualKeyCompare : contextualKeysFieldCompare)
            {
                if (contextualKey.first == contextualKeyCompare.first && contextualKey.second.as_string() == contextualKeyCompare.second.as_string())
                {
                    isValid = true;
                    break;
                }
            }

            VERIFY_IS_TRUE(isValid);
        }

        auto& titleField = statsField[L"title"].as_object();
        auto& titleFieldCompare = statsFieldCompare[L"title"].as_object();
        auto& headshotField = titleFieldCompare[L"headshots"];
        headshotField[L"value"] = 8;

        for (auto& titleKey : titleField)
        {
            bool isValid = false;
            for (auto& titleFieldCompare : titleFieldCompare)
            {
                if (titleKey.first == titleFieldCompare.first && VerifyStatJSON(titleKey.second, titleFieldCompare.second))
                {
                    isValid = true;
                    break;
                }
            }

            VERIFY_IS_TRUE(isValid);
        }

        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://statswrite.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(
            L"/stats/users/TestXboxUserId/scids/MockScid",
            httpCall->PathQueryFragment.to_string()
            );
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END