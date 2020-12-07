```cpp
const char* encounters[2] = {};
encounters[0] = encounter1.c_str();
encounters[1] = encounter2.c_str();
size_t encountersCount = 2;

HRESULT hr = XblMultiplayerSessionCurrentUserSetEncounters(
    sessionHandle,
    encounters,
    encountersCount);
```
