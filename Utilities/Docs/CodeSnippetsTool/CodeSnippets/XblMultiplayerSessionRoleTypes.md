```cpp
const XblMultiplayerRoleType* roleTypes = nullptr;
size_t roleTypesCount = 0;

HRESULT hr = XblMultiplayerSessionRoleTypes(
    sessionHandle,
    &roleTypes,
    &roleTypesCount);
```
