```cpp
std::vector<XblTitleManagedStatistic> stats{ s_svd->Stats() };
auto stat1 = std::find_if(stats.begin(), stats.end(), [](const XblTitleManagedStatistic& s)
{
    return std::string{ "AddedStat" } == s.statisticName;
});

auto asyncBlock = std::make_unique<XAsyncBlock>();
asyncBlock->queue = queue;
asyncBlock->callback = [](XAsyncBlock* asyncBlock)
{
    std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
    HRESULT hr = XAsyncGetStatus(asyncBlock, false);
};

HRESULT hr = XblTitleManagedStatsUpdateStatsAsync(
    xboxLiveContext,
    &(*stat1),
    1,
    asyncBlock.get()
);

if (SUCCEEDED(hr))
{
    // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
    // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
    asyncBlock.release();
}
```
