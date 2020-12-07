```cpp
state.titlePresenceChangedHandlerToken =  XblPresenceAddTitlePresenceChangedHandler(
    xboxLiveContext,
    [](void* context, uint64_t xuid, uint32_t titleId, XblPresenceTitleState titleState)
    {
        UNREFERENCED_PARAMETER(context);
        LogToFile("Title presence change notification received:");
        LogToFile("Xuid = %u, titleId = %u, titleState = %u", xuid, titleId, titleState);
    },
    nullptr
);
```
