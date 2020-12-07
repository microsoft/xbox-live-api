```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultCount{ 0 };
    HRESULT hr = XblPresenceGetPresenceForMultipleUsersResultCount(asyncBlock, &resultCount);

    if (SUCCEEDED(hr) && resultCount > 0)
    {
        std::vector<XblPresenceRecordHandle> handles(resultCount, nullptr);
        hr = XblPresenceGetPresenceForMultipleUsersResult(asyncBlock, handles.data(), resultCount);

        // Be sure to call XblPresenceRecordCloseHandle for each presence record when they are
        // no longer needed.
    }

};

std::vector<uint64_t> xuids{ 2814639011617876, 2814641789541994 };

// Filter results to only online users
XblPresenceQueryFilters filters{};
filters.onlineOnly = true;

HRESULT hr = XblPresenceGetPresenceForMultipleUsersAsync(xboxLiveContext, xuids.data(), xuids.size(), &filters, asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}

```
