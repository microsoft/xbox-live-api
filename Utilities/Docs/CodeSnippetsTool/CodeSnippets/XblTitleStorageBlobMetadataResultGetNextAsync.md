```cpp
XblTitleStorageBlobMetadataResultHandle handle = blobMetadataResultHandle;

auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    XblTitleStorageBlobMetadataResultHandle handle{ nullptr };
    
    HRESULT hr = XblTitleStorageBlobMetadataResultGetNextResult(asyncBlock, &handle);

};

HRESULT hr = XblTitleStorageBlobMetadataResultGetNextAsync(handle, maxItems, asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
