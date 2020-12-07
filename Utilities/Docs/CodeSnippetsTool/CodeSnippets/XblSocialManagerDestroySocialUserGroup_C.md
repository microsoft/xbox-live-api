```cpp
HRESULT hr = XblSocialManagerDestroySocialUserGroup(groupHandle);
if (SUCCEEDED(hr))
{
    state.groups.erase(groupHandle);
}
```
