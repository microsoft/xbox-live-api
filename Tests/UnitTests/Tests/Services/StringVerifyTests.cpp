// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char* url = "https://client-strings.xboxlive.com/system/strings/validate";

const char* defaultStringVerifyResult =
R"({
        "verifyStringResult":
        [
            {
                "resultCode": 0
            },
            {
                "resultCode": 1,
                "offendingString":"sdfasdf"
            },
            {
                "resultCode": 7,
                "offendingString":"whatasfkjasl"
            }
        ]
})";

DEFINE_TEST_CLASS(StringVerifyTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(StringVerifyTests);

    DEFINE_TEST_CASE(TestVerifyString)
    {
        TEST_LOG(L"Test starting: TestVerifyString");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("POST", url, 200);
        mock->SetResponseBody(defaultStringVerifyResult);

        auto requestStr = "xboxUserId";
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed, requestStr](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument requestJson;
                requestJson.Parse(requestBody.c_str());
                auto requestStrJson = requestJson["stringsToVerify"].GetArray();

                requestWellFormed &= requestStrJson.Size() == 1;
                requestWellFormed &= strcmp(requestStr, requestStrJson[0].GetString()) == 0;
            }
        );

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblStringVerifyStringAsync(xboxLiveContext.get(), requestStr, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblStringVerifyStringResultSize(&async, &resultSize));
        
        std::shared_ptr<char> buffer(new char[resultSize], std::default_delete<char[]>());
        XblVerifyStringResult* result{};
        VERIFY_SUCCEEDED(XblStringVerifyStringResult(&async, resultSize, buffer.get(), &result, nullptr));
        VERIFY_ARE_EQUAL_INT(XblVerifyStringResultCode::Success, result->resultCode);
        VERIFY_IS_NULL(result->firstOffendingSubstring);
    }

    DEFINE_TEST_CASE(TestVerifyStringWithLargeBuffer)
    {
        TEST_LOG(L"Test starting: TestVerifyStringWithLargeBuffer");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("POST", url, 200);
        mock->SetResponseBody(defaultStringVerifyResult);

        auto requestStr = "xboxUserId";
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed, requestStr](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
        {
            UNREFERENCED_PARAMETER(mock);
            UNREFERENCED_PARAMETER(requestUrl);

            JsonDocument requestJson;
            requestJson.Parse(requestBody.c_str());
            auto requestStrJson = requestJson["stringsToVerify"].GetArray();

            requestWellFormed &= requestStrJson.Size() == 1;
            requestWellFormed &= strcmp(requestStr, requestStrJson[0].GetString()) == 0;
        }
        );

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblStringVerifyStringAsync(xboxLiveContext.get(), requestStr, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblStringVerifyStringResultSize(&async, &resultSize));

        size_t bufferUsed{};
        std::shared_ptr<char> buffer(new char[resultSize * 2], std::default_delete<char[]>());
        XblVerifyStringResult* result{};
        VERIFY_SUCCEEDED(XblStringVerifyStringResult(&async, resultSize * 2, buffer.get(), &result, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);
        VERIFY_ARE_EQUAL_INT(XblVerifyStringResultCode::Success, result->resultCode);
        VERIFY_IS_NULL(result->firstOffendingSubstring);
    }

    DEFINE_TEST_CASE(TestVerifyStrings)
    {
        TEST_LOG(L"Test starting: TestVerifyStrings");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("POST", url, 200);
        mock->SetResponseBody(defaultStringVerifyResult);

        const size_t requestStrsCount{ 2 };
        const char* requestStrs[requestStrsCount]{ "asdf", "asdfasdf" };
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed, requestStrs, requestStrsCount](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestUrl);

                JsonDocument requestJson;
                requestJson.Parse(requestBody.c_str());
                auto requestStrsJson = requestJson["stringsToVerify"].GetArray();

                requestWellFormed &= requestStrsJson.Size() == requestStrsCount;
                requestWellFormed &= strcmp(requestStrs[0], requestStrsJson[0].GetString()) == 0;
                requestWellFormed &= strcmp(requestStrs[1], requestStrsJson[1].GetString()) == 0;
            }
        );

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblStringVerifyStringsAsync(xboxLiveContext.get(), requestStrs, requestStrsCount, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblStringVerifyStringsResultSize(&async, &resultSize));

        size_t resultStrsCount{};
        XblVerifyStringResult* results{};
        std::shared_ptr<char> buffer(new char[resultSize], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblStringVerifyStringsResult(&async, resultSize, buffer.get(), &results, &resultStrsCount, nullptr));
        VERIFY_ARE_EQUAL_INT(resultStrsCount, 3);
        
        JsonDocument responseJson;
        responseJson.Parse(defaultStringVerifyResult);
        auto resultsJson = responseJson["verifyStringResult"].GetArray();
        VERIFY_ARE_EQUAL_INT(resultsJson[0]["resultCode"].GetInt(), results[0].resultCode);
        VERIFY_ARE_EQUAL_INT(resultsJson[1]["resultCode"].GetInt(), results[1].resultCode);
        VERIFY_ARE_EQUAL_INT(resultsJson[2]["resultCode"].GetInt(), results[2].resultCode);

        VERIFY_IS_NULL(results[0].firstOffendingSubstring);
        VERIFY_ARE_EQUAL_STR(resultsJson[1]["offendingString"].GetString(), results[1].firstOffendingSubstring);
        VERIFY_ARE_EQUAL_STR(resultsJson[2]["offendingString"].GetString(), results[2].firstOffendingSubstring);
    }

    DEFINE_TEST_CASE(TestVerifyStringsWithLargeBuffer)
    {
        TEST_LOG(L"Test starting: TestVerifyStringsWithLargeBuffer");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        auto mock = std::make_shared<HttpMock>("POST", url, 200);
        mock->SetResponseBody(defaultStringVerifyResult);

        const size_t requestStrsCount{ 2 };
        const char* requestStrs[requestStrsCount]{ "asdf", "asdfasdf" };
        bool requestWellFormed{ true };
        mock->SetMockMatchedCallback(
            [&requestWellFormed, requestStrs, requestStrsCount](HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
        {
            UNREFERENCED_PARAMETER(mock);
            UNREFERENCED_PARAMETER(requestUrl);

            JsonDocument requestJson;
            requestJson.Parse(requestBody.c_str());
            auto requestStrsJson = requestJson["stringsToVerify"].GetArray();

            requestWellFormed &= requestStrsJson.Size() == requestStrsCount;
            requestWellFormed &= strcmp(requestStrs[0], requestStrsJson[0].GetString()) == 0;
            requestWellFormed &= strcmp(requestStrs[1], requestStrsJson[1].GetString()) == 0;
        }
        );

        XAsyncBlock async{};
        size_t resultSize{};
        VERIFY_SUCCEEDED(XblStringVerifyStringsAsync(xboxLiveContext.get(), requestStrs, requestStrsCount, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));
        VERIFY_IS_TRUE(requestWellFormed);
        VERIFY_SUCCEEDED(XblStringVerifyStringsResultSize(&async, &resultSize));

        size_t bufferUsed{};
        size_t resultStrsCount{};
        XblVerifyStringResult* results{};
        std::shared_ptr<char> buffer(new char[resultSize * 2], std::default_delete<char[]>());
        VERIFY_SUCCEEDED(XblStringVerifyStringsResult(&async, resultSize * 2, buffer.get(), &results, &resultStrsCount, &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(resultSize, bufferUsed);
        VERIFY_ARE_EQUAL_INT(resultStrsCount, 3);

        JsonDocument responseJson;
        responseJson.Parse(defaultStringVerifyResult);
        auto resultsJson = responseJson["verifyStringResult"].GetArray();
        VERIFY_ARE_EQUAL_INT(resultsJson[0]["resultCode"].GetInt(), results[0].resultCode);
        VERIFY_ARE_EQUAL_INT(resultsJson[1]["resultCode"].GetInt(), results[1].resultCode);
        VERIFY_ARE_EQUAL_INT(resultsJson[2]["resultCode"].GetInt(), results[2].resultCode);

        VERIFY_IS_NULL(results[0].firstOffendingSubstring);
        VERIFY_ARE_EQUAL_STR(resultsJson[1]["offendingString"].GetString(), results[1].firstOffendingSubstring);
        VERIFY_ARE_EQUAL_STR(resultsJson[2]["offendingString"].GetString(), results[2].firstOffendingSubstring);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

