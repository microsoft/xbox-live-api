```cpp
XblPresenceRichPresenceIds ids{};
pal::strcpy(ids.scid, sizeof(ids.scid), scid);
ids.presenceId = "playingMap";
std::vector<const char*> tokenIds{ "CurrentMap" };
ids.presenceTokenIds = tokenIds.data();
ids.presenceTokenIdsCount = tokenIds.size();

HRESULT hr = XblPresenceSetPresenceAsync(xboxLiveContext, true, &ids, asyncBlock.get());
```
