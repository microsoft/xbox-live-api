// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include <atomic>

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( push )
#pragma warning( disable : 4365 )
#pragma warning( disable : 4061 )
#pragma warning( disable : 4996 )
#endif

#include "rapidjson/document.h"

static std::thread s_doWorkThread{};
static std::atomic<bool> s_doWork{ false };

void CheckMemberFound(XblMultiplayerSessionType sessionType, uint64_t xuid)
{
    size_t count{};
    std::vector<XblMultiplayerManagerMember> members{};
    if (sessionType == XblMultiplayerSessionType::LobbySession)
    {
        count = XblMultiplayerManagerLobbySessionMembersCount();
        assert(count > 0);
        members = std::vector<XblMultiplayerManagerMember>(count);
        XblMultiplayerManagerLobbySessionMembers(count, members.data());
    }
    else
    {
        count = XblMultiplayerManagerGameSessionMembersCount();
        assert(count > 0);
        members = std::vector<XblMultiplayerManagerMember>(count);
        XblMultiplayerManagerGameSessionMembers(count, members.data());
    }

    bool memberFound{ false };
    for (auto member : members)
    {
        if (member.Xuid == xuid)
        {
            memberFound = true;
            break;
        }
    }

    assert(memberFound);
}

std::vector<XblMultiplayerManagerMember> GetSessionMembers(XblMultiplayerSessionType sessionType)
{
    size_t count{};
    std::vector<XblMultiplayerManagerMember> members{};
    if (sessionType == XblMultiplayerSessionType::LobbySession)
    {
        count = XblMultiplayerManagerLobbySessionMembersCount();
        members = std::vector<XblMultiplayerManagerMember>(count);
        if (count > 0)
        {
            XblMultiplayerManagerLobbySessionMembers(count, members.data());
        }
    }
    else
    {
        count = XblMultiplayerManagerGameSessionMembersCount();
        members = std::vector<XblMultiplayerManagerMember>(count);
        if (count > 0)
        {
            XblMultiplayerManagerGameSessionMembers(count, members.data());
        }
    }

    return members;
}

