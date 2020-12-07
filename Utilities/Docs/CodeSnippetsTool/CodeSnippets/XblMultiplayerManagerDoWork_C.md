```cpp
size_t eventCount{ 0 };
const XblMultiplayerEvent* events{ nullptr };
HRESULT hr = XblMultiplayerManagerDoWork(&events, &eventCount);
if (FAILED(hr))
{
    // Handle failure
}

for (auto i = 0u; i < eventCount; ++i)
{
    switch (events[i].EventType)
    {
        case XblMultiplayerEventType::MemberJoined:
        {
            // Handle MemberJoined
            size_t memberCount = 0;
            hr = XblMultiplayerEventArgsMembersCount(events[i].EventArgsHandle, &memberCount);
            assert(SUCCEEDED(hr));

            std::vector<XblMultiplayerManagerMember> eventMembers(memberCount);
            hr = XblMultiplayerEventArgsMembers(events[i].EventArgsHandle, memberCount, eventMembers.data());
            assert(SUCCEEDED(hr));
            ...

            auto sessionMembers{ GetSessionMembers(events[i].SessionType) };
            assert(memberCount <= sessionMembers.size());

            for (auto eventMember : eventMembers)
            {
                bool memberFound{ false };
                for (auto sessionMember : sessionMembers)
                {
                    if (eventMember.Xuid == sessionMember.Xuid)
                    {
                        memberFound = true;
                        break;
                    }
                }

                assert(memberFound);
            }

            for (auto& member : eventMembers)
            {
                LogToScreen("    member %llu", static_cast<unsigned long long>(member.Xuid));
            }
            break;
        }

        case XblMultiplayerEventType::SessionPropertyChanged:
        {
            // Handle SessionPropertyChanged
            const char* changedProperty{ nullptr };
            hr = XblMultiplayerEventArgsPropertiesJson(events[i].EventArgsHandle, &changedProperty);
            assert(SUCCEEDED(hr));
            ...

            rapidjson::Document changedDoc;
            changedDoc.Parse(changedProperty);

            rapidjson::Document doc;
            if (events[i].SessionType == XblMultiplayerSessionType::LobbySession)
            {
                doc.Parse(XblMultiplayerManagerLobbySessionPropertiesJson());
            }
            else
            {
                doc.Parse(XblMultiplayerManagerGameSessionPropertiesJson());
            }

            for (auto& member : changedDoc.GetObject())
            {
                assert(doc.HasMember(member.name));
                assert(doc[member.name] == member.value);
                UNREFERENCED_PARAMETER(member);
            }

            break;
        }
        ...
    }
}
```
