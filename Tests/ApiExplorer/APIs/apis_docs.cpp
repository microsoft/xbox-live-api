// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"

XBL_WARNING_DISABLE_DEPRECATED

void CodeSnippets()
{
    // Non-functional code used to illustrate examples in docs
    {
        auto asyncBlock = std::make_unique<XAsyncBlock>();

        // CODE SNIPPET START: XblPresenceSetPresenceAsyncWithIDs_C
        XblPresenceRichPresenceIds ids{};
        pal::strcpy(ids.scid, sizeof(ids.scid), Data()->scid);
        ids.presenceId = "playingMap";
        std::vector<const char*> tokenIds{ "CurrentMap" };
        ids.presenceTokenIds = tokenIds.data();
        ids.presenceTokenIdsCount = tokenIds.size();

        HRESULT hr = XblPresenceSetPresenceAsync(Data()->xboxLiveContext, true, &ids, asyncBlock.get());
        // CODE SNIPPET END

        UNREFERENCED_PARAMETER(hr);
    }

    {
        std::string scid = "123";
        std::string leaderboardName = "123";
        // CODE SNIPPET START: XblLeaderboardGetLeaderboardAsync-Rank
        XblLeaderboardQuery leaderboardQuery = {};
        pal::strcpy(leaderboardQuery.scid, sizeof(leaderboardQuery.scid), scid.c_str());
        leaderboardQuery.leaderboardName = leaderboardName.c_str();
        leaderboardQuery.skipResultToRank = 100;
        leaderboardQuery.maxItems = 100;
        // CODE SNIPPET END
    }

    {
        std::string scid = "123";
        std::string leaderboardName = "123";
        uint64_t xboxUserId = 123;
        // CODE SNIPPET START: XblLeaderboardGetLeaderboardAsync-User
        XblLeaderboardQuery leaderboardQuery = {};
        pal::strcpy(leaderboardQuery.scid, sizeof(leaderboardQuery.scid), scid.c_str());
        leaderboardQuery.leaderboardName = leaderboardName.c_str();
        leaderboardQuery.skipToXboxUserId = xboxUserId;
        leaderboardQuery.maxItems = 100;
        // CODE SNIPPET END
    }

    {
#if HC_PLATFORM != HC_PLATFORM_XDK
        // CODE SNIPPET START: XblEventsWriteInGameEvent
        HRESULT hr = XblEventsWriteInGameEvent(
            Data()->xboxLiveContext,
            "PuzzleSolved",
            R"({"DifficultyLevelId":100, "GameplayModeId":"Adventure"})",
            R"({"LocationX":1,"LocationY":1})"
        );
        // CODE SNIPPET END

        UNREFERENCED_PARAMETER(hr);
#endif
    }

    {
        auto xblContextHandle = Data()->xboxLiveContext;
        auto xboxUserId = Data()->xboxUserId;
        auto scid = Data()->scid;

        {
            // CODE SNIPPET START: DocsSubscribeToStatisticChange
            // Subscribe for statistic change events
            std::string statisticName = "totalPuzzlesSolved";
            XblRealTimeActivitySubscriptionHandle subscriptionHandle{ nullptr };
            HRESULT hr = XblUserStatisticsSubscribeToStatisticChange(
                xblContextHandle,
                xboxUserId,
                scid,
                statisticName.c_str(),
                &subscriptionHandle
            );

            // Add a statistic changed handler
            void* context{ nullptr };
            XblFunctionContext statisticChangedFunctionContext = XblUserStatisticsAddStatisticChangedHandler(
                Data()->xboxLiveContext,
                [](XblStatisticChangeEventArgs eventArgs, void* context)
                {
                    // Handle stat change 
                    LogToScreen("Statistic changed callback: stat changed (%s = %s)",
                        eventArgs.latestStatistic.statisticName,
                        eventArgs.latestStatistic.value);
                    UNREFERENCED_PARAMETER(context); // CODE SNIP SKIP
                },
                context
                );
            // CODE SNIPPET END

            UNREFERENCED_PARAMETER(statisticChangedFunctionContext);
            UNREFERENCED_PARAMETER(statisticName);
            UNREFERENCED_PARAMETER(hr);
        }

        {
            XblFunctionContext statisticChangedFunctionContext = 0;

            // CODE SNIPPET START: DocsUnsubscribeFromStatisticChange
            // Remove the statistic changed handler
            XblUserStatisticsRemoveStatisticChangedHandler(
                xblContextHandle,
                statisticChangedFunctionContext
            );

            // Unsubscribe for statistic change events
            HRESULT hr = XblUserStatisticsUnsubscribeFromStatisticChange(
                Data()->xboxLiveContext,
                Data()->statisticChangeSubscriptionHandle
            );
            // CODE SNIPPET END

            UNREFERENCED_PARAMETER(statisticChangedFunctionContext);
            UNREFERENCED_PARAMETER(hr);
        }

        {
            // CODE SNIPPET START: DocsAddConnectionIdChangedHandler
            void* context{ nullptr };
            XblFunctionContext connectionIdChangedFunctionContext = XblMultiplayerAddConnectionIdChangedHandler(
                xblContextHandle,
                [](void* context) {
                    UNREFERENCED_PARAMETER(context); // CODE SNIP SKIP
                    XTaskQueueHandle queue{ nullptr }; // CODE SNIP SKIP
                    auto xblContextHandle = Data()->xboxLiveContext; // CODE SNIP SKIP
                    XblMultiplayerSessionHandle sessionHandle; // Retrieve the MPSD session to update
                    sessionHandle = nullptr; // CODE SNIP SKIP
                    XblMultiplayerSessionCurrentUserSetStatus(sessionHandle, XblMultiplayerSessionMemberStatus::Active);

                    auto asyncBlock = std::make_unique<XAsyncBlock>();
                    asyncBlock->queue = queue;
                    asyncBlock->context = nullptr;
                    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
                    {
                        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock };

                        XblMultiplayerSessionHandle sessionHandle;
                        HRESULT hr = XblMultiplayerWriteSessionResult(asyncBlock, &sessionHandle);
                        if (SUCCEEDED(hr))
                        {
                            // If the write call succeeds, the connection id has been updated and no further action is needed.
                        }
                        else
                        {
                            // If the write call fails, it is likely the user has been removed from the session.
                        }
                    };

                    auto hr = XblMultiplayerWriteSessionAsync(xblContextHandle, sessionHandle, XblMultiplayerSessionWriteMode::UpdateExisting, asyncBlock.get());
                    if (SUCCEEDED(hr))
                    {
                        asyncBlock.release();
                    }
                }, 
                context);
            // CODE SNIPPET END
            UNREFERENCED_PARAMETER(connectionIdChangedFunctionContext);
        }
    }
}

