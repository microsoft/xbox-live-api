// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#if !XSAPI_NO_PPL
#include "pplx/pplxtasks.h"
#endif
#include "multiplayer_manager_internal.h"
#include "multiplayer_internal.h"
#include "xbox_live_app_config_internal.h"

#if HC_PLATFORM != HC_PLATFORM_WIN32 && !XSAPI_NO_PPL
#include "xsapi-cpp/title_callable_ui.h"
#endif


#if HC_PLATFORM == HC_PLATFORM_GDK
#include "XGameUI.h"
#endif

using namespace xbox::services;
using namespace xbox::services::legacy;
using namespace xbox::services::multiplayer;
#if __cplusplus_winrt
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerClientManager::MultiplayerClientManager(_In_ const MultiplayerClientManager& other) 
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    m_sessionChangedContext = other.m_sessionChangedContext;
    m_subscriptionLostContext = other.m_subscriptionLostContext;
    m_rtaResyncContext = other.m_rtaResyncContext;
    m_primaryXboxLiveContext = other.m_primaryXboxLiveContext == nullptr ? nullptr : other.m_primaryXboxLiveContext;
    m_lastPendingRead = other.m_lastPendingRead == nullptr ? nullptr : other.m_lastPendingRead;
    m_latestPendingRead = other.m_latestPendingRead == nullptr ? nullptr : other.m_latestPendingRead;
    m_queue = other.m_queue;
}

MultiplayerClientManager::MultiplayerClientManager(
    _In_ const xsapi_internal_string& lobbySessionTemplateName,
    _In_ const TaskQueue& queue
) :
    m_subscriptionsLostFired(false),
    m_autoFillMembers(false),
    m_lobbySessionTemplateName(lobbySessionTemplateName),
    m_sessionChangedContext(0),
    m_subscriptionLostContext(0),
    m_rtaResyncContext(0),
    m_queue{ queue.DeriveWorkerQueue() }
{
    m_multiplayerLocalUserManager = MakeShared<MultiplayerLocalUserManager>();
}

void MultiplayerClientManager::RegisterLocalUserManagerEvents()
{
    std::weak_ptr<MultiplayerClientManager> weakSessionWriter = shared_from_this();
    m_sessionChangedContext = m_multiplayerLocalUserManager->AddMultiplayerSessionChangedHandler([weakSessionWriter](_In_ XblMultiplayerSessionChangeEventArgs args)
    {
        std::shared_ptr<MultiplayerClientManager> pThis(weakSessionWriter.lock());
        if (pThis != nullptr)
        {
            pThis->OnSessionChanged(args);
        }
    });

    m_connectionIdChangedContext = m_multiplayerLocalUserManager->AddMultiplayerConnectionIdChangedHandler([weakSessionWriter](void)
    {
        std::shared_ptr<MultiplayerClientManager> pThis(weakSessionWriter.lock());
        if (pThis != nullptr)
        {
            pThis->OnMultiplayerConnectionIdChanged();
        }
    });

    m_subscriptionLostContext = m_multiplayerLocalUserManager->AddMultiplayerSubscriptionLostHandler([weakSessionWriter](void)
    {
        std::shared_ptr<MultiplayerClientManager> pThis(weakSessionWriter.lock());
        if (pThis != nullptr)
        {
            pThis->OnMultiplayerSubscriptionsLost();
        }
    });

    m_rtaResyncContext = m_multiplayerLocalUserManager->AddRtaResyncHandler([weakSessionWriter](void)
    {
        std::shared_ptr<MultiplayerClientManager> pThis(weakSessionWriter.lock());
        if (pThis != nullptr)
        {
            pThis->OnResyncMessageReceived();
        }
    });
}

void
MultiplayerClientManager::Initialize()
{
    if (m_multiplayerLocalUserManager == nullptr)
    {
        m_multiplayerLocalUserManager = MakeShared<MultiplayerLocalUserManager>();
        RegisterLocalUserManagerEvents();
    }

    m_latestPendingRead = MakeShared<MultiplayerClientPendingReader>(
        m_queue,
        m_lobbySessionTemplateName, 
        m_multiplayerLocalUserManager
        );

    m_lastPendingRead = MakeShared<MultiplayerClientPendingReader>(m_queue);
    m_subscriptionsLostFired.store(false);
    m_latestPendingRead->SetAutoFillMembersDuringMatchmaking(m_autoFillMembers);
}

void MultiplayerClientManager::Shutdown()
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    Destroy();
}

void
MultiplayerClientManager::Destroy()
{
    m_latestPendingRead.reset();
    m_lastPendingRead.reset();
    if (m_multiplayerLocalUserManager != nullptr)
    {
        m_multiplayerLocalUserManager->RemoveMultiplayerSessionChangedHandler(m_sessionChangedContext);
        m_multiplayerLocalUserManager->RemoveMultiplayerSubscriptionLostHandler(m_subscriptionLostContext);
        m_multiplayerLocalUserManager->RemoveMultiplayerConnectionIdChangedHandler(m_connectionIdChangedContext);
        m_multiplayerLocalUserManager->RemoveRtaResyncHandler(m_rtaResyncContext);
        m_multiplayerLocalUserManager.reset();
    }
}

std::shared_ptr<MultiplayerLocalUserManager>
MultiplayerClientManager::LocalUserManager()
{
    return m_multiplayerLocalUserManager;
}

HRESULT
MultiplayerClientManager::SetProperties(
    _In_ const XblMultiplayerSessionReference& sessionRef,
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    // Note: sessionRef can be empty for the lobby initially as we may have not created one yet.
    RETURN_HR_IF(name.empty(), E_INVALIDARG);

    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto latestPending = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPending == nullptr || GetXboxLiveContextMap().size() == 0, E_UNEXPECTED, "Call add_local_user() before writing lobby properties.");

    latestPending->SetProperties(sessionRef, name, valueJson, context);
    return S_OK;
}

