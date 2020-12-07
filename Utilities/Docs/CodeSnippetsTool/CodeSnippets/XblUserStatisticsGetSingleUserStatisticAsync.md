```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultSize;
    HRESULT hr = XblUserStatisticsGetSingleUserStatisticResultSize(asyncBlock, &resultSize);

    if (SUCCEEDED(hr))
    {
        std::vector<char> buffer(resultSize, 0);
        XblUserStatisticsResult* result{};

        hr = XblUserStatisticsGetSingleUserStatisticResult(asyncBlock, resultSize, buffer.data(), &result, nullptr);

        if (SUCCEEDED(hr) && result->serviceConfigStatisticsCount > 0 && result->serviceConfigStatistics->statisticsCount > 0)
        {
            int64_t userStatValue = atoll(result->serviceConfigStatistics[0].statistics[0].value);
            // Now you can show or store userStatValue
        }
    }

};

HRESULT hr = XblUserStatisticsGetSingleUserStatisticAsync(
    xboxLiveContext,
    xboxUserId,
    scid,
    statisticName.c_str(),
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}

```
