```cpp
const XblDeviceToken* deviceTokens = nullptr;
size_t deviceTokensCount = 0;
HRESULT hr = XblMultiplayerSessionHostCandidates(sessionHandle, &deviceTokens, &deviceTokensCount);
```
