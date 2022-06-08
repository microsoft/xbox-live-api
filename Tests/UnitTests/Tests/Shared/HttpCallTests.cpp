// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(HttpCallTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(HttpCallTests);

    DEFINE_TEST_CASE(TestHttpCall)
    {
        TEST_LOG(L"Test starting: TestHttpCall");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        uint32_t httpStatus{ 200 };
        String url{ "https://xboxlive.com" };
        HttpHeaders responseHeaders
        {
            { "key1", "value1" },
            { "key2", "value2" }
        };
        String responseBody{ "responseBody" };

        HttpMock mock{ "", url.data(), httpStatus };
        mock.SetResponseBody(responseBody);
        mock.SetResponseHeaders(responseHeaders);

        XblHttpCallHandle callHandle{};
        VERIFY_SUCCEEDED(XblHttpCallCreate(xboxLiveContext.get(), "GET", url.data(), &callHandle));

        const char* actualRequestUrl{ nullptr };
        VERIFY_SUCCEEDED(XblHttpCallGetRequestUrl(callHandle, &actualRequestUrl));
        VERIFY_IS_TRUE(url == actualRequestUrl);

        // Test request APIs
        VERIFY_SUCCEEDED(XblHttpCallRequestSetRetryAllowed(callHandle, false));
        VERIFY_SUCCEEDED(XblHttpCallRequestSetRequestBodyString(callHandle, "requestBody"));
        VERIFY_SUCCEEDED(XblHttpCallRequestSetHeader(callHandle, "header1", "value1", false));

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblHttpCallPerformAsync(callHandle, XblHttpCallResponseBodyType::String, &async));
        VERIFY_SUCCEEDED(XAsyncGetStatus(&async, true));

        uint32_t actualStatus{};
        VERIFY_SUCCEEDED(XblHttpCallGetStatusCode(callHandle, &actualStatus));
        VERIFY_ARE_EQUAL_INT(actualStatus, httpStatus);

        uint32_t headerCount{};
        VERIFY_SUCCEEDED(XblHttpCallGetNumHeaders(callHandle, &headerCount));
        VERIFY_ARE_EQUAL_INT(headerCount, responseHeaders.size());

        uint32_t index{ 0 };
        for (auto& pair : responseHeaders)
        {
            const char* headerName{ nullptr };
            const char* headerValue{ nullptr };
            VERIFY_SUCCEEDED(XblHttpCallGetHeaderAtIndex(callHandle, index++, &headerName, &headerValue));

            VERIFY_ARE_EQUAL_STR(headerName, pair.first);
            VERIFY_ARE_EQUAL_STR(headerValue, pair.second);
        }

        const char* actualResponseBody{ nullptr };
        VERIFY_SUCCEEDED(XblHttpCallGetResponseString(callHandle, &actualResponseBody));
        VERIFY_IS_TRUE(actualResponseBody == responseBody);

        size_t bufferSize{};
        VERIFY_SUCCEEDED(XblHttpCallGetResponseBodyBytesSize(callHandle, &bufferSize));

        // Verify exact buffer size
        size_t bufferUsed{};
        std::vector<uint8_t> buffer(bufferSize);
        VERIFY_SUCCEEDED(XblHttpCallGetResponseBodyBytes(callHandle, bufferSize, buffer.data(), &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(bufferSize, bufferUsed);

        // Verify larger buffer size
        bufferUsed = 0;
        buffer = std::vector<uint8_t>(bufferSize * 2);
        VERIFY_SUCCEEDED(XblHttpCallGetResponseBodyBytes(callHandle, bufferSize * 2, buffer.data(), &bufferUsed));
        VERIFY_ARE_EQUAL_UINT(bufferSize, bufferUsed);
    }

    DEFINE_TEST_CASE(CppTestHttpCall)
    {
        TEST_LOG(L"Test starting: CppTestHttpCall");

        TestEnvironment env{};

        uint32_t httpStatus{ 200 };
        String url{ "https://xboxlive.com" };
        HttpHeaders responseHeaders
        {
            { "key1", "value1" },
            { "key2", "value2" }
        };
        String responseBody{ "responseBody" };

        HttpMock mock{ "GET", url.data(), httpStatus };
        mock.SetResponseBody(responseBody);
        mock.SetResponseHeaders(responseHeaders);

        auto xboxLiveContext = env.CreateLegacyMockXboxLiveContext();

        auto httpCall = create_xbox_live_http_call(
            xboxLiveContext->settings(),
            _T("GET"),
            Utils::StringTFromUtf8(url.data()),
            web::uri()
        );

        VERIFY_ARE_EQUAL_STR(Utils::StringTFromUtf8(url.data()), httpCall->server_name());

        auto task = httpCall->get_response_with_auth(
            xboxLiveContext->user(),
            http_call_response_body_type::string_body,
            false
        );

        auto result{ task.get() };

        VERIFY_IS_TRUE(result->body_type() == http_call_response_body_type::string_body);
        VERIFY_IS_TRUE(result->http_status() == httpStatus);

        auto actualResponseHeaders{ result->response_headers() };
        VERIFY_ARE_EQUAL_INT(actualResponseHeaders.size(), responseHeaders.size());

        for (auto& pair : responseHeaders)
        {
            VERIFY_ARE_EQUAL_STR(actualResponseHeaders[Utils::StringTFromUtf8(pair.first.data())], Utils::StringTFromUtf8(pair.second.data()));
        }

        VERIFY_ARE_EQUAL_STR(result->response_body_string(), Utils::StringTFromUtf8(responseBody.data()));
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

