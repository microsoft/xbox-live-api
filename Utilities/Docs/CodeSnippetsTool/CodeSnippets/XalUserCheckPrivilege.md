```cpp
XalPrivilege privilege = XalPrivilege_Multiplayer;
bool hasPrivilege = false;
XalPrivilegeCheckDenyReasons reasons = { };
HRESULT hr = XalUserCheckPrivilege(xalUser, privilege, &hasPrivilege, &reasons);
```
