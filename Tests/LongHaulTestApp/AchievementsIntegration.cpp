#include "pch.h"
#include "Tests.h"

///////////////////////////////////////
//////            Tests          //////
///////////////////////////////////////

void Tests::TestAchievementsFlow()
{
    Log("===== Starting TestAchievementFlow =====");
    GetAchievmentsForTitle();
    m_progress = (m_progress + 1) % 101;
}

///////////////////////////////////////
//////            Utils          //////
///////////////////////////////////////

void Tests::GetAchievmentsForTitle()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Tests *pThis = reinterpret_cast<Tests*>(asyncBlock->context);

        pThis->Log("XblAchievementsGetAchievementsForTitleIdResult");
        xbl_achievements_result_handle achievementsResult;
        auto result = XblAchievementsGetAchievementsForTitleIdResult(asyncBlock, &achievementsResult);
        
        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully got achievements for this title!");

            pThis->AchievementResultsGetNext(achievementsResult);
        }
        else
        {
            pThis->Log(L"[Test] Failed getting achievements for this title.");
        }

        delete asyncBlock;
    };

    Log("XblAchievementsGetAchievementsForTitleIdAsync");
    XblAchievementsGetAchievementsForTitleIdAsync(
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

void Tests::AchievementResultsGetNext(xbl_achievements_result_handle result)
{
    struct context_t
    {
        Tests* pThis;
        xbl_achievements_result_handle resultHandle;
    };

    bool hasNext;
    XblAchievementsResultHasNext(result, &hasNext);

    XblAchievement* achievements;
    uint32_t achievementsCount;
    XblAchievementsResultGetAchievements(result, &achievements, &achievementsCount);

    if (hasNext)
    {
        AsyncBlock* asyncBlock = new AsyncBlock{};
        asyncBlock->queue = m_queue;
        asyncBlock->context = new context_t{ this, result };
        asyncBlock->callback = [](AsyncBlock* asyncBlock)
        {
            auto context = reinterpret_cast<context_t*>(asyncBlock->context);
            auto pThis = context->pThis;

            pThis->Log("XblAchievementsResultGetNextResult");
            xbl_achievements_result_handle achievementsResult;
            auto result = XblAchievementsResultGetNextResult(asyncBlock, &achievementsResult);

            if (SUCCEEDED(result))
            {
                pThis->Log(L"[Test] Successfully got next page of achievements!");

                pThis->AchievementResultsGetNext(achievementsResult);

                XblAchievementsResultCloseHandle(context->resultHandle);
            }
            else
            {
                pThis->Log(L"[Test] Failed getting next page of achievements.");
            }

            delete asyncBlock->context;
            delete asyncBlock;
        };

        Log("XblAchievementsResultGetNextAsync");
        XblAchievementsResultGetNextAsync(
            asyncBlock,
            m_xboxLiveContext,
            result,
            1);
    }
    else if (achievementsCount > 0)
    {
        GetAchievement(achievements[0].serviceConfigurationId, achievements[0].id);
        XblAchievementsResultCloseHandle(result);
    }
}

void Tests::GetAchievement(PCSTR scid, PCSTR achievementId)
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Tests *pThis = reinterpret_cast<Tests*>(asyncBlock->context);

        pThis->Log("XblAchievementsGetAchievementResult");
        xbl_achievements_result_handle achievementsResult;
        auto result = XblAchievementsGetAchievementResult(asyncBlock, &achievementsResult);
        
        if (SUCCEEDED(result))
        {
            pThis->Log("XblAchievementsResultGetAchievements");
            XblAchievement* achievement;
            uint32_t achievementCount;
            XblAchievementsResultGetAchievements(achievementsResult, &achievement, &achievementCount);
            
            pThis->Log(L"[Test] Successfully got achievement!");
            pThis->UpdateAchievement(achievement->serviceConfigurationId, achievement->id);

            XblAchievementsResultCloseHandle(achievementsResult);
        }
        else
        {
            pThis->Log(L"[Test] Failed getting achievement.");
        }

        delete asyncBlock;
    };

    Log("XblAchievementsGetAchievementAsync");
    XblAchievementsGetAchievementAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        scid,
        achievementId);
}

void Tests::UpdateAchievement(PCSTR scid, PCSTR achievementId)
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Tests *pThis = reinterpret_cast<Tests*>(asyncBlock->context);

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
    };

    Log("XblAchievementsUpdateAchievement");
    auto tid = m_config->titleId;
    XblAchievementsUpdateAchievementAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        &tid,
        scid,
        achievementId,
        m_progress);
}
