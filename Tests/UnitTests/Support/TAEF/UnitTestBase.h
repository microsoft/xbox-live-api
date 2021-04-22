// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "WexTestClass.h"

#define DATETIME_STRING_LENGTH_TO_SECOND 19
#define TICKS_PER_SECOND 10000000i64
typedef std::chrono::duration<long long, std::ratio<1, 10000000>> ticks;

#define MOCK_XUID 101010101010
#define MOCK_GAMERTAG "MockLocalUser"

std::wstring FormatString(LPCWSTR strMsg, ...);
void LogFormatString(LPCWSTR strMsg, ...);

#define VERIFY_ARE_EQUAL_STRING_IGNORE_CASE(__str1, __str2) COMPARE_STR_IGNORE_CASE_HELPER(__str1, __str2, PRIVATE_VERIFY_ERROR_INFO)

inline void COMPARE_STR_IGNORE_CASE_HELPER(LPCWSTR pwsz1, LPCWSTR pwsz2, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    WEX::Logging::Log::Comment( FormatString(L"Verify: AreEqualIgnoreCase(%s, %s)", pwsz1, pwsz2).c_str() );
    if( _wcsicmp(pwsz1, pwsz2) != 0 )
    {
        WEX::Logging::Log::Error( FormatString( L"EXPECTED: \"%s\"", pwsz1 ).c_str());
        WEX::Logging::Log::Error( FormatString( L"ACTUAL: \"%s\"", pwsz2 ).c_str());
        WEX::TestExecution::Private::MacroVerify::IsTrue(false, L"false", errorInfo, nullptr);
    }
}

inline void COMPARE_STR_IGNORE_CASE_HELPER(const char* lhs, const char* rhs, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    COMPARE_STR_IGNORE_CASE_HELPER(utils::string_t_from_utf8(lhs).data(), utils::string_t_from_utf8(rhs).data(), errorInfo);
}

#define VERIFY_IS_EQUAL_STR(__expected, __actual, ...) VERIFY_IS_EQUAL_STR_HELPER((__expected), (__actual), (L#__actual), PRIVATE_VERIFY_ERROR_INFO)

inline void VERIFY_IS_EQUAL_STR_HELPER(std::wstring expected, std::wstring actual, const wchar_t* pszParamName, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    if (expected != actual)
    {
        WEX::Logging::Log::Error(FormatString(L"EXPECTED: %s = \"%s\"", pszParamName, expected.c_str()).c_str());
        WEX::Logging::Log::Error(FormatString(L"ACTUAL: %s = \"%s\"", pszParamName, actual.c_str()).c_str());
        WEX::TestExecution::Private::MacroVerify::IsTrue(false, L"false", errorInfo, nullptr);
    }
    else
    {
        WEX::Logging::Log::Comment(FormatString(L"Verify: AreEqual(%s,\"%s\")", pszParamName, expected.c_str()).c_str());
    }
}

inline void VERIFY_IS_EQUAL_STR_HELPER(const char* expected, const char* actual, const wchar_t* pszParamName, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    if (strcmp(expected, actual) != 0)
    {
        WEX::Logging::Log::Error(FormatString(L"EXPECTED: %s = \"%s\"", pszParamName, expected).c_str());
        WEX::Logging::Log::Error(FormatString(L"ACTUAL: %s = \"%s\"", pszParamName, actual).c_str());
        WEX::TestExecution::Private::MacroVerify::IsTrue(false, L"false", errorInfo, nullptr);
    }
    else
    {
        WEX::Logging::Log::Comment(FormatString(L"Verify: AreEqual(%s,\"%s\")", pszParamName, expected).c_str());
    }
}

inline void VERIFY_IS_EQUAL_STR_HELPER(LPCWSTR pwsz1, LPCWSTR pwsz2, const wchar_t* pszParamName, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    if (pwsz1 == nullptr && pwsz2 == nullptr)
    {
        WEX::Logging::Log::Comment(FormatString(L"Verify: AreEqual(%s)", pszParamName).c_str());
    }
    else if (pwsz1 == nullptr || pwsz2 == nullptr)
    {
        WEX::Logging::Log::Error(FormatString(L"EXPECTED: %s = \"%s\"", pszParamName, pwsz1 == nullptr ? L"" : pwsz1).c_str());
        WEX::Logging::Log::Error(FormatString(L"ACTUAL: %s = \"%s\"", pszParamName, pwsz1 == nullptr ? L"" : pwsz2).c_str());
        WEX::TestExecution::Private::MacroVerify::IsTrue(false, L"false", errorInfo, nullptr);
    }
    else
    {
        if (CSTR_EQUAL != CompareStringEx(
            LOCALE_NAME_INVARIANT,
            0,
            pwsz1,
            -1,
            pwsz2,
            -1,
            NULL,
            NULL,
            0))
        {
            WEX::Logging::Log::Error(FormatString(L"EXPECTED: %s = \"%s\"", pszParamName, pwsz1).c_str());
            WEX::Logging::Log::Error(FormatString(L"ACTUAL: %s = \"%s\"", pszParamName, pwsz2).c_str());
            WEX::TestExecution::Private::MacroVerify::IsTrue(false, L"false", errorInfo, nullptr);
        }
        else
        {
            WEX::Logging::Log::Comment(FormatString(L"Verify: AreEqual(%s,\"%s\")", pszParamName, pwsz1).c_str());
        }
    }
}

#define VERIFY_IS_EQUAL_JSON_FROM_STRINGS(__expected, __actual, ...) VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS((__expected), (__actual), (L#__actual), PRIVATE_VERIFY_ERROR_INFO)

