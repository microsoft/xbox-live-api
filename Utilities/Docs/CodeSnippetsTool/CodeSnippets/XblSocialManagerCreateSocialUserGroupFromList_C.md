```cpp
std::vector<uint64_t> xuids
{
    listXuids.begin() + static_cast<int>(offset),
    listXuids.begin() + static_cast<int>(offset + count) 
};

XblSocialManagerUserGroupHandle groupHandle{ nullptr };
HRESULT hr = XblSocialManagerCreateSocialUserGroupFromList(user, xuids.data(), xuids.size(), &groupHandle);

if (SUCCEEDED(hr))
{
    state.groups.insert(groupHandle);
}
```
