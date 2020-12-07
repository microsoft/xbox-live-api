```cpp
XblMultiplayerSessionRestriction joinRestriction{ XblMultiplayerSessionRestriction::Unknown };

HRESULT hr = XblMultiplayerSearchHandleGetJoinRestriction(
    MPState()->searchHandle,
    &joinRestriction
);
```
