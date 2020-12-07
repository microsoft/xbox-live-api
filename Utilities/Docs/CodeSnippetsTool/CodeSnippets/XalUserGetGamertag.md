```cpp
size_t gamerTagSize = XalUserGetGamertagSize(xalUser, XalGamertagComponent_Classic);
std::vector<char> gamerTag(gamerTagSize, '\0');

size_t bufferUsed;
HRESULT hr = XalUserGetGamertag(xalUser, XalGamertagComponent_Classic, gamerTagSize, gamerTag.data(), &bufferUsed);
```
