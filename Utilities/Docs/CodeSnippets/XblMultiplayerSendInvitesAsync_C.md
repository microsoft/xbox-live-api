```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock };
    size_t handlesCount = 1; // must be equal to invites requested
    XblMultiplayerInviteHandle handles[1] = {};
    HRESULT hr = XblMultiplayerSendInvitesResult(asyncBlock, handlesCount, handles);
};

uint64_t xuids[1] = {};
xuids[0] = targetXuid;
size_t xuidsCount = 1;

HRESULT hr = XblMultiplayerSendInvitesAsync(
    xblContextHandle,
    &sessionReference,
    xuids,
    xuidsCount,
    titleId,
    contextStringId,
    customActivationContext,
    asyncBlock.get());
if (SUCCEEDED(hr))
{
    asyncBlock.release();
}
```
