```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultCount{ 0 };
    auto hr = XblMultiplayerGetSearchHandlesResultCount(asyncBlock, &resultCount);
    if (SUCCEEDED(hr) && resultCount > 0)
    {
        auto handles = new XblMultiplayerSearchHandle[resultCount];

        hr = XblMultiplayerGetSearchHandlesResult(asyncBlock, handles, resultCount);

        if (SUCCEEDED(hr))
        {

            // Process handles
            for (auto i = 0u; i < resultCount; ++i)
            {
                const char* handleId{ nullptr };
                XblMultiplayerSearchHandleGetId(handles[i], &handleId);

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
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
