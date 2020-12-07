```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultSize{ 0 };
    HRESULT hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResultSize(asyncBlock, &resultSize);
    if (SUCCEEDED(hr))
    {
        if (resultSize > 0)
        {
            size_t count{ 0 };
            std::vector<char> buffer(resultSize, 0);
            XblMultiplayerActivityDetails* activityDetails{};
            hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupResult(asyncBlock, resultSize, buffer.data(), &activityDetails, &count, nullptr);
        }
    }

};

HRESULT hr = XblMultiplayerGetActivitiesWithPropertiesForSocialGroupAsync(
    xboxLiveContext,
    scid,
    socialGroupOwnerXuid,
    socialGroup.c_str(),
    asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
