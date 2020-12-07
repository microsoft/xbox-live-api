```cpp
std::vector<XblReputationFeedbackItem> feedbackItems;
feedbackItems.push_back(XblReputationFeedbackItem
    {
        2814639011617876,
        XblReputationFeedbackType::PositiveHelpfulPlayer,
        nullptr,
        "Helpful player",
        nullptr
    });
// Add any additional feedback items here

auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    HRESULT hr = XAsyncGetStatus(asyncBlock, false);
};

HRESULT hr = XblSocialSubmitBatchReputationFeedbackAsync(
    xboxLiveContext,
    feedbackItems.data(),
    feedbackItems.size(),
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
