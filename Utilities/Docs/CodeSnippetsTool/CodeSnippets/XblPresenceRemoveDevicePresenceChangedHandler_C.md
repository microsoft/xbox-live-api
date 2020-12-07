```cpp
HRESULT hr = XblPresenceRemoveDevicePresenceChangedHandler(
    xboxLiveContext,
    state.devicePresenceChangedHandlerToken
);

state.devicePresenceChangedHandlerToken = 0;
```
