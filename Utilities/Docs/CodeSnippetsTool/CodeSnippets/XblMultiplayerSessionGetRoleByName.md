```cpp
const XblMultiplayerRole* role = nullptr;
HRESULT hr = XblMultiplayerSessionGetRoleByName(
    sessionHandle,
    roleTypeName.c_str(),
    roleName.c_str(),
    &role);
```
