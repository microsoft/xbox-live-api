```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = blobBuffer.get();
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    HRESULT hr = XblTitleStorageUploadBlobResult(asyncBlock, &binaryBlobMetadata);
};

XblTitleStorageBlobMetadata blobMetadata{};
pal::strcpy(blobMetadata.displayName, displayName.size() + 1, displayName.c_str());
pal::strcpy(blobMetadata.serviceConfigurationId, XBL_SCID_LENGTH, scid);
pal::strcpy(blobMetadata.blobPath, blobPath.size() + 1, blobPath.c_str());
blobMetadata.storageType = storageType;
blobMetadata.blobType = blobType;
time(&blobMetadata.clientTimestamp);

HRESULT hr = XblTitleStorageUploadBlobAsync(
    xboxLiveContext,
    blobMetadata,
    reinterpret_cast<const uint8_t*>(blobBuffer->data()),
    blobBufferSize,
    eTagMatchCondition,
    preferredUploadBlockSize, // optional
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
    blobBuffer.release();
}
```
