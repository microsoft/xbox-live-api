// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>
#include <set>
#include "xbox_live_context_internal.h"
#include "multiplayer_internal.h"
#include "xsapi-c/multiplayer_manager_c.h"

typedef void* context_t;

struct XblMultiplayerEventArgs : public xbox::services::RefCounter, public std::enable_shared_from_this<XblMultiplayerEventArgs>
{
    XblMultiplayerEventArgs() = default;
    virtual ~XblMultiplayerEventArgs() = default;

private:
    std::shared_ptr<xbox::services::RefCounter> GetSharedThis() override
    {
        return shared_from_this();
    }
};

namespace xbox { namespace services { namespace multiplayer { namespace manager {

class MultiplayerMatchClient;
class MultiplayerClientManager;
class MultiplayerLocalUserManager;
class MultiplayerLobbyClient;
class MultiplayerGameClient;
class MultiplayerLocalUser;

enum class MultiplayerLocalUserLobbyState
{
    Unknown,
    Add,
    Join,
    InSession,
    Leave,
    Remove
};

enum class MultiplayerLocalUserGameState
{
    Unknown,
    PendingJoin,
    Join,
    InSession,
    Leave
};

enum class PendingRequestType
{
    SynchronizedChanges,
    NonSynchronizedChanges
};

class MultiplayerMember
{
public:

    MultiplayerMember();

    MultiplayerMember(
        _In_ const XblMultiplayerSessionMember* member,
        _In_ bool isLocal,
        _In_ bool isGameHost,
        _In_ bool isLobbyHost,
        _In_ bool isInLobby,
        _In_ bool isInGame
        );

    uint32_t MemberId() const;
    const xsapi_internal_string& TeamId() const;
    const xsapi_internal_string& InitialTeam() const;
    uint64_t Xuid() const;
    const xsapi_internal_string& DebugGamertag() const;
    bool IsLocal() const;
    bool IsInLobby() const;
    bool IsInGame() const;
    XblMultiplayerSessionMemberStatus Status() const;
    const xsapi_internal_string& ConnectionAddress() const;
    const xsapi_internal_string& CustomPropertiesJson() const;
    bool IsMemberOnSameDevice(
        _In_ std::shared_ptr<MultiplayerMember> member
        ) const;
    const xsapi_internal_string& DeviceToken() const;
    static std::shared_ptr<MultiplayerMember> CreateFromSessionMember(
        _In_ const XblMultiplayerSessionMember* member,
        _In_ const std::shared_ptr<XblMultiplayerSession>& lobbySession,
        _In_ const std::shared_ptr<XblMultiplayerSession>& gameSession,
        _In_ const xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>& xboxLiveContextMap
        );
    static std::shared_ptr<MultiplayerMember> CreateFromSessionMember(
        _In_ const XblMultiplayerSessionMember* member,
        _In_ const std::shared_ptr<XblMultiplayerSession>& lobbySession,
        _In_ const std::shared_ptr<XblMultiplayerSession>& gameSession,
        _In_ bool isLocal
        );

    XblMultiplayerManagerMember GetReference() const;

private:
    xsapi_internal_string m_teamId;
    xsapi_internal_string m_initialTeam;
    uint32_t m_memberId;
    uint64_t m_xuid;
    xsapi_internal_string m_gamertag;
    xsapi_internal_string m_deviceToken;
    bool m_isLocal;
    bool m_isInLobby;
    bool m_isInGame;
    XblMultiplayerSessionMemberStatus m_status;
    xsapi_internal_string m_connectionAddress;
    xsapi_internal_string m_jsonProperties;
};

class MultiplayerLobbySession
{
public:
    MultiplayerLobbySession();
    MultiplayerLobbySession(_In_ const MultiplayerLobbySession& other);
    MultiplayerLobbySession(_In_ std::shared_ptr<MultiplayerClientManager> multiplayerClientManagerInstance);
    MultiplayerLobbySession(
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ std::shared_ptr<MultiplayerMember> host,
        _In_ const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& members,
        _In_ const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& localMmembers
        );

    ~MultiplayerLobbySession();

    const xsapi_internal_string& CorrelationId() const;
    const XblMultiplayerSessionReference& SessionReference() const;
    const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& LocalMembers() const;
    const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& Members() const;
    std::shared_ptr<MultiplayerMember> Host() const;
    const xsapi_internal_string& CustomPropertiesJson() const;
    const XblMultiplayerSessionConstants& SessionConstants() const;

    HRESULT AddLocalUser(
        _In_ xbox_live_user_t user
        );

    HRESULT RemoveLocalUser(
        _In_ xbox_live_user_t user
        );

    HRESULT SetLocalMemberProperties(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context = nullptr
        );

    HRESULT DeleteLocalMemberProperties(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& name,
        _In_opt_ context_t context = nullptr
        );

    HRESULT SetLocalMemberConnectionAddress(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& connectionAddress,
        _In_opt_ context_t context = nullptr
        );

    bool IsHost(
        _In_ uint64_t xuid
        );

    HRESULT SetProperties(
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context = nullptr
        );

    HRESULT SetSynchronizedProperties(
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context = nullptr
        );

    HRESULT SetSynchronizedHost(
        _In_ const xsapi_internal_string& hostDeviceToken,
        _In_opt_ context_t context = nullptr
        );

#if HC_PLATFORM_IS_MICROSOFT
    HRESULT InviteFriends(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& contextStringId = xsapi_internal_string(),
        _In_ const xsapi_internal_string& customActivationContext = xsapi_internal_string()
        );
#endif

    HRESULT InviteUsers(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_vector<uint64_t>& xuids,
        _In_ const xsapi_internal_string& contextStringId = xsapi_internal_string(),
        _In_ const xsapi_internal_string& customActivationContext = xsapi_internal_string()
        );

    uint64_t ChangeNumber() const;

    void SetMultiplayerClientManager(
        _In_ std::shared_ptr<MultiplayerClientManager> clientManager
        );

    void SetHost(_In_ std::shared_ptr<MultiplayerMember> hostMember);

#if defined(XSAPI_CPPWINRT)
#if HC_PLATFORM == HC_PLATFORM_XDK
    // TODO is there a better way to do this?
    HRESULT AddLocalUser(
        _In_ winrt::Windows::Xbox::System::User user
        )
    {
        return AddLocalUser(convert_user_to_cppcx(user));
    }

    HRESULT RemoveLocalUser(
        _In_ winrt::Windows::Xbox::System::User user
        )
    {
        return RemoveLocalUser(convert_user_to_cppcx(user));
    }

    HRESULT SetLocalMemberProperties(
        _In_ winrt::Windows::Xbox::System::User user,
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context = nullptr
        )
    {
        return SetLocalMemberProperties(
            convert_user_to_cppcx(user),
            name,
            valueJson,
            context
            );
    }

    HRESULT DeleteLocalMemberProperties(
        _In_ winrt::Windows::Xbox::System::User user,
        _In_ const xsapi_internal_string& name,
        _In_opt_ context_t context = nullptr
        )
    {
        return DeleteLocalMemberProperties(
            convert_user_to_cppcx(user),
            name,
            context
            );
    }

    HRESULT SetLocalMemberConnectionAddress(
        _In_ winrt::Windows::Xbox::System::User user,
        _In_ const xsapi_internal_string& connectionAddress,
        _In_opt_ context_t context = nullptr
        )
    {
        return SetLocalMemberConnectionAddress(
            convert_user_to_cppcx(user),
            connectionAddress,
            context
            );
    }