#define VERIFY_IS_EQUAL_JSON(__expected, __actual, ...) VERIFY_IS_EQUAL_JSON_HELPER((__expected), (__actual), (L#__actual), PRIVATE_VERIFY_ERROR_INFO)

inline void VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(const JsonValue& expected, const JsonValue& actual, bool& jsonMatches)
{
    if (expected.IsObject())
    {
        for (auto& jsonValue : expected.GetObject())
        {
            if (actual.IsObject() && actual.HasMember(jsonValue.name.GetString()))
            {
                const JsonValue& actualValue = actual[jsonValue.name.GetString()];
                if (actualValue.IsNull() && !jsonValue.value.IsNull())
                {
                    jsonMatches = false;
                    WEX::Logging::Log::Error(FormatString(L"JSON Key Not Present %s", jsonValue.name.GetString()).c_str());
                    return;
                }

                WEX::Logging::Log::Comment(FormatString(L"Testing JSON Key %s", jsonValue.name.GetString()).c_str());
                VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(jsonValue.value, actualValue, jsonMatches);
            }
            else
            {
                jsonMatches = false;
                WEX::Logging::Log::Error(FormatString(L"JSON Key Not Present %s", jsonValue.name.GetString()).c_str());
                return;
            }
        }
    }
    else
    {
        VERIFY_IS_EQUAL_STR(JsonUtils::SerializeJson(expected).c_str(), JsonUtils::SerializeJson(actual).c_str());
    }
}

inline void VERIFY_IS_EQUAL_JSON_HELPER(const JsonValue& expected, const JsonValue& actual, const wchar_t* pszParamName, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    bool jsonMatches1 = true;
    bool jsonMatches2 = true;
    VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(expected, actual, jsonMatches1);
    VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(actual, expected, jsonMatches2);
    
    if (jsonMatches1 && jsonMatches2)
    {
        WEX::Logging::Log::Comment(FormatString(L"Verify: AreEqual(%s,\"%s\")", JsonUtils::SerializeJson(expected).c_str(), JsonUtils::SerializeJson(actual).c_str()).c_str());
    }
    else
    {
        WEX::Logging::Log::Error(FormatString(L"EXPECTED: %s = \"%s\"", pszParamName, JsonUtils::SerializeJson(expected).c_str()).c_str());
        WEX::Logging::Log::Error(FormatString(L"ACTUAL: %s = \"%s\"", pszParamName, JsonUtils::SerializeJson(actual).c_str()).c_str());
        WEX::TestExecution::Private::MacroVerify::IsTrue(false, L"false", errorInfo, nullptr);
    }
}

inline void VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS(std::wstring expected, std::wstring actual, const wchar_t* pszParamName, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    JsonDocument expectedJson;
    expectedJson.Parse(utils::internal_string_from_string_t(expected).c_str());
    JsonDocument actualJson;
    actualJson.Parse(utils::internal_string_from_string_t(actual).c_str());
    return VERIFY_IS_EQUAL_JSON_HELPER(expectedJson, actualJson, pszParamName, errorInfo);
}

inline void VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS(std::wstring expected, xsapi_internal_string actual, const wchar_t* pszParamName, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    JsonDocument expectedJson;
    expectedJson.Parse(utils::internal_string_from_string_t(expected).c_str());
    JsonDocument actualJson;
    actualJson.Parse(actual.c_str());
    return VERIFY_IS_EQUAL_JSON_HELPER(expectedJson, actualJson, pszParamName, errorInfo);
}

inline void VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS(xsapi_internal_string expected, xsapi_internal_string actual, const wchar_t* pszParamName, const WEX::TestExecution::ErrorInfo& errorInfo)
{
    JsonDocument expectedJson;
    expectedJson.Parse(expected.c_str());
    JsonDocument actualJson;
    actualJson.Parse(actual.c_str());
    return VERIFY_IS_EQUAL_JSON_HELPER(expectedJson, actualJson, pszParamName, errorInfo);
}

#define VERIFY_THROWS_HR(__operation, __hr)                                                                                                                                     \
{                                                                                                                                                                               \
    bool __exceptionHit = false;                                                                                                                                                \
    try                                                                                                                                                                         \
    {                                                                                                                                                                           \
        __operation;                                                                                                                                                            \
    }                                                                                                                                                                           \
    catch(...)                                                                                                                                                                  \
    {                                                                                                                                                                           \
        HRESULT hrGot = legacy::ConvertExceptionToHRESULT();                                                                                                                     \
        if(hrGot == __hr)                                                                                                                                                       \
        {                                                                                                                                                                       \
            WEX::Logging::Log::Comment( FormatString( L"Verify: Expected Exception Thrown ( hr == %s )", L#__hr ).c_str() );                                                    \
            __exceptionHit = true;                                                                                                                                              \
        }                                                                                                                                                                       \
    }                                                                                                                                                                           \
    if(!__exceptionHit)                                                                                                                                                         \
    {                                                                                                                                                                           \
        WEX::Logging::Log::Comment( FormatString( L"Error: Expected Exception Not Thrown ( hr == %s )", L#__hr ).c_str() );                                                     \
        (bool)WEX::TestExecution::Private::MacroVerify::Fail(PRIVATE_VERIFY_ERROR_INFO, FormatString( L"Expected Exception Not Thrown ( hr == %s )", L#__hr ).c_str());         \
    }                                                                                                                                                                           \
}

inline bool VerifyTime(time_t time, std::string timeString)
{
    auto datetime = xbox::services::datetime::from_string(timeString.data(), xbox::services::datetime::date_format::ISO_8601);
    return xbox::services::utils::time_t_from_datetime(datetime) == time;
}