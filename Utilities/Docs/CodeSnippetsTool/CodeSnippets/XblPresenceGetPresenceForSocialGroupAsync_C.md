```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultCount{ 0 };
    HRESULT hr = XblPresenceGetPresenceForSocialGroupResultCount(asyncBlock, &resultCount);

    if (SUCCEEDED(hr) && resultCount > 0)
    {
        std::vector<XblPresenceRecordHandle> handles(resultCount, nullptr);
        hr = XblPresenceGetPresenceForSocialGroupResult(asyncBlock, handles.data(), resultCount);

        // Be sure to call XblPresenceRecordCloseHandle for each presence record when they are
        // no longer needed.
    }

};

HRESULT hr = XblPresenceGetPresenceForSocialGroupAsync(xboxLiveContext, "Favorites", nullptr, nullptr, asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}

```
