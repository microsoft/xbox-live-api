```cpp
const char* groups[2] = {};
groups[0] = group1.c_str();
groups[1] = group2.c_str();
size_t groupsCount = 2;

HRESULT hr = XblMultiplayerSessionCurrentUserSetGroups(
    sessionHandle,
    groups,
    groupsCount);
```
