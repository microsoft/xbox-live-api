```cpp
// Remove the statistic changed handler
XblUserStatisticsRemoveStatisticChangedHandler(
    xblContextHandle,
    statisticChangedFunctionContext
);

// Unsubscribe for statistic change events
HRESULT hr = XblUserStatisticsUnsubscribeFromStatisticChange(
    xboxLiveContext,
    statisticChangeSubscriptionHandle
);
```
