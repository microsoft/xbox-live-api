```cpp
state.socialRelationshipChangedHandlerToken = XblSocialAddSocialRelationshipChangedHandler(
    xboxLiveContext,
    [](const XblSocialRelationshipChangeEventArgs* args, void* context)
    {
        UNREFERENCED_PARAMETER(context);
        LogToFile("Social relationship changed:");
        std::stringstream ss;
        for (size_t i = 0; i < args->xboxUserIdsCount; ++i)
        {
            if (i > 0) 
            {
                ss << ", ";
            }
            ss << args->xboxUserIds[i];
        }
        LogToFile("socialNotification = %u, affectedXuids = %s", args->socialNotification, ss.str().data());
    },
    nullptr
);
```
