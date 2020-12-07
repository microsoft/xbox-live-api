```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    HRESULT hr = XblPresenceGetPresenceResult(asyncBlock, &state.presenceRecord);
    
    // Be sure to call XblPresenceRecordCloseHandle when the presence record is no longer needed.
};

uint64_t xuid = xboxUserId;
if (m_multiDeviceManager->GetRemoteXuid() != 0)
{
    xuid = m_multiDeviceManager->GetRemoteXuid();
}

HRESULT hr = XblPresenceGetPresenceAsync(xboxLiveContext, xuid, asyncBlock.get());
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}

```
