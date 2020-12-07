```cpp
const XblMultiplayerSessionTag* tags{ nullptr };
size_t tagsCount{ 0 };

HRESULT hr = XblMultiplayerSearchHandleGetTags(
    MPState()->searchHandle,
    &tags,
    &tagsCount
);
```
