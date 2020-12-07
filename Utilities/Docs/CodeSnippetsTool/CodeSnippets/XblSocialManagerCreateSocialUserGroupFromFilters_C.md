```cpp
XblPresenceFilter presenceFilter{ XblPresenceFilter::All };
XblRelationshipFilter relationshipFilter{ XblRelationshipFilter::Friends };

XblSocialManagerUserGroupHandle groupHandle{ nullptr };
HRESULT hr = XblSocialManagerCreateSocialUserGroupFromFilters(user, presenceFilter, relationshipFilter, &groupHandle);

if (SUCCEEDED(hr))
{
    state.groups.insert(groupHandle);
}
```
