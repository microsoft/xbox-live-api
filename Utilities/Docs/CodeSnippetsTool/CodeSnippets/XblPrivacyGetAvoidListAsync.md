```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultCount{};
    HRESULT hr = XblPrivacyGetAvoidListResultCount(asyncBlock, &resultCount);

    if (SUCCEEDED(hr))
    {
        std::vector<uint64_t> avoidedXuids(resultCount);
        hr = XblPrivacyGetAvoidListResult(asyncBlock, resultCount, avoidedXuids.data());
    }

};

HRESULT hr = XblPrivacyGetAvoidListAsync(xboxLiveContext, asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
