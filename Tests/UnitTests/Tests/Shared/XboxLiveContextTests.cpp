// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(XboxLiveContextTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(XboxLiveContextTests);

    DEFINE_TEST_CASE(TestXboxLiveContext)
    {
        TEST_LOG(L"Test starting: TestXboxLiveContext");

        TestEnvironment env{};

        uint64_t mockXuid{ 1 };
        std::string mockGamertag = "MockGamertag";
        User mockUser = CreateMockUser(mockXuid, mockGamertag);

        struct State
        {
            XblContextHandle xboxLiveContext{ nullptr };
            XalUserHandle userFromContext{ nullptr };

            ~State()
            {
                XblContextCloseHandle(xboxLiveContext);
                XalUserCloseHandle(userFromContext);
            }
        } state;

        VERIFY_SUCCEEDED(XblContextCreateHandle(mockUser.Handle(), &state.xboxLiveContext));
        VERIFY_SUCCEEDED(XblContextGetUser(state.xboxLiveContext, &state.userFromContext));

        // Validate the attributes of the returned handle
        uint64_t xuid{ 0 };
        VERIFY_SUCCEEDED(XalUserGetId(state.userFromContext, &xuid));
        VERIFY_IS_TRUE(xuid == mockXuid);

        auto requiredSize = XalUserGetGamertagSize(state.userFromContext, XalGamertagComponent_Classic);
        VERIFY_IS_TRUE(mockGamertag.size() + 1 == requiredSize);

        std::vector<char> gamertag(requiredSize, char{});
        VERIFY_SUCCEEDED(XalUserGetGamertag(state.userFromContext, XalGamertagComponent_Classic, requiredSize, &gamertag[0], nullptr));
        VERIFY_IS_TRUE(mockGamertag == gamertag.data());

        VERIFY_SUCCEEDED(XblContextGetXboxUserId(state.xboxLiveContext, &xuid));
        VERIFY_IS_TRUE(xuid == mockXuid);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