HRESULT
MultiplayerClientManager::SetJoinability(
    _In_ XblMultiplayerJoinability value,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto latestPending = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPending == nullptr || GetXboxLiveContextMap().size() == 0, E_UNEXPECTED, "Call add_local_user() before writing lobby properties.");

    return latestPending->LobbyClient()->SetJoinability(value, context);
}

HRESULT
MultiplayerClientManager::SetSynchronizedHost(
    _In_ const XblMultiplayerSessionReference& sessionRef,
    _In_ const xsapi_internal_string& hostDeviceToken,
    _In_opt_ context_t context
    )
{
    // Note: sessionRef can be empty for the lobby initially as we may have not created one yet.
    RETURN_HR_IF(hostDeviceToken.empty(), E_INVALIDARG);

    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto latestPending = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPending == nullptr || GetXboxLiveContextMap().size() == 0, E_UNEXPECTED, "Call add_local_user() before writing host properties.");

    return latestPending->SetSynchronizedHost(sessionRef, hostDeviceToken, context);
}

HRESULT
MultiplayerClientManager::SetSynchronizedProperties(
    _In_ const XblMultiplayerSessionReference& sessionRef,
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    // Note: sessionRef can be empty for the lobby initially as we may have not created one yet.
    RETURN_HR_IF(name.empty(), E_INVALIDARG);

    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto latestPending = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPending == nullptr || GetXboxLiveContextMap().size() == 0, E_UNEXPECTED, "Call add_local_user() before writing lobby properties.");

    return latestPending->SetSynchronizedProperties(sessionRef, name, valueJson, context);
}

void
MultiplayerClientManager::SynchronizedWriteCompleted(
    _In_ std::error_code errorCode,
    _In_ XblMultiplayerEventType eventType,
    _In_ XblMultiplayerSessionType sessionType
    )
{
    AddToLatestPendingReadEventQueue(eventType, sessionType, nullptr, errorCode);
}

HRESULT
MultiplayerClientManager::JoinLobbyByHandle(
    _In_ const xsapi_internal_string& handleId,
    _In_ const xsapi_internal_vector<xbox_live_user_t>& users
)
{
    RETURN_HR_INVALIDARGUMENT_IF(handleId.empty() || users.empty());

    auto latestPending = LatestPendingRead();
    if (latestPending == nullptr)
    {
        Initialize();
        latestPending = LatestPendingRead();
    }

    latestPending->LobbyClient()->AddLocalUsers(users, handleId);
    return S_OK;
}

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK 
HRESULT
MultiplayerClientManager::JoinLobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ xsapi_internal_vector<xbox_live_user_t> users
    )
{
    RETURN_HR_INVALIDARGUMENT_IF(users.empty());
    return JoinLobby(ref new Windows::Foundation::Uri(eventArgs->Uri->RawUri), users);
}

HRESULT
MultiplayerClientManager::JoinLobby(
    _In_ Windows::Foundation::Uri^ url,
    _In_ xsapi_internal_vector<xbox_live_user_t> users
    )
{
    xsapi_internal_string handleId;
    uint64_t invitedXuid;
    if (utils::str_icmp(url->Host->Data(), _T("inviteHandleAccept")) == 0)
    {
        handleId = utils::internal_string_from_utf16(url->QueryParsed->GetFirstValueByName("handle")->Data());
        invitedXuid = utils::string_t_to_uint64(url->QueryParsed->GetFirstValueByName("invitedXuid")->Data());
    }
    else if(utils::str_icmp(url->Host->Data(), _T("activityHandleJoin")) == 0)
    {
        handleId = utils::internal_string_from_utf16(url->QueryParsed->GetFirstValueByName("handle")->Data());
        invitedXuid = utils::string_t_to_uint64(url->QueryParsed->GetFirstValueByName("joinerXuid")->Data());
    }
    else
    {
        return E_INVALIDARG;
    }

    // Check if the xuid matches with the sent users.
    bool invitedUserFound = false;
    int invitedUserIndex = 0;
    for (auto& user: users)
    {
        auto userResult = User::WrapHandle(user);
        RETURN_HR_IF_FAILED(userResult.Hresult());
        if (invitedXuid == userResult.ExtractPayload().Xuid())
        {
            invitedUserFound = true;
            break;
        }
        invitedUserIndex++;
    }

    if (!invitedUserFound)
    {
        // The invited user hasn't been added.
        std::shared_ptr<JoinLobbyCompletedEventArgs> joinLobbyEventArgs = MakeShared<JoinLobbyCompletedEventArgs>(invitedXuid);

        // Since m_latestPendingRead hasn't been initialized yet, this will ensure 
        // the event is still returned correctly through multiplayer_manager::do_work();
        // InvitedXuid's user hasn't been added. Pass in the invited user into join_lobby() API.
        m_multiplayerEventQueue.AddEvent(
            XblMultiplayerEventType::JoinLobbyCompleted,
            XblMultiplayerSessionType::LobbySession,
            std::dynamic_pointer_cast<JoinLobbyCompletedEventArgs>(joinLobbyEventArgs),
            E_UNEXPECTED,
            "Pass in the invited user into join_lobby() API."
        );

        LOG_DEBUG("Pass in the invited user into join_lobby() API.");
        return E_UNEXPECTED;
    }
    else if (invitedUserFound && invitedUserIndex > 0)
    {
        auto invitedUser = users[0];
        users[0] = users[invitedUserIndex];
        users[invitedUserIndex] = invitedUser;
    }

    // This will also join any game that is associated with the lobby.
    if (!handleId.empty())
    {
        return JoinLobbyByHandle(handleId, users);
    }

    return S_OK;
}
#endif

