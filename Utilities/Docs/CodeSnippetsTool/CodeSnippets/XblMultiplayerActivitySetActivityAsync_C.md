```cpp
auto async = std::make_unique<XAsyncBlock>();
async->queue = queue;
async->callback = [](XAsyncBlock* async)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ async }; // take ownership of XAsyncBlock
    HRESULT hr = XAsyncGetStatus(async, false);
};

XblMultiplayerActivityInfo info{};
info.connectionString = "dummyConnectionString";
info.joinRestriction = XblMultiplayerActivityJoinRestriction::Followed;
info.maxPlayers = 10;
info.currentPlayers = 1;
info.groupId = "dummyGroupId";

HRESULT hr = XblMultiplayerActivitySetActivityAsync(
    xblContext,
    &info,
    false,
    async.get()
);

if (SUCCEEDED(hr))
{
    async.release();
}
```
