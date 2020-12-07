```cpp
void* context = nullptr;
XblFunctionContext fnContext = XblMultiplayerAddSessionChangedHandler(
    xboxLiveContext,
    [](_In_opt_ void*, _In_ XblMultiplayerSessionChangeEventArgs args)
{
    LogToFile("XblMultiplayerAddSessionChangedHandler");
    LogToFile("ChangeNumber: %d", args.ChangeNumber);
},
    context);
```
