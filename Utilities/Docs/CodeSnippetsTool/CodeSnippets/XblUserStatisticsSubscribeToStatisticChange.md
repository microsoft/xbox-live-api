```cpp
XblRealTimeActivitySubscriptionHandle subscriptionHandle{ nullptr };

HRESULT hr = XblUserStatisticsSubscribeToStatisticChange(
    xboxLiveContext,
    xboxUserId,
    scid,
    statisticName.c_str(),
    &subscriptionHandle
);
```
