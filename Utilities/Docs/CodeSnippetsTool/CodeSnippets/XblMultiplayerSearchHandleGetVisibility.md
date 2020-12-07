```cpp
XblMultiplayerSessionVisibility visibility{ XblMultiplayerSessionVisibility::Unknown };

HRESULT hr = XblMultiplayerSearchHandleGetVisibility(
    MPState()->searchHandle,
    &visibility
);
```
