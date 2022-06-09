// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

template<typename Ret, typename TWork>
Ret ApiImpl(Ret&& fallbackReturnValue, TWork&& work, bool requireInit = true) noexcept
{
    auto state{ GlobalState::Get() };
    if (!state)
    {
        return fallbackReturnValue;
    }

    assert(state->MultiplayerManager());
    if (requireInit)
    {
        RETURN_HR_IF(state->MultiplayerManager()->IsInitialized() == false, fallbackReturnValue);
    }
    return work(*state->MultiplayerManager());
}

template<typename TWork>
HRESULT ApiImpl(TWork&& work, bool requireInit = true) noexcept
{
    return ApiImpl<HRESULT, TWork>(E_XBL_NOT_INITIALIZED, std::move(work), requireInit);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END

STDAPI XblMultiplayerManagerInitialize(
    _In_z_ const char* lobbySessionTemplateName,
    _In_opt_ XTaskQueueHandle asyncQueue
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(lobbySessionTemplateName);

            mpm.Initialize(lobbySessionTemplateName, asyncQueue);
            return S_OK;
        }, false); // doesn't require init
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerDoWork(
    _Deref_out_opt_ const XblMultiplayerEvent** multiplayerEvents,
    _Out_ size_t* multiplayerEventsCount
) XBL_NOEXCEPT
try
{
    INIT_OUT_PTR_PARAM(multiplayerEvents);

    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF(multiplayerEvents == nullptr || multiplayerEventsCount == nullptr);
            auto& events = mpm.DoWork();

            if (!events.Empty())
            {
                *multiplayerEvents = &(*events.begin());
            }
            else
            {
                *multiplayerEvents = nullptr;
            }
            *multiplayerEventsCount = events.Size();

            return S_OK;
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerJoinLobby(
    _In_z_ const char* handleId,
    _In_ XblUserHandle user
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(handleId);
            return mpm.JoinLobby(handleId, user);
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerJoinGameFromLobby(
    _In_z_ const char* sessionTemplateName
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(sessionTemplateName);
            return mpm.JoinGameFromLobby(sessionTemplateName);
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerJoinGame(
    _In_z_ const char* sessionName,
    _In_z_ const char* sessionTemplateName,
    _In_opt_ const uint64_t* xuids,
    _In_ size_t xuidsCount
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF(sessionName == nullptr || sessionTemplateName == nullptr || (xuids == nullptr && xuidsCount > 0));

            return mpm.JoinGame(
                sessionName,
                sessionTemplateName,
                xsapi_internal_vector<uint64_t>(xuids, xuids + xuidsCount)
            );
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLeaveGame() XBL_NOEXCEPT
try
{
    return ApiImpl([](MultiplayerManager& mpm)
        {
            return mpm.LeaveGame();
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerFindMatch(
    _In_z_ const char* hopperName,
    _In_opt_z_ const char* attributesJson,
    _In_ uint32_t timeoutInSeconds
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF(hopperName == nullptr);

            JsonDocument json;
            if (attributesJson)
            {
                json.Parse(attributesJson);
                if (json.HasParseError() &&
                    json.GetParseError() != rapidjson::kParseErrorDocumentEmpty)
                {
                    return E_INVALIDARG;
                }
            }

            return mpm.FindMatch(
                hopperName,
                json,
                std::chrono::seconds(timeoutInSeconds)
            );
        });
}
CATCH_RETURN()

STDAPI_(void) XblMultiplayerManagerCancelMatch() XBL_NOEXCEPT
try
{
    ApiImpl([](MultiplayerManager& mpm)
        {
            mpm.CancelMatch();
            return S_OK;
        });
}
CATCH_RETURN_WITH(;)

STDAPI_(XblMultiplayerMatchStatus) XblMultiplayerManagerMatchStatus() XBL_NOEXCEPT
try
{
    return ApiImpl(XblMultiplayerMatchStatus::None, [](MultiplayerManager& mpm)
        {
            return mpm.MatchStatus();
        });
}
CATCH_RETURN_WITH(XblMultiplayerMatchStatus::None)

STDAPI_(uint32_t) XblMultiplayerManagerEstimatedMatchWaitTime() XBL_NOEXCEPT
try
{
    return ApiImpl(0, [](MultiplayerManager& mpm)
        {
            return static_cast<uint32_t>(mpm.EstimatedMatchWaitTime().count());
        });
}
CATCH_RETURN_WITH(0)

STDAPI_(bool) XblMultiplayerManagerAutoFillMembersDuringMatchmaking() XBL_NOEXCEPT
try
{
    return ApiImpl(false, [](MultiplayerManager& mpm)
        {
            return mpm.AutoFillMembersDuringMatchmaking();
        });
}
CATCH_RETURN_WITH(false)

STDAPI_(void) XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking(
    _In_ bool autoFillMembers
) XBL_NOEXCEPT
try
{
    ApiImpl([&](MultiplayerManager& mpm)
        {
            mpm.SetAutoFillMembersDuringMatchmaking(autoFillMembers);
            return S_OK;
        });
}
CATCH_RETURN_WITH(;)

STDAPI XblMultiplayerManagerSetQosMeasurements(
    _In_z_ const char* measurementsJson
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(measurementsJson);

            JsonDocument json;
            json.Parse(measurementsJson);
            if (json.HasParseError() &&
                json.GetParseError() != rapidjson::kParseErrorDocumentEmpty)
            {
                return E_INVALIDARG;
            }

            mpm.SetQosMeasurements(json);
            return S_OK;
        });
}
CATCH_RETURN()

STDAPI_(XblMultiplayerJoinability) XblMultiplayerManagerJoinability() XBL_NOEXCEPT
try
{
    return ApiImpl(XblMultiplayerJoinability::None, [](MultiplayerManager& mpm)
        {
            return mpm.Joinability();
        });
}
CATCH_RETURN_WITH(XblMultiplayerJoinability::None)

STDAPI XblMultiplayerManagerSetJoinability(
    _In_ XblMultiplayerJoinability joinability,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            return mpm.SetJoinability(joinability, reinterpret_cast<context_t>(context));
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionCorrelationId(
    _Out_ XblGuid* correlationId
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(correlationId);
            utils::strcpy(correlationId->value, sizeof(correlationId->value), mpm.LobbySession()->CorrelationId().data());
            return S_OK;
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionSessionReference(
    _Out_ XblMultiplayerSessionReference* sessionReference
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(sessionReference);
            *sessionReference = mpm.LobbySession()->SessionReference();
            return S_OK;
        });
}
CATCH_RETURN()

STDAPI_(size_t) XblMultiplayerManagerLobbySessionLocalMembersCount() XBL_NOEXCEPT
try
{
    return ApiImpl<size_t>(0, [](MultiplayerManager& mpm)
        {
            return mpm.LobbySession()->LocalMembers().size();
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionLocalMembers(
    _In_ size_t localMembersCount,
    _Out_writes_(localMembersCount) XblMultiplayerManagerMember* localMembers
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            auto& localMembersVector = mpm.LobbySession()->LocalMembers();
            RETURN_HR_INVALIDARGUMENT_IF(localMembersCount < localMembersVector.size());
            RETURN_HR_INVALIDARGUMENT_IF(localMembers == nullptr && localMembersCount > 0);
            for (size_t i = 0; i < localMembersVector.size(); ++i)
            {
                DISABLE_WARNING_PUSH;
                SUPPRESS_WARNING_NULL_PTR_DEREFERENCE; // null pointer deref
                localMembers[i] = localMembersVector[i]->GetReference();
                DISABLE_WARNING_POP;
            }
            return S_OK;
        });
}
CATCH_RETURN()

STDAPI_(size_t) XblMultiplayerManagerLobbySessionMembersCount() XBL_NOEXCEPT
try
{
    return ApiImpl<size_t>(0, [](MultiplayerManager& mpm)
        {
            return mpm.LobbySession()->Members().size();
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionMembers(
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            auto& membersVector = mpm.LobbySession()->Members();
            RETURN_HR_INVALIDARGUMENT_IF(membersCount < membersVector.size() || members == nullptr);
            for (size_t i = 0; i < membersVector.size(); ++i)
            {
                members[i] = membersVector[i]->GetReference();
            }
            return S_OK;
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionHost(
    _Out_ XblMultiplayerManagerMember* hostMember
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            if (mpm.LobbySession()->Host())
            {
                *hostMember = mpm.LobbySession()->Host()->GetReference();
                return S_OK;
            }
            else
            {
                return __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER);
            }
        });
}
CATCH_RETURN()

STDAPI_(const char*) XblMultiplayerManagerLobbySessionPropertiesJson() XBL_NOEXCEPT
try
{
    return ApiImpl<const char*>(nullptr, [](MultiplayerManager& mpm)-> const char*
        {
            auto& customProperties = mpm.LobbySession()->CustomPropertiesJson();
            if (!customProperties.empty())
            {
                return customProperties.data();
            }
            return nullptr;
        });
    
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(const XblMultiplayerSessionConstants*) XblMultiplayerManagerLobbySessionConstants() XBL_NOEXCEPT
try
{
    return ApiImpl<const XblMultiplayerSessionConstants*>(nullptr, [](MultiplayerManager& mpm)
        {
            return &mpm.LobbySession()->SessionConstants();
        });
}
CATCH_RETURN_WITH(nullptr)

XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED
STDAPI_(const XblTournamentTeamResult*) XblMultiplayerManagerLobbySessionLastTournamentTeamResult() XBL_NOEXCEPT
try
{
    return nullptr;
}
CATCH_RETURN_WITH(nullptr)
XBL_WARNING_POP

STDAPI XblMultiplayerManagerLobbySessionAddLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            return mpm.LobbySession()->AddLocalUser(user);
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionRemoveLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            return mpm.LobbySession()->RemoveLocalUser(user);
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionSetLocalMemberProperties(
    _In_ XblUserHandle user,
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF(name == nullptr || valueJson == nullptr);

            JsonDocument json;
            json.Parse(valueJson);
            if (json.HasParseError() &&
                json.GetParseError() != rapidjson::kParseErrorDocumentEmpty)
            {
                return E_INVALIDARG;
            }

            return mpm.LobbySession()->SetLocalMemberProperties(
                user,
                name,
                json,
                reinterpret_cast<context_t>(context)
            );
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(
    _In_ XblUserHandle user,
    _In_z_ const char* name,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(name);
            return mpm.LobbySession()->DeleteLocalMemberProperties(
                user,
                name,
                reinterpret_cast<context_t>(context)
            );
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
    _In_ XblUserHandle user,
    _In_z_ const char* connectionAddress,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(connectionAddress);
            return mpm.LobbySession()->SetLocalMemberConnectionAddress(
                user,
                connectionAddress,
                reinterpret_cast<context_t>(context)
            );
        });
}
CATCH_RETURN()

STDAPI_(bool) XblMultiplayerManagerLobbySessionIsHost(
    _In_ uint64_t xuid
) XBL_NOEXCEPT
try
{
    return ApiImpl(false, [&](MultiplayerManager& mpm)
        {
            return mpm.LobbySession()->IsHost(xuid);
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionSetProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF(name == nullptr || valueJson == nullptr);

            JsonDocument json;
            json.Parse(valueJson);
            if (json.HasParseError() &&
                json.GetParseError() != rapidjson::kParseErrorDocumentEmpty)
            {
                return E_INVALIDARG;
            }

            return mpm.LobbySession()->SetProperties(
                name,
                json,
                reinterpret_cast<context_t>(context)
            );
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionSetSynchronizedProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF(name == nullptr || valueJson == nullptr);

            JsonDocument json;
            json.Parse(valueJson);
            if (json.HasParseError() &&
                json.GetParseError() != rapidjson::kParseErrorDocumentEmpty)
            {
                return E_INVALIDARG;
            }

            return mpm.LobbySession()->SetSynchronizedProperties(
                name,
                json,
                reinterpret_cast<context_t>(context)
            );
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerLobbySessionSetSynchronizedHost(
    _In_ const char* deviceToken,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(deviceToken);
            return mpm.LobbySession()->SetSynchronizedHost(
                deviceToken,
                reinterpret_cast<context_t>(context)
            );
        });
}
CATCH_RETURN()

#if HC_PLATFORM_IS_MICROSOFT
STDAPI XblMultiplayerManagerLobbySessionInviteFriends(
    _In_ XblUserHandle user,
    _In_opt_z_ const char* contextStringId,
    _In_opt_z_ const char* customActivationContext
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            return mpm.LobbySession()->InviteFriends(
                user,
                contextStringId != nullptr ? contextStringId : xsapi_internal_string(),
                customActivationContext != nullptr ? customActivationContext : xsapi_internal_string()
            );
        });
}
CATCH_RETURN()
#endif

STDAPI XblMultiplayerManagerLobbySessionInviteUsers(
    _In_ XblUserHandle user,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_opt_z_ const char* contextStringId,
    _In_opt_z_ const char* customActivationContext
) XBL_NOEXCEPT
try
{
    return ApiImpl([&](MultiplayerManager& mpm)
        {
            return mpm.LobbySession()->InviteUsers(
                user,
                xsapi_internal_vector<uint64_t>(xuids, xuids + xuidsCount),
                contextStringId != nullptr ? contextStringId : xsapi_internal_string(),
                customActivationContext != nullptr ? customActivationContext : xsapi_internal_string()
            );
        });
}
CATCH_RETURN()

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

// GameSession APIs
template<typename Ret, typename TWork>
Ret GameSessionApiImpl(Ret&& fallbackReturnValue, TWork&& work) noexcept
{
    auto state{ GlobalState::Get() };
    if (!state)
    {
        return fallbackReturnValue;
    }

    assert(state->MultiplayerManager());
    auto gameSession{ state->MultiplayerManager()->GameSession() };
    RETURN_HR_IF(state->MultiplayerManager()->IsInitialized() == false, fallbackReturnValue);
    if (!gameSession)
    {
        LOGS_ERROR << "XblMultiplayerManagerGameSession* API called before GameSession was established";
        return fallbackReturnValue;
    }

    return work(state->MultiplayerManager()->GameSession());
}

template<typename TWork>
HRESULT GameSessionApiImpl(TWork&& work) noexcept
{
    auto state{ GlobalState::Get() };
    if (!state)
    {
        return E_XBL_NOT_INITIALIZED;
    }

    assert(state->MultiplayerManager());
    RETURN_HR_IF(state->MultiplayerManager()->IsInitialized() == false, E_XBL_NOT_INITIALIZED);
    auto gameSession{ state->MultiplayerManager()->GameSession() };
    if (!gameSession)
    {
        LOGS_ERROR << "XblMultiplayerManagerGameSession* API called before GameSession was established";
        return E_UNEXPECTED;
    }

    return work(state->MultiplayerManager()->GameSession());
}

typedef std::shared_ptr<MultiplayerGameSession> GameSessionPtr;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END

STDAPI_(bool) XblMultiplayerManagerGameSessionActive() XBL_NOEXCEPT
try
{
    return ApiImpl(false, [](MultiplayerManager& mpm)
        {
            return mpm.GameSession() != nullptr;
        });
}
CATCH_RETURN()

STDAPI_(const char*) XblMultiplayerManagerGameSessionCorrelationId() XBL_NOEXCEPT
try
{
    return GameSessionApiImpl<const char*>(nullptr, [](GameSessionPtr gameSession)
        {
            return gameSession->CorrelationId().data();
        });
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(const XblMultiplayerSessionReference*) XblMultiplayerManagerGameSessionSessionReference() XBL_NOEXCEPT
try
{
    return GameSessionApiImpl<const XblMultiplayerSessionReference *>(nullptr, [](GameSessionPtr gameSession)
        {
            return &gameSession->SessionReference();
        });
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(size_t) XblMultiplayerManagerGameSessionMembersCount() XBL_NOEXCEPT
try
{
    return GameSessionApiImpl<size_t>(0, [](GameSessionPtr gameSession)
        {
            return gameSession->Members().size();
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerGameSessionMembers(
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT
try
{
    return GameSessionApiImpl([&](GameSessionPtr gameSession)
        {
            auto& membersVector = gameSession->Members();
            RETURN_HR_INVALIDARGUMENT_IF(membersCount < membersVector.size());
            RETURN_HR_INVALIDARGUMENT_IF(members == nullptr && membersCount > 0);
            for (size_t i = 0; i < membersVector.size(); ++i)
            {
                DISABLE_WARNING_PUSH;
                SUPPRESS_WARNING_NULL_PTR_DEREFERENCE;
                members[i] = membersVector[i]->GetReference();
                DISABLE_WARNING_POP;
            }
            return S_OK;
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerGameSessionHost(
    _Out_ XblMultiplayerManagerMember* hostMember
) XBL_NOEXCEPT
try
{
    return GameSessionApiImpl([&](GameSessionPtr gameSession)
        {
            if (gameSession->Host())
            {
                *hostMember = gameSession->Host()->GetReference();
                return S_OK;
            }
            else
            {
                return __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER);
            }
        });
}
CATCH_RETURN()

STDAPI_(const char*) XblMultiplayerManagerGameSessionPropertiesJson() XBL_NOEXCEPT
try
{
    return GameSessionApiImpl<const char*>(nullptr, [](GameSessionPtr gameSession)
        {
            return gameSession->Properties().data();
        });
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(const XblMultiplayerSessionConstants*) XblMultiplayerManagerGameSessionConstants() XBL_NOEXCEPT
try
{
    return GameSessionApiImpl<const XblMultiplayerSessionConstants*>(nullptr, [](GameSessionPtr gameSession)
        {
            return &gameSession->SessionConstants();
        });
}
CATCH_RETURN_WITH(nullptr)

STDAPI_(bool) XblMultiplayerManagerGameSessionIsHost(
    _In_ uint64_t xuid
) XBL_NOEXCEPT
try
{
    return GameSessionApiImpl(false, [&](GameSessionPtr gameSession)
        {
            return gameSession->IsHost(xuid);
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerGameSessionSetProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return GameSessionApiImpl([&](GameSessionPtr gameSession)
        {
            RETURN_HR_INVALIDARGUMENT_IF(name == nullptr || valueJson == nullptr);

            JsonDocument json;
            json.Parse(valueJson);
            if (json.HasParseError() &&
                json.GetParseError() != rapidjson::kParseErrorDocumentEmpty)
            {
                return E_INVALIDARG;
            }

            return gameSession->SetProperties(
                name,
                json,
                reinterpret_cast<context_t>(context)
            );
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerGameSessionSetSynchronizedProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return GameSessionApiImpl([&](GameSessionPtr gameSession)
        {
            RETURN_HR_INVALIDARGUMENT_IF(name == nullptr || valueJson == nullptr);

            JsonDocument json;
            json.Parse(valueJson);
            if (json.HasParseError() &&
                json.GetParseError() != rapidjson::kParseErrorDocumentEmpty)
            {
                return E_INVALIDARG;
            }

            return gameSession->SetSynchronizedProperties(
                name,
                json,
                reinterpret_cast<context_t>(context)
            );
        });
}
CATCH_RETURN()

STDAPI XblMultiplayerManagerGameSessionSetSynchronizedHost(
    _In_ const char* deviceToken,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    return GameSessionApiImpl([&](GameSessionPtr gameSession)
        {
            RETURN_HR_INVALIDARGUMENT_IF_NULL(deviceToken);

            return gameSession->SetSynchronizedHost(deviceToken, reinterpret_cast<context_t>(context));
        });
}
CATCH_RETURN()

