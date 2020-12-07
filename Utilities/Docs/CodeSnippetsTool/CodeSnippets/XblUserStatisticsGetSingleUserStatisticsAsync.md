```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultSize;
    HRESULT hr = XblUserStatisticsGetSingleUserStatisticsResultSize(asyncBlock, &resultSize);

    if (SUCCEEDED(hr))
    {
        std::vector<char> buffer(resultSize, 0);
        XblUserStatisticsResult* results{};

        hr = XblUserStatisticsGetSingleUserStatisticsResult(asyncBlock, resultSize, buffer.data(), &results, nullptr);
        if (SUCCEEDED(hr))
        {
            // Now you can use the XblUserStatisticsResult array in results
        }
    }

};

const char* statisticNames[2] = {};
statisticNames[0] = statisticName1.c_str();
statisticNames[1] = statisticName2.c_str();

HRESULT hr = XblUserStatisticsGetSingleUserStatisticsAsync(
    xboxLiveContext,
    xboxUserId,
    scid,
    statisticNames, 2, 
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}

```
