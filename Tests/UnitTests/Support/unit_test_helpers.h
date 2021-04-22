// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "mock_rta_service.h"

#define MOCK_LOCALID 101010101010101
#define MOCK_XUID 101010101010
#define MOCK_GAMERTAG "MockLocalUser"
#define MOCK_SCID "MockScid"
#define MOCK_TITLEID 1234
#define MOCK_SANDBOX "MockSandbox"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// Create a mock User. Doesn't depend on any XSAPI public APIs since it's just a wrapper around Xal mocks
User CreateMockUser(
    uint64_t xuid = MOCK_XUID,
    const std::string& gamertag = MOCK_GAMERTAG,
    uint64_t localId = MOCK_LOCALID
);

// Base class for manual XTaskQueue dispatcher
struct ITaskQueueDispatcher
{
public:
    virtual ~ITaskQueueDispatcher() = default;
protected:
    ITaskQueueDispatcher() = default;
};

// RAII Wrapper class around XblInitialize/XblCleanup. Each test method should assume XSAPI
// is not initialized and it should end in a clean state (whether it succeeds or fails).
class TestEnvironment
{
public:
    TestEnvironment() noexcept;
    TestEnvironment(const XblInitArgs* args);
    virtual ~TestEnvironment() noexcept;

    // Create an XblContext with a mock XalUser
    std::shared_ptr<XblContext> CreateMockXboxLiveContext(
        uint64_t xuid = MOCK_XUID,
        const std::string& gamertag = MOCK_GAMERTAG
    ) const noexcept;

    std::shared_ptr<xbox_live_context> CreateLegacyMockXboxLiveContext(
        uint64_t xuid = MOCK_XUID,
        const std::string& gamertag = MOCK_GAMERTAG
    ) const noexcept;

    system::MockRealTimeActivityService& MockRtaService() const noexcept;

private:
    std::shared_ptr<ITaskQueueDispatcher> m_dispatcher{ nullptr };
};

inline bool VerifyTime(
    time_t time,
    const std::string& timeString
)
{
    auto datetime = xbox::services::datetime::from_string(timeString.data(), xbox::services::datetime::date_format::ISO_8601);
    return xbox::services::utils::time_t_from_datetime(datetime) == time;
}

inline bool VerifyJson(
    const JsonValue& expected,
    const char* actual
)
{
    JsonDocument actualJson{};
    actualJson.Parse(actual);
    if (actualJson.HasParseError())
    {
        return false;
    }

    return expected == actualJson;
}

inline bool VerifyJson(
    const char* expected,
    const char* actual
)
{
    JsonDocument expectedJson{};
    expectedJson.Parse(expected);
    if (expectedJson.HasParseError())
    {
        return false;
    }

    return VerifyJson(expectedJson, actual);
}

inline std::string GetTestBasePath()
{
#ifdef USING_TAEF
    // Find TAEF DLL and use that as base path
    char path[MAX_PATH] = { 0 };
    HMODULE module = NULL;
    if (GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
        (LPCWSTR)&CreateMockUser, &module) != 0)
    {
        if (GetModuleFileNameA(module, path, sizeof(path)) != 0)
        {
            *strrchr(path, '\\') = 0;
            std::string folder = std::string(path);
            return folder + "\\";
        }
    }

    assert(false);
#endif

    return "";
}

inline JsonDocument GetTestResponses(
    const char* filePath
)
{
    std::string path = GetTestBasePath() + filePath;
    std::ifstream fileStream{ path, std::ifstream::binary };

    std::stringstream buffer;
    buffer << fileStream.rdbuf();

    JsonDocument d;
    d.Parse(buffer.str().data());
    return d;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END