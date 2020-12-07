```cpp
XblMultiplayerSessionMemberRole roles[2] = {};
roles[0].roleTypeName = roleTypeName1.c_str();
roles[0].roleName = roleName1.c_str();
roles[1].roleTypeName = roleTypeName2.c_str();
roles[1].roleName = roleName2.c_str();
size_t rolesCount = 2;
HRESULT hr = XblMultiplayerSessionCurrentUserSetRoles(
    sessionHandle,
    roles,
    rolesCount);
```
