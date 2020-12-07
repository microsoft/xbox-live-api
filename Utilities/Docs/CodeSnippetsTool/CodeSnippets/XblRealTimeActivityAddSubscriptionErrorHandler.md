```cpp
s_subscriptionErrorHandlerContext = XblRealTimeActivityAddSubscriptionErrorHandler(xboxLiveContext,
    [](void* context, _In_ XblRealTimeActivitySubscriptionHandle subscription, HRESULT subscriptionError)
{
    // Handle subscription error
    LogToFile("Rta subscription error %s", ConvertHR(subscriptionError).c_str());
}, nullptr);
```
