```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>(); 
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t profilesCount = 0;
    HRESULT hr = XblProfileGetUserProfilesForSocialGroupResultCount(asyncBlock, &profilesCount);
    if (SUCCEEDED(hr) && profilesCount > 0)
    {
        std::vector<XblUserProfile> profiles(profilesCount);
        hr = XblProfileGetUserProfilesForSocialGroupResult(asyncBlock, profilesCount, profiles.data());
    }
};

HRESULT hr = XblProfileGetUserProfilesForSocialGroupAsync(xboxLiveContext, socialGroup.c_str(), asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
