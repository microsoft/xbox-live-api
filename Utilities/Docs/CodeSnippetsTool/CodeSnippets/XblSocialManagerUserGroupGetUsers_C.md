```cpp
XblSocialManagerUserPtrArray users{ nullptr };
size_t userCount{ 0 };
HRESULT hr = XblSocialManagerUserGroupGetUsers(group, &users, &userCount);

for (size_t i = 0; i < userCount; ++i)
{
    // Display user info etc.
}
```
