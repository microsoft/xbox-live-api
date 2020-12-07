```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    XblAchievementsResultHandle resultHandle;
    auto hr = XblAchievementsResultGetNextResult(asyncBlock, &resultHandle);

    if (SUCCEEDED(hr))
    {
        const XblAchievement* achievements = nullptr;
        size_t achievementsCount = 0;
        hr = XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount);

        for (size_t i = 0; i < achievementsCount; i++)
        {
            LogToScreen("Achievement %s: %s = %s",
                achievements[i].id,
                achievements[i].name,
                (achievements[i].progressState == XblAchievementProgressState::Achieved) ? "Achieved" : "Not achieved");
        }

        XblAchievementsResultCloseHandle(resultHandle); // when done with handle, close it
        achievements = nullptr; // Clear array after calling XblAchievementsResultCloseHandle to pointer to freed memory
    }
};

HRESULT hr = XblAchievementsResultGetNextAsync(
    achievementsResult,
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