void DocsMultiplayerCreateSession()
{
    auto xblContextHandle = Data()->xboxLiveContext;
    auto XUID = Data()->xboxUserId;
    const char* SCID{ Data()->scid };
    const char* SESSION_TEMPLATE_NAME{ "MinGameSession" };
    const char* SESSION_NAME{ "" };

    auto queue = Data()->queue;

    // CODE SNIPPET START: DocsMultiplayerCreateSession_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock };

        XblMultiplayerSessionHandle sessionHandle;
        HRESULT hr = XblMultiplayerWriteSessionResult(asyncBlock, &sessionHandle);
        if (SUCCEEDED(hr))
        {
            // Process multiplayer session handle
        }
        else
        {
            // Handle failure
        }
    };

    XblMultiplayerSessionReference ref;
    pal::strcpy(ref.Scid, sizeof(ref.Scid), SCID);
    pal::strcpy(ref.SessionTemplateName, sizeof(ref.SessionTemplateName), SESSION_TEMPLATE_NAME);
    pal::strcpy(ref.SessionName, sizeof(ref.SessionName), SESSION_NAME);

    XblMultiplayerSessionInitArgs args = {};

    XblMultiplayerSessionHandle sessionHandle = XblMultiplayerSessionCreateHandle(XUID, &ref, &args);

    auto hr = XblMultiplayerWriteSessionAsync(xblContextHandle, sessionHandle, XblMultiplayerSessionWriteMode::CreateNew, asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        asyncBlock.release();
    }
    // CODE SNIPPET END
}

