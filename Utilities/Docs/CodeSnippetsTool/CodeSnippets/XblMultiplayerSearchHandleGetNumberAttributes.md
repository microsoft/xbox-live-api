```cpp
const XblMultiplayerSessionNumberAttribute* attributes{ nullptr };
size_t attributesCount{ 0 };

HRESULT hr = XblMultiplayerSearchHandleGetNumberAttributes(
    MPState()->searchHandle,
    &attributes,
    &attributesCount
);
```
