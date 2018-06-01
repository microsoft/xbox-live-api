#include "pch.h"
#include "GameLogic\Game.h"

using namespace LongHaulTestApp;

///////////////////////////////////////
//////            Tests          //////
///////////////////////////////////////

void Game::TestProfileFlow()
{
    if (m_test == 0)
    {
        Log("===== Starting TestProfileFlow =====");
        TestGetUserProfile();
        m_test++;
    }
    else if (m_test == 1)
    {
        TestGetUserProfiles();
        m_test++;
    }
    else if (m_test == 2)
    {
        TestGetUserProfilesForSocialGroup();
        m_test++;
    }
    else if (m_test == 3)
    {
        m_test = 0;
        Log("===== Finished TestProfileFlow =====");
        EndTest();
    }
}

///////////////////////////////////////
//////            Utils          //////
///////////////////////////////////////

void Game::TestGetUserProfile()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        pThis->Log(L"XblProfileGetUserProfileResult");
        XblUserProfile profile;
        auto result = XblProfileGetUserProfileResult(asyncBlock, &profile);

        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully got the user profile!");

            pThis->TestProfileFlow();
        }
        else
        {
            pThis->Log(L"[Test] Failed getting the user profile.");
        }

        delete asyncBlock;
    };

    Log(L"XblProfileGetUserProfile");
    XblProfileGetUserProfileAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid
    );
}

void Game::TestGetUserProfiles()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        pThis->Log(L"XblProfileGetUserProfilesResult");
        uint32_t profilesCount;
        XblProfileGetUserProfilesResultCount(asyncBlock, &profilesCount);

        pThis->Log(L"XblProfileGetUserProfilesResult");
        XblUserProfile* profiles = (XblUserProfile*)malloc(sizeof(XblUserProfile) * profilesCount);
        auto result = XblProfileGetUserProfilesResult(asyncBlock, profilesCount, profiles);

        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully got user profiles!");

            pThis->TestProfileFlow();
        }
        else
        {
            pThis->Log(L"[Test] Failed getting user profiles.");
        }

        free(profiles);
        delete asyncBlock;
    };

    Log(L"XblProfileGetUserProfiles");
    std::vector<uint64_t> xuids;
    xuids.push_back(m_xuid);
    XblProfileGetUserProfilesAsync(
        asyncBlock,
        m_xboxLiveContext,
        xuids.data(),
        xuids.size()
    );
}

void Game::TestGetUserProfilesForSocialGroup()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        uint32_t profilesCount;

        pThis->Log(L"XblProfileGetUserProfilesForSocialGroupResultCount");
        XblProfileGetUserProfilesForSocialGroupResultCount(asyncBlock, &profilesCount);

        pThis->Log(L"XblProfileGetUserProfilesForSocialGroupResult");
        XblUserProfile* profiles = (XblUserProfile*)malloc(profilesCount * sizeof(XblUserProfile));
        auto result = XblProfileGetUserProfilesForSocialGroupResult(asyncBlock, profilesCount, profiles);

        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully got profiles for this title!");

            pThis->TestProfileFlow();
        }
        else
        {
            pThis->Log(L"[Test] Failed getting profiles for this title.");
        }

        free(profiles);
        delete asyncBlock;
    };

    Log(L"XblProfileGetUserProfilesForSocialGroup");
    XblProfileGetUserProfilesForSocialGroupAsync(
        asyncBlock,
        m_xboxLiveContext,
        "People"
    );
}