HRESULT
MultiplayerClientManager::JoinGameFromLobby(
    _In_ const xsapi_internal_string& sessionTemplateName
    )
{
    auto primaryContext = GetPrimaryContext();
    auto latestPending = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(primaryContext == nullptr || latestPending == nullptr || latestPending->LobbyClient()->Session() == nullptr, E_UNEXPECTED, "No lobby session exists. Call add_local_user() to create a lobby first.");
    RETURN_HR_IF_LOG_DEBUG(latestPending->GameClient()->Session() != nullptr, E_UNEXPECTED, "A game session already exists. Call leave_game() to leave existing game before creating a new one.");
    RETURN_HR_IF_LOG_DEBUG (latestPending->MatchClient()->MatchStatus() > XblMultiplayerMatchStatus::None, E_UNEXPECTED, "Matchmaking is currently in progress. Call cancel_match() before joining a game.")

    latestPending->GameClient()->SetGameSessionTemplate(sessionTemplateName);

    // We don't care about the async result here, Join result raised as an event after calling do_work so just pass nullptr as callback
    return latestPending->GameClient()->JoinGameFromLobbyHelper( 
        [](Result<std::shared_ptr<XblMultiplayerSession>> result)
    {
        assert(SUCCEEDED(result.Hresult()));
    });
}

HRESULT
MultiplayerClientManager::JoinGame(
    _In_ const xsapi_internal_string& sessionName,
    _In_ const xsapi_internal_string& sessionTemplateName,
    _In_ const xsapi_internal_vector<uint64_t>& xboxUserIds
    )
{
    RETURN_HR_INVALIDARGUMENT_IF(sessionName.empty());
    std::shared_ptr<XblContext> primaryContext = GetPrimaryContext();
    RETURN_HR_IF(primaryContext == nullptr, E_UNEXPECTED);

    auto latestPending = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPending == nullptr, E_UNEXPECTED, "No lobby session exists. Call add_local_user() to create a lobby first.");
    RETURN_HR_IF_LOG_DEBUG(latestPending->MatchClient()->MatchStatus() > XblMultiplayerMatchStatus::None, E_UNEXPECTED, "Matchmaking is currently in progress. Call cancel_match() before joining a game");

    auto gameClient = latestPending->GameClient();
    RETURN_HR_IF(gameClient == nullptr, E_FAIL);

    Function<void()> joinGameHelper = [sharedThis{ shared_from_this() }, gameClient, sessionTemplateName, sessionName]()
    {
        gameClient->SetGameSessionTemplate(sessionTemplateName);
        gameClient->JoinGameHelper(sessionName, nullptr);
    };

    if (xboxUserIds.size() > 0)
    {
        auto gameSessionRef = XblMultiplayerSessionReferenceCreate(
            AppConfig::Instance()->OverrideScid().data(),
            sessionTemplateName.data(),
            sessionName.data()
        );

        XblMultiplayerSessionInitArgs initArgs{};
        initArgs.InitiatorXuids = xboxUserIds.data();
        initArgs.InitiatorXuidsCount = static_cast<uint32_t>(xboxUserIds.size());

        auto gameSession = MakeShared<XblMultiplayerSession>(
            primaryContext->Xuid(),
            &gameSessionRef,
            &initArgs
            );

        gameSession->Join(nullptr, false);
        for (const auto& memberXuid : xboxUserIds)
        {
            if (memberXuid != primaryContext->Xuid())
            {
                gameSession->AddMemberReservation(memberXuid);
            }
        }

        std::weak_ptr<MultiplayerClientManager> weakThis = shared_from_this();
        return primaryContext->MultiplayerService()->WriteSession(gameSession, XblMultiplayerSessionWriteMode::UpdateOrCreateNew, { m_queue,
            [weakThis, sessionTemplateName, joinGameHelper](Result<std::shared_ptr<XblMultiplayerSession>> result)
        {
            std::shared_ptr<MultiplayerClientManager> pThis(weakThis.lock());
            if (pThis == nullptr || pThis->m_latestPendingRead == nullptr)
            {
                return;
            }

            if (FAILED(result.Hresult()))
            {
                pThis->AddToLatestPendingReadEventQueue(
                    XblMultiplayerEventType::JoinGameCompleted,
                    XblMultiplayerSessionType::GameSession,
                    nullptr,
                    result
                );
            }
            else
            {
                // Continue joining the session for all local users.
                joinGameHelper();
            }
        }
        });
    }
    else
    {
        joinGameHelper();
    }

    return S_OK;
}

HRESULT
MultiplayerClientManager::LeaveGame()
{
    std::shared_ptr<XblContext> primaryContext = GetPrimaryContext();
    auto latestPendingRead = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPendingRead == nullptr || primaryContext == nullptr, E_UNEXPECTED, "Call add_local_user() before committing.");

    auto gameSession = latestPendingRead->GameClient()->Session();
    if (gameSession != nullptr)
    {
        latestPendingRead->GameClient()->LeaveRemoteSession(gameSession, true, true);
    }

    if (latestPendingRead->MatchClient()->MatchStatus() != XblMultiplayerMatchStatus::None)
    {
        latestPendingRead->MatchClient()->CancelMatch();
        latestPendingRead->MatchClient()->SetMatchStatus(XblMultiplayerMatchStatus::Canceled);
        // Matchmaking request was canceled since leave_game() was called
        latestPendingRead->MatchClient()->HandleFindMatchCompleted({ xbl_error_code::generic_error, "Matchmaking request was canceled since leave_game() was called" });
    }

    m_multiplayerLocalUserManager->ChangeAllLocalUserGameState(MultiplayerLocalUserGameState::Unknown);
    return S_OK;
}

