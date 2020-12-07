```cpp
auto async = std::make_unique<XAsyncBlock>();
async->queue = queue;
async->callback = [](XAsyncBlock* async)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ async }; // take ownership of XAsyncBlock

    size_t resultSize{};
    HRESULT hr = XblMultiplayerActivityGetActivityResultSize(async, &resultSize);
    if (SUCCEEDED(hr))
    {
        std::vector<uint8_t> buffer(resultSize);
        XblMultiplayerActivityInfo* activityInfo{};
        size_t resultCount{};
        hr = XblMultiplayerActivityGetActivityResult(async, buffer.size(), buffer.data(), &activityInfo, &resultCount, nullptr);
        if (SUCCEEDED(hr))
        {
            // ...
        }
    }
};

HRESULT hr = XblMultiplayerActivityGetActivityAsync(
    xblContext,
    &xuid,
    1,
    async.get()
);

if (SUCCEEDED(hr))
{
    async.release();
}
```
