```cpp
auto async = std::make_unique<XAsyncBlock>();
async->queue = queue;
async->callback = [](XAsyncBlock* async)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ async }; // take ownership of XAsyncBlock
    HRESULT hr = XAsyncGetStatus(async, false);
};

HRESULT hr = XblMultiplayerActivityFlushRecentPlayersAsync(xblContext, async.get());
if (SUCCEEDED(hr))
{
    async.release();
}
```