HRESULT
MultiplayerClientManager::GetActivitiesForSocialGroup(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_string& socialGroup,
    _In_ XTaskQueueHandle queue,
    _In_ Callback<Result<xsapi_internal_vector<XblMultiplayerActivityDetails>>> callback
    )
{
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);
    auto wrapUserResult{ User::WrapHandle(user) };
    RETURN_HR_IF_FAILED(wrapUserResult.Hresult());

    auto serviceResult = GetMultiplayerService(user);
    RETURN_HR_IF_FAILED(serviceResult.Hresult());

    return serviceResult.ExtractPayload()->GetActivitiesForSocialGroup(
        AppConfig::Instance()->OverrideScid(),
        wrapUserResult.Payload().Xuid(),
        socialGroup,
        AsyncContext<Result<Vector<XblMultiplayerActivityDetails>>>{ queue, std::move(callback) }
        );
}

HRESULT
MultiplayerClientManager::InviteFriends(
    _In_ xbox_live_user_t requestingUser,
    _In_ const xsapi_internal_string& invitationText,
    _In_ const xsapi_internal_string& customActivationContext
    )
{
    UNREFERENCED_PARAMETER(customActivationContext);
    UNREFERENCED_PARAMETER(invitationText);
    RETURN_HR_INVALIDARGUMENT_IF(requestingUser == nullptr);
    auto latestPendingRead = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPendingRead == nullptr || latestPendingRead->LobbyClient()->Session() == nullptr, E_UNEXPECTED, "Call add_local_user() and wait for user_added completion event before sending invites.");
    HRESULT hr = S_OK;

    std::weak_ptr<MultiplayerClientManager> thisWeakPtr = shared_from_this();

#if HC_PLATFORM == HC_PLATFORM_XDK

    auto sessionRef = latestPendingRead->LobbyClient()->Session()->SessionReference();
    auto sessionReferenceToInviteTo = ref new Windows::Xbox::Multiplayer::MultiplayerSessionReference(
        utils::PlatformStringFromUtf8(sessionRef.SessionName),
        utils::PlatformStringFromUtf8(sessionRef.Scid),
        utils::PlatformStringFromUtf8(sessionRef.SessionTemplateName)
    );

    Windows::Xbox::System::IUser^ systemUser = nullptr;
    hr = XalUserToXboxSystemUser(requestingUser, &systemUser);
    if (FAILED(hr)) return hr;

    auto asyncOp = Windows::Xbox::UI::SystemUI::ShowSendGameInvitesAsync(
        systemUser,
        sessionReferenceToInviteTo,
        utils::PlatformStringFromUtf8(invitationText.data()),
        utils::PlatformStringFromUtf8(customActivationContext.data())
    );

    asyncOp->Completed = ref new AsyncActionCompletedHandler(
        [thisWeakPtr](IAsyncAction^ asyncInfo, AsyncStatus asyncStatus)
    {
        std::shared_ptr<MultiplayerClientManager> pThis(thisWeakPtr.lock());
        if(pThis != nullptr)
        {
            if (asyncStatus == AsyncStatus::Completed && SUCCEEDED(asyncInfo->ErrorCode.Value))
            {
                pThis->AddToLatestPendingReadEventQueue(
                    XblMultiplayerEventType::InviteSent,
                    XblMultiplayerSessionType::LobbySession
                );
            }
            else
            {
                pThis->AddToLatestPendingReadEventQueue(
                    XblMultiplayerEventType::InviteSent,
                    XblMultiplayerSessionType::LobbySession,
                    nullptr,
                    ConvertHr(asyncInfo->ErrorCode.Value)
                );
            }
        }
    });

#elif HC_PLATFORM == HC_PLATFORM_GDK

    XAsyncBlock* asyncBlock = Make<XAsyncBlock>();
    asyncBlock->queue = m_queue.GetHandle();
    asyncBlock->context = utils::store_shared_ptr(shared_from_this());
    asyncBlock->callback = [](_In_ XAsyncBlock* asyncBlock)
    {
        HRESULT hr = XAsyncGetStatus(asyncBlock, false);
        auto pThis = utils::get_shared_ptr<MultiplayerClientManager>(asyncBlock->context);
        pThis->AddToLatestPendingReadEventQueue(
            XblMultiplayerEventType::InviteSent,
            XblMultiplayerSessionType::LobbySession,
            nullptr,
            make_error_code(xbl_error_code(hr)));

        Delete(asyncBlock);
    };

    // TODO have a way to set async queue here
    hr = XGameUiShowSendGameInviteAsync(
        asyncBlock,
        requestingUser,
        LobbyClient()->Session()->SessionReference().Scid,
        LobbyClient()->Session()->SessionReference().SessionTemplateName,
        LobbyClient()->Session()->SessionReference().SessionName,
        invitationText.c_str(),
        customActivationContext.c_str());

#elif HC_PLATFORM != HC_PLATFORM_WIN32 && !XSAPI_NO_PPL

    UNREFERENCED_PARAMETER(customActivationContext);
    auto asyncOp = xbox::services::system::title_callable_ui::show_game_invite_ui(
        latestPendingRead->LobbyClient()->Session()->SessionReference(),
        utils::string_t_from_internal_string(invitationText)
        );

    pplx::create_task(asyncOp)
    .then([thisWeakPtr](xbox_live_result<void> result)
    {
        std::shared_ptr<MultiplayerClientManager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->AddToLatestPendingReadEventQueue(
                XblMultiplayerEventType::InviteSent,
                XblMultiplayerSessionType::LobbySession,
                nullptr,
                result.err()
            );
        }
    });

