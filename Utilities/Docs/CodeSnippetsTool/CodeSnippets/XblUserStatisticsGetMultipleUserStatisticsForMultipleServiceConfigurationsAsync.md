```cpp
auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    size_t resultSize;
    HRESULT hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResultSize(asyncBlock, &resultSize);

    if (SUCCEEDED(hr))
    {
        std::vector<char> buffer(resultSize, 0);
        XblUserStatisticsResult* results{};
        size_t resultsCount = 0;

        hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsResult(asyncBlock, resultSize, buffer.data(), &results, &resultsCount, nullptr);

        if (SUCCEEDED(hr))
        {
            for (size_t i = 0; i < resultsCount; i++)
            {
                // Log results
                std::stringstream stream;
                stream << "XUID: " << results[i].xboxUserId << std::endl;;

                for (size_t j = 0; j < results[i].serviceConfigStatisticsCount; j++)
                {
                    stream << " " << results[i].serviceConfigStatistics[j].serviceConfigurationId << ": " << std::endl;
                    for (size_t k = 0; k < results[i].serviceConfigStatistics[j].statisticsCount; k++)
                    {
                        stream << " " << results[i].serviceConfigStatistics[j].statistics[k].statisticName << "=" << results[i].serviceConfigStatistics[j].statistics[k].value << std::endl;
                    }
                }
            }
        }
    }

};

const char* requestedStatsNames1[2] = {};
requestedStatsNames1[0] = statisticName1.c_str();
requestedStatsNames1[1] = statisticName2.c_str();

XblRequestedStatistics requestedStats[1] = {};
pal::strcpy(requestedStats[0].serviceConfigurationId, sizeof(XblRequestedStatistics::serviceConfigurationId), scid);
requestedStats[0].statistics = requestedStatsNames1;
requestedStats[0].statisticsCount = 2;

uint64_t xuids[2] = {};
xuids[0] = xuid1;
xuids[1] = xuid2;
    
HRESULT hr = XblUserStatisticsGetMultipleUserStatisticsForMultipleServiceConfigurationsAsync(
    xboxLiveContext,
    xuids, 2,
    requestedStats, 1,
    asyncBlock.get()
);
if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}

```