    HRESULT InviteFriends(
        _In_ winrt::Windows::Xbox::System::User user,
        _In_ const xsapi_internal_string& contextStringId = xsapi_internal_string(),
        _In_ const xsapi_internal_string& customActivationContext = xsapi_internal_string()
        )
    {
        return InviteFriends(
            convert_user_to_cppcx(user),
            contextStringId,
            customActivationContext
            );
    }

    HRESULT InviteUsers(
        _In_ winrt::Windows::Xbox::System::User user,
        _In_ const xsapi_internal_vector<xsapi_internal_string>& xboxUserIds,
        _In_ const xsapi_internal_string& contextStringId = xsapi_internal_string(),
        _In_ const xsapi_internal_string& customActivationContext = xsapi_internal_string()
        )
    {
        return InviteUsers(
            convert_user_to_cppcx(user),
            xboxUserIds,
            contextStringId,
            customActivationContext
            );
    }
#endif
#endif

private:
    void DeepCopyConstants(const XblMultiplayerSessionConstants& other);

    std::shared_ptr<MultiplayerClientManager> m_multiplayerClientManager;

    xsapi_internal_string m_correlationId;
    uint64_t m_changeNumber;
    XblMultiplayerSessionReference m_sessionReference;
    std::shared_ptr<MultiplayerMember> m_host;
    xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> m_members;
    xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> m_localMembers;
    xsapi_internal_string m_customPropertiesJson;

    XblMultiplayerSessionConstants m_sessionConstants{};
    xsapi_internal_vector<uint64_t> m_initiatorXuids;
    XblMultiplayerMemberInitialization m_memberInitialization{};
    xsapi_internal_string m_constantsCustomJson;
    xsapi_internal_string m_constantsCloudComputePackageJson;
    xsapi_internal_string m_constantsMeasurementServerAddressesJson;
};


class MultiplayerGameSession
{
public:
    MultiplayerGameSession();
    MultiplayerGameSession(_In_ const MultiplayerGameSession& other);
    MultiplayerGameSession(
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ std::shared_ptr<MultiplayerMember> host,
        _In_ xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> members
        );

    const xsapi_internal_string& CorrelationId() const;
    const XblMultiplayerSessionReference& SessionReference() const;
    const xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& Members() const;
    std::shared_ptr<MultiplayerMember> Host() const;
    const xsapi_internal_string& Properties() const;
    const XblMultiplayerSessionConstants& SessionConstants() const;
    bool IsHost(
        _In_ uint64_t xuid
        );
    HRESULT SetProperties(
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context = nullptr
        );
    HRESULT SetSynchronizedProperties(
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context = nullptr
        );
    HRESULT SetSynchronizedHost(
        _In_ const xsapi_internal_string& deviceToken,
        _In_opt_ context_t context = nullptr
        );
    uint64_t ChangeNumber() const;
    void SetMultiplayerClientManager(
        _In_ std::shared_ptr<MultiplayerClientManager> clientManager
        );
    void SetHost(_In_ std::shared_ptr<MultiplayerMember> hostMember);

private:
    void DeepCopyConstants(const XblMultiplayerSessionConstants& other);

    xsapi_internal_string m_correlationId;
    uint64_t m_changeNumber;
    XblMultiplayerSessionReference m_sessionReference;
    std::shared_ptr<MultiplayerMember> m_host;
    xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> m_members;
    xsapi_internal_string m_properties;
    std::shared_ptr<MultiplayerClientManager> m_multiplayerClientManager;

    // Constants
    XblMultiplayerSessionConstants m_sessionConstants;
    xsapi_internal_vector<uint64_t> m_initiatorXuids;
    XblMultiplayerMemberInitialization m_memberInitialization;
    xsapi_internal_string m_constantsCustomJson;
    xsapi_internal_string m_constantsCloudComputePackageJson;
    xsapi_internal_string m_constantsMeasurementServerAddressesJson;
};

struct UserAddedEventArgs : public XblMultiplayerEventArgs
{
    UserAddedEventArgs(_In_ uint64_t xuid) : Xuid(xuid) {}
    uint64_t Xuid;
};

struct UserRemovedEventArgs : public XblMultiplayerEventArgs
{
    UserRemovedEventArgs(_In_ uint64_t xuid) : Xuid(xuid) {}
    uint64_t Xuid;
};

struct MemberJoinedEventArgs : public XblMultiplayerEventArgs
{
    MemberJoinedEventArgs(const _In_ xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& members) : Members(members) {}
    xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> Members;
};

struct MemberLeftEventArgs : public XblMultiplayerEventArgs
{
    MemberLeftEventArgs(const _In_ xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>& members) : Members(members) {}
    xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> Members;
};

struct HostChangedEventArgs : public XblMultiplayerEventArgs
{
    HostChangedEventArgs(_In_ std::shared_ptr<MultiplayerMember> hostMember) : HostMember(hostMember) {}
    std::shared_ptr<MultiplayerMember> HostMember;
};

struct MemberPropertyChangedEventArgs : public XblMultiplayerEventArgs
{
    MemberPropertyChangedEventArgs(
        _In_ std::shared_ptr<MultiplayerMember> member,
        _In_ const xsapi_internal_string& jsonProperties
        ) : Member(member),
        Properties(jsonProperties)
    {
    }

    std::shared_ptr<MultiplayerMember> Member;
    xsapi_internal_string Properties;
};

struct SessionPropertyChangedEventArgs : public XblMultiplayerEventArgs
{
    SessionPropertyChangedEventArgs(_In_ const xsapi_internal_string& jsonProperties) : Properties(jsonProperties) {}
    xsapi_internal_string Properties;
};

struct JoinLobbyCompletedEventArgs : public XblMultiplayerEventArgs
{
    JoinLobbyCompletedEventArgs(_In_ uint64_t xuid) : Xuid(xuid) {}
    uint64_t Xuid;
};

struct FindMatchCompletedEventArgs : public XblMultiplayerEventArgs
{
    FindMatchCompletedEventArgs(
        _In_ XblMultiplayerMatchStatus status,
        _In_ XblMultiplayerMeasurementFailure failure
        ) : MatchStatus(status),
        InitializationFailure(failure)
    {
    }

    XblMultiplayerMatchStatus MatchStatus;
    XblMultiplayerMeasurementFailure InitializationFailure;
};

struct PerformQosMeasurementsEventArgs : public XblMultiplayerEventArgs
{
    PerformQosMeasurementsEventArgs() { }
    ~PerformQosMeasurementsEventArgs()
    {
        for (auto& client : remoteClients)
        {
            Delete(client.connectionAddress);
        }
    }

    void AddRemoteClient(const xsapi_internal_string& connectionAddress, const xsapi_internal_string& deviceToken)
    {
        XblMultiplayerConnectionAddressDeviceTokenPair client{};
        client.connectionAddress = Make(connectionAddress);
        utils::strcpy(client.deviceToken.Value, sizeof(client.deviceToken.Value), deviceToken.data());

        remoteClients.push_back(std::move(client));
    }

    xsapi_internal_vector<XblMultiplayerConnectionAddressDeviceTokenPair> remoteClients;
};

class MultiplayerEventQueue
{
public:
    MultiplayerEventQueue();
    MultiplayerEventQueue(const MultiplayerEventQueue& other);
    MultiplayerEventQueue& operator=(MultiplayerEventQueue other);
    ~MultiplayerEventQueue();

    size_t Size() const;
    bool Empty() const;
    void Clear();

