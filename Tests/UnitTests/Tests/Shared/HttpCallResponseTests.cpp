// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"HttpCallResponse"
#include "UnitTestIncludes.h"
#define MAKE_HTTP_HRESULT(code) MAKE_HRESULT(1, 0x019, code)


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(HttpCallResponseTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(HttpCallResponseTests)

    DEFINE_TEST_CASE(TestHttpStatus)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestHttpStatus);

        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_UNEXPECTED, utils::convert_http_status_to_hresult(600));
        VERIFY_ARE_EQUAL_INT(S_OK, utils::convert_http_status_to_hresult(200));
        VERIFY_ARE_EQUAL_INT(S_OK, utils::convert_http_status_to_hresult(299));
        VERIFY_ARE_EQUAL_INT(S_OK, utils::convert_http_status_to_hresult(1223));

        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_AMBIGUOUS, utils::convert_http_status_to_hresult(300));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_MOVED, utils::convert_http_status_to_hresult(301));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_REDIRECT, utils::convert_http_status_to_hresult(302));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_REDIRECT_METHOD, utils::convert_http_status_to_hresult(303));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_NOT_MODIFIED, utils::convert_http_status_to_hresult(304));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_USE_PROXY, utils::convert_http_status_to_hresult(305));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_REDIRECT_KEEP_VERB, utils::convert_http_status_to_hresult(307));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_BAD_REQUEST, utils::convert_http_status_to_hresult(400));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_DENIED, utils::convert_http_status_to_hresult(401));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_PAYMENT_REQ, utils::convert_http_status_to_hresult(402));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_FORBIDDEN, utils::convert_http_status_to_hresult(403));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_NOT_FOUND, utils::convert_http_status_to_hresult(404));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_BAD_METHOD, utils::convert_http_status_to_hresult(405));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_NONE_ACCEPTABLE, utils::convert_http_status_to_hresult(406));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_PROXY_AUTH_REQ, utils::convert_http_status_to_hresult(407));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_REQUEST_TIMEOUT, utils::convert_http_status_to_hresult(408));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_CONFLICT, utils::convert_http_status_to_hresult(409));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_GONE, utils::convert_http_status_to_hresult(410));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_LENGTH_REQUIRED, utils::convert_http_status_to_hresult(411));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_PRECOND_FAILED, utils::convert_http_status_to_hresult(412));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_REQUEST_TOO_LARGE, utils::convert_http_status_to_hresult(413));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_URI_TOO_LONG, utils::convert_http_status_to_hresult(414));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_UNSUPPORTED_MEDIA, utils::convert_http_status_to_hresult(415));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_RANGE_NOT_SATISFIABLE, utils::convert_http_status_to_hresult(416));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_EXPECTATION_FAILED, utils::convert_http_status_to_hresult(417));

        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(421), utils::convert_http_status_to_hresult(421));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(422), utils::convert_http_status_to_hresult(422));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(423), utils::convert_http_status_to_hresult(423));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(424), utils::convert_http_status_to_hresult(424));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(426), utils::convert_http_status_to_hresult(426));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(428), utils::convert_http_status_to_hresult(428));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(429), utils::convert_http_status_to_hresult(429));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(431), utils::convert_http_status_to_hresult(431));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(449), utils::convert_http_status_to_hresult(449));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(451), utils::convert_http_status_to_hresult(451));

        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_SERVER_ERROR, utils::convert_http_status_to_hresult(500));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_NOT_SUPPORTED, utils::convert_http_status_to_hresult(501));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_BAD_GATEWAY, utils::convert_http_status_to_hresult(502));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_SERVICE_UNAVAIL, utils::convert_http_status_to_hresult(503));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_GATEWAY_TIMEOUT, utils::convert_http_status_to_hresult(504));
        VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_VERSION_NOT_SUP, utils::convert_http_status_to_hresult(505));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(506), utils::convert_http_status_to_hresult(506));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(507), utils::convert_http_status_to_hresult(507));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(508), utils::convert_http_status_to_hresult(508));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(510), utils::convert_http_status_to_hresult(510));
        VERIFY_ARE_EQUAL_INT(MAKE_HTTP_HRESULT(511), utils::convert_http_status_to_hresult(511));

        int32_t result = utils::convert_http_status_to_hresult(429);
        VERIFY_IS_TRUE(0x801901AD == result); // 0x1AD = 429
        VERIFY_ARE_EQUAL_STR("HTTP_E_STATUS_429_TOO_MANY_REQUESTS", utils::convert_hresult_to_error_name(0x801901AD));  
    }

    DEFINE_TEST_CASE(TestHttpCallResponse)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestHttpCallResponse);

        std::shared_ptr<http_call_response> httpCallResponse = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(L"{}"));
        VERIFY_ARE_EQUAL_INT(http_call_response_body_type::json_body, httpCallResponse->body_type());
        VERIFY_ARE_EQUAL_STR(L"", httpCallResponse->response_body_string().c_str());
        VERIFY_ARE_EQUAL_STR(L"{}", httpCallResponse->response_body_json().serialize().c_str());
        VERIFY_ARE_EQUAL_INT(0, httpCallResponse->response_body_vector().size());
        VERIFY_ARE_EQUAL_INT(3, httpCallResponse->response_headers().size());
        VERIFY_ARE_EQUAL_INT(200, httpCallResponse->http_status());
        VERIFY_ARE_EQUAL_STR(L"MockETag", httpCallResponse->e_tag());
        VERIFY_ARE_EQUAL_INT(1, httpCallResponse->retry_after().count());
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

