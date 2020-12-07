```cpp
std::unique_ptr<std::vector<uint8_t>> downloadBlobBuffer = std::make_unique<std::vector<uint8_t>>(binaryBlobMetadata.length);

auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = downloadBlobBuffer.get();
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    std::unique_ptr<std::vector<uint8_t>> downloadBlobBuffer{ static_cast<std::vector<uint8_t>*>(asyncBlock->context) };
    HRESULT hr = XblTitleStorageDownloadBlobResult(asyncBlock, &binaryBlobMetadata);
};

HRESULT hr = XblTitleStorageDownloadBlobAsync(
    xboxLiveContext,
    binaryBlobMetadata,
    downloadBlobBuffer->data(),
    binaryBlobMetadata.length,
    XblTitleStorageETagMatchCondition::NotUsed,
    selectQuery.c_str(), // optional
    preferredDownloadBlockSize, // optional
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
    downloadBlobBuffer.release();
}
```
