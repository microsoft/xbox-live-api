// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "UnitTestHelpers_TE.h"
#include "CppUnitTest.h"
#include <strsafe.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace pplx;

class AssertHelper
{
public:
    static void AreEqual(const wchar_t* expected, const wchar_t* actual)
    {
        Assert::AreEqual(expected, actual, false, nullptr, nullptr);
    }

    static void AreEqual(std::wstring expected, const wchar_t* actual)
    {
        Assert::AreEqual(expected.c_str(), actual);
    }

    static void AreEqual(const wchar_t* expected, std::wstring actual)
    {
        Assert::AreEqual(expected, actual.c_str());
    }

    static void AreEqual(std::wstring expected, std::wstring actual)
    {
        Assert::AreEqual(expected.c_str(), actual.c_str());
    }

    static void AreEqual(bool expected, bool actual)
    {
        Assert::AreEqual(expected, actual);
    }

    static void AreEqual(double expected, double actual)
    {
        Assert::AreEqual(expected, actual);
    }

    static void AreEqual(std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> expected, std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> actual)
    {
        Assert::IsTrue(expected == actual);
    }

    static void AreEqual(xsapi_internal_string expected, xsapi_internal_string actual)
    {
        Assert::AreEqual(expected.data(), actual.data(), false, nullptr, nullptr);
    }

    static void AreEqual(const std::string& expected, const std::string& actual)
    {
        Assert::AreEqual(expected.data(), actual.data(), false, nullptr, nullptr);
    }

    static void AreEqual(size_t expected, size_t actual)
    {
        Assert::IsTrue(expected == actual);
    }

    static void AreEqual(HRESULT expected, HRESULT actual)
    {
        Assert::IsTrue(expected == actual);
    }
};

#define VERIFY_ARE_EQUAL_UINT(expected, actual) \
    Assert::IsTrue(static_cast<uint64_t>(expected) == static_cast<uint64_t>(actual))

#define VERIFY_ARE_EQUAL_INT(expected, actual) \
    Assert::IsTrue(static_cast<int64_t>(expected) == static_cast<int64_t>(actual))

#define VERIFY_ARE_EQUAL_DOUBLE(expected, actual) \
    Assert::IsTrue(static_cast<double>(expected) == static_cast<double>(actual))

#define VERIFY_ARE_EQUAL(expected,actual) \
    AssertHelper::AreEqual(expected,actual)

#define VERIFY_ARE_EQUAL_STR(expected,actual) \
    AssertHelper::AreEqual(expected,actual)

#define VERIFY_ARE_EQUAL_STR_IGNORE_CASE(expected,actual) \
    Assert::AreEqual(expected, actual, true)

