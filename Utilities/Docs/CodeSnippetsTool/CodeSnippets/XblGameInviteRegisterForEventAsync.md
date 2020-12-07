```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    XblRealTimeActivitySubscriptionHandle subscriptionHandle{ nullptr };
    HRESULT hr = XblGameInviteRegisterForEventResult(asyncBlock, &subscriptionHandle);
    
};

HRESULT hr = XblGameInviteRegisterForEventAsync(
    xboxLiveContext,
    asyncBlock.get()
);

if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
