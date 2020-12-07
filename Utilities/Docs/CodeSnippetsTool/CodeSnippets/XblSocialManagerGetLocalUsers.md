```cpp
size_t localUsersCount = XblSocialManagerGetLocalUserCount();
std::vector<XblUserHandle> localUsers(localUsersCount, nullptr);
HRESULT hr = XblSocialManagerGetLocalUsers(localUsersCount, &localUsers[0]);
```
