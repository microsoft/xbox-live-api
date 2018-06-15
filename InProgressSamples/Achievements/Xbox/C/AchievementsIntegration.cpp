#include "pch.h"
#include "Support\Game.h"
#include "AsyncIntegration.h"

IInspectable* AsInspectable(Platform::Object^ object)
{
    return reinterpret_cast<IInspectable*>(object);
}

void Game::InitializeXboxLive()
{
    uint32_t sharedAsyncQueueId = 0;
    CreateSharedAsyncQueue(
        sharedAsyncQueueId,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        &m_queue);

    XblInitialize();
    InitializeAsync(m_queue, &m_asyncQueueCallbackToken);

    XblContextCreateHandle(AsInspectable(m_userController->GetCurrentUser()), &m_xboxLiveContext);
    XblContextGetXboxUserId(m_xboxLiveContext, &m_xuid);
    XblGetXboxLiveAppConfig(&m_config);

    m_achievementsResultSet = false;
}

void Game::CleanupXboxLive()
{
    if (m_achievementsResultSet)
    {
        XblAchievementsResultCloseHandle(m_achievementsResult);
    }

    if (m_xboxLiveContext != nullptr)
    {
        XblContextCloseHandle(m_xboxLiveContext);
    }

    CleanupAsync(m_queue, m_asyncQueueCallbackToken);

    XblCleanup();
}

void Game::SetAchievementsResult(
    _In_ xbl_achievements_result_handle achievementsResult
    )
{
    if (m_achievementsResultSet)
    {
        XblAchievementsResultCloseHandle(m_achievementsResult);
    }
    m_achievementsResult = achievementsResult;
    m_achievementsResultSet = true;
}

void Game::GetAchievementsForTitle(
    _In_ uint32_t skipItems, 
    _In_ uint32_t maxItems
    )
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        xbl_achievements_result_handle resultHandle;
        auto hr = XblAchievementsGetAchievementsForTitleIdResult(asyncBlock, &resultHandle);

        if (SUCCEEDED(hr))
        {
            pThis->Log(L"Successfully got achievements for this title!");
            
            XblAchievement* achievements;
            uint32_t achievementsCount;
            XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount);

            // Process achievements
            pThis->SetAchievementsResult(resultHandle);
        }
        else
        {
            pThis->Log(L"Failed getting achievements for this title.");
        }

        delete asyncBlock;
    };

    XblAchievementsGetAchievementsForTitleIdAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        m_config->titleId,
        XblAchievementType_All,
        false,
        XblAchievementOrderBy_DefaultOrder,
        skipItems,
        maxItems);
}

void Game::AchievementResultsGetNext(
    _In_ xbl_achievements_result_handle resultHandle,
    _In_ uint32_t maxItems
    )
{
    bool hasNext = false;
    XblAchievementsResultHasNext(resultHandle, &hasNext);
    
    if (!hasNext)
    {
        Log(L"Achievements result doesn't have a next page");
        return;
    }

    struct context_t
    {
        Game* pThis;
        xbl_achievements_result_handle resultHandle;
    };

    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = new context_t{ this, resultHandle };
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        auto context = reinterpret_cast<context_t*>(asyncBlock->context);

        xbl_achievements_result_handle nextResultHandle;
        auto hr = XblAchievementsResultGetNextResult(asyncBlock, &nextResultHandle);

        if (SUCCEEDED(hr))
        {
            context->pThis->Log(L"Successfully got next page of achievements!");

            // Process next page of achievements
            context->pThis->SetAchievementsResult(nextResultHandle);
        }
        else
        {
            context->pThis->Log(L"Failed getting next page of achievements.");
        }

        delete asyncBlock->context;
        delete asyncBlock;
    };

    XblAchievementsResultGetNextAsync(
        asyncBlock,
        m_xboxLiveContext,
        resultHandle,
        maxItems);
}

void Game::GetAchievement(
    _In_ const char* achievementId
    )
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        xbl_achievements_result_handle resultHandle;
        auto hr = XblAchievementsGetAchievementResult(asyncBlock, &resultHandle);

        if (SUCCEEDED(hr))
        {
            pThis->Log(L"Successfully got achievement!");

            // Process achievement
            pThis->SetAchievementsResult(resultHandle);
        }
        else
        {
            pThis->Log(L"Failed getting achievement.");
        }

        delete asyncBlock;
    };

    XblAchievementsGetAchievementAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        m_config->scid,
        achievementId
    );
}

void Game::UpdateAchievement(
    _In_ const char* achievementId,
    _In_ uint32_t percentComplete
    )
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
            pThis->Log(L"Successfully updated achievement!");
        }
        else if (result == HTTP_E_STATUS_NOT_MODIFIED)
        {
            pThis->Log(L"Achievement not modified!");
        }
        else
        {
            pThis->Log(L"Failed updating achievement.");
        }

        delete asyncBlock;
    };

    XblAchievementsUpdateAchievementAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        &m_config->titleId,
        m_config->scid,
        achievementId,
        percentComplete);
}