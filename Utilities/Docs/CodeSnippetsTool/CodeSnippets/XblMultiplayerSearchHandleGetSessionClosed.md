```cpp
bool closed{ false };

HRESULT hr = XblMultiplayerSearchHandleGetSessionClosed(
    MPState()->searchHandle,
    &closed
);
```
