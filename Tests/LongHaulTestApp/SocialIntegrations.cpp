#include "pch.h"
#include "Tests.h"

///////////////////////////////////////
//////            Tests          //////
///////////////////////////////////////
void Tests::TestSocialFlow()
{
    Log("===== Starting TestSocialFlow =====");
    GetSocialRelationship();
}

///////////////////////////////////////
//////            Utils          //////
///////////////////////////////////////

void Tests::GetSocialRelationship()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Tests *pThis = reinterpret_cast<Tests*>(asyncBlock->context);

        pThis->Log(L"XblSocialGetSocialRelationshipsResult");
        xbl_social_relationship_result_handle relationshipResult;
        auto result = XblSocialGetSocialRelationshipsResult(asyncBlock, &relationshipResult);

        if (SUCCEEDED(result))
        {
            pThis->Log(L"[Test] Successfully got the social relationship!");

            pThis->SocialRelationshipGetNext(relationshipResult);

            XblSocialRelationshipResultCloseHandle(relationshipResult);
        }
        else
        {
            pThis->Log(L"[Test] Failed getting the social relationship.");
        }

        delete asyncBlock;
    };

    Log(L"XblSocialGetSocialRelationships");
    XblSocialGetSocialRelationshipsAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        XblSocialRelationshipFilter::XblSocialRelationshipFilter_All
    );
}

void Tests::SocialRelationshipGetNext(xbl_social_relationship_result_handle relationshipResult)
{
    bool hasNext;
    XblSocialRelationshipResultHasNext(relationshipResult, &hasNext);

    if (hasNext)
    {
        AsyncBlock* asyncBlock = new AsyncBlock{};
        asyncBlock->queue = m_queue;
        asyncBlock->context = this;
        asyncBlock->callback = [](AsyncBlock* asyncBlock)
        {
            Tests *pThis = reinterpret_cast<Tests*>(asyncBlock->context);

            pThis->Log("XblSocialRelationshipResultGetNextResult");
            xbl_social_relationship_result_handle relationship;
            auto result = XblSocialRelationshipResultGetNextResult(asyncBlock, &relationship);
            
            if (SUCCEEDED(result))
            {
                pThis->Log(L"[Test] Successfully got next page of relationships!");

                pThis->SocialRelationshipGetNext(relationship);

                XblSocialRelationshipResultCloseHandle(relationship);
            }
            else
            {
                pThis->Log(L"[Test] Failed getting next page of relationships.");
            }

            delete asyncBlock;
        };

        Log("XblSocialRelationshipResultGetNext");
        XblSocialRelationshipResultGetNextAsync(
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

void Tests::TestResputationFeedback()
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
            pThis->Log(L"[Test] Successfully got the social relationship!");

            pThis->Log("===== Finished TestSocialFlow =====");
        }
        else
        {
            pThis->Log(L"[Test] Failed getting the social relationship.");
        }

        delete asyncBlock;
    };

    Log(L"XblSocialSubmitReputationFeedback");
    XblSocialSubmitReputationFeedbackAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        XblReputationFeedbackType_PositiveSkilledPlayer,
        nullptr,
        nullptr,
        nullptr
    );
}