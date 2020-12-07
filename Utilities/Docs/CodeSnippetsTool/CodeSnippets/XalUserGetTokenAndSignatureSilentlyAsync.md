```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>(); 
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t bufferSize;
    HRESULT hr = XalUserGetTokenAndSignatureSilentlyResultSize(asyncBlock, &bufferSize);
    std::vector<byte> buffer(bufferSize);
    XalUserGetTokenAndSignatureData* result;
    hr = XalUserGetTokenAndSignatureSilentlyResult(asyncBlock, bufferSize, buffer.data(), &result, nullptr);
};

XalUserGetTokenAndSignatureArgs args{};
args.method = method.data();
args.url = url.data();

HRESULT hr = XalUserGetTokenAndSignatureSilentlyAsync(xalUser, &args, asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
