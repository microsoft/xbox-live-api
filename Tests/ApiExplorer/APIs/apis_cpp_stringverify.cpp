// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

int StringServiceVerifyString_Lua(lua_State *L)
{
#if CPP_TESTS_ENABLED
    string_t testString = _T("Shltstring");

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->string_service().verify_string(
        testString
    ).then(
        [](xbox::services::xbox_live_result<xbox::services::system::verify_string_result> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("StringServiceVerifyString: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                xbox::services::system::verify_string_result verifyStringResult = result.payload();

                LogToFile(
                    "Result: Result Code: %d - First Offending String: %s",
                    verifyStringResult.result_code(),
                    (verifyStringResult.result_code() == xbox::services::system::verify_string_result_code::offensive) ? xbox::services::Utils::StringFromStringT(verifyStringResult.first_offending_substring()).c_str() : "");
            }

            CallLuaFunctionWithHr(hr, "OnStringServiceVerifyString");
        });
#else
    LogToFile("StringServiceVerifyString is disabled on this platform.");
    CallLuaFunctionWithHr(S_OK, "OnStringServiceVerifyString");
#endif
    return LuaReturnHR(L, S_OK);
}

int StringServiceVerifyStrings_Lua(lua_State* L)
{
#if CPP_TESTS_ENABLED
    string_t testString1 = _T("Shltstring");
    string_t testString2 = _T("Goodstring");
    string_t testString3 = _T("Shltstring2");
    std::vector<string_t> testStrings{ testString1, testString2, testString3 };

    std::shared_ptr<xbox::services::xbox_live_context> xblc = std::make_shared<xbox::services::xbox_live_context>(Data()->xalUser);
    xblc->string_service().verify_strings(
        testStrings
    ).then(
        [](xbox::services::xbox_live_result<std::vector<xbox::services::system::verify_string_result>> result)
        {
            HRESULT hr = ConvertXboxLiveErrorCodeToHresult(result.err());
            LogToFile("StringServiceVerifyStrings: hr=%s", ConvertHR(hr).c_str());

            if (SUCCEEDED(hr))
            {
                std::vector<xbox::services::system::verify_string_result> verifyStringResults = result.payload();

                LogToScreen("StringServiceVerifyStrings: count=%d", verifyStringResults.size());
                assert(verifyStringResults.size() == 3);

                for (xbox::services::system::verify_string_result verifyStringResult : verifyStringResults)
                {
                    LogToFile(
                        "Result: Result Code: %d - First Offending String: %s",
                        verifyStringResult.result_code(),
                        (verifyStringResult.result_code() == xbox::services::system::verify_string_result_code::offensive) ? xbox::services::Utils::StringFromStringT(verifyStringResult.first_offending_substring()).c_str() : "");
                }
            }

            CallLuaFunctionWithHr(hr, "OnStringServiceVerifyStrings");
        });
#else
    LogToFile("StringServiceVerifyStrings is disabled on this platform.");
    CallLuaFunctionWithHr(S_OK, "OnStringServiceVerifyStrings");
#endif
    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_CppStringVerify()
{
    lua_register(Data()->L, "StringServiceVerifyString", StringServiceVerifyString_Lua);
    lua_register(Data()->L, "StringServiceVerifyStrings", StringServiceVerifyStrings_Lua);
}