```cpp
size_t memberCount = XblMultiplayerManagerGameSessionMembersCount();
if (memberCount > 0)
{
    std::vector<XblMultiplayerManagerMember> gameSessionMembers(memberCount, XblMultiplayerManagerMember{});
    hr = XblMultiplayerManagerGameSessionMembers(memberCount, gameSessionMembers.data());
    if (SUCCEEDED(hr))
    {
        for (const auto& member : gameSessionMembers)
        {
            LogMultiplayerSessionMember(member);
        }
    }
}
```