#else

    UNREFERENCED_PARAMETER(invitationText);
    UNREFERENCED_PARAMETER(customActivationContext);

#endif

    return hr;
}

HRESULT
MultiplayerClientManager::InviteUsers(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_vector<uint64_t>& xboxUserIds,
    _In_ const xsapi_internal_string& invitationText,
    _In_ const xsapi_internal_string& customActivationContext
    )
{
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);
    auto latestPendingRead = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPendingRead == nullptr || latestPendingRead->LobbyClient()->Session() == nullptr,
        E_UNEXPECTED, "Call add_local_user() and wait for user_added completion event before sending invites.");

    std::weak_ptr<MultiplayerClientManager> weakSessionWriter = shared_from_this();

    auto serviceResult = GetMultiplayerService(user);
    RETURN_HR_IF_FAILED(serviceResult.Hresult());

    return serviceResult.ExtractPayload()->SendInvites(
        latestPendingRead->LobbyClient()->Session()->SessionReference(),
        xboxUserIds,
        AppConfig::Instance()->OverrideTitleId(),
        invitationText,
        customActivationContext,
        AsyncContext<Result<Vector<String>>>{ m_queue,
        [weakSessionWriter](Result<Vector<String>> result)
    {
        std::shared_ptr<MultiplayerClientManager> pThis(weakSessionWriter.lock());
        if (pThis != nullptr)
        {
            std::lock_guard<std::mutex> guard(pThis->m_clientRequestLock);
            pThis->AddToLatestPendingReadEventQueue(
                XblMultiplayerEventType::InviteSent,
                XblMultiplayerSessionType::LobbySession,
                nullptr,
                result
            );
        }
    }
    });
}

Result<std::shared_ptr<MultiplayerService>>
MultiplayerClientManager::GetMultiplayerService(
    _In_ xbox_live_user_t user
    )
{
    auto localUser = m_multiplayerLocalUserManager->GetLocalUser(user);
    if (localUser != nullptr)
    {
        return localUser->Context()->MultiplayerService();
    }
    else
    {
        std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings = MakeShared<xbox::services::XboxLiveContextSettings>();
        std::shared_ptr<AppConfig> appConfig = xbox::services::AppConfig::Instance();
        auto wrapUserResult{ User::WrapHandle(user) };
        RETURN_HR_IF_FAILED(wrapUserResult.Hresult());

        auto multiplayerService = MakeShared<MultiplayerService>(wrapUserResult.ExtractPayload(), xboxLiveContextSettings, appConfig, nullptr);
        return multiplayerService;
    }
}

std::shared_ptr<MultiplayerClientPendingReader>
MultiplayerClientManager::LatestPendingRead() const
{
    return m_latestPendingRead;
}

std::shared_ptr<MultiplayerClientPendingReader>
MultiplayerClientManager::LastPendingRead() const
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    return m_lastPendingRead;
}

std::shared_ptr<MultiplayerLobbyClient>
MultiplayerClientManager::LobbyClient() const
{
    return m_latestPendingRead->LobbyClient();
}

bool
MultiplayerClientManager::IsRequestInProgress()
{
    if (m_latestPendingRead->LobbyClient()->IsRequestInProgress() ||
        m_latestPendingRead->GameClient()->IsRequestInProgress())
    {
        return true;
    }

    return false;
}

bool
MultiplayerClientManager::IsUpdateAvailable()
{
    if (m_latestPendingRead == nullptr || m_lastPendingRead == nullptr)
    {
        return false;
    }

    if (m_lastPendingRead->IsUpdateAvailable(*m_latestPendingRead))
    {
        return true;
    }

    if (GetXboxLiveContextMap().size() == 0 && IsRequestInProgress())
    {
        return true;
    }

    // Always do work for match
    m_latestPendingRead->ProcessMatchEvents();

    return false;
}

MultiplayerEventQueue
MultiplayerClientManager::DoWork()
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    m_multiplayerEventQueue.Clear();

    if (m_latestPendingRead == nullptr)
    {
        return m_multiplayerEventQueue;
    }

    m_latestPendingRead->DoWork();

    ProcessEvents(m_latestPendingRead->LobbyClient()->Session(), m_lastPendingRead->LobbyClient()->Session(), XblMultiplayerSessionType::LobbySession);
    ProcessEvents(m_latestPendingRead->GameClient()->Session(), m_lastPendingRead->GameClient()->Session(), XblMultiplayerSessionType::GameSession);
    ProcessEvents(m_latestPendingRead->MatchClient()->Session(), m_lastPendingRead->MatchClient()->Session(), XblMultiplayerSessionType::MatchSession);

    m_lastPendingRead->deep_copy_if_updated(*m_latestPendingRead);
    auto eventQueue = m_lastPendingRead->EventQueue();

    if (GetXboxLiveContextMap().size() == 0 && !IsRequestInProgress())
    {
        if (!m_subscriptionsLostFired)
        {
            // Force client disconnected event to fire for consistent developer behavior.
            OnMultiplayerSubscriptionsLost();
        }
        else
        {
            // If the last person just left and no more events left, destroy all objects.
            Destroy();
            return eventQueue;
        }
    }

    m_latestPendingRead->ClearEventQueue();
    m_lastPendingRead->ClearEventQueue();

    return eventQueue;
}

xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>
MultiplayerClientManager::GetXboxLiveContextMap()
{
    return m_multiplayerLocalUserManager->GetLocalUserMap();
}

std::shared_ptr<XblContext>
MultiplayerClientManager::GetPrimaryContext()
{
    return m_multiplayerLocalUserManager->GetPrimaryContext();
}

