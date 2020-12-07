```cpp
const XblSocialRelationship* relationships = nullptr;
size_t relationshipsCount = 0;
HRESULT hr = XblSocialRelationshipResultGetRelationships(state.socialResultHandle, &relationships, &relationshipsCount);

LogToFile("Got %u SocialRelationships:", relationshipsCount);
for (size_t i = 0; i < relationshipsCount; ++i)
{
    LogToFile("Xuid = %u, isFollowingCaller = %u", relationships[i].xboxUserId, relationships[i].isFollowingCaller);
}
```
