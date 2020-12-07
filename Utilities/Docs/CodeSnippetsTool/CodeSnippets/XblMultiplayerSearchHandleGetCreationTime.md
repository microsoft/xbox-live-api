```cpp
time_t creationTime{ 0 };

HRESULT hr = XblMultiplayerSearchHandleGetCreationTime(
    MPState()->searchHandle,
    &creationTime
);
```