#define VERIFY_IS_TRUE(x) \
    Assert::IsTrue(x, L#x)

#define VERIFY_IS_FALSE(x) \
    Assert::IsFalse(x, L#x)

#define VERIFY_IS_NULL(x) \
    Assert::IsNull(x, L#x)

#define VERIFY_IS_NOT_NULL(x) \
    Assert::IsNotNull(x)

#define VERIFY_SUCCEEDED(x) \
    Assert::IsTrue(SUCCEEDED(x))

#define VERIFY_FAILED(x) \
    Assert::IsTrue(FAILED(x))

#define VERIFY_FAIL() \
    Assert::Fail()

#define VERIFY_INVALIDARG(x) \
    Assert::AreEqual(E_INVALIDARG, x)

#define VERIFY_ARE_EQUAL_TIMESPAN_TO_SECONDS(__timespan, __seconds) VERIFY_ARE_EQUAL(Microsoft::Xbox::Services::System::timeSpanTicks(__timespan.Duration), std::chrono::seconds(__seconds))
#define VERIFY_ARE_EQUAL_TIMESPAN_TO_MILLISECONDS(__timespan, __seconds) VERIFY_ARE_EQUAL(Microsoft::Xbox::Services::System::timeSpanTicks(__timespan.Duration), std::chrono::milliseconds(__seconds))

#define VERIFY_THROWS_CX(__operation, __exception)                                                                                                                          \
{                                                                                                                                                                           \
    bool __exceptionHit = false;                                                                                                                                            \
    try                                                                                                                                                                     \
    {                                                                                                                                                                       \
        __operation;                                                                                                                                                        \
    }                                                                                                                                                                       \
    catch(__exception^ __e)                                                                                                                                                 \
    {                                                                                                                                                                       \
        Logger::WriteMessage( FormatString( L"Verify: Expected Exception Thrown( %s )", L#__exception ).c_str() );                                                          \
        __exceptionHit = true;                                                                                                                                              \
    }                                                                                                                                                                       \
    catch(...)                                                                                                                                                              \
    {                                                                                                                                                                       \
    }                                                                                                                                                                       \
    if(!__exceptionHit)                                                                                                                                                     \
    {                                                                                                                                                                       \
        Logger::WriteMessage( FormatString( L"Error: Expected Exception Not Thrown ( %s )", L#__exception ).c_str() );                                                      \
        Assert::IsTrue(__exceptionHit);                                                                                                                                     \
    }                                                                                                                                                                       \
}

#define VERIFY_THROWS_HR_CX(__operation, __hr)                                                                                                                                  \
{                                                                                                                                                                               \
    bool __exceptionHit = false;                                                                                                                                                \
    try                                                                                                                                                                         \
    {                                                                                                                                                                           \
        __operation;                                                                                                                                                            \
    }                                                                                                                                                                           \
    catch(Platform::Exception^ __e)                                                                                                                                             \
    {                                                                                                                                                                           \
        if(__e->HResult == (__hr))                                                                                                                                              \
        {                                                                                                                                                                       \
            Logger::WriteMessage( FormatString( L"Verify: Expected Exception Thrown ( hr == %s )", L#__hr ).c_str() );                                                          \
            __exceptionHit = true;                                                                                                                                              \
        }                                                                                                                                                                       \
    }                                                                                                                                                                           \
    catch(...)                                                                                                                                                                  \
    {                                                                                                                                                                           \
    }                                                                                                                                                                           \
    if(!__exceptionHit)                                                                                                                                                         \
    {                                                                                                                                                                           \
        Logger::WriteMessage( FormatString( L"Error: Expected Exception Not Thrown ( hr == %s )", L#__hr ).c_str() );                                                           \
        Assert::IsTrue(__exceptionHit);                                                                                                                                         \
    }                                                                                                                                                                           \
}


#define VERIFY_NO_THROW(__operation, ...)                                                                                          \
{                                                                                                                                    \
    bool __exceptionHit = false;                                                                                                     \
    try                                                                                                                              \
    {                                                                                                                                \
        __operation;                                                                                                                 \
    }                                                                                                                                \
    catch(...)                                                                                                                       \
    {                                                                                                                                \
        __exceptionHit = true;                                                                                                       \
    }                                                                                                                                \
                                                                                                                                     \
    if (__exceptionHit)                                                                                                              \
    {                                                                                                                                \
    }                                                                                                                                \
}        

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
                    Assert::Fail(FormatString(L"JSON Key Not Present %s", jsonValue.name.GetString()).c_str());
                    return;
                }

                Logger::WriteMessage(FormatString(L"Testing JSON Key %s", jsonValue.name.GetString()).c_str());
                VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(jsonValue.value, actualValue, jsonMatches);
            }
            else
            {
                jsonMatches = false;
                Assert::Fail(FormatString(L"JSON Key Not Present %s", jsonValue.name.GetString()).c_str());
                return;
            }
        }
    }
    else
    {
        VERIFY_ARE_EQUAL(JsonUtils::SerializeJson(expected).c_str() , JsonUtils::SerializeJson(actual).c_str());
    }
}

inline void VERIFY_IS_EQUAL_JSON_HELPER(const JsonValue& expected, const JsonValue& actual, const wchar_t* pszParamName)
{
    bool jsonMatches1 = true;
    bool jsonMatches2 = true;
    VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(expected, actual, jsonMatches1);
    VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(actual, expected, jsonMatches2);

    if (jsonMatches1 && jsonMatches2)
    {
        Logger::WriteMessage(FormatString(L"Verify: AreEqual(%s,\"%s\")", JsonUtils::SerializeJson(expected).c_str(), JsonUtils::SerializeJson(actual).c_str()).c_str());
    }
    else
    {
        Assert::Fail(FormatString(L"EXPECTED: %s = \"%s\"", pszParamName, JsonUtils::SerializeJson(expected).c_str()).c_str());
        Assert::Fail(FormatString(L"ACTUAL: %s = \"%s\"", pszParamName, JsonUtils::SerializeJson(actual).c_str()).c_str());
    }
}

inline void VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS(std::wstring expected, std::wstring actual, const wchar_t* pszParamName)
{
    JsonDocument expectedJson;
    expectedJson.Parse(utils::internal_string_from_string_t(expected).c_str());
    JsonDocument actualJson;
    actualJson.Parse(utils::internal_string_from_string_t(actual).c_str());
    return VERIFY_IS_EQUAL_JSON_HELPER(expectedJson, actualJson, pszParamName);
}

inline void VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS(std::wstring expected, xsapi_internal_string actual, const wchar_t* pszParamName)
{
    JsonDocument expectedJson;
    expectedJson.Parse(utils::internal_string_from_string_t(expected).c_str());
    JsonDocument actualJson;
    actualJson.Parse(actual.c_str());
    return VERIFY_IS_EQUAL_JSON_HELPER(expectedJson, actualJson, pszParamName);
}

inline void VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS(std::string expected, std::string actual, const wchar_t* pszParamName)
{
    JsonDocument expectedJson;
    expectedJson.Parse(expected.c_str());
    JsonDocument actualJson;
    actualJson.Parse(actual.c_str());
    return VERIFY_IS_EQUAL_JSON_HELPER(expectedJson, actualJson, pszParamName);
}

#define VERIFY_IS_EQUAL_JSON_FROM_STRINGS(__expected, __actual, ...) VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS((__expected), (__actual), (L#__actual))

#define VERIFY_IS_EQUAL_JSON(__expected, __actual, ...) VERIFY_IS_EQUAL_JSON_HELPER((__expected), (__actual), (L#__actual))
