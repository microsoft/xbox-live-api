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
        m_test = 3; // todo remove
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

        size_t size = 0;
        auto result = GetAsyncResultSize(asyncBlock, &size);

        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully got the user profile!");

            pThis->Log(L"XblProfileGetUserProfileResult");
            XblUserProfile* profile = (XblUserProfile*)malloc(size);
            auto result = XblProfileGetUserProfileResult(asyncBlock, profile);

            pThis->TestProfileFlow();
        }
        else
        {
            pThis->Log(L"[Test] Failed getting the user profile.");
        }

        delete asyncBlock;
    };

    Log(L"XblProfileGetUserProfile");
    XblProfileGetUserProfile(
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

        size_t size = 0;
        auto result = GetAsyncResultSize(asyncBlock, &size);

        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully got user profiles!");

            uint32_t profilesCount, profilesWritten;

            pThis->Log(L"XblProfileGetUserProfilesResultCount");
            XblProfileGetUserProfilesResultCount(asyncBlock, &profilesCount);

            pThis->Log(L"XblProfileGetUserProfilesResult");
            XblUserProfile* profiles = (XblUserProfile*)malloc(size);
            auto result = XblProfileGetUserProfilesResult(asyncBlock, profilesCount, profiles, &profilesWritten);

            pThis->TestProfileFlow();
        }
        else
        {
            pThis->Log(L"[Test] Failed getting user profiles.");
        }

        delete asyncBlock;
    };

    Log(L"XblProfileGetUserProfiles");
    std::vector<uint64_t> xuids;
    xuids.push_back(m_xuid);
    XblProfileGetUserProfiles(
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

        size_t size = 0;
        auto result = GetAsyncResultSize(asyncBlock, &size);

        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully got achievements for this title!");

            uint32_t profilesCount, profilesWritten;

            pThis->Log(L"XblProfileGetUserProfilesForSocialGroupResultCount");
            XblProfileGetUserProfilesForSocialGroupResultCount(asyncBlock, &profilesCount);

            pThis->Log(L"XblProfileGetUserProfilesForSocialGroupResult");
            XblUserProfile* profiles = (XblUserProfile*)malloc(size);
            auto result = XblProfileGetUserProfilesForSocialGroupResult(asyncBlock, profilesCount, profiles, &profilesWritten);

            pThis->TestProfileFlow();
        }
        else
        {
            pThis->Log(L"[Test] Failed getting achievements for this title.");
        }

        delete asyncBlock;
    };

    Log(L"XblProfileGetUserProfilesForSocialGroup");
    XblProfileGetUserProfilesForSocialGroup(
        asyncBlock,
        m_xboxLiveContext,
        "People"
    );
}