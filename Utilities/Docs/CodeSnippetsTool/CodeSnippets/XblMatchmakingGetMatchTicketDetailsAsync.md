```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t bufferSize;
    auto hr = XblMatchmakingGetMatchTicketDetailsResultSize(asyncBlock, &bufferSize);
    if (SUCCEEDED(hr))
    {
        std::vector<char> buffer(bufferSize, 0);
        XblMatchTicketDetailsResponse* resultPtr;
        hr = XblMatchmakingGetMatchTicketDetailsResult(asyncBlock, bufferSize, buffer.data(), &resultPtr, nullptr);
            
            if (resultPtr->ticketAttributes != nullptr)
            {
            }
    }

};

HRESULT hr = XblMatchmakingGetMatchTicketDetailsAsync(
    xboxLiveContext,
    scid.c_str(),
    hopperName.c_str(),
    ticketId.c_str(),
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
