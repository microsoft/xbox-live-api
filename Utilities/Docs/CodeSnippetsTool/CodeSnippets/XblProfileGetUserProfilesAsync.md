```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>(); 
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    uint32_t profilesCount = 1;
    XblUserProfile profiles[1] = { 0 };
    HRESULT hr = XblProfileGetUserProfilesResult(asyncBlock, profilesCount, profiles);
};

uint64_t xboxUserIds[1];
xboxUserIds[0] = xboxUserId;
size_t xboxUserIdsCount = 1;

HRESULT hr = XblProfileGetUserProfilesAsync(xboxLiveContext, xboxUserIds, xboxUserIdsCount, asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
