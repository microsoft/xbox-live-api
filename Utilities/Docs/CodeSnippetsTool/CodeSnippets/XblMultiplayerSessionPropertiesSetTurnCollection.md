```cpp
uint32_t turnCollectionMemberIds[2] = {};
turnCollectionMemberIds[0] = turnId1;
turnCollectionMemberIds[1] = turnId2;
size_t turnCollectionMemberIdsCount = 2;
HRESULT hr = XblMultiplayerSessionPropertiesSetTurnCollection(
    sessionHandle,
    turnCollectionMemberIds,
    turnCollectionMemberIdsCount);
```