HRESULT MultiplayerManagerDoWork()
{
    // CODE SNIPPET START: XblMultiplayerManagerDoWork_C
    size_t eventCount{ 0 };
    const XblMultiplayerEvent* events{ nullptr };
    HRESULT hr = XblMultiplayerManagerDoWork(&events, &eventCount);
    if (FAILED(hr))
    {
        // Handle failure
        return hr; // CODE SNIP SKIP
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
                // DOTS

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

                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::MemberJoined"); // CODE SNIP SKIP
                for (auto& member : eventMembers)
                {
                    LogToScreen("    member %llu", static_cast<unsigned long long>(member.Xuid));
                }
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_MemberJoined"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::SessionPropertyChanged:
            {
                // Handle SessionPropertyChanged
                const char* changedProperty{ nullptr };
                hr = XblMultiplayerEventArgsPropertiesJson(events[i].EventArgsHandle, &changedProperty);
                assert(SUCCEEDED(hr));
                // DOTS

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

                // CODE SKIP START
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::SessionPropertyChanged"); // CODE SNIP SKIP
                if (events[i].SessionType == XblMultiplayerSessionType::GameSession)
                {
                    CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_GameSessionPropertyChanged"); // CODE SNIP SKIP
                }
                else
                {
                    CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_LobbySessionPropertyChanged"); // CODE SNIP SKIP
                }
                // CODE SKIP END
                break;
            }
            // DOTS
            // CODE SKIP START

            case XblMultiplayerEventType::UserAdded:
            {
                auto h = events[i].Result;
                if (SUCCEEDED(h))
                {
                    // Handle UserAdded
                    uint64_t xuid = 0;
                    hr = XblMultiplayerEventArgsXuid(events[i].EventArgsHandle, &xuid);
                    assert(SUCCEEDED(hr));

                    CheckMemberFound(events[i].SessionType, xuid);
                    LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::UserAdded. XUID: %llu", static_cast<unsigned long long>(xuid)); // CODE SNIP SKIP
                }
           
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_UserAdded"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::UserRemoved:
            {
                // Handle UserRemoved
                hr = events[i].Result == __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND) ? S_OK : events[i].Result;
                if (SUCCEEDED(hr))
                {
                    uint64_t xuid = 0;
                    hr = XblMultiplayerEventArgsXuid(events[i].EventArgsHandle, &xuid);
                    assert(SUCCEEDED(hr));

                    auto members{ GetSessionMembers(events[i].SessionType) };

                    for (auto member : members)
                    {
                        assert(xuid != member.Xuid);
                    }
                }
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::UserRemoved"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(hr, "OnXblMultiplayerEventType_UserRemoved"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::JoinLobbyCompleted:
            {
                // Handle JoinLobbyCompleted
                uint64_t xuid = 0;
                hr = XblMultiplayerEventArgsXuid(events[i].EventArgsHandle, &xuid);
                assert(SUCCEEDED(hr));

                if (SUCCEEDED(events[i].Result))
                {
                    CheckMemberFound(events[i].SessionType, xuid);

                    hr = events[i].Result == __HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND) ? S_OK : events[i].Result;
                    LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::JoinLobbyCompleted"); // CODE SNIP SKIP
                }
                CallLuaFunctionWithHr(hr, "OnXblMultiplayerEventType_JoinLobbyCompleted"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::JoinGameCompleted:
            {
                // Handle JoinGameCompleted
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::JoinGameCompleted"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_JoinGameCompleted"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::LeaveGameCompleted:
            {
                // Handle LeaveGameCompleted
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::LeaveGameCompleted"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_LeaveGameCompleted"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::LocalMemberPropertyWriteCompleted:
            {
                // Handle LocalMemberPropertyWriteCompleted
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::LocalMemberPropertyWriteCompleted"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_LocalMemberPropertyWriteCompleted"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::InviteSent:
            {
                // Handle InviteSent
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::InviteSent"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_InviteSent"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::MemberLeft:
            {
                // Handle MemberLeft
                size_t memberCount = 0;
                hr = XblMultiplayerEventArgsMembersCount(events[i].EventArgsHandle, &memberCount);
                assert(SUCCEEDED(hr));
                assert(memberCount > 0);

                std::vector<XblMultiplayerManagerMember> members(memberCount);
                hr = XblMultiplayerEventArgsMembers(events[i].EventArgsHandle, memberCount, members.data());
                assert(SUCCEEDED(hr));

                auto sessionMembers{ GetSessionMembers(events[i].SessionType) };

                for (auto remainingMember : sessionMembers)
                {
                    for (auto member : members)
                    {
                        assert(member.Xuid != remainingMember.Xuid);
                    }
                }

                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::MemberLeft"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_MemberLeft"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::JoinabilityStateChanged:
            {
                // Handle JoinabilityStateChanged
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::JoinabilityStateChanged"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_JoinabilityStateChanged"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::HostChanged:
            {
                XblMultiplayerManagerMember member{};
                hr = XblMultiplayerEventArgsMember(events[i].EventArgsHandle, &member);

                XblMultiplayerManagerMember host{};
                HRESULT hr2 = XblMultiplayerManagerLobbySessionHost(&host);
                (void)(hr2); //suppress unused warning
                //If a host leaves and there is no new host, XblMultiplayerEventArgsMember returns 0x80070714 HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)
                //Likewise, since there is no host, XblMultiplayerManagerLobbySessionHost returns 0x80070525 HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER)
                assert((SUCCEEDED(hr) && SUCCEEDED(hr2)) || (hr == 0x80070714 && hr2 == 0x80070525));

                assert(member.Xuid == host.Xuid);

                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::HostChanged"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_HostChanged"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::MemberPropertyChanged:
            {
                XblMultiplayerManagerMember member;
                hr = XblMultiplayerEventArgsMember(events[i].EventArgsHandle, &member);
                assert(SUCCEEDED(hr));

                const char* propertiesJson = nullptr;
                hr = XblMultiplayerEventArgsPropertiesJson(events[i].EventArgsHandle, &propertiesJson);
                assert(SUCCEEDED(hr));

                auto sessionMembers{ GetSessionMembers(events[i].SessionType) };

                XblMultiplayerManagerMember sessionMember{};
                for (size_t j = 0; j < sessionMembers.size(); ++j)
                {
                    if (member.Xuid == sessionMembers[j].Xuid)
                    {
                        sessionMember = sessionMembers[j];
                        break;
                    }
                }
                
                assert(sessionMember.Xuid == member.Xuid);
                assert(strcmp(sessionMember.PropertiesJson, member.PropertiesJson) == 0);

                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::MemberPropertyChanged"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_MemberPropertyChanged"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::FindMatchCompleted:
            {
                XblMultiplayerMatchStatus matchStatus;
                XblMultiplayerMeasurementFailure initializationFailureCause;
                hr = XblMultiplayerEventArgsFindMatchCompleted(events[i].EventArgsHandle, &matchStatus, &initializationFailureCause);
                assert(SUCCEEDED(hr));
                assert(initializationFailureCause == XblMultiplayerMeasurementFailure::None);
                assert(matchStatus == XblMultiplayerMatchStatus::Completed);
                assert(XblMultiplayerManagerMatchStatus() == matchStatus);

                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::FindMatchCompleted"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_FindMatchCompleted"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::PerformQosMeasurements:
            {
                XblMultiplayerPerformQoSMeasurementsArgs performQoSMeasurementsArgs;
                hr = XblMultiplayerEventArgsPerformQoSMeasurements(events[i].EventArgsHandle, &performQoSMeasurementsArgs);
                assert(SUCCEEDED(hr));

                auto sessionMembers{ GetSessionMembers(events[i].SessionType) };

                assert(performQoSMeasurementsArgs.remoteClientsSize == sessionMembers.size() - 1);

                for (size_t j = 0; j < performQoSMeasurementsArgs.remoteClientsSize; ++j)
                {
                    bool clientFound{ false };
                    for (size_t k = 0; k < sessionMembers.size(); ++k)
                    {
                        if (strcmp(sessionMembers[k].ConnectionAddress, performQoSMeasurementsArgs.remoteClients[j].connectionAddress) == 0)
                        {
                            clientFound = true;
                            assert(strcmp(sessionMembers[k].DeviceToken, performQoSMeasurementsArgs.remoteClients[j].deviceToken.Value) == 0);
                        }
                    }

                    assert(clientFound);
                }

                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::PerformQosMeasurements"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_PerformQosMeasurements"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted:
            {
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_LocalMemberConnectionAddressWriteCompleted"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::SessionPropertyWriteCompleted:
            {
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::SessionPropertyWriteCompleted"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_SessionPropertyWriteCompleted"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted:
            {
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted"); // CODE SNIP SKIP
                if (events[i].Result == HTTP_E_STATUS_PRECOND_FAILED)
                {
                    // Request rejected due to session conflict. Evaluate the need to write again and re-submit if needed.
                    auto fnName = events[i].SessionType == XblMultiplayerSessionType::LobbySession ? "OnXblMultiplayerEventType_SessionSynchronizedPropertyWriteCompleted_412_LobbySession" // CODE SNIP SKIP
                                                                                                   : "OnXblMultiplayerEventType_SessionSynchronizedPropertyWriteCompleted_412_GameSession"; // CODE SNIP SKIP
                    CallLuaFunctionWithHr(S_OK, fnName); // CODE SNIP SKIP
                }
                else
                {
                    CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_SessionSynchronizedPropertyWriteCompleted"); // CODE SNIP SKIP
                }
                break;
            }

            case XblMultiplayerEventType::SynchronizedHostWriteCompleted:
            {
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::SynchronizedHostWriteCompleted"); // CODE SNIP SKIP
                if (events[i].Result == HTTP_E_STATUS_PRECOND_FAILED)
                {
                    // Request rejected due to session conflict. Evaluate the need to write again and re-submit if needed.
                    auto fnName = events[i].SessionType == XblMultiplayerSessionType::LobbySession ? "OnXblMultiplayerEventType_SynchronizedHostWriteCompleted_412_LobbySession" // CODE SNIP SKIP
                                                                                                   : "OnXblMultiplayerEventType_SynchronizedHostWriteCompleted_412_GameSession"; // CODE SNIP SKIP
                    CallLuaFunctionWithHr(S_OK, fnName); // CODE SNIP SKIP
                }
                else
                {
                    CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_SynchronizedHostWriteCompleted"); // CODE SNIP SKIP
                }
                break;
            }

            case XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService:
            {
                LogToScreen("XblMultiplayerManagerDoWork event XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService"); // CODE SNIP SKIP
                CallLuaFunctionWithHr(events[i].Result, "OnXblMultiplayerEventType_ClientDisconnectedFromMultiplayerService"); // CODE SNIP SKIP
                break;
            }

            case XblMultiplayerEventType::TournamentRegistrationStateChanged:
            case XblMultiplayerEventType::TournamentGameSessionReady:
            case XblMultiplayerEventType::ArbitrationComplete:
            default:
            {
                LogToScreen("Received MPM event of type %u, hr=%s", events[i].EventType, ConvertHR(events[i].Result).data()); // CODE SNIP SKIP
                LogToScreen("XblMultiplayerManagerDoWork event Other"); // CODE SNIP SKIP
                break;
            }
        // CODE SKIP END
        }
    }
    // CODE SNIPPET END

    return hr;
}

int StartDoWorkLoop_Lua(lua_State* L)
{
    if (!s_doWork)
    {
        s_doWork = true;
        s_doWorkThread = std::thread([]()
        {
            Data()->m_mpmDoWorkDone = false;
            while (s_doWork && !Data()->m_quit)
            {
                MultiplayerManagerDoWork();
                pal::Sleep(10);
            }
            Data()->m_mpmDoWorkDone = true;
            LogToScreen("Exiting MPM DoWork thread");
        });
    }
    return LuaReturnHR(L, S_OK);
}

void MPMStopDoWorkHelper()
{
    if (s_doWork)
    {
        s_doWork = false;
        s_doWorkThread.join();
    }
}

int StopDoWorkLoop_Lua(lua_State* L)
{
    MPMStopDoWorkHelper();
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerInitialize_Lua(lua_State* L)
{
    const char* lobbySessionTemplateName = "LobbySession";
    auto queueUsedByMultiplayerManager = Data()->queue;
    // CODE SNIPPET START: XblMultiplayerManagerInitialize_C
    HRESULT hr = XblMultiplayerManagerInitialize(lobbySessionTemplateName, queueUsedByMultiplayerManager);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerInitialize: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerDoWork_Lua(lua_State* L)
{
    HRESULT hr = MultiplayerManagerDoWork();
    LogToScreen("XblMultiplayerManagerDoWork: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionAddLocalUser_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionAddLocalUser
    HRESULT hr = XblMultiplayerManagerLobbySessionAddLocalUser(Data()->xalUser);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionAddLocalUser: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionRemoveLocalUser_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionAddLocalUser
    HRESULT hr = XblMultiplayerManagerLobbySessionRemoveLocalUser(Data()->xalUser);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionRemoveLocalUser: hr=%s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionCorrelationId_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionAddLocalUser
    XblGuid correlationId;
    HRESULT hr = XblMultiplayerManagerLobbySessionCorrelationId(&correlationId);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionCorrelationId: id=%s", correlationId.value);
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionSessionReference_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionAddLocalUser
    XblMultiplayerSessionReference sessionReference{};
    HRESULT hr = XblMultiplayerManagerLobbySessionSessionReference(&sessionReference);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionSessionReference: scid = %s, sessionName = %s", sessionReference.Scid, sessionReference.SessionName);
    return LuaReturnHR(L, hr);
}

void LogMultiplayerSessionMember(const XblMultiplayerManagerMember& member)
{
    LogToScreen("XblMultiplayerManagerMember: \n\tMemberId = %u\n\tXuid = %llu\n\tDebugGamertag = %s\n\tIsLocal = %u\n\tIsInLobby = %u\n\tIsInGame = %u\n\tStatus = %u",
        member.MemberId,
        static_cast<unsigned long long>(member.Xuid),
        member.DebugGamertag,
        member.IsLocal,
        member.IsInLobby,
        member.IsInGame,
        member.Status
    );
}

int XblMultiplayerManagerLobbySessionLocalMembers_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionLocalMembers
    size_t localMembersCount = XblMultiplayerManagerLobbySessionLocalMembersCount();
    std::vector<XblMultiplayerManagerMember> localMembers(localMembersCount, XblMultiplayerManagerMember{});
    HRESULT hr = XblMultiplayerManagerLobbySessionLocalMembers(localMembersCount, localMembers.data());
    // CODE SNIPPET END
    if (SUCCEEDED(hr))
    {
        for (const auto& member : localMembers)
        {
            LogMultiplayerSessionMember(member);
        }
    }
    LogToScreen("XblMultiplayerManagerLobbySessionLocalMembers: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionSetLocalMemberProperties_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionSetLocalMemberProperties
    HRESULT hr = XblMultiplayerManagerLobbySessionSetLocalMemberProperties(Data()->xalUser, "Health", "1", nullptr);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionSetLocalMemberProperties: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionInviteFriends_Lua(lua_State* L)
{
#if HC_PLATFORM == HC_PLATFORM_WIN32
    HRESULT hr = XblMultiplayerManagerLobbySessionInviteFriends(Data()->xalUser, nullptr, "//MPSD/custominvitestrings_JoinMyGame");
#else
    HRESULT hr = S_OK;
#endif
    LogToScreen("XblMultiplayerManagerLobbySessionInviteFriends: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerJoinGameFromLobby_Lua(lua_State* L)
{
    const char* gameSessionTemplateName = "GameSession";
    // CODE SNIPPET START: XblMultiplayerManagerJoinGameFromLobby_C
    HRESULT hr = XblMultiplayerManagerJoinGameFromLobby(gameSessionTemplateName);
    if (!SUCCEEDED(hr))
    {
        // Handle error
    }
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerJoinGameFromLobby: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerGameSessionActive_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionActive
    bool isGameSessionAcive = XblMultiplayerManagerGameSessionActive();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionActive: active = %d", isGameSessionAcive);
    lua_pushboolean(L, isGameSessionAcive);
    return 1;
}

int XblMultiplayerManagerGameSessionMembers_Lua(lua_State* L)
{
    HRESULT hr = S_OK;
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionMembers
    size_t memberCount = XblMultiplayerManagerGameSessionMembersCount();
    if (memberCount > 0)
    {
        std::vector<XblMultiplayerManagerMember> gameSessionMembers(memberCount, XblMultiplayerManagerMember{});
        hr = XblMultiplayerManagerGameSessionMembers(memberCount, gameSessionMembers.data());
        if (SUCCEEDED(hr))
        {
            for (const auto& member : gameSessionMembers)
            {
                LogMultiplayerSessionMember(member);
            }
        }
    }
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionMembers: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerGameSessionSetProperties_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionSetProperties
    HRESULT hr = XblMultiplayerManagerGameSessionSetProperties("CustomProperty", "\"CustomPropertyValue\"", nullptr);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionSetProperties: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLeaveGame_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLeaveGame
    HRESULT hr = XblMultiplayerManagerLeaveGame();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLeaveGame: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerJoinLobbyViaActivity_Lua(lua_State* L)
{
    uint64_t xuid1 = GetUint64FromLua(L, 1, Data()->m_multiDeviceManager->GetRemoteXuid());

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = Data()->queue;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; // Take over ownership of the XAsyncBlock*
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);

        if (SUCCEEDED(hr))
        {
            size_t resultSize{ 0 };
            hr = XblMultiplayerGetActivitiesWithPropertiesForUsersResultSize(asyncBlock, &resultSize);
            if (SUCCEEDED(hr))
            {
                size_t count{ 0 };
                std::vector<char> buffer(resultSize, 0);
                XblMultiplayerActivityDetails* activityDetails{};
                hr = XblMultiplayerGetActivitiesWithPropertiesForUsersResult(asyncBlock, resultSize, buffer.data(), &activityDetails, &count, nullptr);
                if (SUCCEEDED(hr))
                {
                    if (resultSize > 0)
                    {
                        std::string handleIdStr = activityDetails[0].HandleId;
                        LogToScreen("Joining lobby via handle %s", handleIdStr.c_str());

                        auto handleId = handleIdStr.c_str();
                        auto xblUserHandle = Data()->xalUser;
                        // CODE SNIPPET START: XblMultiplayerManagerJoinLobby_C
                        hr = XblMultiplayerManagerJoinLobby(handleId, xblUserHandle);
                        // CODE SNIPPET END
                    }
                    else
                    {
                        LogToScreen("No activity handle to join.  Failing...");
                        hr = E_FAIL;
                    }
                }
            }
        }

        CallLuaFunctionWithHr(hr, "OnXblMultiplayerGetActivitiesForUsersAsync");
    };

    uint64_t xuids[1] = {};
    xuids[0] = xuid1;
    size_t xuidsCount = 1;

    HRESULT hr = XblMultiplayerGetActivitiesWithPropertiesForUsersAsync(
        Data()->xboxLiveContext,
        Data()->scid,
        xuids,
        xuidsCount,
        asyncBlock.get());
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }

    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerJoinLobby_lua(lua_State* L)
{
    std::string handleId = { GetStringFromLua(L, 1, "GameSessionName") };
    HRESULT hr = XblMultiplayerManagerJoinLobby(handleId.c_str(), Data()->xalUser);
    LogToScreen("XblMultiplayerManagerJoinLobby: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerJoinGame_Lua(lua_State* L)
{
    // Params:
    // 1) Session name
    // 2) Session template name
    auto sessionName{ GetStringFromLua(L, 1, "GameSessionName") };
    auto sessionTemplateName{ GetStringFromLua(L, 2, "GameSession") };

    // CODE SNIPPET START: XblMultiplayerManagerJoinGame
    HRESULT hr = XblMultiplayerManagerJoinGame(sessionName.data(), sessionTemplateName.data(), &Data()->xboxUserId, 1);
    // CODE SNIPPET END

    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerSetJoinability_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerSetJoinability
    HRESULT hr = XblMultiplayerManagerSetJoinability(XblMultiplayerJoinability::JoinableByFriends, nullptr);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerSetJoinability: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerMemberAreMembersOnSameDevice_Lua(lua_State* L)
{
    const XblMultiplayerManagerMember* first = nullptr;
    const XblMultiplayerManagerMember* second = nullptr;

    // CODE SNIPPET START: XblMultiplayerManagerMemberAreMembersOnSameDevice
    bool areOnSameDevice = XblMultiplayerManagerMemberAreMembersOnSameDevice(first, second);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerMemberAreMembersOnSameDevice");
    LogToScreen("areOnSameDevice: %d", areOnSameDevice);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerLobbySessionMembersCount_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionMembersCount
    size_t count = XblMultiplayerManagerLobbySessionMembersCount();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionMembersCount");
    LogToScreen("count: %d", count);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerLobbySessionMembers_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionMembers
    size_t count = XblMultiplayerManagerLobbySessionMembersCount();
    std::vector<XblMultiplayerManagerMember> members(count);
    HRESULT hr = XblMultiplayerManagerLobbySessionMembers(count, members.data());
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionMembers: hr = %s", ConvertHR(hr).c_str());
    LogToScreen("count: %d", count);
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionHost_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionHost
    XblMultiplayerManagerMember hostMember;
    HRESULT hr = XblMultiplayerManagerLobbySessionHost(&hostMember);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionHost: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionPropertiesJson_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionPropertiesJson
    const char* json = XblMultiplayerManagerLobbySessionPropertiesJson();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionPropertiesJson");
    LogToScreen("json: %s", json);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerLobbySessionConstants_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionConstants
    const XblMultiplayerSessionConstants* consts = XblMultiplayerManagerLobbySessionConstants();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionConstants:\n"
        "ClientMatchmakingCapable = %d\n"
        "EnableMetricsBandwidthDown = %d\n"
        "EnableMetricsBandwidthUp = %d\n"
        "EnableMetricsCustom = %d\n"
        "EnableMetricsLatency = %d\n"
        "MaxMembersInSession = %d\n"
        "Visibility = %d\n"
        "InitiatorXuidsCount = %z\n"
        "MemberInactiveTimeout = %l\n"
        "MemberReadyTimeout = %l\n"
        "MemberReservedTimeout = %l\n"
        "SessionEmptyTimeout = %l\n"
        "MeasurementServerAddressesJson = %s\n"
        "SessionCloudComputePackageConstantsJson = %s\n"
        "CustomJson = %s",
        consts->ClientMatchmakingCapable,
        consts->EnableMetricsBandwidthDown,
        consts->EnableMetricsBandwidthUp,
        consts->EnableMetricsCustom,
        consts->EnableMetricsLatency,
        consts->MaxMembersInSession,
        consts->InitiatorXuidsCount,
        consts->MemberInactiveTimeout,
        consts->MemberReadyTimeout,
        consts->MemberReservedTimeout,
        consts->SessionEmptyTimeout,
        consts->Visibility,
        consts->MeasurementServerAddressesJson,
        consts->SessionCloudComputePackageConstantsJson,
        consts->CustomJson);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties_Lua(lua_State* L)
{
    std::string name = "name";
    void* context = nullptr;

#if HC_PLATFORM != HC_PLATFORM_UWP
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties
    HRESULT hr = XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(
        Data()->xalUser, name.c_str(), context);
    // CODE SNIPPET END
#else
    HRESULT hr = S_OK;
#endif
    LogToScreen("XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress_Lua(lua_State* L)
{
    auto connectionAddressStr = GetStringFromLua(L, 1, "connectionAddress1");
    auto connectionAddress = connectionAddressStr.c_str();
    void* context = nullptr;

    auto xblUserHandle = Data()->xalUser;
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress_C
    HRESULT hr = XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
        xblUserHandle, connectionAddress, context);
    // CODE SNIPPET END

    LogToScreen("XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionIsHost_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionIsHost
    bool isHost = XblMultiplayerManagerLobbySessionIsHost(Data()->xboxUserId);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionIsHost: isHost = %d", isHost);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerLobbySessionSetProperties_Lua(lua_State* L)
{
    std::string name = GetStringFromLua(L, 1, "role");
    std::string valueJson = GetStringFromLua(L, 1, "{\"class\":\"fighter\"}");
    void* context = nullptr;

    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionSetProperties
    HRESULT hr = XblMultiplayerManagerLobbySessionSetProperties(name.c_str(), valueJson.c_str(), context);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionSetProperties: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionSetSynchronizedProperties_Lua(lua_State* L)
{
    std::string name = GetStringFromLua(L, 1, "name1");
    std::string valueJson = GetStringFromLua(L, 1, "{}");
    void* context = nullptr;

    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionSetSynchronizedProperties
    HRESULT hr = XblMultiplayerManagerLobbySessionSetSynchronizedProperties(name.c_str(), valueJson.c_str(), context);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionSetSynchronizedProperties: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionSetSynchronizedHost_Lua(lua_State* L)
{
    size_t membersCount = XblMultiplayerManagerLobbySessionMembersCount();
    std::vector<XblMultiplayerManagerMember> members(membersCount);
    XblMultiplayerManagerLobbySessionMembers(membersCount, members.data());

    std::string defaultDeviceToken = "";

    for (auto member : members)
    {
        if (member.Xuid == Data()->xboxUserId)
        {
            defaultDeviceToken = member.DeviceToken;
            break;
        }
    }

    std::string deviceToken = GetStringFromLua(L, 1, defaultDeviceToken.empty() ? "deviceToken1" : defaultDeviceToken);
    void* context = nullptr;

    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionSetSynchronizedHost
    HRESULT hr = XblMultiplayerManagerLobbySessionSetSynchronizedHost(deviceToken.c_str(), context);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionSetSynchronizedHost: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerLobbySessionInviteUsers_Lua(lua_State* L)
{
    //TODOs: Change XUID to whatever account is used in XblGameInviteAddNotificationHandler_Lua
    // to test sending and receiving invites

    auto xblUserHandle = Data()->xalUser;
    // CODE SNIPPET START: XblMultiplayerManagerLobbySessionInviteUsers_C
    size_t xuidsCount = 1;
    uint64_t xuids[1] = {};
    xuids[0] = 1234567891234567;
    xuids[0] = 2814620188564745; // CODE SNIP SKIP
    HRESULT hr = XblMultiplayerManagerLobbySessionInviteUsers(
        xblUserHandle, 
        xuids, 
        xuidsCount, 
        nullptr,    // ContextStringId 
        nullptr     // CustomActivationContext
    );
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerLobbySessionInviteUsers: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerGameSessionCorrelationId_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionCorrelationId
    const char* id = XblMultiplayerManagerGameSessionCorrelationId();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionCorrelationId: id = %s", id);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerGameSessionSessionReference_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionSessionReference
    const XblMultiplayerSessionReference* session = XblMultiplayerManagerGameSessionSessionReference();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionSessionReference: scid = %s", session->Scid);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerGameSessionHost_Lua(lua_State* L)
{
    XblMultiplayerManagerMember hostMember;
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionHost
    HRESULT hr = XblMultiplayerManagerGameSessionHost(&hostMember);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionHost: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerGameSessionPropertiesJson_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionPropertiesJson
    const char* json = XblMultiplayerManagerGameSessionPropertiesJson();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionPropertiesJson: json = %s", json);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerGameSessionConstants_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionConstants
    const XblMultiplayerSessionConstants* consts = XblMultiplayerManagerGameSessionConstants();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionConstants:\n"
        "ClientMatchmakingCapable = %d\n"
        "EnableMetricsBandwidthDown = %d\n"
        "EnableMetricsBandwidthUp = %d\n"
        "EnableMetricsCustom = %d\n"
        "EnableMetricsLatency = %d\n"
        "MaxMembersInSession = %d",
        consts->ClientMatchmakingCapable,
        consts->EnableMetricsBandwidthDown,
        consts->EnableMetricsBandwidthUp,
        consts->EnableMetricsCustom,
        consts->EnableMetricsLatency,
        consts->MaxMembersInSession
        );
    LogToScreen(
        "InitiatorXuidsCount = %llu\n"
        "MemberInactiveTimeout = %llu\n"
        "MemberReadyTimeout = %llu\n"
        "MemberReservedTimeout = %llu\n"
        "SessionEmptyTimeout = %llu",
        static_cast<unsigned long long>(consts->InitiatorXuidsCount),
        static_cast<unsigned long long>(consts->MemberInactiveTimeout),
        static_cast<unsigned long long>(consts->MemberReadyTimeout),
        static_cast<unsigned long long>(consts->MemberReservedTimeout),
        static_cast<unsigned long long>(consts->SessionEmptyTimeout)
    );
    LogToScreen("MeasurementServerAddressesJson = %s", consts->MeasurementServerAddressesJson);
    LogToScreen("SessionCloudComputePackageConstantsJson = %s", consts->SessionCloudComputePackageConstantsJson);
    LogToScreen("CustomJson = %s", consts->CustomJson); 
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerGameSessionIsHost_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionIsHost
    bool isHost = XblMultiplayerManagerGameSessionIsHost(Data()->xboxUserId);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionIsHost: isHost = %d", isHost);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerGameSessionSetSynchronizedProperties_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerGameSessionSetSynchronizedProperties
    HRESULT hr = XblMultiplayerManagerGameSessionSetSynchronizedProperties("CustomSyncProperty", "\"CustomSyncPropertyValue\"", nullptr);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionSetSynchronizedProperties: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerGameSessionSetSynchronizedHost_Lua(lua_State* L)
{
    std::string deviceToken = GetStringFromLua(L, 1, "deviceToken1");
    void* context = nullptr;

    // CODE SNIPPET START: XblMultiplayerManagerGameSessionSetSynchronizedHost
    HRESULT hr = XblMultiplayerManagerGameSessionSetSynchronizedHost(deviceToken.c_str(), context);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerGameSessionSetSynchronizedHost: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerFindMatch_Lua(lua_State* L)
{
    // Params:
    // 1) matchmaking hopper name
    // 2) attributes json
    auto hopperNameStr{ GetStringFromLua(L, 1, "") };
    auto attributesJsonStr{ GetStringFromLua(L, 2, "") };

    const char* hopperName = hopperNameStr.c_str();
    const char* attributesJson = attributesJsonStr.c_str();

    // CODE SNIPPET START: XblMultiplayerManagerFindMatch_C
    uint32_t timeoutInSeconds = 30;
    HRESULT hr = XblMultiplayerManagerFindMatch(hopperName, attributesJson, timeoutInSeconds);
    if (!SUCCEEDED(hr))
    {
        // Handle failure
    }
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerFindMatch: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerCancelMatch_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerCancelMatch
    XblMultiplayerManagerCancelMatch();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerCancelMatch");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerMatchStatus_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerMatchStatus
    XblMultiplayerMatchStatus status = XblMultiplayerManagerMatchStatus();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerMatchStatus: status = %d", status);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerEstimatedMatchWaitTime_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerEstimatedMatchWaitTime
    uint32_t waitTime = XblMultiplayerManagerEstimatedMatchWaitTime();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerEstimatedMatchWaitTime: waitTime = %d", waitTime);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerAutoFillMembersDuringMatchmaking_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerAutoFillMembersDuringMatchmaking
    bool autoFill = XblMultiplayerManagerAutoFillMembersDuringMatchmaking();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerAutoFillMembersDuringMatchmaking: autoFill = %d", autoFill);
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking
    XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking(true);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking");
    return LuaReturnHR(L, S_OK);
}

int XblMultiplayerManagerSetQosMeasurements_Lua(lua_State* L)
{
    const char* qosJson = "{\"e69c43a8\": {"
        "\"latency\": 5953,"
        "\"bandwidthDown\" : 19342,"
        "\"bandwidthUp\" : 944,"
        "\"customProperty\" : \"customVal\"}}";

    // CODE SNIPPET START: XblMultiplayerManagerSetQosMeasurements
    HRESULT hr = XblMultiplayerManagerSetQosMeasurements(qosJson);
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerSetQosMeasurements: hr = %s", ConvertHR(hr).c_str());
    return LuaReturnHR(L, hr);
}

int XblMultiplayerManagerJoinability_Lua(lua_State* L)
{
    // CODE SNIPPET START: XblMultiplayerManagerJoinability
    XblMultiplayerJoinability joinability = XblMultiplayerManagerJoinability();
    // CODE SNIPPET END
    LogToScreen("XblMultiplayerManagerJoinability: joinability = %d", joinability);
    return LuaReturnHR(L, S_OK);
}

int VerifyMPMGameSessionProperites_Lua(lua_State* L)
{
    bool isActive = XblMultiplayerManagerGameSessionActive();
    assert(isActive);
    UNREFERENCED_PARAMETER(isActive);

    const XblMultiplayerSessionConstants* consts = XblMultiplayerManagerGameSessionConstants();
    assert(consts != nullptr);
    assert(consts->MaxMembersInSession == 20); // defined by template
    UNREFERENCED_PARAMETER(consts);

    const char* corr = XblMultiplayerManagerGameSessionCorrelationId();
    assert(corr != nullptr);
    assert(strlen(corr) > 0);
    UNREFERENCED_PARAMETER(corr);

    XblMultiplayerManagerMember hostMember;
    HRESULT hr = XblMultiplayerManagerGameSessionHost(&hostMember);
    assert(SUCCEEDED(hr) || hr == __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER));
    UNREFERENCED_PARAMETER(hr);

    bool isHost = XblMultiplayerManagerGameSessionIsHost(Data()->xboxUserId);
    UNREFERENCED_PARAMETER(isHost);

    size_t membersCount = XblMultiplayerManagerGameSessionMembersCount();
    std::vector<XblMultiplayerManagerMember> members(membersCount);
    hr = XblMultiplayerManagerGameSessionMembers(membersCount, members.data());
    assert(SUCCEEDED(hr));
    assert(membersCount == 2);
    assert(strlen(members[0].DebugGamertag) > 0);
    assert(strlen(members[1].DebugGamertag) > 0);
    UNREFERENCED_PARAMETER(membersCount);
    UNREFERENCED_PARAMETER(members);

    bool sameDevice = XblMultiplayerManagerMemberAreMembersOnSameDevice(&members[0], &members[1]);
    assert(sameDevice == false);
    UNREFERENCED_PARAMETER(sameDevice);

    const char* props = XblMultiplayerManagerGameSessionPropertiesJson();
    assert(props != nullptr);
    assert(strlen(props) > 0);
    UNREFERENCED_PARAMETER(props);

    const XblMultiplayerSessionReference* sessionRef = XblMultiplayerManagerGameSessionSessionReference();
    assert(sessionRef != nullptr);
    assert(strlen(sessionRef->Scid) > 0);
    assert(strlen(sessionRef->SessionName) > 0);
    assert(strlen(sessionRef->SessionTemplateName) > 0);
    UNREFERENCED_PARAMETER(sessionRef);

    return LuaReturnHR(L, S_OK);
}

int VerifyMPMLobbySessionProperites_Lua(lua_State* L)
{
    const XblMultiplayerSessionConstants* consts = XblMultiplayerManagerGameSessionConstants();
    assert(consts != nullptr);
    assert(consts->MaxMembersInSession == 20); // defined by template
    UNREFERENCED_PARAMETER(consts);

    XblGuid correlationId;
    HRESULT hr = XblMultiplayerManagerLobbySessionCorrelationId(&correlationId);
    assert(SUCCEEDED(hr));
    assert(correlationId.value[0] != 0);
    UNREFERENCED_PARAMETER(correlationId);

    XblMultiplayerManagerMember hostMember;
    hr = XblMultiplayerManagerLobbySessionHost(&hostMember);
    assert(SUCCEEDED(hr) || hr == __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER));
    
    bool isHost = XblMultiplayerManagerLobbySessionIsHost(Data()->xboxUserId);
    UNREFERENCED_PARAMETER(isHost);

    size_t localMembersCount = XblMultiplayerManagerLobbySessionLocalMembersCount();
    std::vector<XblMultiplayerManagerMember> localmembers(localMembersCount);
    hr = XblMultiplayerManagerLobbySessionLocalMembers(localMembersCount, localmembers.data());
    assert(SUCCEEDED(hr));
    assert(localMembersCount == 1);
    assert(strlen(localmembers[0].DebugGamertag) > 0);
    UNREFERENCED_PARAMETER(localmembers);

    size_t membersCount = XblMultiplayerManagerLobbySessionMembersCount();
    std::vector<XblMultiplayerManagerMember> members(membersCount);
    hr = XblMultiplayerManagerLobbySessionMembers(membersCount, members.data());
    assert(SUCCEEDED(hr));
    assert(membersCount >= 1);
    assert(strlen(members[0].DebugGamertag) > 0);
    UNREFERENCED_PARAMETER(members);

    const char* props = XblMultiplayerManagerLobbySessionPropertiesJson();
    assert(props != nullptr);
    assert(strlen(props) > 0);
    UNREFERENCED_PARAMETER(props);

    XblMultiplayerSessionReference sessionRef{};
    hr = XblMultiplayerManagerLobbySessionSessionReference(&sessionRef);
    assert(SUCCEEDED(hr));
    assert(strlen(sessionRef.Scid) > 0);
    assert(strlen(sessionRef.SessionName) > 0);
    assert(strlen(sessionRef.SessionTemplateName) > 0);
    UNREFERENCED_PARAMETER(sessionRef);

    return LuaReturnHR(L, S_OK);
}

void SetupAPIs_XblMultiplayerManager()
{
    // Non XSAPI APIs
    lua_register(Data()->L, "StartDoWorkLoop", StartDoWorkLoop_Lua);
    lua_register(Data()->L, "StopDoWorkLoop", StopDoWorkLoop_Lua);

    // XSAPI MPM APIs
    lua_register(Data()->L, "XblMultiplayerManagerMemberAreMembersOnSameDevice", XblMultiplayerManagerMemberAreMembersOnSameDevice_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerInitialize", XblMultiplayerManagerInitialize_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerDoWork", XblMultiplayerManagerDoWork_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionCorrelationId", XblMultiplayerManagerLobbySessionCorrelationId_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionSessionReference", XblMultiplayerManagerLobbySessionSessionReference_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionLocalMembers", XblMultiplayerManagerLobbySessionLocalMembers_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionMembersCount", XblMultiplayerManagerLobbySessionMembersCount_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionMembers", XblMultiplayerManagerLobbySessionMembers_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionHost", XblMultiplayerManagerLobbySessionHost_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionPropertiesJson", XblMultiplayerManagerLobbySessionPropertiesJson_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionConstants", XblMultiplayerManagerLobbySessionConstants_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionAddLocalUser", XblMultiplayerManagerLobbySessionAddLocalUser_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionRemoveLocalUser", XblMultiplayerManagerLobbySessionRemoveLocalUser_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionSetLocalMemberProperties", XblMultiplayerManagerLobbySessionSetLocalMemberProperties_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties", XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress", XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionIsHost", XblMultiplayerManagerLobbySessionIsHost_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionSetProperties", XblMultiplayerManagerLobbySessionSetProperties_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionSetSynchronizedProperties", XblMultiplayerManagerLobbySessionSetSynchronizedProperties_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionSetSynchronizedHost", XblMultiplayerManagerLobbySessionSetSynchronizedHost_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionInviteFriends", XblMultiplayerManagerLobbySessionInviteFriends_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLobbySessionInviteUsers", XblMultiplayerManagerLobbySessionInviteUsers_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionActive", XblMultiplayerManagerGameSessionActive_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionCorrelationId", XblMultiplayerManagerGameSessionCorrelationId_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionSessionReference", XblMultiplayerManagerGameSessionSessionReference_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionMembers", XblMultiplayerManagerGameSessionMembers_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionHost", XblMultiplayerManagerGameSessionHost_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionPropertiesJson", XblMultiplayerManagerGameSessionPropertiesJson_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionConstants", XblMultiplayerManagerGameSessionConstants_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionIsHost", XblMultiplayerManagerGameSessionIsHost_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionSetProperties", XblMultiplayerManagerGameSessionSetProperties_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionSetSynchronizedProperties", XblMultiplayerManagerGameSessionSetSynchronizedProperties_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerGameSessionSetSynchronizedHost", XblMultiplayerManagerGameSessionSetSynchronizedHost_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerJoinLobby", XblMultiplayerManagerJoinLobby_lua);
    lua_register(Data()->L, "XblMultiplayerManagerJoinLobbyViaActivity", XblMultiplayerManagerJoinLobbyViaActivity_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerJoinGameFromLobby", XblMultiplayerManagerJoinGameFromLobby_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerJoinGame", XblMultiplayerManagerJoinGame_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerLeaveGame", XblMultiplayerManagerLeaveGame_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerFindMatch", XblMultiplayerManagerFindMatch_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerCancelMatch", XblMultiplayerManagerCancelMatch_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerMatchStatus", XblMultiplayerManagerMatchStatus_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerEstimatedMatchWaitTime", XblMultiplayerManagerEstimatedMatchWaitTime_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerAutoFillMembersDuringMatchmaking", XblMultiplayerManagerAutoFillMembersDuringMatchmaking_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking", XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerSetQosMeasurements", XblMultiplayerManagerSetQosMeasurements_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerJoinability", XblMultiplayerManagerJoinability_Lua);
    lua_register(Data()->L, "XblMultiplayerManagerSetJoinability", XblMultiplayerManagerSetJoinability_Lua);
    lua_register(Data()->L, "VerifyMPMGameSessionProperites", VerifyMPMGameSessionProperites_Lua);
    lua_register(Data()->L, "VerifyMPMLobbySessionProperites", VerifyMPMLobbySessionProperites_Lua);
}

#if HC_PLATFORM_IS_MICROSOFT
#pragma warning( pop )
#endif
