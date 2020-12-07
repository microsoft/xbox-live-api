```cpp
HRESULT hr = XblPresenceUnsubscribeFromDevicePresenceChange(
    xboxLiveContext,
    state.devicePresenceChangeSubscription
);

state.devicePresenceChangeSubscription = nullptr;
```
