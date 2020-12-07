```cpp
std::vector<uint64_t> xuids
{ 
    listXuids.begin() + static_cast<int>(offset),
    listXuids.begin() + static_cast<int>(offset + count)
};

HRESULT hr = XblSocialManagerUpdateSocialUserGroup(group, xuids.data(), xuids.size());
```
