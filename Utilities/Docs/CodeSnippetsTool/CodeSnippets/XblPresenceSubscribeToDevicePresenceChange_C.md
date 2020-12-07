```cpp
uint64_t xuid{ 2814639011617876 };

HRESULT hr = XblPresenceSubscribeToDevicePresenceChange(
    xboxLiveContext,
    xuid,
    &state.devicePresenceChangeSubscription
);
```
