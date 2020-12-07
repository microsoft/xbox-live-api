```cpp
const uint64_t* xuids{ nullptr };
size_t xuidsCount{ 0 };

HRESULT hr = XblMultiplayerSearchHandleGetSessionOwnerXuids(
    MPState()->searchHandle,
    &xuids,
    &xuidsCount
);
```
