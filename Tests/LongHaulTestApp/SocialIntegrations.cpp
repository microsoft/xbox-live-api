#include "pch.h"
#include "GameLogic\Game.h"

using namespace LongHaulTestApp;

///////////////////////////////////////
//////            Tests          //////
///////////////////////////////////////
void Game::TestSocialFlow()
{
    Log("===== Starting TestSocialFlow =====");
    GetSocialRelationship();
}

///////////////////////////////////////
//////            Utils          //////
///////////////////////////////////////

void Game::GetSocialRelationship()
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
            pThis->Log(L"[Test] Successfully got the social relationship!");

            pThis->Log(L"XblSocialGetSocialRelationshipsResult");
            size_t bytesWritten;
            XblSocialRelationshipResult* relationshipResult = (XblSocialRelationshipResult*)malloc(size);
            auto result = XblSocialGetSocialRelationshipsResult(asyncBlock, size, relationshipResult, &bytesWritten);
            
            pThis->SocialRelationshipGetNext(relationshipResult);
        }
        else
        {
            pThis->Log(L"[Test] Failed getting the social relationship.");
        }

        delete asyncBlock;
    };

    Log(L"XblSocialGetSocialRelationships");
    XblSocialGetSocialRelationships(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        XblSocialRelationshipFilter::XblSocialRelationshipFilter_All
    );
}

void Game::SocialRelationshipGetNext(XblSocialRelationshipResult* relationshipResult)
{
    if (relationshipResult->hasNext)
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
                pThis->Log(L"[Test] Successfully got next page of relationships!");

                pThis->Log("XblSocialRelationshipResultGetNextResult");
                XblSocialRelationshipResult* relationship = (XblSocialRelationshipResult*)malloc(size);
                XblSocialRelationshipResultGetNextResult(asyncBlock, size, relationship, nullptr);

                pThis->SocialRelationshipGetNext(relationship);
            }
            else
            {
                pThis->Log(L"[Test] Failed getting next page of relationships.");
            }

            delete asyncBlock;
        };

        Log("XblSocialRelationshipResultGetNext");
        XblSocialRelationshipResultGetNext(
            asyncBlock,
            m_xboxLiveContext,
            relationshipResult,
            1);
    }
    else
    {
        TestResputationFeedback();
    }
}

void Game::TestResputationFeedback()
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
            pThis->Log(L"[Test] Successfully got the social relationship!");

            pThis->Log("===== Finished TestSocialFlow =====");
            pThis->EndTest();
        }
        else
        {
            pThis->Log(L"[Test] Failed getting the social relationship.");
        }

        delete asyncBlock;
    };

    Log(L"XblSocialSubmitReputationFeedback");
    XblSocialSubmitReputationFeedback(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        XblReputationFeedbackType_PositiveSkilledPlayer,
        nullptr,
        nullptr,
        nullptr
    );
}