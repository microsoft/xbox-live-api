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
#include "UnitTestIncludes.h"
#define TEST_CLASS_OWNER L"jasonsa"
#include "DefineTestMacros.h"
#include "Utils.h"

NAMESPACE_MICROSOFT_XBOX_SYSTEM_CPP_BEGIN

#define VERIFY_EXCEPTION_TO_HR(x,hrVerify) \
        try \
        { \
            throw x; \
        } \
        catch (...) \
        { \
            HRESULT hr = utils::convert_exception_to_hresult(); \
            VERIFY_ARE_EQUAL(hr, hrVerify); \
        }

class UtilTests : public UnitTestBase
{
public:
    TEST_CLASS(UtilTests);
    DEFINE_TEST_CLASS_SETUP();

    TEST_METHOD(TestConvertExceptionToHRESULT)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        VERIFY_EXCEPTION_TO_HR(std::bad_alloc(), E_OUTOFMEMORY);
        VERIFY_EXCEPTION_TO_HR(std::bad_cast(), E_NOINTERFACE);
        VERIFY_EXCEPTION_TO_HR(std::invalid_argument("test"), E_INVALIDARG);
        VERIFY_EXCEPTION_TO_HR(std::out_of_range("test"), E_BOUNDS);
        VERIFY_EXCEPTION_TO_HR(std::length_error("test"), __HRESULT_FROM_WIN32(ERROR_BAD_LENGTH));
        VERIFY_EXCEPTION_TO_HR(std::overflow_error("test"), HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
        VERIFY_EXCEPTION_TO_HR(std::underflow_error("test"), HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
        VERIFY_EXCEPTION_TO_HR(std::range_error("test"), E_BOUNDS);
        VERIFY_EXCEPTION_TO_HR(std::system_error(0x12, std::system_category(), "test"), 0x80070012);
        VERIFY_EXCEPTION_TO_HR(std::system_error(0x1234, std::generic_category(), "test"), 0x1234);
        VERIFY_EXCEPTION_TO_HR(std::logic_error("test"), E_UNEXPECTED);
        VERIFY_EXCEPTION_TO_HR(std::runtime_error("test"), E_FAIL);
        VERIFY_EXCEPTION_TO_HR(web::json::json_exception(L"test"), WEB_E_INVALID_JSON_STRING);
        VERIFY_EXCEPTION_TO_HR(web::http::http_exception(L"test"), HTTP_E_STATUS_UNEXPECTED);
        VERIFY_EXCEPTION_TO_HR(web::uri_exception("test"), WEB_E_UNEXPECTED_CONTENT);
        VERIFY_EXCEPTION_TO_HR(std::exception("test"), E_FAIL);
        VERIFY_EXCEPTION_TO_HR(E_NOINTERFACE, E_NOINTERFACE);
        VERIFY_EXCEPTION_TO_HR(false, E_FAIL);
    }

    TEST_METHOD(TestHeadersToString)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::wstringstream ss;
        ss << L"header1: value1\r\n";
        ss << L"header2: value2\r\n";
        ss << L"header3: value3\r\n";
        std::wstring expected = ss.str();

        web::http::http_headers headers;
        headers[U("header1")] = U("value1");
        headers[U("header2")] = U("value2");
        headers[U("header3")] = U("value3");

        std::wstring actual = utils::headers_to_string(headers);

        VERIFY_IS_EQUAL_STR(expected, actual);
    }

    void AssertHeadersEquivalent(const web::http::http_headers& expected, const web::http::http_headers& actual)
    {
        VERIFY_ARE_EQUAL(expected.size(), actual.size());

        for (auto it = expected.begin(); it != expected.end(); ++it)
        {
            std::wstring header = it->first;
            std::wstring value = it->second;

            auto it1 = actual.find(header);
            VERIFY_ARE_NOT_EQUAL(actual.end(), it1);
            VERIFY_ARE_EQUAL(value, it1->second);
        }
    }

    web::http::http_headers StockHeaders()
    {
        web::http::http_headers headers;
        headers[U("header1")] = U("value1");
        headers[U("header2")] = U("value2");
        headers[U("header3")] = U("value3");
        return headers;
    }

    TEST_METHOD(StringToHeadersMainline)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::wstringstream ss;
        ss << L"header1:      value1\r\n";
        ss << L"header2: value2   \r\n";
        ss << L"header3:    value3   \r\n";

        web::http::http_headers expected = StockHeaders();
        web::http::http_headers actual = utils::string_to_headers(ss.str());

        AssertHeadersEquivalent(expected, actual);
    }

    TEST_METHOD(StringToHeadersValuesCanHaveWhitespace)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::wstringstream ss;
        ss << L"Accept-Encoding: gzip, deflate\r\n";
        ss << L"Header-One: a b c  d  1   5   \r\n";

        web::http::http_headers expected;
        expected[U("Accept-Encoding")] = U("gzip, deflate");
        expected[U("Header-One")] = U("a b c  d  1   5");

        web::http::http_headers actual = utils::string_to_headers(ss.str());

        AssertHeadersEquivalent(expected, actual);
    }

    TEST_METHOD(StringToHeadersNoTrailingNewline)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::wstringstream ss;
        ss << L"header1:      value1\r\n";
        ss << L"header2: value2   \r\n";
        ss << L"header3:    value3";

        web::http::http_headers expected = StockHeaders();
        web::http::http_headers actual = utils::string_to_headers(ss.str());

        AssertHeadersEquivalent(expected, actual);
    }

    TEST_METHOD(StringToHeadersWithEmptyString)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        AssertHeadersEquivalent(web::http::http_headers(), utils::string_to_headers(L""));
    }

    TEST_METHOD(StringToHeadersThrowsOnInvalidHeaders)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::wstring str = L"header1: value1\r\nsdf  jifewj -n: jfiewa:";
        VERIFY_THROWS(utils::string_to_headers(str), std::invalid_argument);
    }

    TEST_METHOD(TestExtractJsonString)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        web::json::value jsonValue = web::json::value::parse(LR"({"testname":"testvalue"})");
        VERIFY_ARE_EQUAL(L"testvalue", utils::extract_json_string(jsonValue, L"testname"));
        VERIFY_ARE_EQUAL(L"testvalue", utils::extract_json_string(jsonValue, L"testname", true));
        VERIFY_ARE_EQUAL(L"default", utils::extract_json_string(jsonValue, L"missingname", false, L"default"));
    }

    TEST_METHOD(TestExtractJsonBool)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        web::json::value jsonValue = web::json::value::parse(LR"({"testname":true})");
        VERIFY_IS_TRUE(utils::extract_json_bool(jsonValue, L"testname"));
    }

    TEST_METHOD(TestExtractJsonVector)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        web::json::value jsonValue = web::json::value::parse(LR"({"totalCount":0, "people" : ["1", "2"]})");
        //web::json::value jsonValue = web::json::value::parse(LR"({"names":["1","2"]})");
        std::vector< std::wstring > names = utils::extract_json_vector<std::wstring>(utils::json_string_extractor, jsonValue, _T("people"), false);
        VERIFY_ARE_EQUAL(names.size(), 2);
    }

    TEST_METHOD(TestSerializeString)
    {
        DEFINE_TEST_CASE_PROPERTIES();

        std::vector<std::wstring> values;
        values.push_back(L"1");
        values.push_back(L"2");
        values.push_back(L"3");
        web::json::value jsonArray = utils::serialize_string_vector_to_json(values);
    }
};

NAMESPACE_MICROSOFT_XBOX_SYSTEM_CPP_END

