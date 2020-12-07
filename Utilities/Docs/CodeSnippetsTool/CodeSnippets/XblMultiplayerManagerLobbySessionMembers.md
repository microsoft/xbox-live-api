```cpp
size_t count = XblMultiplayerManagerLobbySessionMembersCount();
std::vector<XblMultiplayerManagerMember> members(count);
HRESULT hr = XblMultiplayerManagerLobbySessionMembers(count, members.data());
```
