```cpp
// Subscribe for statistic change events
std::string statisticName = "totalPuzzlesSolved";
XblRealTimeActivitySubscriptionHandle subscriptionHandle{ nullptr };
HRESULT hr = XblUserStatisticsSubscribeToStatisticChange(
    xblContextHandle,
    xboxUserId,
    scid,
    statisticName.c_str(),
    &subscriptionHandle
);

// Add a statistic changed handler
void* context{ nullptr };
XblFunctionContext statisticChangedFunctionContext = XblUserStatisticsAddStatisticChangedHandler(
    xboxLiveContext,
    [](XblStatisticChangeEventArgs eventArgs, void* context)
    {
        // Handle stat change 
        LogToScreen("Statistic changed callback: stat changed (%s = %s)",
            eventArgs.latestStatistic.statisticName,
            eventArgs.latestStatistic.value);
    },
    context
    );
```
