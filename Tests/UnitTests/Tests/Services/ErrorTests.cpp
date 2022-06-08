// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(ErrorTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(ErrorTests)

    DEFINE_TEST_CASE(TestHttpErrors)
    {
        TEST_LOG(L"Test starting: TestHttpErrors");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        HttpMock mock{ "", "https://achievements.xboxlive.com", 404 };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            MOCK_SCID,
            "AchievementId",
            &async
        ));
        auto hr = XAsyncGetStatus(&async, true);
        VERIFY_ARE_EQUAL(hr, HTTP_E_STATUS_NOT_FOUND);

        mock.SetResponseHttpStatus(200);
        mock.SetResponseBody("{\"asdfasdf\":1234}");

        ZeroMemory(&async, sizeof(XAsyncBlock));
        VERIFY_SUCCEEDED(XblAchievementsGetAchievementAsync(
            xboxLiveContext.get(),
            xboxLiveContext->Xuid(),
            MOCK_SCID,
            "AchievementId",
            &async
        ));
        hr = XAsyncGetStatus(&async, true);
        VERIFY_ARE_EQUAL(hr, WEB_E_INVALID_JSON_STRING);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
