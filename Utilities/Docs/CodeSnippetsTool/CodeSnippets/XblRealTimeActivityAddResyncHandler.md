```cpp
s_resyncHandlerContext = XblRealTimeActivityAddResyncHandler(xboxLiveContext,
    [](void* context)
{
    // Handle resync
    LogToFile("XblResyncHandler called");
}, nullptr);
```