    xsapi_internal_vector<XblMultiplayerEvent>::const_iterator begin() const;
    xsapi_internal_vector<XblMultiplayerEvent>::const_iterator end() const;

    void AddEvent(
        _In_ XblMultiplayerEventType eventType,
        _In_ XblMultiplayerSessionType sessionType,
        _In_ std::shared_ptr<XblMultiplayerEventArgs> eventArgs = nullptr,
        _In_ Result<void> error = {},
        _In_opt_ context_t context = nullptr
    );

    void AddEvent(_In_ const XblMultiplayerEvent& multiplayerEvent);

private:
    xsapi_internal_vector<XblMultiplayerEvent> m_events;
    mutable std::mutex m_lock;
};

class MultiplayerManager
{
public:
    MultiplayerManager() = default;
    ~MultiplayerManager();

    void Initialize(
        _In_ const xsapi_internal_string& lobbySessionTemplateName,
        _In_opt_ XTaskQueueHandle asyncQueue
    );

    bool IsInitialized();

    const MultiplayerEventQueue& DoWork();
    std::shared_ptr<MultiplayerLobbySession> LobbySession() const;
    std::shared_ptr<MultiplayerGameSession> GameSession() const;

    HRESULT JoinLobby(
        _In_ const xsapi_internal_string& handleId,
        _In_ xbox_live_user_t user
        );

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK 
    HRESULT JoinLobby(
        _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
        _In_ xbox_live_user_t user
        );
#endif

#if HC_PLATFORM == HC_PLATFORM_XDK
    HRESULT JoinLobby(
        _In_ const xsapi_internal_string& handleId,
        _In_ xsapi_internal_vector<Windows::Xbox::System::User^> users
        );

    HRESULT JoinLobby(
        _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
        _In_ xsapi_internal_vector<Windows::Xbox::System::User^> users
        );

    void InvitePartyToGame();

#endif

    HRESULT JoinGameFromLobby(
        _In_ const xsapi_internal_string& sessionTemplateName
        );

    HRESULT JoinGame(
        _In_ const xsapi_internal_string& sessionName,
        _In_ const xsapi_internal_string& sessionTemplateName,
        _In_ const xsapi_internal_vector<uint64_t>& xuids = xsapi_internal_vector<uint64_t>()
        );

    HRESULT LeaveGame();

    HRESULT FindMatch(
        _In_ const xsapi_internal_string& hopperName,
        _In_ JsonValue& attributes,
        _In_ const std::chrono::seconds& timeout = std::chrono::seconds(60)
        );

    void CancelMatch();

    XblMultiplayerMatchStatus MatchStatus() const;

    std::chrono::seconds EstimatedMatchWaitTime() const;

    bool AutoFillMembersDuringMatchmaking() const;

    void SetAutoFillMembersDuringMatchmaking(
        _In_ bool autoFillMembers
        );

    void SetQosMeasurements(
        _In_ const JsonValue& measurements
        );

    XblMultiplayerJoinability Joinability() const;

    HRESULT SetJoinability(
        _In_ XblMultiplayerJoinability value,
        _In_opt_ context_t context = nullptr
        );

    bool IsDirty();

    std::shared_ptr<MultiplayerClientManager> GetMultiplayerClientManager() { return m_multiplayerClientManager; }

    std::shared_ptr<MultiplayerGameClient> GameClient();

    std::shared_ptr<MultiplayerLobbyClient> LobbyClient();

#if XSAPI_UNIT_TESTS
    void Shutdown();
#endif

#if defined(XSAPI_CPPWINRT)
#if HC_PLATFORM == HC_PLATFORM_XDK
    xbox_live_result<void> join_lobby(
        _In_ const xsapi_internal_string& handleId,
        _In_ xsapi_internal_vector<winrt::Windows::Xbox::System::User> users
        )
    {
        return join_lobby(handleId, convert_user_vector_to_cppcx(users));
    }
#endif
#endif

private:
    MultiplayerManager(MultiplayerManager const&) = delete;
    void operator=(MultiplayerManager const&) = delete;

    bool m_isDirty = false;
    void SetMultiplayerGameSession(_In_ std::shared_ptr<MultiplayerGameSession> gameSession);
    void SetMultiplayerLobbySession(_In_ std::shared_ptr<MultiplayerLobbySession> multiplayerLobby);

    mutable std::mutex m_lock;
    XblMultiplayerJoinability m_joinability = XblMultiplayerJoinability::None;
    std::shared_ptr<MultiplayerLobbySession> m_multiplayerLobbySession;
    std::shared_ptr<MultiplayerGameSession> m_multiplayerGameSession;
    std::shared_ptr<MultiplayerClientManager> m_multiplayerClientManager;

    MultiplayerEventQueue m_eventQueue;
    XTaskQueueHandle m_queue{ nullptr };
};

typedef Callback<Result<std::shared_ptr<XblMultiplayerSession>>> MultiplayerSessionCallback;
typedef Callback<Result<MultiplayerEventQueue>> MultiplayerEventQueueCallback; // Maybe just pass queue

class MultiplayerClientPendingRequest
{
public:
    MultiplayerClientPendingRequest();

    PendingRequestType RequestType() const;

    context_t Context();

    uint32_t Identifier() const;

    // Local user properties
    std::shared_ptr<MultiplayerLocalUser> LocalUser();
    void SetLocalUser(_In_ std::shared_ptr<MultiplayerLocalUser> user);

    MultiplayerLocalUserLobbyState LobbyState();
    void SetLobbyState(_In_ MultiplayerLocalUserLobbyState userState);

    const xsapi_internal_string& LobbyHandleId() const;
    void SetLobbyHandleId(_In_ const xsapi_internal_string& handleId);

    const xsapi_internal_string& LocalUserSecureDeivceAddress() const;
    void SetLocalUserConnectionAddress(
        _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
        _In_ const xsapi_internal_string& connectionAddress,
        _In_opt_ context_t context
        );

    const xsapi_internal_map<xsapi_internal_string, JsonDocument>& LocalUserProperties() const;
    void SetLocalUserProperties(
        _In_ std::shared_ptr<MultiplayerLocalUser> localUser, 
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson, 
        _In_opt_ context_t context
        );

    // Session non-synchronized properties
    XblMultiplayerJoinability Joinability();
    void SetJoinability(_In_ XblMultiplayerJoinability value, _In_opt_ context_t context);

    const xsapi_internal_map<xsapi_internal_string, JsonDocument>& SessionProperties() const;
    void SetSessionProperties(_In_ const xsapi_internal_string& name, _In_ const JsonValue& valueJson, _In_opt_ context_t context);

    // Session synchronized properties
    const xsapi_internal_string& SynchronizedHostDeviceToken() const;
    void SetSynchronizedHostDeviceToken(_In_ const xsapi_internal_string& hostDeviceToken, _In_opt_ context_t context);

    const xsapi_internal_map<xsapi_internal_string, JsonDocument>& SynchronizedSessionProperties() const;
    void SetSynchronizedSessionProperties(_In_ const xsapi_internal_string& name, const _In_ JsonValue& valueJson, _In_opt_ context_t context);

    void AppendPendingChanges(
        _In_ std::shared_ptr<XblMultiplayerSession> sessionToCommit, 
        _In_ std::shared_ptr<MultiplayerLocalUser> localUser, 
        _In_ bool isGameInProgress = false
        );

private:
    context_t m_context;
    PendingRequestType m_requestType{};
    uint32_t m_identifier{ s_nextUniqueIdentifier++ };

