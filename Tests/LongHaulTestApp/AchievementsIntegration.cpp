#include "pch.h"
#include "GameLogic\Game.h"

using namespace LongHaulTestApp;

///////////////////////////////////////
//////            Tests          //////
///////////////////////////////////////

void Game::TestAchievementsFlow()
{
    Log("===== Starting TestAchievementFlow =====");
    GetAchievmentsForTitle();
    m_progress = (m_progress + 1) % 101;
}

///////////////////////////////////////
//////            Utils          //////
///////////////////////////////////////

void Game::GetAchievmentsForTitle()
{
    Log("XblGetXboxLiveAppConfig");
    XblGetXboxLiveAppConfig(&m_config);

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

            pThis->Log("XblAchievementsGetAchievementsForTitleIdResult");
            XblAchievementsResult* achievementsResult = (XblAchievementsResult*)malloc(size);
            XblAchievementsGetAchievementsForTitleIdResult(asyncBlock, size, achievementsResult, nullptr);

            pThis->AchievementResultsGetNext(achievementsResult);
        }
        else
        {
            pThis->Log(L"[Test] Failed getting achievements for this title.");
        }

        delete asyncBlock;
    };

    Log("XblAchievementsGetAchievementsForTitleId");
    XblAchievementsGetAchievementsForTitleId(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        m_config->titleId,
        XblAchievementType_All,
        false,
        XblAchievementOrderBy_DefaultOrder,
        0,
        1);
}

void Game::AchievementResultsGetNext(XblAchievementsResult* result)
{
    if (result->hasNext)
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
                pThis->Log(L"[Test] Successfully got next page of achievements!");

                pThis->Log("XblAchievementsResultGetNextResult");
                XblAchievementsResult* achievementsResult = (XblAchievementsResult*)malloc(size);
                XblAchievementsResultGetNextResult(asyncBlock, size, achievementsResult, nullptr);

                pThis->AchievementResultsGetNext(achievementsResult);
            }
            else
            {
                pThis->Log(L"[Test] Failed getting next page of achievements.");
            }

            delete asyncBlock;
        };

        Log("XblAchievementsResultGetNext");
        XblAchievementsResultGetNext(
            asyncBlock,
            m_xboxLiveContext,
            result,
            1);
    }
    else if (result->itemsCount > 0)
    {
        GetAchievement(result->items[0]->serviceConfigurationId, result->items[0]->id);
    }
}

void Game::GetAchievement(PCSTR scid, PCSTR achievementId)
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
            pThis->Log(L"[Test] Successfully got achievement!");

            pThis->Log("XblAchievementsGetAchievementResult");
            XblAchievement* achievement = (XblAchievement*)malloc(size);
            XblAchievementsGetAchievementResult(asyncBlock, size, achievement, nullptr);

            pThis->UpdateAchievement(achievement->serviceConfigurationId, achievement->id);
        }
        else
        {
            pThis->Log(L"[Test] Failed getting achievement.");
        }

        delete asyncBlock;
    };

    Log("XblAchievementsGetAchievement");
    XblAchievementsGetAchievement(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        scid,
        achievementId);
}

void Game::UpdateAchievement(PCSTR scid, PCSTR achievementId)
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        auto result = GetAsyncStatus(asyncBlock, false);
        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully updated achievement!");
        }
        else if (result == HTTP_E_STATUS_NOT_MODIFIED)
        {
            pThis->Log(L"[Test] Achievement not modified!");
        }
        else
        {
            pThis->Log(L"[Test] Failed updating achievement.");
        }

        delete asyncBlock;


        pThis->Log("===== Finished TestAchievementFlow =====");
        pThis->EndTest();
    };

    Log("XblAchievementsUpdateAchievement");
    auto tid = m_config->titleId;
    XblAchievementsUpdateAchievement(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        &tid,
        scid,
        achievementId,
        m_progress);
}
