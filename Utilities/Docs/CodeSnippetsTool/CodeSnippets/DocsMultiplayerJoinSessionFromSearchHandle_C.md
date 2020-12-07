```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
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
                XblMultiplayerSessionCreateHandle(xboxUserId, &multiplayerSessionReference, nullptr);

            XblMultiplayerSessionJoin(gameSession, nullptr, true, true);

            // TODO finish
              XblMultiplayerWriteSessionByHandleAsync(xboxLiveContext, gameSession, XblMultiplayerSessionWriteMode::UpdateExisting, handleId, async);

               XblMultiplayerManagerJoinGame(handleId, xalUser);

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
```