void
MultiplayerClientManager::OnMultiplayerConnectionIdChanged()
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto lobbyClient = m_latestPendingRead->LobbyClient();
    auto lobbySession = lobbyClient->Session();
    XblMultiplayerSessionReadLockGuard lobbyClientSessionSafe(lobbySession);
    if (lobbySession && lobbyClientSessionSafe.CurrentUser() && lobbyClientSessionSafe.CurrentUser()->Status == XblMultiplayerSessionMemberStatus::Active)
    {
        MultiplayerSessionMember::Get(lobbyClientSessionSafe.CurrentUser())->SetStatus(lobbyClientSessionSafe.CurrentUser()->Status);
        auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
        lobbyClient->AddToPendingQueue(pendingRequest);
    }

    auto gameClient = m_latestPendingRead->GameClient();
    auto gameSession = gameClient->Session();
    XblMultiplayerSessionReadLockGuard gameClientSessionSafe(gameSession);
    if (gameSession && gameClientSessionSafe.CurrentUser() && gameClientSessionSafe.CurrentUser()->Status == XblMultiplayerSessionMemberStatus::Active)
    {
        MultiplayerSessionMember::Get(gameClientSessionSafe.CurrentUser())->SetStatus(gameClientSessionSafe.CurrentUser()->Status);
        auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
        gameClient->AddToPendingQueue(pendingRequest);
    }
}

void MultiplayerClientManager::OnMultiplayerSubscriptionsLost()
{
    HRESULT hr = m_queue.RunWork([weakThis = std::weak_ptr<MultiplayerClientManager>{ shared_from_this() }]
    {
        auto pThis{ weakThis.lock() };
        if (pThis)
        {
            std::lock_guard<std::mutex> guard(pThis->m_clientRequestLock);

            bool expected = false;
            if (pThis->m_subscriptionsLostFired.compare_exchange_strong(expected, true))
            {
                // Fired when the title's connection to MPSD using the real-time activity service is lost. 
                // When this event occurs, the title should shut down the multiplayer.

                auto lobbyClient = pThis->LobbyClient();
                if (lobbyClient != nullptr)
                {
                    lobbyClient->RemoveAllLocalUsers();
                }
                pThis->AddToLatestPendingReadEventQueue(XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService, XblMultiplayerSessionType::LobbySession);
            }
        }
    });

    if (FAILED(hr))
    {
        LOGS_INFO << __FUNCTION__ << ": RunWork failed with hr=" << hr;
    }
}

void
MultiplayerClientManager::OnResyncMessageReceived()
{
    // Upon receiving RTA resync message, re-fetch all multiplayer sessions.
    // Note: You could get multiple re-sync messages. It's recommended that you only fetch once every 30 secs.
    if (m_latestPendingRead != nullptr)
    {
        m_latestPendingRead->LobbyClient()->SessionWriter()->OnResyncMessageReceived();
        m_latestPendingRead->GameClient()->SessionWriter()->OnResyncMessageReceived();
    }
}

void
MultiplayerClientManager::OnSessionChanged(
    _In_ XblMultiplayerSessionChangeEventArgs args
    )
{
    std::lock_guard<std::mutex> guard(m_synchronizeWriteWithTapLock);

    if (m_latestPendingRead != nullptr)
    {
        if (m_latestPendingRead->IsMatch(args.SessionReference))
        {
            m_latestPendingRead->MatchClient()->OnSessionChanged(args);
        }
        
        if (m_latestPendingRead->IsLobby(args.SessionReference))
        {
            m_latestPendingRead->LobbyClient()->SessionWriter()->OnSessionChanged(args);
        }
        else if (m_latestPendingRead->IsGame(args.SessionReference))
        {
            m_latestPendingRead->GameClient()->SessionWriter()->OnSessionChanged(args);
        }
    }
}

const MultiplayerEventQueue&
MultiplayerClientManager::EventQueue() const
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    return m_multiplayerEventQueue;
}

void
MultiplayerClientManager::ClearEventQueue()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_multiplayerEventQueue.Clear();
}

void 
MultiplayerClientManager::AddToLatestPendingReadEventQueue(
    _In_ XblMultiplayerEventType eventType,
    _In_ XblMultiplayerSessionType sessionType,
    _In_ std::shared_ptr<XblMultiplayerEventArgs> eventArgs,
    _In_opt_ Result<void> error,
    _In_opt_ context_t context
)
{
    // Note: This function does not require a lock. Caller already has a m_clientRequestLock TODO is this actually true?
    if (m_latestPendingRead != nullptr)
    {
        m_latestPendingRead->AddEvent(eventType, eventArgs, sessionType, error, context);
    }
}

XblMultiplayerSessionType
MultiplayerClientManager::GetSessionType(
    _In_ std::shared_ptr<XblMultiplayerSession> session
    )
{
    XblMultiplayerSessionType sessionType = XblMultiplayerSessionType::Unknown;
    auto latestPendingRead = LatestPendingRead();
    if (latestPendingRead != nullptr)
    {
        if (latestPendingRead->IsLobby(session->SessionReference()))
        {
            sessionType = XblMultiplayerSessionType::LobbySession;
        }
        else if (latestPendingRead->IsGame(session->SessionReference()))
        {
            sessionType = XblMultiplayerSessionType::GameSession;
        }
    }

    return sessionType;
}