    // Local user properties
    std::shared_ptr<MultiplayerLocalUser> m_localUser;
    MultiplayerLocalUserLobbyState m_localUserLobbyState;
    xsapi_internal_map<xsapi_internal_string, JsonDocument> m_localUserProperties;
    xsapi_internal_string m_localUserSecureDeivceAddress;
    xsapi_internal_string m_lobbyHandleId;   // Only used while joining a friend's lobby

    // Session non-synchronized properties
    xsapi_internal_map<xsapi_internal_string, JsonDocument> m_sessionProperties;
    XblMultiplayerJoinability m_joinability{};

    // Session synchronized properties
    xsapi_internal_string m_synchronizedHostDeviceToken;
    xsapi_internal_map<xsapi_internal_string, JsonDocument> m_synchronizedSessionProperties;

    static std::atomic<uint32_t> s_nextUniqueIdentifier;
};


class MultiplayerSessionWriter : public std::enable_shared_from_this<MultiplayerSessionWriter>
{
public:
    MultiplayerSessionWriter(const TaskQueue& queue) noexcept;
    MultiplayerSessionWriter(
        const TaskQueue& queue,
        _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
    ) noexcept;

    uint64_t Id() const;

    const std::shared_ptr<XblMultiplayerSession>& Session() const;
    void UpdateSession(_In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession);

    uint64_t TapChangeNumber() const;
    void SetTapChangeNumber(_In_ uint64_t changeNumber);

    bool IsTapReceived() const;
    void SetTapReceived(_In_ bool bReceived);

    bool IsWriteInProgress() const;
    void SetWriteInProgress(_In_ bool writeInProgress);

    void OnSessionChanged(
        _In_ XblMultiplayerSessionChangeEventArgs args
    ) noexcept;

