```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    auto result = XAsyncGetStatus(asyncBlock, false);
    if (SUCCEEDED(result))
    {
        // Achievement updated
    }
    else if (result == HTTP_E_STATUS_NOT_MODIFIED)
    {
        // Achievement not modified
    }
    else
    {
        // Achievement failed to update
    }
};

HRESULT hr = XblAchievementsUpdateAchievementAsync(
    xboxLiveContext,
    xboxUserId,
    achievementId.c_str(),
    percentComplete,
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
