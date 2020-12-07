```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
auto contextPtr = std::make_unique<size_t>(static_cast<size_t>(sessionIndex));
asyncBlock->queue = queue;
asyncBlock->context = contextPtr.get();
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    std::unique_ptr<size_t> sessionIndexPtr{ static_cast<size_t*>(asyncBlock->context) };
    auto sessionIndex{ *sessionIndexPtr };

    XblMultiplayerSessionHandle sessionHandle = nullptr;
    auto hr = XblMultiplayerGetSessionByHandleResult(asyncBlock, &sessionHandle);

};

HRESULT hr = XblMultiplayerGetSessionByHandleAsync(
    xboxLiveContext,
    handleId.c_str(),
    asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
    contextPtr.release();
}
```
