```cpp
uint32_t maxMemberCount = 0;
uint32_t targetMemberCount = 0;

HRESULT hr = XblMultiplayerSessionSetMutableRoleSettings(
    sessionHandle,
    roleTypeName.c_str(),
    roleName.c_str(),
    &maxMemberCount,
    &targetMemberCount);
```
