```cpp
state.devicePresenceChangedHandlerToken = XblPresenceAddDevicePresenceChangedHandler(
    xboxLiveContext,
    [](void* context, uint64_t xuid, XblPresenceDeviceType deviceType, bool isUserLoggedOnDevice)
    {
        UNREFERENCED_PARAMETER(context);
        LogToFile("Device presence change notification received:");
        LogToFile("Xuid = %u, deviceType = %u, isUserLoggedOnDevice = %u", xuid, deviceType, isUserLoggedOnDevice);
    },
    nullptr
);
```
