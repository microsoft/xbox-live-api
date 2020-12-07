```cpp
const uint64_t* xuids{ nullptr };
size_t count{ 0 };

HRESULT hr = XblSocialManagerUserGroupGetUsersTrackedByGroup(
    group,
    &xuids,
    &count
);
```
