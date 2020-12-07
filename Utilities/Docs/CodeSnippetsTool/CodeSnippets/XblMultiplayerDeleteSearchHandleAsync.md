```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    HRESULT hr = XAsyncGetStatus(asyncBlock, true);
    LogToFile("XblMultiplayerDeleteSearchHandleAsync completed with result = %s", ConvertHR(hr).data());
};

const char* handleId{ nullptr };
XblMultiplayerSearchHandleGetId(MPState()->searchHandle, &handleId);

HRESULT hr = XblMultiplayerDeleteSearchHandleAsync(
    xboxLiveContext,
    handleId,
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
