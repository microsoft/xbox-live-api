```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->context = nullptr;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultSize;
    HRESULT hr = XblStringVerifyStringResultSize(asyncBlock, &resultSize);
    if (SUCCEEDED(hr))
    {
        std::vector<char> buffer(resultSize, 0);
        XblVerifyStringResult* result{};

        hr = XblStringVerifyStringResult(asyncBlock, resultSize, buffer.data(), &result, nullptr);
        LogToFile(
            "Result: Result Code: %d - First Offending String: %s",
            result->resultCode,
    }

};

HRESULT hr = XblStringVerifyStringAsync(
    xboxLiveContext,
    testString,
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}

```