void DocsMultiplayerJoinSessionFromSearchHandle()
{
    auto xblContextHandle = Data()->xboxLiveContext;
    auto scid = Data()->scid;

    // CODE SNIPPET START: DocsMultiplayerJoinSessionFromSearchHandle_C
    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock };
        size_t resultCount{ 0 };
        auto hr = XblMultiplayerGetSearchHandlesResultCount(asyncBlock, &resultCount);
        if (SUCCEEDED(hr) && resultCount > 0)
        {
            auto handles = new XblMultiplayerSearchHandle[resultCount];

            hr = XblMultiplayerGetSearchHandlesResult(asyncBlock, handles, resultCount);

            if (SUCCEEDED(hr))
            {
                // Join the game session
                const char* handleId{ nullptr };
                XblMultiplayerSearchHandleGetId(handles[0], &handleId);
                
                XblMultiplayerSessionReference multiplayerSessionReference;
                XblMultiplayerSearchHandleGetSessionReference(handles[0], &multiplayerSessionReference);

                XblMultiplayerSessionHandle gameSession =
                    XblMultiplayerSessionCreateHandle(Data()->xboxUserId, &multiplayerSessionReference, nullptr);

                XblMultiplayerSessionJoin(gameSession, nullptr, true, true);

                // TODO finish
//                XblMultiplayerWriteSessionByHandleAsync(Data()->xboxLiveContext, gameSession, XblMultiplayerSessionWriteMode::UpdateExisting, handleId, async);

//                 XblMultiplayerManagerJoinGame(handleId, Data()->xalUser);

                // Close handles
                for (auto i = 0u; i < resultCount; ++i)
                {
                    XblMultiplayerSearchHandleCloseHandle(handles[i]);
                }
            }
        }

    };

    const char* sessionName{ "MinGameSession" };
    const char* orderByAttribute{ nullptr };
    bool orderAscending{ false };
    const char* searchFilter{ nullptr };
    const char* socialGroup{ nullptr };

    HRESULT hr = XblMultiplayerGetSearchHandlesAsync(
        xblContextHandle,
        scid,
        sessionName,
        orderByAttribute,
        orderAscending,
        searchFilter,
        socialGroup,
        asyncBlock.get()
    );
    if (SUCCEEDED(hr))
    {
        asyncBlock.release();
    }
    // CODE SNIPPET END
}

void DocsMultiplayerManagerAddLocalUser_Single()
{
    auto xblUserHandle = Data()->xalUser;
    void* context = nullptr;

    // CODE SNIPPET START: DocsMultiplayerManagerAddLocalUser_Single_C
    HRESULT hr = XblMultiplayerManagerLobbySessionAddLocalUser(xblUserHandle);

    if (!SUCCEEDED(hr))
    {
        // Handle failure
    }

    // Set member connection address
    const char* connectionAddress = "1.1.1.1";
    hr = XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
        xblUserHandle, connectionAddress, context);

    if (!SUCCEEDED(hr))
    {
        // Handle failure
    }

    // Set custom member properties
    const char* propName = "Name";
    const char* propValueJson = "{}";
    hr = XblMultiplayerManagerLobbySessionSetProperties(propName, propValueJson, context);

    if (!SUCCEEDED(hr))
    {
        // Handle failure
    }
    // DOTS
    // CODE SNIPPET END
}

void DocsMultiplayerManagerAddLocalUser_Multiple()
{
    void* context = nullptr;

    // CODE SNIPPET START: DocsMultiplayerManagerAddLocalUser_Multiple_C
    std::vector<XblUserHandle> xblUsers;
    for (XblUserHandle xblUserHandle : xblUsers)
    {
        HRESULT hr = XblMultiplayerManagerLobbySessionAddLocalUser(xblUserHandle);

        if (!SUCCEEDED(hr))
        {
            // Handle failure
        }

        // Set member connection address
        const char* connectionAddress = "1.1.1.1";
        hr = XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
            xblUserHandle, connectionAddress, context);

        if (!SUCCEEDED(hr))
        {
            // Handle failure
        }

        // Set custom member properties
        const char* propName = "Name";
        const char* propValueJson = "{}";
        hr = XblMultiplayerManagerLobbySessionSetProperties(propName, propValueJson, context);

        if (!SUCCEEDED(hr))
        {
            // Handle failure
        }
        // DOTS
    }
    // CODE SNIPPET END
}

void DocsMultiplayerManagerJoinLobby()
{
    auto xblUserHandle = Data()->xalUser;
    auto inviteHandleId = "8907df69-558a-43cc-93ce-34a0a219da63";
    void* context = nullptr;

    // CODE SNIPPET START: DocsMultiplayerManagerJoinLobby_C
    HRESULT hr = XblMultiplayerManagerJoinLobby(inviteHandleId, xblUserHandle);
    if (!SUCCEEDED(hr))
    {
        // Handle failure
    }
    
    // Set member connection address
    const char* connectionAddress = "1.1.1.1";
    hr = XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
        xblUserHandle, connectionAddress, context);
    // CODE SNIPPET END
}