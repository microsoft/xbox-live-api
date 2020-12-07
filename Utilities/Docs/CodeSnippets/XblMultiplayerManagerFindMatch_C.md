```cpp
uint32_t timeoutInSeconds = 30;
HRESULT hr = XblMultiplayerManagerFindMatch(hopperName, attributesJson, timeoutInSeconds);
if (!SUCCEEDED(hr))
{
    // Handle failure
}
```
