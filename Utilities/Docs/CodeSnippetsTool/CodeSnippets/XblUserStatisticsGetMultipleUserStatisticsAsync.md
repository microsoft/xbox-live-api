```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultSize;
    HRESULT hr = XblUserStatisticsGetMultipleUserStatisticsResultSize(asyncBlock, &resultSize);

    if (SUCCEEDED(hr))
    {
        std::vector<char> buffer(resultSize, 0);
        XblUserStatisticsResult* results{};
        size_t resultsCount = 0;

        hr = XblUserStatisticsGetMultipleUserStatisticsResult(asyncBlock, resultSize, buffer.data(), &results, &resultsCount, nullptr);

        // Process results array to read the user stats data
        for (size_t iResult = 0; iResult < resultsCount; iResult++)
        {
            LogToFile("%d", results[iResult].xboxUserId);
            for (size_t iScid = 0; iScid < results[iResult].serviceConfigStatisticsCount; iScid++)
            {
                LogToFile("SCID: %s", results[iResult].serviceConfigStatistics[iScid].serviceConfigurationId);
                for (size_t iStat = 0; iStat < results[iResult].serviceConfigStatistics[iScid].statisticsCount; iStat++)
                {
                    LogToFile("Stat %d: name:%s value:%s type:%s", iResult,
                        results[iResult].serviceConfigStatistics[iScid].statistics[iStat].statisticName,
                        results[iResult].serviceConfigStatistics[iScid].statistics[iStat].value,
                        results[iResult].serviceConfigStatistics[iScid].statistics[iStat].statisticType );
                }
            }
        }
    }

};

const char* statisticNames[2] = {};
statisticNames[0] = statisticName1.c_str();
statisticNames[1] = statisticName2.c_str();

uint64_t xuids[2] = {};
xuids[0] = xuid1;
xuids[1] = xuid2;

HRESULT hr = XblUserStatisticsGetMultipleUserStatisticsAsync(
    xboxLiveContext,
    xuids, 2,
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
