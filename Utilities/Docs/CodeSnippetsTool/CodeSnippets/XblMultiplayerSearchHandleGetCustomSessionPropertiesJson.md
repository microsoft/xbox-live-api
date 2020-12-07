```cpp
const char* properties{ nullptr };

HRESULT hr = XblMultiplayerSearchHandleGetCustomSessionPropertiesJson(
    MPState()->searchHandle,
    &properties
);
```
