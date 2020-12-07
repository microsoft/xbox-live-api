```cpp
XTaskQueueHandle queue = nullptr;
HRESULT hr = XTaskQueueCreate(
    XTaskQueueDispatchMode::Manual,
    XTaskQueueDispatchMode::Manual,
    &queue);
```
