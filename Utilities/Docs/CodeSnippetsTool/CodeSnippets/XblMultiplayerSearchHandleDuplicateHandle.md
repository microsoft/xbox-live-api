```cpp
XblMultiplayerSearchHandle duplicate{ nullptr };

HRESULT hr = XblMultiplayerSearchHandleDuplicateHandle(
    MPState()->searchHandle,
    &duplicate
);

if (SUCCEEDED(hr))
{
    XblMultiplayerSearchHandleCloseHandle(duplicate);
}
```
