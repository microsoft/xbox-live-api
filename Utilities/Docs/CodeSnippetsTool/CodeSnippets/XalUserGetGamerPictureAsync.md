```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>(); 
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t bufferSize;
    HRESULT hr = XalUserGetGamerPictureResultSize(asyncBlock, &bufferSize);
    std::vector<byte> buffer(bufferSize);
    hr = XalUserGetGamerPictureResult(asyncBlock, bufferSize, buffer.data());
};

HRESULT hr = XalUserGetGamerPictureAsync(xalUser, XalGamerPictureSize_Small, asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
