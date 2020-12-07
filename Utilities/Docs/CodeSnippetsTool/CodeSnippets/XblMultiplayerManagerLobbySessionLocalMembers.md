```cpp
size_t localMembersCount = XblMultiplayerManagerLobbySessionLocalMembersCount();
std::vector<XblMultiplayerManagerMember> localMembers(localMembersCount, XblMultiplayerManagerMember{});
HRESULT hr = XblMultiplayerManagerLobbySessionLocalMembers(localMembersCount, localMembers.data());
```
