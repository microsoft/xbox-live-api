```cpp
const XblMultiplayerSessionStringAttribute* attributes{ nullptr };
size_t attributesCount{ 0 };

HRESULT hr = XblMultiplayerSearchHandleGetStringAttributes(
    MPState()->searchHandle,
    &attributes,
    &attributesCount
);
```
