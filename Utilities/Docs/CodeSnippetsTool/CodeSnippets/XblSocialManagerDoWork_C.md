```cpp
const XblSocialManagerEvent* events{ nullptr };
size_t eventCount{ 0 };
HRESULT hr = XblSocialManagerDoWork(&events, &eventCount);
if (SUCCEEDED(hr))
{
    for (size_t i = 0; i < eventCount; i++)
    {
        // Act on the event
        auto& socialEvent = events[i];
        std::stringstream ss;
        ss << "XblSocialManagerDoWork: Event of type " << eventTypesMap[socialEvent.eventType] << std::endl;
        for (size_t j = 0; j < XBL_SOCIAL_MANAGER_MAX_AFFECTED_USERS_PER_EVENT; j++)
        {
            if (socialEvent.usersAffected[j] != nullptr)
            {
                if (j == 0)
                {
                    ss << "Users affected: " << std::endl;
                }
                ss << "\t" << socialEvent.usersAffected[j]->gamertag << std::endl;
            }
        }
        LogToFile(ss.str().c_str());
    }
}
```
