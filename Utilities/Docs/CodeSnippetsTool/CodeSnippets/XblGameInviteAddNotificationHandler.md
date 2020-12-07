```cpp
void* context = nullptr;
auto t = xboxLiveContext;
XblFunctionContext gameinviteFunctionContext = XblGameInviteAddNotificationHandler(
    t,
    [](_In_ const XblGameInviteNotificationEventArgs* args, _In_opt_ void*)
    {
        LogToScreen("XblGameInviteAddNotificationHandler");
        LogToScreen("Invite Handle ID:");
        LogToScreen(args->inviteHandleId);
        LogToScreen("Invite Protocol:");
        LogToScreen(args->inviteProtocol);
        LogToScreen("Sender Gamertag:");
        LogToScreen(args->senderGamertag);
        LogToScreen("Modern Gamertag:");
        LogToScreen(args->senderModernGamertag);
        LogToScreen("Modern Gamertag Suffix:");
        LogToScreen(args->senderModernGamertagSuffix);
        LogToScreen("Unique Modern Gamertag:");
        LogToScreen(args->senderUniqueModernGamertag);
        LogToScreen("Sender Gamertag:");
        LogToScreen(args->senderGamertag);
        LogToScreen("Sender Image URL:");
        LogToScreen(args->senderImageUrl);
        std::string s2 = std::to_string(args->senderXboxUserId);
        LogToScreen("Sender Xbox User ID:");
        LogToScreen(s2.c_str());
    },
    context
);
```
