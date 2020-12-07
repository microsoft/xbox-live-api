```cpp
XblPresenceFilter presenceFilter{ XblPresenceFilter::Unknown };
XblRelationshipFilter relationshipFilter{ XblRelationshipFilter::Unknown };
HRESULT hr = XblSocialManagerUserGroupGetFilters(group, &presenceFilter, &relationshipFilter);
```
