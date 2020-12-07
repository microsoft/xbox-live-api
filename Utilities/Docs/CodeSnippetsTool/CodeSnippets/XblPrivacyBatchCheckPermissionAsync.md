```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
auto contextPtr = std::make_unique<size_t>(expectedResultCount);
asyncBlock->queue = queue;
asyncBlock->context = contextPtr.get(); 
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    std::unique_ptr<size_t> expectedCount{ static_cast<size_t*>(asyncBlock->context) };
    size_t resultSize;
    HRESULT hr = XblPrivacyBatchCheckPermissionResultSize(asyncBlock, &resultSize);

    if (SUCCEEDED(hr))
    {
        std::vector<char> buffer(resultSize, 0);
        size_t resultCount{};
        XblPermissionCheckResult* results{};

        hr = XblPrivacyBatchCheckPermissionResult(asyncBlock, resultSize, buffer.data(), &results, &resultCount, nullptr);
        if (SUCCEEDED(hr))
        {
            assert(resultCount == *expectedCount);
        }
    }

};

HRESULT hr = XblPrivacyBatchCheckPermissionAsync(
    xboxLiveContext,
    permissionsToCheck,
    _countof(permissionsToCheck),
    targetXuids,
    _countof(targetXuids),
    targetUserTypes,
    _countof(targetUserTypes),
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
    contextPtr.release();
}

```
