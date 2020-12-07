```cpp
uint32_t memberIds[1] = {};
memberIds[0] = memberId1;
size_t memberIdsCount = 1;
HRESULT hr = XblMultiplayerSessionCurrentUserSetMembersInGroup(
    sessionHandle,
    memberIds,
    memberIdsCount);
```
