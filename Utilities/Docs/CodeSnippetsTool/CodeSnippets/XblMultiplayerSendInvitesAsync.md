```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t handlesCount = 1; // must be equal to invites requested
    XblMultiplayerInviteHandle handles[1] = {};
    HRESULT hr = XblMultiplayerSendInvitesResult(asyncBlock, handlesCount, handles);
};

uint64_t xuids[1] = {};
xuids[0] = xuid1;
size_t xuidsCount = 1;

HRESULT hr = XblMultiplayerSendInvitesAsync(
    xboxLiveContext,
    &MPState()->sessionRef,
    xuids,
    xuidsCount,
    titleId,
    contextStringId.c_str(),
    customActivationContext.c_str(),
    asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
