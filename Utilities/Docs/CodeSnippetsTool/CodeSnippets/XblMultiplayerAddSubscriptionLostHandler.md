```cpp
void* context = nullptr;
XblFunctionContext fnContext = XblMultiplayerAddSubscriptionLostHandler(
    xboxLiveContext,
    [](_In_opt_ void*)
{
    LogToFile("XblMultiplayerAddSubscriptionLostHandler");
},
    context);
```
