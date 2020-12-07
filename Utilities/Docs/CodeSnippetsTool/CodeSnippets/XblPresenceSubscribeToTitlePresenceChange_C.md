```cpp
uint64_t xuid{ 2814639011617876 };

HRESULT hr = XblPresenceSubscribeToTitlePresenceChange(
    xboxLiveContext,
    xuid,
    titleId,
    &state.titlePresenceChangeSubscription
);
```
