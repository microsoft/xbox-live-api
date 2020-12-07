```cpp
size_t currentMembers{ 0 };
size_t maxMembers{ 0 };

HRESULT hr = XblMultiplayerSearchHandleGetMemberCounts(
    MPState()->searchHandle,
    &maxMembers,
    &currentMembers
);
```
