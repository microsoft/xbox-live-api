```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    XblTitleStorageBlobMetadataResultHandle handle;
    HRESULT hr = XblTitleStorageGetBlobMetadataResult(asyncBlock, &handle);

    if (SUCCEEDED(hr))
    {
        if (blobMetadataResultHandle != nullptr)
        {
            XblTitleStorageBlobMetadataResultCloseHandle(blobMetadataResultHandle);
        }

        blobMetadataResultHandle = handle;
    }

};

HRESULT hr = XblTitleStorageGetBlobMetadataAsync(
    xboxLiveContext,
    scid,
    storageType,
    blobPath.c_str(),
    xboxUserId,
    skipItems,
    maxItems,
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
