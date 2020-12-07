```cpp
#if HC_PLATFORM == HC_PLATFORM_GDK
HRESULT hr = XalUserRegisterChangeEventHandler(queue, context, OnXalUserChangeEventHandler_GDK, &token);
#else
HRESULT hr = XalUserRegisterChangeEventHandler(queue, context, OnXalUserChangeEventHandler, &token);
#endif
```
