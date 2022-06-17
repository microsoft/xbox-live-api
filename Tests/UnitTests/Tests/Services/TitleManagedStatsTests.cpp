// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char* writeStatsJson = R"(
{
    "title": {
        "MyStatName1": {
            "value": 47
        },
        "MyStatName2": {
            "value": "47"
        }
    }
})";

const char* patchStatsJson = R"(
{
    "title": {
        "MyStatName1": {
            "value": "patch"
        }
    }
})";

const char* deleteStatsJson = R"(
{
    "title": {
        "MyStatName1": null
    }
})";

DEFINE_TEST_CLASS(TitleManagedStatsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(TitleManagedStatsTests);

    DEFINE_TEST_CASE(TestWriteStats)
    {
        TEST_LOG(L"Test starting: TestWriteStats");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto mock{ WriteStatsMock(writeStatsJson) };

        XblTitleManagedStatistic stats[]
        {
            { "MyStatName1", XblTitleManagedStatType::Number, 47 },
            { "MyStatName2", XblTitleManagedStatType::String, 0, "47" }
        };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleManagedStatsWriteAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            stats,
            _countof(stats),
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(m_wellFormedRequestReceived);
    }

    DEFINE_TEST_CASE(TestUpdateStats)
    {
        TEST_LOG(L"Test starting: TestUpdateStats");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto mock{ WriteStatsMock(patchStatsJson) };

        XblTitleManagedStatistic stat{ "MyStatName1", XblTitleManagedStatType::String, 0, "patch" };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleManagedStatsUpdateStatsAsync(
            xboxLiveContext.get(),
            &stat,
            1,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(m_wellFormedRequestReceived);
    }

    DEFINE_TEST_CASE(TestDeleteStats)
    {
        TEST_LOG(L"Test starting: TestDeleteStats");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto mock{ WriteStatsMock(deleteStatsJson) };

        const char* statName{ "MyStatName1" };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblTitleManagedStatsDeleteStatsAsync(
            xboxLiveContext.get(),
            &statName,
            1,
            &async
        ));

        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(m_wellFormedRequestReceived);
    }

private:
    bool m_wellFormedRequestReceived{ false };

    std::shared_ptr<HttpMock> WriteStatsMock(const char* expectedStatsJson) noexcept
    {
        m_wellFormedRequestReceived = false;

        auto mock = std::make_shared<HttpMock>("", "https://statswrite.xboxlive.com", 200);
        mock->SetMockMatchedCallback(
            [
                &, expectedStatsJson
            ]
        (HttpMock*, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
        {
            JsonDocument requestJson;
            requestJson.Parse(requestBody.data());
            if (!requestJson.HasParseError() && requestJson.HasMember("stats"))
            {
                m_wellFormedRequestReceived = VerifyJson(requestJson["stats"], expectedStatsJson);
            }
        });

        return mock;
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END