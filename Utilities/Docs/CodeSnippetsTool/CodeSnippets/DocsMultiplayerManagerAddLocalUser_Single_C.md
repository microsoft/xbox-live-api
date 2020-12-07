```cpp
HRESULT hr = XblMultiplayerManagerLobbySessionAddLocalUser(xblUserHandle);

if (!SUCCEEDED(hr))
{
    // Handle failure
}

// Set member connection address
const char* connectionAddress = "1.1.1.1";
hr = XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
    xblUserHandle, connectionAddress, context);

if (!SUCCEEDED(hr))
{
    // Handle failure
}

// Set custom member properties
const char* propName = "Name";
const char* propValueJson = "{}";
hr = XblMultiplayerManagerLobbySessionSetProperties(propName, propValueJson, context);

if (!SUCCEEDED(hr))
{
    // Handle failure
}
...
```