void
MultiplayerClientManager::ProcessEvents(
    _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
    _In_ std::shared_ptr<XblMultiplayerSession> oldSession,
    _In_ XblMultiplayerSessionType sessionType
    )
{
    if (oldSession == nullptr || currentSession == nullptr || oldSession->SessionInfo().ChangeNumber == currentSession->SessionInfo().ChangeNumber)
    {
        return;
    }

    xbl_result<XblMultiplayerSessionChangeTypes> diff = currentSession->XblMultiplayerSession::CompareMultiplayerSessions(oldSession);
    if (!diff.err() && diff.payload() == XblMultiplayerSessionChangeTypes::None)
    {
        return;
    }

    XblMultiplayerSessionChangeTypes diffType = diff.payload();

    if (sessionType != XblMultiplayerSessionType::MatchSession)
    {
        if (MultiplayerManagerUtils::IsMultiplayerSessionChangeType(diffType, XblMultiplayerSessionChangeTypes::HostDeviceTokenChange))
        {
            HandleHostChanged(currentSession, sessionType);
        }

        if (MultiplayerManagerUtils::IsMultiplayerSessionChangeType(diffType, XblMultiplayerSessionChangeTypes::MemberListChange))
        {
            HandleMemberListChanged(currentSession, oldSession, sessionType);
        }

        if (MultiplayerManagerUtils::IsMultiplayerSessionChangeType(diffType, XblMultiplayerSessionChangeTypes::CustomPropertyChange))
        {
            HandleSessionPropertiesChanged(currentSession, oldSession, sessionType);
        }

        if (MultiplayerManagerUtils::IsMultiplayerSessionChangeType(diffType, XblMultiplayerSessionChangeTypes::MemberCustomPropertyChange))
        {
            HandleMemberPropertiesChanged(currentSession, oldSession, sessionType);
        }
    }
    
    if (sessionType != XblMultiplayerSessionType::GameSession)
    {
        // Don't need to process these for game. The match will take care of handling these events.
        if (MultiplayerManagerUtils::IsMultiplayerSessionChangeType(diffType, XblMultiplayerSessionChangeTypes::MatchmakingStatusChange)
            && currentSession->MatchmakingServer())
        {
            m_latestPendingRead->MatchClient()->HandleMatchStatusChanged(currentSession);
        }
    }
}

void
MultiplayerClientManager::HandleMemberListChanged(
    _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
    _In_ std::shared_ptr<XblMultiplayerSession> oldSession,
    _In_ XblMultiplayerSessionType sessionType
    )
{
    xsapi_internal_map<uint64_t, const XblMultiplayerSessionMember*> currentSessionMembers;
    xsapi_internal_map<uint64_t, const XblMultiplayerSessionMember*> oldSessionMembers;

    XblMultiplayerSessionReadLockGuard currentSessionSafe(currentSession);
    for (const auto& currentSessionMember : currentSessionSafe.Members())
    {
        currentSessionMembers[currentSessionMember.Xuid] = &currentSessionMember;
    }

    XblMultiplayerSessionReadLockGuard oldSessionSafe(oldSession);
    for (const auto& oldSessionMember : oldSessionSafe.Members())
    {
        oldSessionMembers[oldSessionMember.Xuid] = &oldSessionMember;
    }

    bool haveMembersJoined = false;
    bool haveMembersLeft = false;

    // See if any new members joined
    xsapi_internal_vector<const XblMultiplayerSessionMember*> membersJoined;
    for (const auto& currentSessionMember : currentSessionSafe.Members())
    {
        if (oldSessionMembers.find(currentSessionMember.Xuid) == oldSessionMembers.end())
        {
            haveMembersJoined = true;
            membersJoined.push_back(&currentSessionMember);
        }
    }

    // See if any members left
    xsapi_internal_vector<const XblMultiplayerSessionMember*> membersLeft;
    for (const auto& oldSessionMember : oldSessionSafe.Members())
    {
        if (currentSessionMembers.find(oldSessionMember.Xuid) == currentSessionMembers.end())
        {
            haveMembersLeft = true;
            membersLeft.push_back(&oldSessionMember);
        }
    }

    if (haveMembersJoined || haveMembersLeft)
    {
        auto latestPendingRead = LatestPendingRead();
        if (latestPendingRead == nullptr)
        {
            return;
        }

        if (haveMembersJoined)
        {
            xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> gameMembers;
            for (auto member : membersJoined)
            {
                gameMembers.push_back(latestPendingRead->ConvertToGameMember(member));
            }

            std::shared_ptr<MemberJoinedEventArgs> memberJoinedEventArgs = MakeShared<MemberJoinedEventArgs>(gameMembers);

            AddToLatestPendingReadEventQueue(
                XblMultiplayerEventType::MemberJoined,
                sessionType,
                memberJoinedEventArgs
            );
        }

        if (haveMembersLeft)
        {
            xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> gameMembers;
            for (const auto& member : membersLeft)
            {
                gameMembers.push_back(latestPendingRead->ConvertToGameMember(member));
            }

            std::shared_ptr<MemberLeftEventArgs> memberLeftEventArgs = MakeShared<MemberLeftEventArgs>(
                gameMembers
                );

            AddToLatestPendingReadEventQueue(
                XblMultiplayerEventType::MemberLeft,
                sessionType,
                memberLeftEventArgs
            );
        }
    }
}

