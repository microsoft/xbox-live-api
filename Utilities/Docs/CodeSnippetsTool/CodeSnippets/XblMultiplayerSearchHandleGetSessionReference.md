```cpp
XblMultiplayerSessionReference sessionRef{};

HRESULT hr = XblMultiplayerSearchHandleGetSessionReference(
    MPState()->searchHandle,
    &sessionRef
);
```
