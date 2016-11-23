//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "UnitTestHelpers.h"
#include "CppUnitTest.h"
#include <strsafe.h>
#include "StockMocks.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace pplx;

#define DEFINE_TEST_CASE(TestCaseMethodName)  \
    BEGIN_TEST_METHOD_ATTRIBUTE(TestCaseMethodName) \
        TEST_OWNER(TEST_CLASS_OWNER) \
        TEST_METHOD_ATTRIBUTE(L"Area", TEST_CLASS_AREA) \
    END_TEST_METHOD_ATTRIBUTE() \
    TEST_METHOD(TestCaseMethodName)

#define DEFINE_TEST_CASE_PROPERTIES_TE() \
    UnitTestHelpers::SetupFactoryHelper(m_mockXboxSystemFactory);

#define DEFINE_TEST_CASE_OWNER2(TestCaseMethodName)  \
    BEGIN_TEST_METHOD_ATTRIBUTE(TestCaseMethodName) \
        TEST_OWNER(TEST_CLASS_OWNER_2) \
        TEST_METHOD_ATTRIBUTE(L"Area", TEST_CLASS_AREA) \
    END_TEST_METHOD_ATTRIBUTE() \
    TEST_METHOD(TestCaseMethodName)

#define DEFINE_TEST_CLASS_SETUP() \
    TEST_CLASS_SETUP(TestClassSetup) { UnitTestBase::StartResponseLogging(); return true; } \
    TEST_CLASS_CLEANUP(TestClassCleanup) { UnitTestBase::RemoveResponseLogging(); return true; }  \

#define DEFINE_MOCK_FACTORY() \
    TEST_METHOD_SETUP(SetupFactory) \
    {  \
        return SetupFactoryHelper(); \
    }\
    TEST_METHOD_CLEANUP(CleanupTest) \
    { \
        return true; \
    }

#define DEFINE_MOCK_STATE_FACTORY() \
    TEST_METHOD_SETUP(SetupStateFactory) \
    {  \
        return SetupStateFactoryHelper(); \
    }\
    TEST_METHOD_CLEANUP(CleanupTest) \
    { \
        return true; \
    }

class AssertHelper
{
public:
    static void AreEqual(const wchar_t* expected, const wchar_t* actual)
    {
        Assert::AreEqual(expected, actual, false, nullptr, LINE_INFO());
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

    static void AreEqual(Platform::String^ expected, Platform::String^ actual)
    {
        Assert::AreEqual(expected->Data(), actual->Data());
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
};

#define VERIFY_ARE_EQUAL_UINT(expected, actual) \
    Assert::IsTrue(static_cast<uint64_t>(expected) == static_cast<uint64_t>(actual))

#define VERIFY_ARE_EQUAL_INT(expected, actual) \
    Assert::IsTrue(static_cast<int64_t>(expected) == static_cast<int64_t>(actual))

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

inline void VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(web::json::value expected, web::json::value actual, bool& jsonMatches)
{
    if (expected.is_object())
    {
        for (auto jsonValue : expected.as_object())
        {
            web::json::value actualValue = actual[jsonValue.first];
            if (actualValue.is_null() && !jsonValue.second.is_null())
            {
                jsonMatches = false;
                Assert::Fail(FormatString(L"JSON Key Not Present %s", jsonValue.first.c_str()).c_str());
                return;
            }

            Logger::WriteMessage(FormatString(L"Testing JSON Key %s", jsonValue.first.c_str()).c_str());
            VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(jsonValue.second, actualValue, jsonMatches);
        }
    }
    else
    {
        VERIFY_ARE_EQUAL(expected.serialize().c_str(), actual.serialize());
    }
}

inline void VERIFY_IS_EQUAL_JSON_HELPER(web::json::value expected, web::json::value actual, const wchar_t* pszParamName)
{
    bool jsonMatches1 = true;
    bool jsonMatches2 = true;
    VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(expected, actual, jsonMatches1);
    VERIFY_IS_EQUAL_JSON_RECURSION_HELPER(actual, expected, jsonMatches2);

    if (jsonMatches1 && jsonMatches2)
    {
        Logger::WriteMessage(FormatString(L"Verify: AreEqual(%s,\"%s\")", expected.serialize().c_str(), actual.serialize().c_str()).c_str());
    }
    else
    {
        Assert::Fail(FormatString(L"EXPECTED: %s = \"%s\"", pszParamName, expected.serialize().c_str()).c_str());
        Assert::Fail(FormatString(L"ACTUAL: %s = \"%s\"", pszParamName, actual.serialize().c_str()).c_str());
    }
}

inline void VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS(std::wstring expected, std::wstring actual, const wchar_t* pszParamName)
{
    web::json::value expectedJson = web::json::value::parse(expected);
    web::json::value actualJson = web::json::value::parse(actual);
    return VERIFY_IS_EQUAL_JSON_HELPER(expectedJson, actualJson, pszParamName);
}

#define VERIFY_IS_EQUAL_JSON_FROM_STRINGS(__expected, __actual, ...) VERIFY_IS_EQUAL_JSON_HELPER_FROM_STRINGS((__expected), (__actual), (L#__actual))

#define VERIFY_IS_EQUAL_JSON(__expected, __actual, ...) VERIFY_IS_EQUAL_JSON_HELPER((__expected), (__actual), (L#__actual))

using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