void
MultiplayerClientManager::HandleMemberPropertiesChanged(
    _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
    _In_ std::shared_ptr<XblMultiplayerSession> oldSession,
    _In_ XblMultiplayerSessionType sessionType
    )
{
    xsapi_internal_map<uint64_t, const XblMultiplayerSessionMember*> oldSessionMembers;
    XblMultiplayerSessionReadLockGuard oldSessionSafe(oldSession);
    for (const auto& oldSessionMember : oldSessionSafe.Members())
    {
        oldSessionMembers[oldSessionMember.Xuid] = &oldSessionMember;
    }

    // See if properties changed and add them to the queue.
    xsapi_internal_vector<const XblMultiplayerSessionMember*> memberPropertiesChanged;
    XblMultiplayerSessionReadLockGuard currentSessionSafe(currentSession);
    for (const auto& currentSessionMember : currentSessionSafe.Members())
    {
        if (oldSessionMembers.find(currentSessionMember.Xuid) != oldSessionMembers.end())
        {
            auto oldSessionMember = oldSessionMembers[currentSessionMember.Xuid];
            if (utils::str_icmp(currentSessionMember.CustomPropertiesJson, oldSessionMember->CustomPropertiesJson) != 0)
            {
                memberPropertiesChanged.push_back(&currentSessionMember);
            }
        }
    }

    if (memberPropertiesChanged.size() > 0)
    {
        xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> gameMembers;
        const auto& localUsersMap = m_multiplayerLocalUserManager->GetLocalUserMap();
        for (auto member : memberPropertiesChanged)
        {
            auto iter = localUsersMap.find(member->Xuid);
            if (iter != localUsersMap.end())
            {
                // Don't trigger member property changed events for local users.
                continue;
            }

            auto latestPendingRead = LatestPendingRead();
            if (latestPendingRead == nullptr)
            {
                continue;
            }
            std::shared_ptr<MemberPropertyChangedEventArgs> memberPropertiesChangedArgs = MakeShared<MemberPropertyChangedEventArgs>(
                latestPendingRead->ConvertToGameMember(member),
                member->CustomPropertiesJson
                );

            AddToLatestPendingReadEventQueue(
                XblMultiplayerEventType::MemberPropertyChanged,
                sessionType,
                memberPropertiesChangedArgs
            );
        }
    }
}

void
MultiplayerClientManager::HandleSessionPropertiesChanged(
    _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
    _In_ std::shared_ptr<XblMultiplayerSession> oldSession,
    _In_ XblMultiplayerSessionType sessionType
    )
{
    if (sessionType == XblMultiplayerSessionType::LobbySession &&
        m_multiplayerLocalUserManager->IsLocalUserGameState(MultiplayerLocalUserGameState::PendingJoin))
    {
        // Don't join the game if matchmaking is in progress.
        auto latestPendingRead = LatestPendingRead();
        if (latestPendingRead != nullptr && latestPendingRead->MatchClient()->MatchStatus() == XblMultiplayerMatchStatus::None)
        {
            // If state is completed, or transfer handle was removed.
            if (latestPendingRead->LobbyClient()->IsTransferHandleState("completed") ||
                (XblMultiplayerSession::HasSessionPropertyChanged(currentSession, oldSession, MultiplayerLobbyClient_TransferHandlePropertyName) &&
                latestPendingRead->LobbyClient()->GetTransferHandle().empty())
                )
            {
                m_multiplayerLocalUserManager->ChangeAllLocalUserGameState(MultiplayerLocalUserGameState::Join);

                // Join the game session using the handleId.
                latestPendingRead->GameClient()->JoinGameFromLobbyHelper(nullptr);
            }
        }
    }

    // Don't trigger property changed event if the transfer handle property changes.
    if (XblMultiplayerSession::HasSessionPropertyChanged(currentSession, oldSession, MultiplayerLobbyClient_TransferHandlePropertyName) ||
        XblMultiplayerSession::HasSessionPropertyChanged(currentSession, oldSession, MultiplayerLobbyClient_JoinabilityPropertyName))
    {
        return;
    }

    XblMultiplayerSessionReadLockGuard currentSessionSafe(currentSession);
    auto gamePropertiesChangedArgs = MakeShared<SessionPropertyChangedEventArgs>(
        currentSessionSafe.SessionProperties().SessionCustomPropertiesJson
    );

    AddToLatestPendingReadEventQueue(
        XblMultiplayerEventType::SessionPropertyChanged,
        sessionType,
        gamePropertiesChangedArgs
    );
}

void
MultiplayerClientManager::HandleHostChanged(
    _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
    _In_ XblMultiplayerSessionType sessionType
    )
{
    /// A host may have left, and there may be no new host.
    std::shared_ptr<MultiplayerMember> hostMember = nullptr;
    auto host = XblMultiplayerSession::HostMember(currentSession);
    if (host != nullptr)
    {
        auto latestPendingRead = LatestPendingRead();
        if (latestPendingRead != nullptr)
        {
            hostMember = latestPendingRead->ConvertToGameMember(host);
        }
    }

    std::shared_ptr<HostChangedEventArgs> hostChangedEventArgs = MakeShared<HostChangedEventArgs>(
        hostMember
        );

    AddToLatestPendingReadEventQueue(
        XblMultiplayerEventType::HostChanged,
        sessionType,
        hostChangedEventArgs
    );
}

std::shared_ptr<MultiplayerMatchClient>
MultiplayerClientManager::MatchClient()
{
    auto latestPendingRead = LatestPendingRead();
    if (latestPendingRead == nullptr)
    {
        return nullptr;
    }

    return latestPendingRead->MatchClient();
}

HRESULT
MultiplayerClientManager::FindMatch(
    _In_ const xsapi_internal_string& hopperName,
    _In_ JsonValue& attributes,
    _In_ const std::chrono::seconds& timeout
)
{
    auto latestPendingRead = LatestPendingRead();
    RETURN_HR_IF_LOG_DEBUG(latestPendingRead == nullptr || latestPendingRead->LobbyClient()->Session() == nullptr, E_UNEXPECTED, "No local user added. Call add_local_user() first.");
    return latestPendingRead->FindMatch(hopperName, attributes, timeout);
}

void
MultiplayerClientManager::SetAutoFillMembersDuringMatchmaking(
    _In_ bool autoFillMembers
    )
{
    m_autoFillMembers = autoFillMembers;
    auto latestPendingRead = LatestPendingRead();
    if (latestPendingRead != nullptr)
    {
        latestPendingRead->SetAutoFillMembersDuringMatchmaking(autoFillMembers);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
