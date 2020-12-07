```cpp
const XblMultiplayerSessionMember* members = nullptr;
size_t membersCount = 0;
HRESULT hr = XblMultiplayerSessionMembers(
    sessionHandle,
    &members,
    &membersCount
);
```
