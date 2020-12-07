```cpp
const char* handleId{ nullptr };

HRESULT hr = XblMultiplayerSearchHandleGetId(
    MPState()->searchHandle,
    &handleId
);
```