    HRESULT CommitSynchronizedChanges(
        _In_ std::shared_ptr<XblMultiplayerSession> sessionToCommit,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    HRESULT LeaveRemoteSession(
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    HRESULT CommitPendingChanges(
        _In_ Vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue,
        _In_ XblMultiplayerSessionType sessionType,
        _In_ bool isGameInProgress,
        _In_ MultiplayerEventQueueCallback callback
    ) noexcept;

    HRESULT CommitPendingSynchronizedChanges(
        _In_ Vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue,
        _In_ XblMultiplayerSessionType sessionType,
        _In_ MultiplayerEventQueueCallback callback
    ) noexcept;

    HRESULT WriteSession(
        _In_ std::shared_ptr<XblContext> xboxLiveContext,
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ XblMultiplayerSessionWriteMode mode,
        _In_ bool updateLatest,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    HRESULT WriteSessionByHandle(
        _In_ std::shared_ptr<XblContext> xboxLiveContext,
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ XblMultiplayerSessionWriteMode mode,
        _In_ const String& handleId,
        _In_ bool updateLatest,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    void OnSessionUpdated(
        _In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession
        );

    XblFunctionContext AddMultiplayerSessionUpdatedHandler(
        _In_ Callback<const std::shared_ptr<XblMultiplayerSession>&> handler
        );

    void OnResyncMessageReceived();

    std::shared_ptr<XblContext> GetPrimaryContext();

    MultiplayerEventQueue HandleEvents(
        _In_ xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue,
        _In_ Result<void> error,
        _In_ XblMultiplayerSessionType sessionType
        );

private:
    void Destroy();
    void Resync();

    // Synchronize write session result with any changes that happened in the interim.
    void HandleWriteSessionResult(
        _In_ Result<std::shared_ptr<XblMultiplayerSession>> writeSessionResult,
        _In_ bool updateLatest,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    HRESULT GetCurrentSessionHelper(
        _In_ std::shared_ptr<XblContext> xboxLiveContext,
        _In_ const XblMultiplayerSessionReference& sessionReference,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    TaskQueue m_queue;

    // resync
    bool m_isResyncTaskInProgress{ false };
    XblFunctionContext m_handleResyncEventCounter{ 0 };
    std::mutex m_resyncLock;

    std::mutex m_stateLock;
    XblFunctionContext m_sessionUpdateEventHandlerCounter{ 1 };
    UnorderedMap<uint32_t, Callback<const std::shared_ptr<XblMultiplayerSession>>> m_sessionUpdateEventHandler;

    uint64_t m_id{ 0 }; // used to ignore calls made before resetting the state via destory()
    std::mutex m_synchronizeWriteWithTapLock;
    uint64_t m_tapChangeNumber{ 0 };
    bool m_isTapReceived{ false };
    uint64_t m_numOfWritesInProgress{ 0 };
    std::shared_ptr<XblMultiplayerSession> m_session;
    std::shared_ptr<MultiplayerLocalUserManager> m_multiplayerLocalUserManager;

};

class MultiplayerGameClient : public std::enable_shared_from_this<MultiplayerGameClient>
{
public:
    MultiplayerGameClient(const TaskQueue& queue) noexcept;
    MultiplayerGameClient(
        const TaskQueue& queue,
        _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
    ) noexcept;
    ~MultiplayerGameClient();

    // TODO
    void deep_copy_if_updated(_In_ const MultiplayerGameClient& other);
    void Initialize();
    void SetGameSessionTemplate(_In_ const xsapi_internal_string& sessionTemplateName);
    std::shared_ptr<MultiplayerSessionWriter> SessionWriter() const;
    std::shared_ptr<MultiplayerGameSession> Game() const;
    void UpdateGame(_In_ const std::shared_ptr<MultiplayerGameSession>& multiplayerGame);

    MultiplayerEventQueue DoWork();
    bool IsRequestInProgress();
    bool IsPendingGameChanges();

    void ClearPendingQueue();
    void AddToPendingQueue(_In_ std::shared_ptr<MultiplayerClientPendingRequest> pendingRequest);
    void AddToProcessingQueue(_In_ xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue);
    void RemoveFromProcessingQueue(_In_ uint32_t identifier);

    const MultiplayerEventQueue& EventQueue();
    xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> GetProcessingQueue();

    void UpdateGameSession(_In_ const std::shared_ptr<XblMultiplayerSession>& session);

    void UpdateObjects(
        const std::shared_ptr<XblMultiplayerSession>& updatedSession,
        const std::shared_ptr<XblMultiplayerSession>& lobbySession
        );

    std::shared_ptr<XblMultiplayerSession> Session() const;

    void UpdateSession(_In_ const std::shared_ptr<XblMultiplayerSession>& session);

    void RemoveStaleUsersFromRemoteSession();
    void SetLocalMemberPropertiesToRemoteSession(
        _In_ const std::shared_ptr<xbox::services::multiplayer::manager::MultiplayerLocalUser>& localUser,
        _In_ const Map<String, JsonDocument>& propertiesToWrite,
        _In_ const String& localUserSecureDeviceAddress
    ) noexcept;

    HRESULT JoinGameHelper(
        _In_ const String& sessionName,
        _In_ Callback<Result<void>> callback
    ) noexcept;

    void LeaveRemoteSession(
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ bool stopAdvertisingGameSession,
        _In_ bool triggerCompletionEvent
    ) noexcept;

    HRESULT JoinGameFromLobbyHelper(
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    HRESULT JoinGameBySessionReference(
        _In_ const XblMultiplayerSessionReference& gameSessionRef,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    HRESULT JoinGameByHandle(
        _In_ const String& handleId,
        _In_ bool createGameIfFailedToJoin,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

private:
    std::shared_ptr<XblMultiplayerSession> LobbySession() const;
    std::shared_ptr<MultiplayerLobbyClient> LobbyClient() const;

    std::shared_ptr<MultiplayerGameSession> ConvertToMultiplayerGame(
        _In_ const std::shared_ptr<XblMultiplayerSession>& sessionToConvert,
        _In_ const std::shared_ptr<XblMultiplayerSession>& lobbySession
        );

    HRESULT JoinGameForAllLocalMembers(
        _In_ const XblMultiplayerSessionReference& sessionRefToJoin,
        _In_ const String& handleIdToJoin,
        _In_ bool createGameIfFailedToJoin,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    HRESULT JoinHelper(
        _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ bool writeMemberPropertiesFromLobby,
        _In_ const String& handleId,
        _In_ MultiplayerSessionCallback callback
    ) const noexcept;

    TaskQueue m_queue;
    mutable std::mutex m_clientRequestLock;
    std::atomic<bool> m_pendingCommitInProgress{ false };
    String m_gameSessionTemplateName;
    uint64_t m_updateNumber{ 0 };
    std::shared_ptr<MultiplayerSessionWriter> m_sessionWriter;
    MultiplayerEventQueue m_multiplayerEventQueue;
    std::shared_ptr<MultiplayerGameSession> m_multiplayerGame;
    std::shared_ptr<MultiplayerLocalUserManager> m_multiplayerLocalUserManager;
    Queue<std::shared_ptr<MultiplayerClientPendingRequest>> m_pendingRequestQueue;
    Vector<std::shared_ptr<MultiplayerClientPendingRequest>> m_processingQueue;
};

#define MultiplayerLobbyClient_TransferHandlePropertyName "GameSessionTransferHandle"
#define MultiplayerLobbyClient_JoinabilityPropertyName "Joinability"

class MultiplayerLobbyClient : public std::enable_shared_from_this<MultiplayerLobbyClient>
{
public:
    MultiplayerLobbyClient(_In_ const TaskQueue& queue) noexcept;
    MultiplayerLobbyClient(
        _In_ const TaskQueue& queue,
        _In_ String lobbySessionTemplateName,
        _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
    ) noexcept;
    ~MultiplayerLobbyClient() noexcept;

    // TODO
    void deep_copy_if_updated(_In_ const MultiplayerLobbyClient& other);
    void Initialize();
    const std::shared_ptr<MultiplayerSessionWriter>& SessionWriter() const;
    const std::shared_ptr<MultiplayerLobbySession>& Lobby() const;
    void ClearPendingQueue();
    void AddToPendingQueue(_In_ std::shared_ptr<MultiplayerClientPendingRequest> pendingRequest);
    void AddToProcessingQueue(_In_ xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue);
    void RemoveFromProcessingQueue(_In_ uint32_t identifier);

    HRESULT AddLocalUser(
        _In_ xbox_live_user_t user,
        _In_ MultiplayerLocalUserLobbyState userState,
        _In_ const xsapi_internal_string& handleId = xsapi_internal_string()
        );

    void AddLocalUsers(_In_ xsapi_internal_vector<xbox_live_user_t> user, _In_ const xsapi_internal_string& handleId);
    void AddLocalUsers(_In_ xsapi_internal_vector<xbox_live_user_t> user);

    HRESULT RemoveLocalUser(_In_ xbox_live_user_t user);
    void RemoveAllLocalUsers();

    HRESULT SetLocalMemberProperties(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context
        );

    HRESULT DeleteLocalMemberProperties(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& name,
        _In_opt_ context_t context
        );

    HRESULT SetLocalMemberConnectionAddress(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& address,
        _In_opt_ context_t context
        );

    MultiplayerEventQueue DoWork();
    bool IsPendingLobbyChanges();
    bool IsRequestInProgress();

    HRESULT CreateGameFromLobby() noexcept;

    XblMultiplayerJoinability Joinability();

    HRESULT SetJoinability(
        _In_ XblMultiplayerJoinability value,
        _In_opt_ context_t context
        );

    const MultiplayerEventQueue& EventQueue();
    xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> GetProcessingQueue();

    void UpdateLobbySession(_In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession);

    void UpdateObjects(
        const std::shared_ptr<XblMultiplayerSession>& updatedSession,
        const std::shared_ptr<XblMultiplayerSession>& gameSession
        );

    const std::shared_ptr<XblMultiplayerSession>& Session() const;

    void UpdateSession(
        _In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession
        );

    void LeaveRemoteSession(
        _In_ std::shared_ptr<XblMultiplayerSession> session
        );

    void StopAdvertisingGameSession(
        _In_ Result<std::shared_ptr<XblMultiplayerSession>> result
        );

    void AdvertiseGameSession() noexcept;
    void ClearGameSessionFromLobby();

    bool IsTransferHandleState(_In_ const xsapi_internal_string& state);
    xsapi_internal_string GetTransferHandle();

    void RemoveStaleXboxLiveContextFromMap();
    const xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>& GetLocalUserMap();
    std::shared_ptr<XblContext> GetPrimaryContext();

private:
    std::shared_ptr<MultiplayerGameClient> GameClient();
    std::shared_ptr<XblMultiplayerSession> GameSession();

    HRESULT CommitPendingLobbyChanges(
        _In_ const Vector<uint64_t>& xuidsInOrder,
        _In_ bool joinByHandleId,
        _In_ XblMultiplayerSessionReference sessionRef,
        _In_ MultiplayerEventQueueCallback callback
    ) noexcept;

    HRESULT CommitLobbyChanges(
        _In_ const Vector<uint64_t>& xuidsInOrder,
        _In_ std::shared_ptr<XblMultiplayerSession> lobbySessionToCommit,
        _In_ Callback<Result<void>> callback
    ) noexcept;

    void UpdateLobby(_In_ std::shared_ptr<MultiplayerLobbySession> multiplayerLobby);
    void UpdateLocalLobbyMembers(
        _In_ const std::shared_ptr<XblMultiplayerSession>& lobbySession,
        _In_ const std::shared_ptr<XblMultiplayerSession>& gameSession
        );

    bool IsPendingLobbyLocalUserChanges();

    bool ShouldUpdateHostToken(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::MultiplayerLocalUser> localUser,
        _In_ std::shared_ptr<XblMultiplayerSession> session
        );

    void UserStateChanged(
        _In_ Result<void> error,
        _In_ MultiplayerLocalUserLobbyState localUserLobbyState,
        _In_ uint64_t xboxUserId
        );

    void HandleLobbyChangeEvents(
        _In_ Result<void> error,
        _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
        _In_ const xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>>& processingQueue
        );

    void HandleJoinLobbyCompleted(
        _In_ Result<void> error,
        _In_ uint64_t joinedXuid
        );

    void JoinLobbyCompleted(
        _In_ Result<void> error,
        _In_ uint64_t invitedXboxUserId
        );

    void AddEvent(
        _In_ XblMultiplayerEventType eventType,
        _In_opt_ std::shared_ptr<XblMultiplayerEventArgs> eventArgs = nullptr,
        _In_opt_ Result<void> error = {},
        _In_opt_ context_t context = nullptr
        );

    std::shared_ptr<MultiplayerLobbySession> ConvertToMultiplayerLobby(
        _In_ const  std::shared_ptr<XblMultiplayerSession>& sessionToConvert,
        _In_ const  std::shared_ptr<XblMultiplayerSession>& gameSession
        );

    TaskQueue m_queue;

    String m_lobbySessionTemplateName;
    std::atomic<bool> m_pendingCommitInProgress{ false };

    uint64_t m_updateNumber{ 0 };
    XblMultiplayerJoinability m_joinability{ XblMultiplayerJoinability::None };
    mutable std::mutex m_clientRequestLock;
    Queue<std::shared_ptr<MultiplayerClientPendingRequest>> m_pendingRequestQueue;
    MultiplayerEventQueue m_multiplayerEventQueue;
    std::shared_ptr<MultiplayerSessionWriter> m_sessionWriter;
    std::shared_ptr<MultiplayerLobbySession> m_multiplayerLobby;
    Vector<std::shared_ptr<MultiplayerMember>> m_localLobbyMembers;
    std::shared_ptr<MultiplayerLocalUserManager> m_multiplayerLocalUserManager;
    Vector<std::shared_ptr<MultiplayerClientPendingRequest>> m_processingQueue;
};

class MultiplayerClientPendingReader : public std::enable_shared_from_this<MultiplayerClientPendingReader>
{
public:
    MultiplayerClientPendingReader(const TaskQueue& queue);
    ~MultiplayerClientPendingReader();
    MultiplayerClientPendingReader(
        _In_ const TaskQueue& queue,
        _In_ const xsapi_internal_string& lobbySessionTemplateName,
        _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
        );

    // TODO
    void deep_copy_if_updated(_In_ const MultiplayerClientPendingReader& other);
    bool IsUpdateAvailable(_In_ const MultiplayerClientPendingReader& other);

    void DoWork();
    void ProcessMatchEvents();

    std::shared_ptr<MultiplayerLobbyClient> LobbyClient();
    std::shared_ptr<MultiplayerGameClient> GameClient();
    std::shared_ptr<MultiplayerMatchClient> MatchClient();

    const MultiplayerEventQueue& EventQueue() const;
    void ClearEventQueue();

    void AddEvent(
        _In_ XblMultiplayerEventType eventType,
        _In_ std::shared_ptr<XblMultiplayerEventArgs> eventArgs,
        _In_ XblMultiplayerSessionType sessionType,
        _In_ Result<void> error,
        _In_opt_ context_t context = nullptr
    );

    void AddEvent(_In_ const XblMultiplayerEvent& multiplayerEvent);
    void AddEvents(_In_ const MultiplayerEventQueue& multiplayerEventQueue);

    std::shared_ptr<XblMultiplayerSession> GetSession(_In_ XblMultiplayerSessionReference sessionRef);
    void UpdateSession(_In_ XblMultiplayerSessionReference sessionRef, _In_ std::shared_ptr<XblMultiplayerSession> session);

    bool IsLobby(_In_ XblMultiplayerSessionReference sessionRef);
    bool IsGame(_In_ XblMultiplayerSessionReference sessionRef);
    bool IsMatch(_In_ XblMultiplayerSessionReference sessionRef);

    HRESULT FindMatch(
        _In_ const xsapi_internal_string& hopperName,
        _In_ JsonValue& attributes,
        _In_ const std::chrono::seconds& timeout
        );

    void SetAutoFillMembersDuringMatchmaking(_In_ bool autoFillMembers);

    HRESULT SetJoinability(
        _In_ XblMultiplayerJoinability value,
        _In_opt_ context_t context
        );

    HRESULT SetProperties(
        _In_ const XblMultiplayerSessionReference& sessionRef,
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context
        );

    HRESULT SetSynchronizedProperties(
        _In_ const XblMultiplayerSessionReference& sessionRef,
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context
        );

    HRESULT SetSynchronizedHost(
        _In_ const XblMultiplayerSessionReference& sessionRef,
        _In_ const xsapi_internal_string& hostDeviceToken,
        _In_opt_ context_t context
        );

    std::shared_ptr<MultiplayerMember> ConvertToGameMember(_In_ const XblMultiplayerSessionMember* member);

private:
    void AddToPendingQueue(
        _In_ const XblMultiplayerSessionReference& sessionRef,
        _In_ std::shared_ptr<MultiplayerClientPendingRequest> pendingRequest
        );

    static bool IsLocal(_In_ uint64_t xuid, _In_ const xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>& xboxLiveContextMap);

    TaskQueue m_queue;
    bool m_autoFillMembers;
    MultiplayerEventQueue m_multiplayerEventQueue;
    mutable std::mutex m_clientRequestLock;
    std::shared_ptr<MultiplayerLobbyClient> m_lobbyClient;
    std::shared_ptr<MultiplayerGameClient> m_gameClient;
    std::shared_ptr<xbox::services::multiplayer::manager::MultiplayerMatchClient> m_matchClient;
    std::shared_ptr<MultiplayerLocalUserManager> m_multiplayerLocalUserManager;
};

class MultiplayerLocalUser
{
public:

    MultiplayerLocalUser(
        _In_ User&& user,
        _In_ uint64_t xboxUserId,
        _In_ bool isPrimary
        );

    ~MultiplayerLocalUser();

    uint64_t Xuid() const;
    std::shared_ptr<XblContext> Context() const;

    const xsapi_internal_string& LobbyHandleId() const;
    void SetLobbyHandleId(_In_ const xsapi_internal_string& handleId);
    MultiplayerLocalUserLobbyState LobbyState() const;
    void SetLobbyState(_In_ MultiplayerLocalUserLobbyState userState);
    MultiplayerLocalUserGameState GameState() const;
    void SetGameState(_In_ MultiplayerLocalUserGameState userState);
    const xsapi_internal_string& ConnectionAddress() const;
    void SetConnectionAddress(_In_ const xsapi_internal_string& address);
    bool IsPrimaryXboxLiveContext() const;
    void SetIsPrimaryXboxLiveContext(_In_ bool isPrimary);
    bool WriteChangesToService() const;
    void SetWriteChangesToService(_In_ bool value);
    XblFunctionContext SessionChangedContext() const;
    void SetSessionChangedContext(_In_ XblFunctionContext functionContext);
    XblFunctionContext RtaResyncContext() const;
    void SetRtaResyncContext(_In_ XblFunctionContext functionContext);
    XblFunctionContext ConnectionIdChangedContext() const;
    void SetConnectionIdChangedContext(_In_ XblFunctionContext functionContext);
    XblFunctionContext SubscriptionLostContext() const;
    void SetSubscriptionLostContext(_In_ XblFunctionContext functionContext);

private:
    XblFunctionContext m_sessionChangedContext{};
    XblFunctionContext m_connectionIdChangedContext{};
    XblFunctionContext m_subscriptionLostContext{};
    XblFunctionContext m_rtaResyncContext{};
    bool m_writeChangesToService{ false };
    uint64_t m_xuid{ 0 };
    xsapi_internal_string m_connectionAddress;
    xsapi_internal_string m_lobbyHandleId;
    MultiplayerLocalUserLobbyState m_lobbyState{ MultiplayerLocalUserLobbyState::Unknown };
    MultiplayerLocalUserGameState m_gameState{ MultiplayerLocalUserGameState::Unknown };
    bool m_isPrimaryXboxLiveContext{ false };
    std::shared_ptr<XblContext> m_xboxLiveContextImpl;
};

class MultiplayerLocalUserManager : public std::enable_shared_from_this<MultiplayerLocalUserManager>
{
public:
    MultiplayerLocalUserManager() = default;
    ~MultiplayerLocalUserManager();

    std::shared_ptr<XblContext> GetPrimaryContext();

    void ChangeAllLocalUserLobbyState(_In_ MultiplayerLocalUserLobbyState state);
    void ChangeAllLocalUserGameState(_In_ MultiplayerLocalUserGameState state);
    bool IsLocalUserGameState(_In_ MultiplayerLocalUserGameState state);

    const xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>& GetLocalUserMap();
    std::shared_ptr<XblContext> GetContext(_In_ uint64_t xuid);

    std::shared_ptr<MultiplayerLocalUser> GetLocalUser(_In_ uint64_t xuid);
    std::shared_ptr<MultiplayerLocalUser> GetLocalUserHelper(_In_ uint64_t xuid);

    std::shared_ptr<MultiplayerLocalUser> GetLocalUser(
        _In_ xbox_live_user_t user
    );

    std::shared_ptr<MultiplayerLocalUser> GetLocalUserHelper(
        _In_ xbox_live_user_t user
    );

    Result<const std::shared_ptr<MultiplayerLocalUser>> AddUserToXboxLiveContextToMap(_In_ xbox_live_user_t user);
    void RemoveStaleLocalUsersFromMap();

    void ActivateMultiplayerEvents(_In_ const std::shared_ptr<xbox::services::multiplayer::manager::MultiplayerLocalUser>& localuser);
    void DeactivateMultiplayerEvents(_In_ const std::shared_ptr<xbox::services::multiplayer::manager::MultiplayerLocalUser>& localUser);

    XblFunctionContext AddMultiplayerSessionChangedHandler(
        _In_ Callback<XblMultiplayerSessionChangeEventArgs> handler
        );
    void RemoveMultiplayerSessionChangedHandler(_In_ XblFunctionContext context);


    XblFunctionContext AddMultiplayerConnectionIdChangedHandler(_In_ Function<void()> handler);
    void RemoveMultiplayerConnectionIdChangedHandler(_In_ XblFunctionContext context);

    XblFunctionContext AddMultiplayerSubscriptionLostHandler(_In_ Function<void()> handler);
    void RemoveMultiplayerSubscriptionLostHandler(_In_ XblFunctionContext context);

    XblFunctionContext AddRtaResyncHandler(_In_ Function<void()> handler);
    void RemoveRtaResyncHandler(_In_ XblFunctionContext context);

private:
    std::mutex m_lock;

    void OnConnectionIdChanged();

    void OnSubscriptionsLost(_In_ uint64_t xuid);

    void OnSessionChanged(
        _In_ const XblMultiplayerSessionChangeEventArgs& args
        );

    void OnConnectionStateChanged(
        _In_ uint64_t xuid,
        _In_ XblRealTimeActivityConnectionState state
        );

    void OnResyncMessageReceived();

    std::mutex m_subscriptionLock;
	XblFunctionContext m_sessionChangeEventHandlerCounter{ 1 };
	XblFunctionContext m_multiplayerConnectionIdChangedEventHandlerCounter{ 1 };
	XblFunctionContext m_multiplayerSubscriptionLostEventHandlerCounter{ 1 };
	XblFunctionContext m_rtaResyncEventHandlerCounter{ 1 };
    xsapi_internal_unordered_map<uint32_t, Callback<XblMultiplayerSessionChangeEventArgs>> m_sessionChangeEventHandler;
    xsapi_internal_unordered_map<uint32_t, Function<void()>> m_multiplayerConnectionIdChangedEventHandler;
    xsapi_internal_unordered_map<uint32_t, Function<void()>> m_multiplayerSubscriptionLostEventHandler;
    xsapi_internal_unordered_map<uint32_t, Function<void()>> m_rtaResyncEventHandler;

    xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>> m_localUserRequestMap;
    std::shared_ptr<XblContext> m_primaryXboxLiveContext;
    TaskQueue m_queue;
};

class MultiplayerClientManager : public std::enable_shared_from_this<MultiplayerClientManager>
{
public:
    MultiplayerClientManager(_In_ const MultiplayerClientManager& other);
    MultiplayerClientManager(
        _In_ const xsapi_internal_string& lobbySessionTemplateName,
        _In_ const TaskQueue& queue
    );

    ~MultiplayerClientManager() = default;

    void Initialize();
    void Shutdown();

    void RegisterLocalUserManagerEvents();
    bool IsUpdateAvailable();
    bool IsRequestInProgress();

    std::shared_ptr<MultiplayerLocalUserManager> LocalUserManager();

    std::shared_ptr<XblContext> GetPrimaryContext();

    std::shared_ptr<MultiplayerLobbyClient> LobbyClient() const;
    std::shared_ptr<MultiplayerClientPendingReader> LatestPendingRead() const;
    std::shared_ptr<MultiplayerClientPendingReader> LastPendingRead() const;

    HRESULT JoinLobbyByHandle(
        _In_ const xsapi_internal_string& handleId,
        _In_ const xsapi_internal_vector<xbox_live_user_t>& users
        );

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK
    HRESULT JoinLobby(
        _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
        _In_ xsapi_internal_vector<xbox_live_user_t> users
        );

    HRESULT JoinLobby( _In_ Windows::Foundation::Uri^ url, _In_ xsapi_internal_vector<xbox_live_user_t> users);
#endif
    HRESULT JoinGameFromLobby(_In_ const xsapi_internal_string& sessionTemplateName);

    HRESULT JoinGame(
        _In_ const xsapi_internal_string& sessionName,
        _In_ const xsapi_internal_string& sessionTemplateName,
        _In_ const xsapi_internal_vector<uint64_t>& xuids
        );

    HRESULT LeaveGame();

    MultiplayerEventQueue DoWork(); // TODO see if we can switch these to ref

    HRESULT GetActivitiesForSocialGroup(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& socialGroup,
        _In_ XTaskQueueHandle queue,
        _In_ Callback<Result<xsapi_internal_vector<XblMultiplayerActivityDetails>>> callback
        );

    HRESULT InviteFriends(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_string& contextStringId,
        _In_ const xsapi_internal_string& customActivationContext
        );

    HRESULT InviteUsers(
        _In_ xbox_live_user_t user,
        _In_ const xsapi_internal_vector<uint64_t>& xboxUserIds,
        _In_ const xsapi_internal_string& contextStringId,
        _In_ const xsapi_internal_string& customActivationContext
        );

    HRESULT SetProperties(
        _In_ const XblMultiplayerSessionReference& sessionRef,
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context
        );

    HRESULT SetJoinability(
        _In_ XblMultiplayerJoinability value,
        _In_opt_ context_t context
        );

    HRESULT SetSynchronizedHost(
        _In_ const XblMultiplayerSessionReference& sessionRef,
        _In_ const xsapi_internal_string& hostDeviceToken,
        _In_opt_ context_t context
        );

    HRESULT SetSynchronizedProperties(
        _In_ const XblMultiplayerSessionReference& sessionRef,
        _In_ const xsapi_internal_string& name,
        _In_ const JsonValue& valueJson,
        _In_opt_ context_t context
        );

    std::shared_ptr<MultiplayerMatchClient> MatchClient();

    HRESULT FindMatch(
        _In_ const xsapi_internal_string& hopperName,
        _In_ JsonValue& attributes,
        _In_ const std::chrono::seconds& timeout
        );

    void SetAutoFillMembersDuringMatchmaking(_In_ bool autoFillMembers);

    void OnSessionChanged(
        _In_ XblMultiplayerSessionChangeEventArgs args
        );

    const MultiplayerEventQueue& EventQueue() const;
    void ClearEventQueue();

    void OnMultiplayerConnectionIdChanged();

    void OnMultiplayerSubscriptionsLost();

private:
    MultiplayerClientManager& operator=(MultiplayerClientManager other) = delete;

    void Destroy();

    Result<std::shared_ptr<xbox::services::multiplayer::MultiplayerService>> GetMultiplayerService(
        _In_ xbox_live_user_t user
        );

    xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>> GetXboxLiveContextMap();

    void OnResyncMessageReceived();

    void AddToLatestPendingReadEventQueue(
        _In_ XblMultiplayerEventType eventType,
        _In_ XblMultiplayerSessionType sessionType,
        _In_ std::shared_ptr<XblMultiplayerEventArgs> eventArgs = nullptr,
        _In_opt_ Result<void> error = {},
        _In_opt_ context_t context = nullptr
    );

    XblMultiplayerSessionType GetSessionType(
        _In_ std::shared_ptr<XblMultiplayerSession> session
        );

    void ProcessEvents(
        _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
        _In_ std::shared_ptr<XblMultiplayerSession> oldSession,
        _In_ XblMultiplayerSessionType sessionType
        );

    void HandleMemberListChanged(
        _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
        _In_ std::shared_ptr<XblMultiplayerSession> oldSession,
        _In_ XblMultiplayerSessionType sessionType
        );

    void HandleMemberPropertiesChanged(
        _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
        _In_ std::shared_ptr<XblMultiplayerSession> oldSession,
        _In_ XblMultiplayerSessionType sessionType
        );

    void HandleSessionPropertiesChanged(
        _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
        _In_ std::shared_ptr<XblMultiplayerSession> oldSession,
        _In_ XblMultiplayerSessionType sessionType
        );

    void HandleHostChanged(
        _In_ std::shared_ptr<XblMultiplayerSession> currentSession,
        _In_ XblMultiplayerSessionType sessionType
        );

    void HandleMatchStatusChanged(
        _In_ std::shared_ptr<XblMultiplayerSession> currentSession
        );

    void HandleInitializationStateChanged(
        _In_ std::shared_ptr<XblMultiplayerSession> currentSession
        );

    void SynchronizedWriteCompleted(
        _In_ std::error_code errorCode,
        _In_ XblMultiplayerEventType eventType,
        _In_ XblMultiplayerSessionType sessionType
        );

    mutable std::mutex m_clientRequestLock;
    std::mutex m_synchronizeWriteWithTapLock;
    std::atomic<bool> m_subscriptionsLostFired;

    bool m_autoFillMembers{ false };
    xsapi_internal_string m_lobbySessionTemplateName;
    XblFunctionContext m_sessionChangedContext{ 0 };
    XblFunctionContext m_connectionIdChangedContext{ 0 };
    XblFunctionContext m_subscriptionLostContext{ 0 };
    XblFunctionContext m_rtaResyncContext{ 0 };

    MultiplayerEventQueue m_multiplayerEventQueue;
    std::shared_ptr<XblContext> m_primaryXboxLiveContext;
    std::shared_ptr<MultiplayerLocalUserManager> m_multiplayerLocalUserManager;
    std::shared_ptr<MultiplayerClientPendingReader> m_lastPendingRead;
    std::shared_ptr<MultiplayerClientPendingReader> m_latestPendingRead;

    TaskQueue m_queue;
};

class MultiplayerMatchClient : public std::enable_shared_from_this<MultiplayerMatchClient>
{
public:
    MultiplayerMatchClient(
        _In_ const TaskQueue& queue,
        _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
    ) noexcept;

    ~MultiplayerMatchClient() noexcept = default;

    MultiplayerEventQueue DoWork();
    const MultiplayerEventQueue& EventQueue();
    // TODO remove in favor of assignment operator
    void deep_copy_if_updated(_In_ const MultiplayerMatchClient& other);

    XblMultiplayerMatchStatus MatchStatus() const;
    void SetMatchStatus(_In_ XblMultiplayerMatchStatus status);

    const std::chrono::seconds EstimatedMatchWaitTime() const;

    HRESULT FindMatch(
        _In_ const xsapi_internal_string& hopperName,
        _In_ JsonValue& attributes,
        _In_ const std::chrono::seconds& timeout,
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ bool preserveSession = false
        );

    HRESULT FindMatch(
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ bool preserveSession
        );

    void CancelMatch();

    void UpdateSession(_In_ std::shared_ptr<XblMultiplayerSession> currentSession);
    std::shared_ptr<XblMultiplayerSession> Session();

    void HandleMatchStatusChanged(
        _In_ std::shared_ptr<XblMultiplayerSession> matchSession
        );

    void HandleInitializationStateChanged(
        _In_ std::shared_ptr<XblMultiplayerSession> matchSession
        );

    void OnSessionChanged(
        _In_ const XblMultiplayerSessionChangeEventArgs& args
        );

    void SetQosMeasurements(
        _In_ const JsonValue& measurements
    ) noexcept;

    void ResubmitMatchmaking(
        _In_ std::shared_ptr<XblMultiplayerSession> session
        );

    void HandleFindMatchCompleted(
        _In_ Result<void> error
        );

    void DisableNextTimer(bool value);

    bool m_disableNextTimer{ false };

private:
    void CheckNextTimer();
    void HandleSessionJoined();
    void GetLatestSession();
    void HandleQosMeasurements();

    void HandleMatchFound(
        _In_ std::shared_ptr<XblMultiplayerSession> currentSession
    ) noexcept;

    HRESULT JoinSession(
        _In_ std::shared_ptr<XblMultiplayerSession> session,
        _In_ MultiplayerSessionCallback callback
    ) noexcept;

    TaskQueue m_queue;
    std::mutex m_lock;
    std::mutex m_getSessionLock;
    xbox::services::datetime m_nextTimerToFetchSession;
    String m_hopperName;
    JsonDocument m_attributes;
    std::chrono::seconds m_timeout{};
    bool m_preservingMatchmakingSession{ false };
    std::atomic<XblMultiplayerMatchStatus> m_matchStatus{ XblMultiplayerMatchStatus::None };
    mutable std::mutex m_multiplayerEventQueueLock;
    MultiplayerEventQueue m_multiplayerEventQueue;
    XblCreateMatchTicketResponse m_matchTicketResponse{};
    XblMultiplayerSessionReference m_matchTicketSessionRef{};
    std::shared_ptr<XblMultiplayerSession> m_matchSession;
    std::shared_ptr<MultiplayerLocalUserManager> m_multiplayerLocalUserManager;

    std::atomic<bool> m_getSessionInProgress{ false };
    std::atomic<bool> m_joinTargetSessionComplete{ false };
    Result<std::shared_ptr<XblMultiplayerSession>> m_joinTargetSessionResult;
};

class MultiplayerManagerUtils
{
public:
    static bool IsMultiplayerSessionChangeType(
        _In_ XblMultiplayerSessionChangeTypes diffType,
        _In_ XblMultiplayerSessionChangeTypes value
        )
    {
        return (diffType & value) == value;
    }

    static bool CompareSessions(
        _In_ const std::shared_ptr<XblMultiplayerSession>& session1,
        _In_ const std::shared_ptr<XblMultiplayerSession>& session2
        );

    static void SetJoinability(
        _In_ XblMultiplayerJoinability value,
        _In_ std::shared_ptr<XblMultiplayerSession> sessionToCommit,
        _In_ bool isGameInProgress
        );

    static XblMultiplayerJoinability GetJoinability(
        _In_ const XblMultiplayerSessionProperties& sessionProperties
        );

    static XblMultiplayerJoinability ConvertStringToJoinability(
        _In_ const xsapi_internal_string& value
        );

    static xsapi_internal_string ConvertJoinabilityToString(_In_ XblMultiplayerJoinability value);
};

}}}}
