#include "pch.h"
#include "Tests.h"
#include "GameLogic\Game.h"

using namespace xbox::services;
using namespace xbox::services::system;

uint32_t Tests::TestDelay = 0;
uint64_t Tests::TestsRun = 0;

Tests::Tests(
    xbl_context_handle xboxLiveContext, 
    async_queue_handle_t queue,
    bool runSocialManagerTests
    ) : 
    m_xboxLiveContext(xboxLiveContext),
    m_queue(queue),
    m_runSocialManagerTests(runSocialManagerTests)
{
    XblContextDuplicateHandle(m_xboxLiveContext);
    XblContextGetUser(m_xboxLiveContext, &m_user);
    XblContextGetXboxUserId(m_xboxLiveContext, &m_xuid);
    XblGetXboxLiveAppConfig(&m_config);

    Log("Starting Tests");
}

Tests::~Tests()
{
    XblContextCloseHandle(m_xboxLiveContext);
}

void Tests::HandleTests()
{
    if ((time(NULL) - m_time) > TestDelay)
    {
        RunTests();
    }
}

void Tests::RunTests()
{
    m_time = time(NULL);
    TestsRun++;

    TestAchievementsFlow();

    TestProfileFlow();

    TestSocialFlow();

    if (m_runSocialManagerTests)
    {
        TestSocialManagerFlow();
    }

    g_sampleInstance->PrintMemoryUsage();
 }

void Tests::Log(std::wstring log, bool showOnUI)
{
    g_sampleInstance->Log(log, showOnUI);
}

void Tests::Log(std::string log, bool showOnUI)
{
    Log(utility::conversions::to_utf16string(log), showOnUI);
}