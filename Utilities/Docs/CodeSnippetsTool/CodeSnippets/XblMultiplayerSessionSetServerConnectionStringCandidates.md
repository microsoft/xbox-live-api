```cpp
const char* serverConnectionStringCandidates[2] = {};
serverConnectionStringCandidates[0] = candidate1.c_str();
serverConnectionStringCandidates[1] = candidate2.c_str();
size_t serverConnectionStringCandidatesCount = 2;

HRESULT hr = XblMultiplayerSessionSetServerConnectionStringCandidates(
    sessionHandle,
    serverConnectionStringCandidates,
    serverConnectionStringCandidatesCount);
```
