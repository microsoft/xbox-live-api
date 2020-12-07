```cpp
HRESULT hr = XblMultiplayerManagerJoinLobby(inviteHandleId, xblUserHandle);
if (!SUCCEEDED(hr))
{
    // Handle failure
}

// Set member connection address
const char* connectionAddress = "1.1.1.1";
hr = XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
    xblUserHandle, connectionAddress, context);
```
